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
 * Authors: Israel Abad <i_abad@terra.es> 
 *          Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/
#ifndef SRTPPACKET_H
#define SRTPPACKET_H

#include<libminisip/libminisip_config.h>

#include<libmnetutil/UDPSocket.h>
#include<libmnetutil/IPAddress.h>

#include<libminisip/rtp/CryptoContext.h>
#include<libminisip/rtp/RtpPacket.h>

RtpPacket * readRtpPacket( MRef<CryptoContext *>, UDPSocket * socket );

class LIBMINISIP_API SRtpPacket : public RtpPacket{
	public:
		SRtpPacket();
		SRtpPacket(CryptoContext *scontext, RtpPacket *rtppacket);
		SRtpPacket(RtpHeader hdr, 
				unsigned char *content, 
				int content_length,
				unsigned char *tag, 
				int tag_length, 
				unsigned char *mki, 
				int mki_length);
		SRtpPacket( unsigned char *content, int content_length, 
			int seq_no, unsigned timestamp,
			unsigned ssrc);
		virtual ~SRtpPacket();

		static SRtpPacket *readPacket( UDPSocket &udp_sock,  MRef<IPAddress *>&from, int timeout=-1);
		// static????

		void protect( MRef<CryptoContext *> scontext );
		int unprotect( MRef<CryptoContext *> scontext );

		unsigned char *get_tag();
		unsigned int get_tag_length();
		void remove_tag();
		void set_tag(unsigned char *tag);
		
		virtual char * getBytes();
		virtual int size();
		

	private:
		bool encrypted;
		unsigned char *tag;
		unsigned int tag_length;
		unsigned char * mki;
		unsigned int mki_length;
};


#endif
