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

#include<libminisip/media/rtp/RtcpReportSenderInfo.h>
#include<stdlib.h>
//#include<netinet/in.h>
#include<iostream>

using namespace std;

RtcpReportSenderInfo::RtcpReportSenderInfo(void *buildfrom, int max_length){
	
	if (max_length<20){
		cerr << "ERROR: to short SenderInfo report in RtcpReportSenderInfo"<< endl;
		exit(1);
	}
	
	unsigned int *iptr;

	iptr = (unsigned int *)buildfrom;
//	this->ntp_msw = ntohl(*iptr);
	this->ntp_msw = U32_AT( iptr );

	iptr++;
	//this->ntp_lsw = ntohl(*iptr);
	this->ntp_lsw = U32_AT( iptr );

	iptr++;
	this->rtp_timestamp = U32_AT( iptr );

	iptr++;
	this->sender_packet_count = U32_AT( iptr );
	
	iptr++;
	this->sender_octet_count = U32_AT( iptr );
}

int RtcpReportSenderInfo::size(){
	return 20;
}

#ifdef DEBUG_OUTPUT
void RtcpReportSenderInfo::debug_print(){
	cerr << " sender info:"<< endl;
	cerr.setf(ios::hex, ios::basefield);
	cerr << "\tntp_msw: 0x"<< ntp_msw<< endl;
	cerr << "\tntp_lsw: 0x"<< ntp_lsw<< endl;
	cerr << "\trtp_timestamp: 0x"<< rtp_timestamp<< endl;
	cerr.setf(ios::dec, ios::basefield);
	cerr << "\tsender_packet_count: "<< sender_packet_count<< endl;
	cerr << "\tsender_octet_count: "<< sender_octet_count<< endl;
}
#endif


void RtcpReportSenderInfo::set_ntp_timestamp_msw(unsigned t){
	ntp_msw = t;
}

unsigned RtcpReportSenderInfo::get_ntp_timestamp_msw(){
	return ntp_msw;
}
		
void RtcpReportSenderInfo::set_ntp_timestamp_lsw(unsigned t){
	ntp_lsw = t;
}

unsigned RtcpReportSenderInfo::get_ntp_timestamp_lsw(){
	return ntp_lsw;
}
		
void RtcpReportSenderInfo::set_rtp_timestamp(unsigned t){
	rtp_timestamp = t;
}

unsigned RtcpReportSenderInfo::get_rtp_timestamp(){
	return rtp_timestamp;
}

void RtcpReportSenderInfo::set_sender_packet_count(int i){
	sender_packet_count=i;
}

unsigned RtcpReportSenderInfo::get_sender_packet_count(){
	return sender_packet_count;
}
		
void RtcpReportSenderInfo::set_sender_octet_count(int i){
	sender_octet_count = i;
}

unsigned RtcpReportSenderInfo::get_sender_octet_count(){
	return sender_octet_count;
}
	
