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

#include<config.h>

#include<libmstun/STUNTest.h>

#include<libmutil/merror.h>
#include<libmnetutil/IPAddress.h>
#include<libmnetutil/UDPSocket.h>

#ifdef LINUX
#	include<sys/select.h>
#endif

#ifdef WIN32
#	include<winsock2.h>
#endif

#include<errno.h>
#include<stdio.h>
#include<stdlib.h>

STUNMessage *STUNTest::test(
		IPAddress *addr, 
		uint16_t port, 
		UDPSocket &sock, 
		bool changeIP, 
		bool changePort)
{
	int timeoutIndex=0;
	short timeouts[9]={100, 200, 400, 800, 1600,1600,1600,1600,1600};
	
	int length;

	STUNMessage br(STUNMessage::BINDING_REQUEST);
//	STUNMessageBindingRequest br;
	br.addAttribute(new STUNAttributeChangeRequest(changeIP,changePort));
	unsigned char *data = br.getMessageData(length);
	bool done=false;
	STUNMessage *msg=NULL;

	do{
		/*int slen = */
		sock.sendTo(*addr, port, data, length);
//		cerr << "Sent "<< slen<< " bytes" << endl;

//		struct pollfd p;
//		p.fd = sock.getFd();
//		p.events = POLLIN;

	fd_set set;
	FD_ZERO(&set);

	#ifdef WIN32
	FD_SET( (uint32_t) sock.getFd(),&set );
	#else
	FD_SET( sock.getFd(), 		&set );
	#endif
	
	struct timeval tv;
	tv.tv_sec = timeouts[timeoutIndex] / 1000;
	tv.tv_usec = ( timeouts[timeoutIndex] % 1000 ) * 1000;

//		cerr << "Waiting for max "<<timeouts[timeoutIndex]<<" ms"<<endl;
		//int avail = poll(&p,1,timeouts[timeoutIndex]);
        int avail = select(sock.getFd()+1,&set,NULL,NULL,&tv );
//		cerr <<"After poll, return value is "<<avail<<endl;
		if (avail < 0){
		#ifndef _WIN32_WCE
			if (errno!=EINTR){
		#else
			if (errno!=WSAEINTR){
		#endif
				merror("Error when using poll:");
				exit(1);
			}else{
//				cerr << "Signal occured in wait_packet"<<endl;
			}
		}

		if (avail>0){
			unsigned char resp[2048];
	
			int rlen = sock.recv(resp, 2048);
	
//			msg=STUNMessage::parseMessage(resp,rlen);
			msg = new STUNMessage(resp,rlen);

			if (msg->sameTransactionID(br)){
//				cerr <<"Accepted: "<<msg->getDesc()<<endl;
				done=true;
			}else{
//				cerr <<"Discarded: "<<msg->getDesc()<<endl;
			}
		}
		if (timeoutIndex>=8)
			done=true;	
		timeoutIndex++;
	}while(!done);	 
	
	return msg;
}
