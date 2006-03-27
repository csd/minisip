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
 * Authors: Israel Abad <i_abad@terra.es> 
 *          Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/
#ifndef SRTPPACKET_H
#define SRTPPACKET_H

#include<config.h>

#include "RtpPacket.h"
#include<libmnetutil/UDPSocket.h>
#include<libmnetutil/IPAddress.h>
#include "CryptoContext.h"


RtpPacket * readRtpPacket( MRef<CryptoContext *>, UDPSocket * socket );

class SRtpPacket : public RtpPacket{
        public:
                SRtpPacket();
                SRtpPacket(CryptoContext *scontext, RtpPacket *rtppacket);
                SRtpPacket(RtpHeader hdr, 
				unsigned char *content, int content_length,
				unsigned char *tag, int tag_length, 
				unsigned char *mki, int mki_length);
		SRtpPacket( unsigned char *content, int content_length, 
			int seq_no, unsigned timestamp,
			unsigned ssrc);
                virtual ~SRtpPacket();

                static SRtpPacket *readPacket( UDPSocket &udp_sock, int timeout=-1 ); // static????

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