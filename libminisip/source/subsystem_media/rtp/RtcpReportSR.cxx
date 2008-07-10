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

#include<libminisip/media/rtp/RtcpReportSR.h>
#include<iostream>
#include<stdlib.h>

using namespace std;

RtcpReportSR::RtcpReportSR(void *buildfrom, int max_length):RtcpReport(0){
	if (max_length<4){
		cerr <<"Too short RTCP SR report (in RtcpReportSR constructor) (size="<<max_length<<")"<< endl;
		exit(1);
	}
	parse_header(buildfrom, max_length);

	int *iptr = &((int *)buildfrom)[1];
	//sender_ssrc = ntohl(*iptr);
	sender_ssrc = U32_AT( iptr );
	
	cerr << "Found SR report with content length of "<< length;
	sender_info = RtcpReportSenderInfo(& ((char*)buildfrom)[8], max_length-8);

	int i=8+sender_info.size();
	for (unsigned j=0; j<rc_sc; j++){
		cerr << "Trying to parse reception block in RTCPReportSR"<< endl;
		RtcpReportReceptionBlock block(& ((char*)buildfrom)[i], max_length-i);
		reception_blocks.push_back(block);
		i+=block.size();
	}
}

RtcpReportSR::~RtcpReportSR(){
//	for (unsigned i=0; i< reception_blocks.size(); i++)
//		delete reception_blocks[i];
}


//vector<unsigned char> RtcpReportSR::get_packet_bytes(){
//
//	vector<unsigned char> ret;
//	ret.push_back(33);
//
//	return ret;
//}

#ifdef DEBUG_OUTPUT
void RtcpReportSR::debug_print(){
	cerr<<"RTCP SR report:"<< endl;
	cerr.setf( ios::hex, ios::basefield );
	cerr<< "\tsender_ssrc: "<< sender_ssrc<< endl;
	cerr.setf( ios::dec, ios::basefield );
       	sender_info.debug_print();
	for (unsigned i=0; i<reception_blocks.size(); i++)
		reception_blocks[i].debug_print();
}
#endif

int RtcpReportSR::size(){
	int totsize=8;
	totsize+=sender_info.size();
	for (unsigned i=0; i< reception_blocks.size(); i++)
		totsize+=reception_blocks[i].size();
	return totsize;
}


RtcpReportSenderInfo &RtcpReportSR::get_sender_info(){
	return sender_info;
}

int RtcpReportSR::get_n_report_blocks(){
	return (int)reception_blocks.size();
}
		
RtcpReportReceptionBlock &RtcpReportSR::get_reception_block(int i){
	return reception_blocks[i];
}
