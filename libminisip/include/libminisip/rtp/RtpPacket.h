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

#ifndef RTPPACKET_H
#define RTPPACKET_H

#include<libminisip/libminisip_config.h>

#include<libmutil/MemObject.h>

#include<libmnetutil/UDPSocket.h>
#include<libmnetutil/IPAddress.h>

#include<libminisip/rtp/RtpHeader.h>

class RtpPacket: public MObject {
	public:
		RtpPacket();
		RtpPacket(unsigned char *content, int content_length, int seq_no, unsigned timestamp, unsigned ssrc);
		RtpPacket(RtpHeader hdr, unsigned char *content, int content_length);
		virtual ~RtpPacket();
		
		static RtpPacket *readPacket(UDPSocket &udp_sock, int timeout=-1);

		void sendTo(UDPSocket &udp_sock, IPAddress &to_addr, int port);
		
		RtpHeader &getHeader();

		unsigned char *getContent();
		int getContentLength();

#ifdef DEBUG_OUTPUT
		void printDebug();
#endif

		virtual char *getBytes();
		virtual int size();
	
	protected:

		RtpHeader header;
		int content_length;
		unsigned char *content;
};
 
#endif
