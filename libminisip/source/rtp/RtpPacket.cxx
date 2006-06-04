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

#include<stdio.h>

#ifdef _MSC_VER
#else
#include<unistd.h>
#endif

#include<errno.h>
#include<libminisip/rtp/RtpPacket.h>
#include<libminisip/rtp/RtpHeader.h>

#ifdef DEBUG_OUTPUT
#include<iostream>
#endif

#include<libmutil/merror.h>
#include<libmnetutil/UDPSocket.h>

using namespace std;

RtpPacket::RtpPacket(){
    content_length=0;
    content=NULL;	
}

RtpPacket::RtpPacket(unsigned char *content, int content_length, 
		     int seq_no, unsigned timestamp, unsigned ssrc):
    content_length(content_length){

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

RtpPacket::RtpPacket(RtpHeader hdr, unsigned char *content, int content_length): header(hdr) {

    extensionLength = 0;
    extensionHeader = NULL;

    /*
     * Check if packet contains an extension header. If yes
     * set pointer to extension header, compute length and
     * adjust content pointer and content_length
     */
    if (header.getExtension() && content_length >= 4) {
	extensionLength = 4;
	content_length -= 4;	// minimum size of extension header

	short tmp = *((short *)(content+2));
	ntoh16(tmp);
	tmp *= 4;		// ext. header length is in words (4 bytes)
	extensionLength += tmp;
	content_length -= tmp;
	
	if (content_length >= 0) {
	    extensionHeader = content;
	}
    }
    this->content_length = content_length;
	
    if( content_length > 0 ){
	this->content = new unsigned char[content_length];
	memcpy(this->content, content+extensionLength, content_length);
    }
    else
	this->content = NULL;

    header.setVersion(2);
}

void RtpPacket::setExtHeader(unsigned char* data, int length) {

    if (data == NULL || length == 0) {
	return;
    }
    // Need to prepend extension header to packet payload. Get a bigger
    // buffer.
    unsigned char* cp = new unsigned char[content_length + length];
    memcpy(cp, data, length);

    if (content != NULL) {
	memcpy(cp+length, content, content_length);
	delete [] content;
    }
    content = cp;
    content_length += length;
    header.setExtension(1);
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
	merror("recvfrom:");
#endif
	return NULL;
    }

    if( i < 12 ){
	/* too small to contain an RTP header */
	return NULL;
    }

    cc = buf[0] & 0x0F;
    if( i < 12 + cc * 4 ){
	/* too small to contain an RTP header with cc CSRC */
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

    int tmp = *((int *)(buf + 4));
    ntoh32(tmp);
    hdr.setTimestamp(tmp);

    tmp = *((int *)(buf + 8));
    ntoh32(tmp);
    hdr.setSSRC(tmp);

    for( j = 0 ; j < cc ; j++ ) {
	tmp = *((int *)(buf + 12 + j*4));
	ntoh32(tmp);
	hdr.setSSRC(tmp);
    }
    int datalen = i - 12 - cc*4;
	
    RtpPacket * rtp = new RtpPacket(hdr, (unsigned char *)&buf[12+4*cc], datalen);
	
    return rtp;
}

char *RtpPacket::getBytes(){

    int hdrSize = header.size();
    char *ret = new char[hdrSize+content_length];
	
    char *hdr = header.getBytes();
	
    memcpy(ret, hdr, hdrSize);
    delete [] hdr;

    memcpy(&ret[hdrSize], content, content_length);
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


