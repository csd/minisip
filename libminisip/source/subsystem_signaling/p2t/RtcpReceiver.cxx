/*
 Copyright (C) 2004-2006 the Minisip Team
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/


#include <config.h>

#include<libminisip/signaling/p2t/RtcpReceiver.h>

#ifndef _MSC_VER
#include<unistd.h>
#endif

#ifdef HAVE_NETINET_IN_H
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/select.h>
#endif

#ifdef WIN32
#include<winsock2.h>
#endif

#include<stdio.h>

#include"signal.h"
#include<libmutil/stringutils.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<libmnetutil/NetworkFunctions.h>
#include<libmnetutil/IPAddress.h>
#include<vector>
#include<libminisip/stun/STUN.h>
#include<libminisip/signaling/p2t/RtcpAPP.h>
#include<libmutil/dbg.h>

//#include<libminisip/signaling/sip/DefaultCallHandler.h>
//#include<libmsip/SipDialogContainer.h>
//#include<libmsip/SipInvite.h>
#include<libmsip/SipSMCommand.h>


#ifdef OPIE
short array1[160];
short array2[160];
#endif

#ifdef DEBUG_OUTPUT
#include<libmutil/termmanip.h>
#endif

/**
 * Constructor
 * open the UDP socket one port above the RTP port.
 * @param config  SipSoftPhoneConfiguration
 *        RTPport the port where the SoundReceiver is listening to. 
 **/
RtcpReceiver::RtcpReceiver(MRef<SipSoftPhoneConfiguration*> config, int RTPport){
	
	//open UDPSocket
	rtcp_sock = new UDPSocket(++RTPport);

	//set contact ip and port without STUN
        contactMediaIP = config->inherited->localIpString;
        contactMediaPort = rtcp_sock->getPort();
 
	//set contact ip and port with STUN    
	if (config->useSTUN){
            uint16_t stunPort = config->stunServerPort;
            MRef<IPAddress*> localAddr =
		    IPAddress::create(config->inherited->localIpString, false);
            MRef<IPAddress*> stunAddr =
		    IPAddress::create(config->stunServerIpString, false);
///            cerr << "local ip="<<phoneConf->inherited.localIpString<< endl;
//            uint16_t localPort = rtcp_sock->getPort();
            char mappedip[16];
            uint16_t mappedport;
#ifdef DEBUG_OUTOUT
            cerr << BREW <<"Doing STUN request from "<<localAddr->get_String()<<":"<<
                localPort<<" to STUN server "<<stunIp.get_String()<<":"<<
                stunPort<<"."<< PLAIN<< endl;
#endif
            STUN::getExternalMapping(   **stunAddr, 
                                        stunPort, 
                                        *rtcp_sock,
                                        mappedip, 
                                        mappedport);                        
            contactMediaIP = string(mappedip);
            contactMediaPort = mappedport;

#ifdef DEBUG_OUTPUT
            mdbg << "Media contact: "<< contactMediaIP <<":"<< itoa(contactMediaPort) << end;
#endif
        }else{
#ifdef DEBUG_OUTPUT
            mdbg << "RtcpReceiver: STUN disabled" << end;
#endif
        }

#ifdef DEBUG_OUTPUT
        mdbg << "Created RtcpReceiver with port"<< itoa(rtcp_sock->getPort())<< end;
#endif
}

/**
 * Destructor
 * closes the UDP socket
 **/
RtcpReceiver::~RtcpReceiver(){
	delete rtcp_sock;
	rtcp_sock = NULL;
}

/**
 * runs when the thread is started as long as
 * <code>stopped</code> is equals true
 **/
