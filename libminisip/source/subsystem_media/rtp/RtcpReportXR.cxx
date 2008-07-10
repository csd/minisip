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

#include<libminisip/media/rtp/RtcpReportXR.h>
//#include<netinet/in.h>
#include<iostream>
#include<stdlib.h> //exit()

using namespace std;

RtcpReportXR::RtcpReportXR(void *build_from, int max_length) : RtcpReport(0){
	if (max_length<4){
		cerr <<"Too short RTCP SR report (in RtcpReportSR constructor) (size="<<max_length<<")"<< endl;
		exit(1);
	}	
	parse_header(build_from, max_length);

	int buflen = length*4;

	int *iptr = &(((int *)build_from)[1]);
//	ssrc_or_csrc = ntohl(*iptr);
	ssrc_or_csrc = U32_AT( iptr );

	int i=8;

	while (i<buflen-4){
		cerr << "Trying to parse reception block in RTCPReportXR"<< endl;
		XRReportBlock *block = XRReportBlock::build_from(& ((char*)build_from)[i], buflen-i);
		xr_blocks.push_back(block);
		i+=block->size();
	}

}


#ifdef DEBUG_OUTPUT
void RtcpReportXR::debug_print(){
	cerr << "RtcpReportXR:"<< endl;
	cerr.setf( ios::hex, ios::basefield );
	cerr << "ssrc_or_csrc: 0x"<< ssrc_or_csrc << endl;
	cerr.setf( ios::dec, ios::basefield );
	for (unsigned i=0; i<xr_blocks.size(); i++)
		xr_blocks[i]->debug_print();
}
#endif

int RtcpReportXR::size(){
	int totsize = 8;
	for (unsigned i=0; i<xr_blocks.size(); i++)
		totsize += xr_blocks[i]->size();

	return totsize;
}

