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

#include<libminisip/media/rtp/RtcpReportRR.h>
#include<config.h>
//#include <netinet/in.h>
#include<iostream>
#include<stdlib.h> //exit()

using namespace std;

RtcpReportRR::RtcpReportRR(unsigned sender_ssrc): RtcpReport(201),sender_ssrc(sender_ssrc){

}

RtcpReportRR::RtcpReportRR(void *buildfrom, int max_length):RtcpReport(0){

	if (max_length<4){
		cerr <<"Too short RTCP RR report (in RtcpReportRR constructor) (size="<<max_length<<")"<< endl;
		exit(1);
	}
	parse_header(buildfrom,max_length);
	cerr << "Found RR report with content length of "<< length;

//	int *iptr = &((int *)buildfrom)[1];
//	sender_ssrc = ntohl(*iptr);
	sender_ssrc = U32_AT( (uint8_t*)buildfrom + 1 );

	int i=8;
	while (i < max_length-1){
		cerr << "Trying to parse reception block in RTCPReportRR"<< endl;
		RtcpReportReceptionBlock block(& ((char*)buildfrom)[i], max_length-i);
		reception_blocks.push_back(block);
		i+=block.size();
	}
}

RtcpReportRR::~RtcpReportRR(){
//	for (unsigned i=0; i< reception_blocks.size(); i++)
//		delete reception_blocks[i];
}


#ifdef DEBUG_OUTPUT
void RtcpReportRR::debug_print(){
	cerr<<"RTCP RR report:"<< endl;
	cerr.setf( ios::dec, ios::basefield );
	cerr << "Sender ssrc="<<sender_ssrc << endl;
	cerr.setf( ios::hex, ios::basefield );
	for (unsigned i=0; i<reception_blocks.size(); i++)
		reception_blocks[i].debug_print();
}
#endif

int RtcpReportRR::size(){
	int totsize=4;
	for (unsigned i=0; i< reception_blocks.size(); i++)
		totsize+=reception_blocks[i].size();
	return totsize;
}


int RtcpReportRR::get_n_report_blocks(){
	return (int)reception_blocks.size();
}
                
RtcpReportReceptionBlock &RtcpReportRR::get_reception_block(int i){
	return reception_blocks[i];
}


void RtcpReportRR::add_reception_block(RtcpReportReceptionBlock block){
	reception_blocks.push_back(block);
}


