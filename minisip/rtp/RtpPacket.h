/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef RTPPACKET_H
#define RTPPACKET_H

#include"RtpHeader.h"
#include<libmnetutil/UDPSocket.h>
#include<libmnetutil/IPAddress.h>

#include<config.h>

#if __BYTE_ORDER == __LITTLE_ENDIAN

#else
#error RTP only works with little endian -- fix.
#endif


struct rtpheader{               // WARNING: ONLY FOR x86 arch...
	unsigned cc:4;
	unsigned x:1;
	unsigned p:1;
	unsigned v:2;

	unsigned pt:7;
	unsigned m:1;

	unsigned seq_no:16;

	unsigned timestamp:32;
	unsigned ssrc:32;
};


class RtpPacket{
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
