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

#include"RtpMonitor.h"
#include"RtcpPacket.h"
#include"RtcpReportRR.h"
#include<libmutil/itoa.h>

RtpMonitor::RtpMonitor(unsigned ssrc){
	this->ssrc = ssrc;
	lookback_used=0;
	seq_lookback_size = 100000;
	seq_no_lookback = new char[seq_lookback_size];
	for (int i=0; i< seq_lookback_size; i++)
		seq_no_lookback[i]=0;
	n_received=0;
	n_sent=0;
	n_discarded=0;
}

RtpMonitor::~RtpMonitor(){
	delete [] seq_no_lookback;
}

void RtpMonitor::packet_sent(RtpPacket &pack){
	n_sent++;
}

void RtpMonitor::packet_received(RtpPacket &pack){
	n_received++;
	seq_no_lookback[pack.getHeader().getSeqNo()%seq_lookback_size]=1;
	if (lookback_used<seq_lookback_size)
		lookback_used++;
//	cerr << "DEBUG: RTP received seq no: "<< pack.get_header().get_seq_no()<< endl;

}

void RtpMonitor::packet_discarded(RtpPacket &pack){
	n_discarded++;

}

RtcpPacket *RtpMonitor::generate_rtcp_packet(){
	RtcpPacket *ret = new RtcpPacket();

	RtcpReportRR *rr = new RtcpReportRR(ssrc);

	RtcpReportReceptionBlock block(ssrc);
	block.set_fraction_lost(0xee);
	block.set_jitter(0xaa);
	rr->add_reception_block(block);
	
	ret->add_report(rr);

	return ret;
}


int RtpMonitor::get_number_lost(){
	int nl=0;
	for (int i=0; i< lookback_used; i++)
		if (seq_no_lookback[i]==0)
			nl++;
	return nl;	
}

int RtpMonitor::get_fraction_lost(){
	int nl =get_number_lost();
	double lossrate = ((((double)nl)/((double)(n_received+nl+n_discarded)))) * ((double)256);
	int iloss = (int)(lossrate+0.5);
	return iloss;
}

int RtpMonitor::get_fraction_discard(){
	int nl=get_number_lost();

	double discardrate = ((((double)n_discarded)/((double)(n_received+n_discarded+nl)))) * ((double)256);
	int idiscard = (int)(discardrate+0.5);
	return idiscard;
}

int RtpMonitor::get_burst_count(){
	int g_min=16;
	int burstcount=0;
	bool maybe_burst=false;
	int possible_burst_count=0;

	int since_last_lost=0;
	
	for (int i=0; i<lookback_used; i++){
		if (maybe_burst){
			if (seq_no_lookback[i]==0)
				since_last_lost=0;
			else
				since_last_lost++;
			if (since_last_lost>g_min-1){ //end of burst
//				cerr << "Found possible burst of length"<<since_last_lost<< "possiblec="<<possible_burst_count<< endl;
				burstcount+=possible_burst_count-g_min;
				maybe_burst=false;
				possible_burst_count=0;
				since_last_lost=0;
			}else
				possible_burst_count++;
					
		}else{
			if (seq_no_lookback[i]==0){
//				cerr << "Found possible start of burst"<< endl;
				maybe_burst=true;
				possible_burst_count++;
			}
		}
	}
	if (possible_burst_count>16)
		burstcount+=possible_burst_count-16;
	
	return burstcount;
}

int RtpMonitor::get_burst_density(){
	int nl=get_number_lost();
//	int burstcount = get_burst_count();
	double burstdens = ((((double)get_burst_count())/((double)(n_received+n_discarded+nl)))) * ((double)256);
	int burst_density = (int)(burstdens+0.5);	
	return burst_density;
}

string RtpMonitor::get_string(){

	return "\rMONITOR: n_sent="+itoa(n_sent) +" n_received=" + itoa(n_received) + " discarded="+ itoa(n_discarded)+ " n_lost="+itoa(get_number_lost())+ " loss rate="+itoa(get_fraction_lost()) +"/256 discard rate="+itoa(get_fraction_discard())+"/256 burst density="+itoa(get_burst_density())+"/256 burst count="+itoa(get_burst_count());	
}
