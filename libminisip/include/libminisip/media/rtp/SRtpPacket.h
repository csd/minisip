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

#include<libminisip/media/rtp/CryptoContext.h>
#include<libminisip/media/rtp/RtpPacket.h>

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
		SRtpPacket(RtpHeader hdr, unsigned char *content, int content_length);
		virtual ~SRtpPacket();

		/**
		 * Reads a (S)RTP packet from a socket.
		 */
		static SRtpPacket *readPacket( UDPSocket &udp_sock,  MRef<IPAddress *>&from, int timeout=-1);

		/**
		 * Reads a (S)RTP packet from a memory buffer.
		 */
		static SRtpPacket *readPacket( byte_t *buf, unsigned buflen);

		void protect( MRef<CryptoContext *> scontext );
		int unprotect( MRef<CryptoContext *> scontext );

                unsigned char *get_tag()      {return tag;}
                unsigned int get_tag_length() {return tag_length;}
//		void remove_tag();
                void set_tag(unsigned char *t) {tag = t;}

		virtual char* getBytes();
		virtual int size();


	private:
		bool encrypted;
		unsigned char *tag;
		unsigned int tag_length;
		unsigned char * mki;
		unsigned int mki_length;
};


#endif
