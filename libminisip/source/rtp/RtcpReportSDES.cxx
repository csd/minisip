
/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<libminisip/RtcpReportSDES.h>
#include<assert.h>
#include<iostream>

using namespace std;

RtcpReportSDES::RtcpReportSDES(void *buildfrom, int max_length):RtcpReport(0){
	
	if (max_length<4){
		cerr <<"Too short RTCP SDES report (in RtcpReportSDES constructor) (size="<<max_length<<")"<< endl;
		exit(1);
	}
	parse_header(buildfrom,max_length);
	cerr << "Found SR report with content length of "<< length << " and will try to parse "<< rc_sc<< " chunks" << endl;
	
	assert(packet_type=PACKET_TYPE_SDES);

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