void RtcpReceiver::run() {

	while (!stopped) {
	
		if (flush_flag) {
			do_flush_socket();
			flush_flag=false;
		}
	
		RtcpAPP *rtcp;
		rtcp = RtcpAPP::readPacket(*rtcp_sock);
		

		if(rtcp->getHeader().getPayloadtype()==204) {
			string cmd;
			string param3="";
			int dest=0;
			int src=0;
			int seqNo=0, optional=0, length, sdes;
			char * buf;
			char * uri;
					
			//check application dependent data
			if(rtcp->getAppdataLength()>0){
				
				//get seqnr and optional
				buf = new char(rtcp->getAppdataLength());
				buf = (char *) rtcp->getAppdata();	
				rtcpAPPfloorControl1 *hdrptr=(rtcpAPPfloorControl1 *)buf;
				seqNo=ntohs(hdrptr->sn);
				optional=ntohs(hdrptr->opt);
			
				if(rtcp->getHeader().getSubtype()==P2T::APP_REQUEST || 
						rtcp->getHeader().getSubtype()==P2T::APP_GRANT) {
					
					//set param3 to cname
					rtcpAPPfloorControl2 *hdrptr=(rtcpAPPfloorControl2 *)&buf[4];
					
					sdes=hdrptr->sdes;
					if(sdes==1) {
						
						
						length=hdrptr->length;
						
						uri = new  char[length];
						memcpy(&uri[0], &buf[6], length);
						
						for(int k=0;k<length;k++){
							param3+= uri[k];
						}
					
					}
				
				}
			
			}
			
			
			if(rtcp->getHeader().getSubtype()== P2T::APP_REQUEST){
				cmd="p2tREQUEST";
				src = SipSMCommand::remote;
				dest = SipSMCommand::ANY;
				//if collision counter is set, set 
				//param3 to it
				if (optional>0)
					param3=itoa(optional);
			}
			else if(rtcp->getHeader().getSubtype()== P2T::APP_GRANT){
				cmd="p2tGRANT";
				src = SipSMCommand::remote;
				dest = SipSMCommand::transaction;
				//if collision counter is set, set 
				//param3 to it
				if (optional>0)
					param3=itoa(optional);
			}
			else if(rtcp->getHeader().getSubtype()== P2T::APP_TAKEN){
				cmd="p2tTAKEN";
				src = SipSMCommand::remote;
				dest = SipSMCommand::transaction;
			}
			else if(rtcp->getHeader().getSubtype()== P2T::APP_DENY){
				//not implemented
			}
			else if(rtcp->getHeader().getSubtype()== P2T::APP_RELEASE){
				cmd="p2tRELEASE";
				src = SipSMCommand::remote;
				dest = SipSMCommand::ANY;
			}
			else if(rtcp->getHeader().getSubtype()== P2T::APP_IDLE){
				cmd="p2tIDLE";
				src = SipSMCommand::remote;
				dest = SipSMCommand::transaction;
			}
			else if(rtcp->getHeader().getSubtype()== P2T::APP_REVOKE){
				cmd="p2tREVOKE";
				src = SipSMCommand::remote;
				dest = SipSMCommand::TU;
				//change param3 to optional value
				param3=itoa(optional);
			}
				
			//command string
			//param1 = ssrc
			//param2 = seqNo
			//param3 = cname or optional (warning code, reason code) or nothing
			CommandString command(call->getCallId(),cmd);
			command.setParam(itoa(rtcp->getHeader().getSSRC()));
			command.setParam2(itoa(seqNo));
			command.setParam3(param3);
			
			merr<<end<<"RtcpPacket::";
			merr<<" cmd="<<cmd;
			merr<<" p1="<<itoa(rtcp->getHeader().getSSRC());
			merr<<" p2="<<itoa(seqNo);
			merr<<" p3="<<param3<<end;
			
			SipSMCommand smcmd(command, src, dest);
			
			call->getDialogContainer()->enqueueCommand(smcmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
			//call->handleCommand(smcmd);
			

		}
	}

#ifdef DEBUG_OUTPUT
	mdbg << "INFO: RtcpReceiver loop thread is exiting"<< end;
#endif
}

/**
 *
 **/
void RtcpReceiver::start(){
#ifdef DEBUG_OUTPUT
	mdbg << "++++++++++running start on RtcpReceiver"<<end;
#endif
	stopped=false;
	Thread t(this);
}

/**
 *
 **/
void RtcpReceiver::stop(){
#ifdef DEBUG_OUTPUT
//	cerr << "-----------running stop on SoundReceiver"<<endl;
#endif
	stopped=true;
}

/**
 *
 **/
void RtcpReceiver::flush(){
	flush_flag=true;
}

/**
 *
 **/
void RtcpReceiver::do_flush_socket(){
	int fd = rtcp_sock->getFd();

	//struct pollfd pfd[1];
	//pfd[0].fd = fd;
	//pfd[0].events=POLLIN;

    fd_set set;
    FD_ZERO(&set);
	#ifdef WIN32
    FD_SET( (uint32_t) fd, &set);
	#else
	FD_SET(fd, &set);
	#endif            
    
	void *buffer = malloc(16384);
	int nread;
	mdbg << "Starting flush in rtcp input socket"<< end;
	int avail;
	do{
//		avail = poll( pfd, 1, 0);
		struct timeval tv = { 0, 0 };
                avail = select( fd+1, &set, NULL, NULL, &tv );
		if (avail==1){
			mdbg << "Flushing one packet from rtcp input queue"<< end;
#ifndef WIN32
			nread = recvfrom(fd, buffer, 16384, 0, NULL, NULL);
#else
			nread = recvfrom(fd, (char *)buffer, 16384, 0, NULL, NULL);
#endif
		}
	}while( avail == 1 );
	mdbg << "Flush: done"<< end;
	free(buffer);
}

void RtcpReceiver::setCall(MRef<SipDialog*> dialog) {
	this->call=dialog;
}

