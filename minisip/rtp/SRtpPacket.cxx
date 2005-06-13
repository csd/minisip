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
 * 	    Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<config.h>
#include<iostream>

#ifdef LINUX
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/select.h>
#include <err.h>
#endif

#ifdef WIN32
#include<winsock2.h>
#endif

#include<stdio.h>

#ifdef _MSC_VER

#else
#include<unistd.h>
#endif


#include<errno.h>

#include"SRtpPacket.h"
#include"CryptoContext.h"

#include<libmutil/print_hex.h>
#include<libmutil/MemObject.h>

#ifdef DEBUG_OUTPUT
#include<iostream>
#endif

using namespace std;


void SRtpPacket::protect( MRef<CryptoContext *> scontext ){
	
	/* Encrypt the packet */
	uint64_t index = 
		((uint64_t)scontext->get_roc() << 16) |
		(uint64_t)(getHeader().getSeqNo());
	scontext->rtp_encrypt( this, index );
	encrypted = true;

	/* Compute MAC */
	tag_length = scontext->get_tag_length();
	tag = new unsigned char[ tag_length ];
	
	scontext->rtp_authenticate( this, tag );

	/* Update the ROC if necessary */
	if( getHeader().getSeqNo() == 0xFFFF )
		scontext->set_roc( scontext->get_roc() + 1 );
}

int SRtpPacket::unprotect( MRef<CryptoContext *> scontext ){
	if( !scontext ){
		// It's probably an RtpPacket
		return 0;
	}

	content_length -= scontext->get_tag_length();
	content_length -= scontext->get_mki_length();

	tag = content + content_length;
	mki = content + content_length + scontext->get_tag_length();

	tag_length = scontext->get_tag_length();
	mki_length = scontext->get_mki_length();
	
	/* Guess the index */
	uint64_t guessed_index = 
		scontext->guess_index( getHeader().getSeqNo() );

	/* Replay control */
	if( !scontext->check_replay( getHeader().getSeqNo() ) ){
#ifdef DEBUG_OUTPUT
		cerr << "replay check failed" <<endl;
#endif
		tag = NULL;
		mki = NULL;
		return 1;
	}

	/* Authentication control */
	int length = get_tag_length();
	
	unsigned char * mac = new unsigned char[length];
	scontext->rtp_authenticate( this, mac );
	//cerr << "MAC computed: " << print_hex( mac, 4 )<< endl;
	for( int i = 0; i < length; i++ ){
		if( tag[i] != mac[i] )
		{
#ifdef DEBUG_OUTPUT
			cerr << "authentication failed in stream: " << getHeader().getSSRC() <<endl;
#endif
			tag = NULL;
			mki = NULL;
			return 1;
		}
	}

	delete [] mac;

	/* Decrypt the content */
	scontext->rtp_encrypt( this, guessed_index );
	encrypted = false;

	/* Update the Crypto-context */
	scontext->update( getHeader().getSeqNo() );

	/* don't delete the tag and mki */
	tag = NULL;
	mki = NULL;

	return 0;
}

SRtpPacket::SRtpPacket(){
        content_length=0;
        content=NULL;
	mki=NULL;
	mki_length=0;
	tag=NULL;
	tag_length=0;
}

SRtpPacket::SRtpPacket( unsigned char *content, int content_length, int seq_no, unsigned timestamp, unsigned ssrc):RtpPacket( content, content_length, seq_no, timestamp, ssrc ),tag(NULL),tag_length(0),mki(NULL),mki_length(0){

}

SRtpPacket::SRtpPacket(RtpHeader hdr, unsigned char *content, int content_length, unsigned char * tag, int tag_length, unsigned char * mki, int mki_length):RtpPacket(hdr,content, content_length ), encrypted(true),tag_length(tag_length),mki_length(mki_length){

	if(tag_length){
		this->tag = new unsigned char[tag_length];
		memcpy( this->tag, tag, tag_length );
	}
	else
		this->tag = NULL;
	
	if(mki_length){
		this->mki = new unsigned char[mki_length];
		memcpy( this->mki, mki, mki_length );
	}
	else
		this->mki = NULL;
}


SRtpPacket::~SRtpPacket(){
	if( mki )
		delete [] mki;
	if( tag )
		delete [] tag;
}


SRtpPacket *SRtpPacket::readPacket(UDPSocket &srtp_socket, int timeout){
#define UDP_SIZE 65536
        int i;
        char buf[UDP_SIZE];
        uint8_t j;
        uint8_t cc;
	//memset( buf, '\0', UDP_SIZE );

        i = srtp_socket.recv( buf, UDP_SIZE );
	
        if( i < 0 ){
#ifdef DEBUG_OUTPUT
		perror("recvfrom:");
#endif
		return NULL;
        }

        if( i < 12 ){
                /* too small to contain an RTP header */
                return NULL;
        }

        cc = buf[0] & 0x0F;
        if( i < 12 + cc * 4 ){
                /* too small to contain an RTP header with cc CCSRC */
                return NULL;
        }

        RtpHeader hdr;
        hdr.setVersion( ( buf[0] >> 6 ) & 0x03 );
        hdr.setExtension(  ( buf[0] >> 4 ) & 0x01 );
        hdr.setCSRCCount( cc );
        hdr.setMarker( ( buf[1] >> 7 ) & 0x01  );
        hdr.setPayloadType( buf[1] & 0x7F );

        hdr.setSeqNo( ( ((uint16_t)buf[2]) << 8 ) & buf[3] );
        hdr.setTimestamp( U32_AT( buf + 4 ) );
        hdr.setSSRC( U32_AT( buf + 8 ) );


        for( j = 0 ; j < cc ; j++ )
                hdr.addCSRC( U32_AT( buf + 12 + j*4 ) );
        
	int datalen = i - 12 - cc*4;

	unsigned char *data = (unsigned char *)&buf[ 12 + 4*cc ];

	SRtpPacket *srtp = new SRtpPacket( hdr, data, datalen, NULL, 0, NULL, 0 );
        
	return srtp;
}

char *SRtpPacket::getBytes(){
	char * ret;
	ret = new char[ size() ];
	
        char * hdr;
	hdr = header.getBytes();

        memcpy( ret, hdr, header.size() );
        delete [] hdr;

        memcpy( &ret[header.size()], content, content_length );
	memcpy( &ret[header.size() + content_length], tag, tag_length );
	memcpy( &ret[header.size() + content_length + tag_length],
			mki, mki_length );

        return ret;
}

unsigned char *SRtpPacket::get_tag(){
        return tag;
}

unsigned int SRtpPacket::get_tag_length(){
	return tag_length;
}

void SRtpPacket::set_tag(unsigned char *tag){
        this->tag=tag;
}

int SRtpPacket::size(){
	return header.size() + content_length + tag_length + mki_length;
}
