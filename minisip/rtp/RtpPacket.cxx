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
	char buf[UDP_SIZE];
//	memset( buf, '\0', 2048 );
	
	i = rtp_socket.recv( buf, UDP_SIZE );

	if( i < 0 ){
#ifdef DEBUG_OUTPUT
		perror("recvfrom:");
#endif
		return NULL;
	}

	
	rtpheader *hdrptr=(rtpheader *)&buf[0];

	RtpHeader hdr;
	hdr.setVersion( hdrptr->v );
	hdr.setExtension( hdrptr->x );
	hdr.setCSRCCount( hdrptr->cc );
	hdr.setMarker( hdrptr->m );
	hdr.setPayloadType( hdrptr->pt );
	
	uint16_t beSeqNo = hdrptr->seq_no;
	uint32_t beTimestamp = hdrptr->timestamp;
	uint32_t beSsrc = hdrptr->ssrc;
	
	hdr.setSeqNo( ntoh16( beSeqNo ) );
	hdr.setTimestamp( ntoh32( beTimestamp ) );
	hdr.setSSRC( ntoh32( beSsrc ) );

	for (unsigned j=0; j<hdrptr->cc; j++)
		hdr.addCSRC(ntoh32( ((int *)&buf[12])[j] ));
	int datalen = i - 12 - hdrptr->cc*4;
	
	RtpPacket * rtp = new RtpPacket(hdr, (unsigned char *)&buf[12+4*hdrptr->cc], datalen);
	
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
	return 12+content_length;
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


