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

#include<config.h>


#include<stdio.h>

#ifdef _MSC_VER

#else
#include<unistd.h>
#endif


#include<errno.h>
#include"RtpPacket.h"
#include"RtpHeader.h"

#ifdef DEBUG_OUTPUT
#include<iostream>
#endif

#include<libmnetutil/UDPSocket.h>

RtpPacket::RtpPacket(){
	content_length=0;
	content=NULL;	
}

RtpPacket::RtpPacket(unsigned char *content, int content_length, int seq_no, unsigned timestamp, unsigned ssrc):content_length(content_length){
	header.setVersion(2);
	header.setSeqNo(seq_no);
	header.setTimestamp(timestamp);
	header.SSRC = ssrc;
	
	if( content_length ){
		this->content = new unsigned char[content_length];
		memcpy(this->content, content, content_length);
	}
	else
		this->content = NULL;
}

RtpPacket::RtpPacket(RtpHeader hdr, unsigned char *content, int content_length): header(hdr){
	this->content_length = content_length;
	
	if( content_length ){
		this->content = new unsigned char[content_length];
		memcpy(this->content, content, content_length);
	}
	else
		this->content = NULL;

	header.setVersion(2);
	

}

RtpHeader &RtpPacket::getHeader(){
	return header;
}


RtpPacket::~RtpPacket(){
	if (content!=NULL)
		delete [] content;
}

void RtpPacket::sendTo(UDPSocket &udp_sock, IPAddress &to_addr, int port){
	char *bytes = getBytes();
	udp_sock.sendTo(to_addr, port, bytes, size());
	delete [] bytes;
}

RtpPacket *RtpPacket::readPacket(UDPSocket &rtp_socket, int timeout){
#define UDP_SIZE 65536
	int i;
	uint8_t buf[UDP_SIZE];
        uint8_t j;
        uint8_t cc;
//	memset( buf, '\0', 2048 );
	
	i = rtp_socket.recv( (char *)buf, UDP_SIZE );

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
	
	hdr.setSeqNo( ( ((uint16_t)buf[2]) << 8 ) | buf[3] );
	cerr << "GOT SEQN" << hdr.getSeqNo() << endl;
	hdr.setTimestamp( U32_AT( buf + 4 ) );
	hdr.setSSRC( U32_AT( buf + 8 ) );

	for( j = 0 ; j < cc ; j++ )
		hdr.addCSRC( U32_AT( buf + 12 + j*4 ) );
                
	int datalen = i - 12 - cc*4;
	
	RtpPacket * rtp = new RtpPacket(hdr, (unsigned char *)&buf[12+4*cc], datalen);
	
	return rtp;
}

char *RtpPacket::getBytes(){
	char *ret = new char[header.size()+content_length];
	
	char *hdr = header.getBytes();
	
	memcpy(ret,hdr, header.size());
	delete [] hdr;

	memcpy(&ret[header.size()], content, content_length);
	return ret;
}

int RtpPacket::size(){
	return header.size()+content_length;
}

unsigned char *RtpPacket::getContent(){
	return content;
}

int RtpPacket::getContentLength(){
	return content_length;
}

#ifdef DEBUG_OUTPUT
void RtpPacket::printDebug(){
	cerr << "_RTP_Header_"<< endl;
	header.printDebug();
	cerr <<"_Content_"<< endl;
	cerr <<"\tContent length: "<< content_length<< endl;
	
}
#endif


