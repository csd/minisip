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
 * 	    Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 */

#include<config.h>

#include<iostream>
#include<string.h>

#ifdef LINUX
#	include<netinet/in.h>
#	include<sys/socket.h>
#	include<sys/select.h>
#	include <err.h>
#endif

#ifdef WIN32
#	include<winsock2.h>
#endif

#include<stdio.h>

#ifdef _MSC_VER

#else
#include<unistd.h>
#endif


#include<errno.h>

#include<libminisip/media/rtp/SRtpPacket.h>
#include<libminisip/media/rtp/CryptoContext.h>

#include<libmutil/stringutils.h>
#include<libmutil/merror.h>
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

    scontext->rtp_authenticate( this, scontext->get_roc(), tag );
    /* Update the ROC if necessary */
    if( getHeader().getSeqNo() == 0xFFFF )
	scontext->set_roc( scontext->get_roc() + 1 );
}

int SRtpPacket::unprotect( MRef<CryptoContext *> scontext ){
    if( !scontext ){
	// It's probably an RtpPacket
	return 0;
    }

    tag_length = scontext->get_tag_length();
    mki_length = scontext->get_mki_length();

    content_length -= (tag_length + mki_length);

    if (content_length < 0) {
#ifdef DEBUG_OUTPUT
            cerr << "unprotect failed: illegal packet length" << endl;
#endif
        return 1;
    }
    tag = content + content_length;
    mki = content + content_length + tag_length;

    // Content_length 0 may happen in case of extension header only and
    // possible checksum adjustment.
    if (content_length == 0) {
        content = NULL;
    }

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

    unsigned char * mac = new unsigned char[tag_length];
    scontext->rtp_authenticate( this, (uint32_t)( guessed_index >> 16 ), mac );
    for( unsigned i = 0; i < tag_length; i++ ){
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

SRtpPacket::SRtpPacket( unsigned char *content_, int clen,
			int seq_no, unsigned timestamp, unsigned ssrc):
    RtpPacket( content_, clen, seq_no, timestamp, ssrc ), tag(NULL), tag_length(0), mki(NULL), mki_length(0){

}

SRtpPacket::SRtpPacket(RtpHeader hdr, unsigned char *content, int content_length,
		       unsigned char * tag, int tag_length_,
		       unsigned char * mki, int mki_length_):
    RtpPacket(hdr, content, content_length ), encrypted(true), tag_length(tag_length_), mki_length(mki_length_){

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

SRtpPacket::SRtpPacket(RtpHeader hdr, unsigned char *content, int content_length):
    RtpPacket(hdr, content, content_length ), encrypted(false), tag_length(0), mki_length(0){

    this->tag = NULL;
    this->mki = NULL;
}


SRtpPacket::~SRtpPacket(){
    if( mki )
	delete [] mki;
    if( tag )
	delete [] tag;
}


SRtpPacket *SRtpPacket::readPacket(UDPSocket &srtp_socket, MRef<IPAddress *> &from, int timeout) {
#define UDP_SIZE 65536
	int i;
	uint8_t buf[UDP_SIZE];
	int32_t port;
	i = srtp_socket.recvFrom((char*)buf, UDP_SIZE, from, port);
	if( i < 0 ){
#ifdef DEBUG_OUTPUT
		merror("recvfrom:");
#endif
		return NULL;
	}
	return readPacket(buf, i);
}

SRtpPacket *SRtpPacket::readPacket(byte_t *buf, unsigned buflen) {
#define UDP_SIZE 65536
    uint8_t j;
    uint8_t cc;

    if( buflen < 12 ){
	/* too small to contain an RTP header */
	return NULL;
    }

    cc = buf[0] & 0x0F;
    if( buflen < 12 + cc * 4 ){
	/* too small to contain an RTP header with cc CCSRC */
	return NULL;
    }

    RtpHeader hdr;
    hdr.setVersion( ( buf[0] >> 6 ) & 0x03 );
    hdr.setExtension(  ( buf[0] >> 4 ) & 0x01 );
    hdr.setCSRCCount( cc );
    hdr.setMarker( ( buf[1] >> 7 ) & 0x01  );
    hdr.setPayloadType( buf[1] & 0x7F );

    hdr.setSeqNo( ( ((uint16_t)buf[2]) << 8 ) | buf[3] );
    hdr.setTimestamp( U32_AT( buf + 4 ) );
    hdr.setSSRC( U32_AT( buf + 8 ) );

    for( j = 0 ; j < cc ; j++ )
	hdr.addCSRC( U32_AT( buf + 12 + j*4 ) );

    int datalen = buflen - 12 - cc*4;

    unsigned char *data = (unsigned char *)&buf[ 12 + 4*cc ];

    SRtpPacket *srtp = new SRtpPacket( hdr, data, datalen, NULL, 0, NULL, 0 );

    return srtp;
}

char *SRtpPacket::getBytes(){
    char* ret;
    ret = new char[ size() ];

    char* hdr = header.getBytes();
    memcpy( ret, hdr, header.size() );
    delete [] hdr;

    int hsize = header.size();
    if (extensionLength > 0) {
        memcpy(&ret[hsize], extensionHeader, extensionLength);
    }
    hsize += extensionLength;
    memcpy( &ret[hsize], content, content_length );
    memcpy( &ret[hsize + content_length], tag, tag_length );
    memcpy( &ret[hsize + content_length + tag_length], mki, mki_length);

    if (zrtpChecksum) {
        uint16_t chkSum = computeChecksum((uint16_t*)ret, size()-20);
        memcpy(&ret[hsize + content_length + tag_length + mki_length], &chkSum, zrtpChecksum);
    }
    return ret;
}

int SRtpPacket::size(){
    return header.size() + content_length + tag_length + mki_length + extensionLength + zrtpChecksum;
}
