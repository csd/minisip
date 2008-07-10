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

#include<libminisip/media/rtp/RtcpReport.h>
#include<libminisip/media/rtp/RtcpReportSR.h>
#include<libminisip/media/rtp/RtcpReportRR.h>
#include<libminisip/media/rtp/RtcpReportSDES.h>
#include<libminisip/media/rtp/RtcpReportXR.h>
#include<stdlib.h> //exit()

#ifdef DEBUG_OUTPUT
#	include<iostream>
#endif

using namespace std;

/*
struct reportheader{
	unsigned int version:2;
	unsigned int padding:1;
	unsigned int rc_sc:5;
	unsigned int packet_type:8;
	unsigned int length:16;
};
*/
/*
struct reportheader{
	unsigned int rc_sc:5;
	unsigned int padding:1;
	unsigned int version:2;
	unsigned int packet_type:8;
	unsigned int length:16;
};
*/

RtcpReport::RtcpReport(unsigned ptype):packet_type(ptype){
	this->version=2;
	this->padding=0;
	this->length=0;
}

RtcpReport *RtcpReport::build_from(void *buildfrom, int max_length){

	//struct reportheader *headerptr = (struct reportheader *)buildfrom;
	uint8_t * bytearray = (uint8_t *)buildfrom;
	uint8_t packet_type = bytearray[1];
	switch(packet_type){
		case PACKET_TYPE_SR:
			return new RtcpReportSR(buildfrom,max_length);
		case PACKET_TYPE_RR:
			return new RtcpReportRR(buildfrom,max_length);

		case PACKET_TYPE_SDES:
			return new RtcpReportSDES(buildfrom,max_length);
		case PACKET_TYPE_XR:
			return new RtcpReportXR(buildfrom,max_length);
		default:
#ifdef DEBUG_OUTPUT
			cerr << "ERROR: Received unknown RTCP report (type=" << packet_type << ")" << endl;
			exit(1);
#endif
			;
	}

	return NULL;
}

int RtcpReport::size(){
	return 0;
}

//vector<unsigned char> RtcpReport::get_packet_bytes(){
//	vector<unsigned char> ret;
//	return ret;
//}

void RtcpReport::parse_header(void *build_from, int max_length){

	uint8_t * bytearray = (uint8_t *)build_from;
	
//	struct reportheader *hdrptr;
//	hdrptr = (struct reportheader *) bytearray;
//	this->version = hdrptr->version;
	this->version = bytearray[0] & 0x3;
//	this->padding = hdrptr->padding;
	this->padding = bytearray[0] >> 2 & 0x1;
//	this->rc_sc = hdrptr->rc_sc;
	this->rc_sc = bytearray[0] >> 3 & 0x1F;
//	this->packet_type = hdrptr->packet_type;
	this->packet_type = bytearray[1];
//	this->length=ntoh16(hdrptr->length);
	this->length = U16_AT(bytearray + 2);
}
