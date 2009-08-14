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

#include<stdlib.h>
#include<stdio.h>
#include<libmnetutil/UDPSocket.h>

#ifdef LINUX
#	include<netinet/in.h>
#	include<sys/socket.h>
#endif

#ifdef WIN32
#	include<winsock2.h>
#endif

#ifndef _MSC_VER
#	include<unistd.h>
#endif

#include<libminisip/media/rtp/RtcpDebugMonitor.h>
#include<libminisip/media/rtp/RtcpPacket.h>
#include<libmutil/Thread.h>

#ifdef DEBUG_OUTPUT
#	include<iostream>
#endif

using namespace std;

RtcpDebugMonitor::RtcpDebugMonitor(UDPSocket *s){
	this->sock=s;
#ifdef DEBUG_OUTPUT
	if (this->sock==NULL)
		cerr << "WARNING: sock==null"<< endl;
#endif

        Thread t(this);
}

void RtcpDebugMonitor::run(){
#ifdef DEBUG_OUTPUT
	cerr << "RTCP debug monitor loop started"<< endl;
	setThreadName("RtcpDebugMonitor::run");
#endif
	while (1){
	
		struct sockaddr_in from;
		int fromlen = sizeof(from);
		char buf[2048];
		
#ifdef DEBUG_OUTPUT
		cerr << "Getting fd"<< endl;
#endif
		int fd = this->sock->getFd();
#ifdef DEBUG_OUTPUT
		cerr << "Getting packet"<< endl;
#endif

		
#ifndef WIN32
		int i=recvfrom(fd, &buf[0], 2048, 0, (struct sockaddr *)&from,(socklen_t *)&fromlen);
#else
		int i=recvfrom(fd, &buf[0], 2048, 0, (struct sockaddr *)&from,(int *)&fromlen);
#endif

#ifdef DEBUG_OUTPUT
		cerr << "RTCP_DEBUG_MONITOR: received "<< i <<" bytes."<< endl;
#endif
		
//		FILE *fil = fopen("rtcpdump","w");
//		int j=write(fileno(fil),buf,i);
//		fflush(fil);
//		cerr << "Wrote "<< j << " bytes"<< endl;
//		fclose(fil);
//		FILE *fil = fopen("rtcpdump","rb");
//		int n = read(fileno(fil),buf, 2048);
//		cerr << "Read "<< n <<" bytes"<< endl;
		
		RtcpPacket rtcp(buf, i);
		
#ifdef DEBUG_OUTPUT
		rtcp.debug_print();
#endif

		
#ifdef DEBUG_OUTPUT
		cerr << "(done printing debug info)" << endl;
#endif
		return;
		
	}
}

