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

#ifdef LINUX
#	include<sys/select.h>
#	include<netinet/in.h>
#	include<sys/socket.h>
#endif

#ifdef WIN32
#	include<winsock2.h>
#endif

#include<stdio.h>

#ifdef _MSC_VER

#else
#	include<unistd.h>
#endif

#include<errno.h>
#include<libminisip/signaling/p2t/RtcpAPP.h>
#include<libminisip/signaling/p2t/RtcpAPPHeader.h>

#include<libmutil/dbg.h>
#include<libmutil/stringutils.h>
#include<libmutil/merror.h>

RtcpAPP::RtcpAPP() {

}

RtcpAPP::RtcpAPP(string name, unsigned ssrc) {
	header.setName(name);
	header.setSSRC(ssrc);

}

RtcpAPP::RtcpAPP(unsigned subtype, string name, unsigned ssrc) {
	header.setSubtype(subtype);
	header.setName(name);
	header.setSSRC(ssrc);
	this->appdata=NULL;
	this->appdata_length=0;
}

RtcpAPP::RtcpAPP(unsigned char *appdata, int appdata_length, unsigned subtype, string name, unsigned ssrc) {
	header.setSubtype(subtype);
	header.setName(name);
	header.setSSRC(ssrc);
	
	this->appdata_length=appdata_length;
	if (appdata_length>0) {
		this->appdata = new unsigned char[appdata_length];
		memcpy(this->appdata, appdata, appdata_length);
	}
	else
		this->appdata=NULL;
	
	
	
	
	header.setLength(2+(appdata_length/4));
}

RtcpAPP::RtcpAPP(RtcpAPPHeader header, unsigned char *appdata, int appdata_length){
	this->header=header;
	this->appdata_length = appdata_length;
	
	if(appdata_length){
		this->appdata = new unsigned char[appdata_length];
		memcpy(this->appdata, appdata, appdata_length);
	}
	else
		this->appdata=NULL;
}


RtcpAPP::~RtcpAPP() {

}

RtcpAPPHeader &RtcpAPP::getHeader() {
	return header;
}

void RtcpAPP::sendTo(UDPSocket &udp_sock, IPAddress &to_addr, int port) {
	char *bytes = getBytes();
	udp_sock.sendTo(to_addr, port, bytes, size());
	delete [] bytes;
#ifdef DEBUG_OUTPUT	
	mdbg << "---------------------"<< end;
	mdbg << "Sent RTCP APP packet:"<< end;
	mdbg << "v=" << header.getVersion()<< end;
	mdbg << "p=" << header.getPadding()<< end;
	mdbg << "subtype=" << header.getSubtype()<< end;
	mdbg << "payload=" << header.getPayloadtype()<< end;
	mdbg << "length=" << header.getLength()<< end;
	mdbg << "ssrc=" << header.getSSRC()<< end;
	mdbg << "name=" << header.getName()<< end;
	mdbg << "sent to " << to_addr.getString() << ":" << itoa(port) <<end;
	mdbg << "--------------------"<<end;
#endif
}



RtcpAPP *RtcpAPP::readPacket(UDPSocket &rtp_socket, int timeout){
	int i;
	char buf[2048];
	for (i=0; i<2048; i++)
		buf[i]=0;

	fd_set set;
	FD_ZERO(&set);
	#ifdef WIN32
    FD_SET( (uint32_t) rtp_socket.getFd(), &set);
	#else
	FD_SET(rtp_socket.getFd(), &set);
	#endif

	struct timeval tv;
    	struct timeval * p_tv;
    	if( timeout > 0 ){
	    	tv.tv_sec = timeout / 1000;
	    	tv.tv_usec = (timeout % 1000)*1000;
        	p_tv = & tv;
    	}
    	else{
        	p_tv = NULL;
    	}
	
	int avail;

	do{
		avail = select(rtp_socket.getFd()+1,&set,NULL,NULL,p_tv);
		if (avail==0){
			return NULL;
		}
		if (avail<0){
				#ifndef _WIN32_WCE
					if ( errno != EINTR ){
				#else
					if( errno != WSAEINTR ) { continue; }
				#endif
				merror("Error when using poll:");
				exit(1);
			}else{
			}
		}
	}while(avail < 0);


	i = recvfrom(rtp_socket.getFd(), buf, 2048, 0, /*(struct sockaddr *) &from*/NULL, /*(socklen_t *)fromlen*/NULL);
	if (i<0){
		merror("recvfrom:");
		return NULL;
	}

//	i = read(0,buf, 2048);
	

	
	rtcpAPPheader *hdrptr=(rtcpAPPheader *)&buf[0];

	RtcpAPPHeader hdr;
	hdr.setVersion(hdrptr->v);
	hdr.setPadding(hdrptr->p);
	hdr.setSubtype(hdrptr->st);
	hdr.setPayloadtype(hdrptr->pt);
	hdr.setLength(ntohs(hdrptr->length));
	hdr.setSSRC(ntohl(hdrptr->ssrc));
	
	string tmp = "xxxx";
	tmp[0]=hdrptr->c1;
	tmp[1]=hdrptr->c2;
	tmp[2]=hdrptr->c3;
	tmp[3]=hdrptr->c4;
	hdr.setName(tmp);

#ifdef DEBUG_OUTPUT
	mdbg << "-------------------------"<< end;
	mdbg << "Received RTCP APP packet:"<< end;
	mdbg << "v=" << hdr.getVersion()<< end;
	mdbg << "p=" << hdr.getPadding()<< end;
	mdbg << "subtype=" << hdr.getSubtype()<< end;
	mdbg << "payload=" << hdr.getPayloadtype()<< end;
	mdbg << "length=" << hdr.getLength()<< end;
	mdbg << "ssrc=" << hdr.getSSRC()<< end;
	mdbg << "name=" << hdr.getName()<<end;
	mdbg << "-------------------------"<<end;
#endif
	
	//check if application dependent data exists
	int appdata_length=(hdr.getLength()*4)-8;
	if(appdata_length>0){
		RtcpAPP * pack = new RtcpAPP(hdr, (unsigned char *)&buf[12],appdata_length);
		return pack;	
	}
	else {
		RtcpAPP * pack = new RtcpAPP(hdr, NULL, 0);
		return pack;
	}
}

char *RtcpAPP::getBytes(){
	char *ret = new char[header.size()+appdata_length];
	char *hdr = header.getBytes();
	
	memcpy(ret,hdr, header.size());
	delete [] hdr;
	
	memcpy(&ret[header.size()], appdata, appdata_length);
	return ret;
}

int RtcpAPP::size() {
	return header.size()+appdata_length;
}

unsigned char *RtcpAPP::getAppdata() {
	return appdata;
}

int RtcpAPP::getAppdataLength() {
	return appdata_length;
}

