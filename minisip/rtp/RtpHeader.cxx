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

#include"RtpHeader.h"
#include"RtpPacket.h"

#ifdef DEBUG_OUTPUT
#include<iostream>
#endif

RtpHeader::RtpHeader(){
	version=0;
	extension=0;
	CSRC_count=0;
	marker=0;
	payload_type=0;
	sequence_number=0;
	timestamp=0;
	SSRC=0;
}

void RtpHeader::setVersion(int v){
	this->version = v;
}

void RtpHeader::setExtension(int x){
	this->extension =  x;
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


void RtpHeader::setSeqNo(int seq_no){
	this->sequence_number=seq_no;
}

int RtpHeader::getSeqNo(){
	return sequence_number;
}


void RtpHeader::setTimestamp(int timestamp){
	this->timestamp = timestamp;
}

int RtpHeader::getTimestamp(){
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
	return 12+4*CSRC.size();
}

char *RtpHeader::getBytes(){
	char *ret = new char[size()/*+4*CSRC.size()*/];
	struct rtpheader *hdrptr = (struct rtpheader *)ret;
	hdrptr->v=version;
	hdrptr->x=extension;
	hdrptr->cc=CSRC_count;
	hdrptr->m=marker;
	hdrptr->pt=payload_type;
	hdrptr->seq_no=hton16(sequence_number);
	hdrptr->timestamp=hton32(timestamp);
	hdrptr->ssrc=hton32(SSRC);
	hdrptr->p=0;
	
	for (unsigned i=0; i<CSRC.size(); i++)
		((int *)ret)[3+i]=hton32(CSRC[i]);
	return ret;
}

#ifdef DEBUG_OUTPUT
void RtpHeader::printDebug(){
	cerr << "\tversion: "<< version<<"\n\textension: "<< extension <<"\n\tCSRC count: "<< CSRC_count << "\n\tmarker: "<< marker << "\n\tpayload type: "<<payload_type <<"\n\tsequence number: "<<sequence_number << "\n\ttimestamp: "<<timestamp <<"\n\tSSRC: "<< SSRC << "\n"<< endl;

	for (int i=0; i< CSRC_count; i++)
		cerr << "\tCSRC "<<i+1 << ": "<<CSRC[i]<< endl;
}
#endif

