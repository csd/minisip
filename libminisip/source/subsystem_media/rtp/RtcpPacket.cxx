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

#include<libminisip/media/rtp/RtcpPacket.h>
#include<libminisip/media/rtp/RtcpReport.h>

#ifdef DEBUG_OUTPUT
#include<iostream>
#endif

using namespace std;

RtcpPacket::RtcpPacket(){
	
}

RtcpPacket::RtcpPacket(void *buildfrom, int max_len){
	int startindex=0;
	while (startindex<max_len-1){
		RtcpReport *report = RtcpReport::build_from(&(((char*)buildfrom)[startindex]), max_len - startindex );
		//RtcpReport *report = new RtcpReport(&(((char*)buildfrom)[startindex]), max_len - startindex );
		reports.push_back(report);
		startindex += report->size();
	};
}

RtcpPacket::~RtcpPacket(){
	for (unsigned i=0; i<reports.size(); i++)
		delete reports[i];
}

vector<RtcpReport *> &RtcpPacket::get_reports(){
	return reports;
}

void RtcpPacket::add_report(RtcpReport *report){
	reports.push_back(report);
}

#ifdef DEBUG_OUTPUT
void RtcpPacket::debug_print(){
	cerr << "__RTCP_packet__";
	for (unsigned i=0; i<reports.size(); i++){
		cerr <<"_report "<< i+1<< "_"<< endl;
		reports[i]->debug_print();
	}

}
#endif
