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

#include<libminisip/media/rtp/RtcpReportSDES.h>

#include<libmutil/massert.h>
#include<iostream>
#include<stdlib.h>

using namespace std;

RtcpReportSDES::RtcpReportSDES(void *buildfrom, int max_length):RtcpReport(0){
	
	if (max_length<4){
		cerr <<"Too short RTCP SDES report (in RtcpReportSDES constructor) (size="<<max_length<<")"<< endl;
		exit(1);
	}
	parse_header(buildfrom,max_length);
	cerr << "Found SR report with content length of "<< length << " and will try to parse "<< rc_sc<< " chunks" << endl;
	
	massert(packet_type==PACKET_TYPE_SDES);

	max_length=length*4;
	int i=4;
	for (unsigned j=0; j<rc_sc; j++){
		SDESChunk chunk(& (((char*)buildfrom)[i]), max_length-i);
		chunks.push_back(chunk);
		i+=chunk.size();
	}
}

RtcpReportSDES::~RtcpReportSDES(){

}


#ifdef DEBUG_OUTPUT
void RtcpReportSDES::debug_print(){
	cerr<<"RTCP SDES report:"<< endl;
	for (unsigned i=0; i<chunks.size(); i++)
		chunks[i].debug_print();
}
#endif

int RtcpReportSDES::size(){
//	int tot = 4;
//	for (unsigned i=0 ; i<chunks.size(); i++){
//		tot+=chunks[i].size();
//	}
//	return tot;
	return length*4+4;
}


