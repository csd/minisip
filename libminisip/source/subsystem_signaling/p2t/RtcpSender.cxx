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

#include<libminisip/signaling/p2t/RtcpSender.h>

#include<libmsip/SipTransaction.h>
#include<libmsip/SipDialogConfig.h>

#ifndef _MSC_VER
#include<unistd.h>
#endif

#ifdef HAVE_NETINET_IN_H
#include<netinet/in.h>
#include<sys/socket.h>
#endif

#ifdef WIN32
#include<winsock2.h>
#endif

#include<stdio.h>
#include<libminisip/signaling/p2t/RtcpAPP.h>

#ifndef _MSC_VER
#include<sys/time.h>
#endif

#ifdef OPIE
int srcb_count=1;
short saved[160];
short sent[160];
#endif


#include<libmutil/dbg.h>

RtcpSender::RtcpSender(UDPSocket * rtcp_socket):rtcp_sock(rtcp_socket){
}

void RtcpSender::send_APP(unsigned subtype, 
			unsigned ssrc, 
			string name,
			IPAddress *to, 
			int port) {
	RtcpAPP * pack;
	pack = new RtcpAPP(subtype,name,ssrc);
	pack->sendTo(*rtcp_sock, *to, port);
	delete pack;
}


void RtcpSender::send_APP_FC(unsigned subtype, unsigned ssrc, string name, IPAddress *to, int port, int seqNo, int optional)
{
	//check if it's for P2T floor control
	if(name==P2T::APP_NAME) {
	
		//create appdata1 (seq_number and optional)
		int appdata1_length=4; //4 bytes
		unsigned char *appdata1 = new unsigned char[appdata1_length];
		
		struct rtcpAPPfloorControl1 *hdrptr = (struct rtcpAPPfloorControl1 *)appdata1;
		hdrptr->sn=hton16(seqNo);
		hdrptr->opt=hton16(optional);

				
		//add appdata2 (sdes cname) for REQUEST and GRANT messages
		if (subtype==(unsigned)P2T::APP_REQUEST || subtype==(unsigned)P2T::APP_GRANT) {
			//string uri = call->getDialogConfig().inherited->userUri;
			string uri= call->getDialogConfig()->inherited->sipIdentity->getSipUri();
			
			//padding
			//length should be multiples of 4
			int padding = 4-((2+uri.size())%4);
			int appdata2_length=2+uri.size()+padding;
			
			unsigned char *appdata2 = new unsigned char[appdata2_length];
			struct rtcpAPPfloorControl2 *hdrptr = (struct rtcpAPPfloorControl2 *)appdata2;
			hdrptr->sdes=1;
			hdrptr->length=uri.size();
			memcpy(&appdata2[2],&uri[0], uri.size());
			
			//combine appdata1 and appdata 2
			int appdata_length=appdata1_length+appdata2_length;
			unsigned char *appdata = new unsigned char[appdata_length];
			memcpy(appdata, appdata1, appdata1_length);
			memcpy(&appdata[appdata1_length], appdata2, appdata2_length);
			
			//create and send packet
			RtcpAPP * pack;
			pack = new RtcpAPP(appdata, appdata_length, subtype,name,ssrc);
			pack->sendTo(*rtcp_sock, *to, port);
			delete pack;
		}
		else {				
			//create and send packet
			RtcpAPP * pack;
			pack = new RtcpAPP(appdata1, appdata1_length, subtype,name,ssrc);
			pack->sendTo(*rtcp_sock, *to, port);
			delete pack;
		}
	}	
	
}


void RtcpSender::setCall(MRef<SipDialog*>call) {
	this->call=call;
}


