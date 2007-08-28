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

#include <config.h>

#include<libminisip/media/rtp/RtpHeader.h>
#include<libminisip/media/rtp/RtpPacket.h>

#ifdef DEBUG_OUTPUT
#	include<iostream>
#endif

using namespace std;

RtpHeader::RtpHeader(){
	version=0;
	extension=0;
	CSRC_count=0;
	marker=0;
	payload_type=0;
	sequence_number=0;
	timestamp=0;
	SSRC=0;

#ifdef TCP_FRIENDLY
	tcpFriendlyMode=false;
	sending_timestamp=0;
	rttestimate=0;
#endif

}

void RtpHeader::setVersion(int v){
	this->version = v;
}

void RtpHeader::setExtension(int x){
	this->extension =  x;
}

int RtpHeader::getExtension() {
    return extension;
}

void RtpHeader::setCSRCCount(int cc){
	this->CSRC_count = cc;
}

void RtpHeader::setMarker(int m){
	this->marker = m;
}

bool RtpHeader::getMarker(){
	return marker == 1;
}

void RtpHeader::setPayloadType(int pt){
	this->payload_type=pt;
}

int RtpHeader::getPayloadType(){
	return payload_type;
}


void RtpHeader::setSeqNo(uint16_t seq_no){
	this->sequence_number=seq_no;
}

uint16_t RtpHeader::getSeqNo(){
	return sequence_number;
}


void RtpHeader::setTimestamp(uint32_t t){
	this->timestamp = t;
}

uint32_t RtpHeader::getTimestamp(){
	return timestamp;
}

void RtpHeader::setSSRC(uint32_t s){
	this->SSRC = s;
}

uint32_t RtpHeader::getSSRC(){
	return SSRC;
}

void RtpHeader::addCSRC(int c){
	CSRC.push_back(c);
}


int RtpHeader::size(){
	int s = 12+4*(int)CSRC.size();
	
#ifdef TCP_FRIENDLY
	s+=8; // 4(rtt)+4(sendts)
#endif
	return s;
}

char *RtpHeader::getBytes(){
        uint8_t i;
	char *ret = new char[size()];

        ret[0] = ( ( version << 6 ) & 0xc0 ) |
                 ( ( extension << 4 ) & 0x10 ) |
                 ( ( CSRC_count & 0x0F ) );

        ret[1] = ( ( marker << 7 ) & 0x80 ) |
                 ( ( payload_type & 0x7F ) );

        ret[2] = ( sequence_number >> 8 ) & 0xFF;
        
        ret[3] = ( sequence_number ) & 0xFF;
        
        ((uint32_t *)ret)[1] = hton32( timestamp );
        ((uint32_t *)ret)[2] = hton32( SSRC );
	
	int i32=3; //index in the packet where we are adding headers. If
	  	   //we are using tcp friendly mode we insert two extra 
		   //32 bit headers and the CSRCs will not be in their
		   //usual place.
#ifdef TCP_FRIENDLY
	if (tcpFriendlyMode){
		((uint32_t *)ret)[i32++] = hton32(sending_timestamp);
		((uint32_t *)ret)[i32++] = hton32(rttestimate);
	}
#endif
	
        for( i = 0; i < CSRC.size(); i++ )
		((uint32_t *)ret)[i32+i]=hton32(CSRC[i]);
        
	return ret;
}

#ifdef DEBUG_OUTPUT
void RtpHeader::printDebug(){
	cerr << "\tversion: "<< version<<"\n\textension: "<< extension <<"\n\tCSRC count: "<< CSRC_count << "\n\tmarker: "<< marker << "\n\tpayload type: "<<payload_type <<"\n\tsequence number: "<<sequence_number << "\n\ttimestamp: "<<timestamp <<"\n\tSSRC: "<< SSRC << "\n"<< endl;

	for (int i=0; i< CSRC_count; i++)
		cerr << "\tCSRC "<<i+1 << ": "<<CSRC[i]<< endl;
}
#endif

