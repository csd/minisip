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

#include<libminisip/media/rtp/RtcpReportReceptionBlock.h>

#include<stdlib.h>
#include<iostream>

using namespace std;

/*
struct receptionblock{
	unsigned int ssrc:32;
	unsigned int fraction_lost:8;
	unsigned int cumulative_n_lost:24;
	unsigned int highest_seq_no:32;
	unsigned int jitter:32;
	unsigned int lsr:32;
	unsigned int dlsr:32;
};

*/

RtcpReportReceptionBlock::RtcpReportReceptionBlock(unsigned ssrc_){
	this->ssrc = ssrc_;
	this->fraction_lost=0;
	this->cumulative_n_lost = 0;
	this->seq_high = 0;
	this->jitter=0;
	this->last_sr=0;
	this->dlsr = 0;
}

RtcpReportReceptionBlock::RtcpReportReceptionBlock(void *build_from, int max_length){

	uint8_t * bytearray = (uint8_t *)build_from;
	if (max_length<24){
		cerr << "ERROR: too short to parse reception block (int RtpReportReceptionBlock)"<<endl;
		exit(1);
	}
//	struct receptionblock *bptr = (struct receptionblock *)buildfrom;
	
	this->ssrc = U32_AT( bytearray );
	this->fraction_lost = bytearray[4];
	this->cumulative_n_lost = U32_AT( bytearray + 4 ) && 0x00FFFFFF;
//	this->ssrc=ntoh32(bptr->ssrc);
//	this->fraction_lost=fraction_lost;
//	this->cumulative_n_lost=ntohl(bptr->cumulative_n_lost);
//	this->cumulative_n_lost = this->cumulative_n_lost | ((this->cumulative_n_lost &0xFF)<<24);
//	this->cumulative_n_lost = this->cumulative_n_lost>>8;
//	unsigned int bak = this->cumulative_n_lost & 0xFF;
//	this->cumulative_n_lost &=0xFFFFFF00;
//	this->cumulative_n_lost |= (this->cumulative_n_lost & 0xFF000000)>>24;
//	this->cumulative_n_lost |= (this->cumulative_n_lost & 0x00FFFFFF) | (bak<<24);
//	this->cumulative_n_lost=ntohl(bptr->cumulative_n_lost);
//	this->seq_high = ntohl(bptr->highest_seq_no);
	this->seq_high = U32_AT( bytearray + 8 );
//	this->jitter=ntohl(bptr->jitter);
	this->jitter = U32_AT( bytearray + 9 );
//	this->last_sr=ntohl(bptr->lsr);
	this->last_sr = U32_AT( bytearray + 10 );
//	this->dlsr = ntohl(bptr->dlsr);
	this->dlsr = U32_AT( bytearray + 11 );
	
}

int RtcpReportReceptionBlock::size(){
	return 24;
}

#ifdef DEBUG_OUTPUT
void RtcpReportReceptionBlock::debug_print(){
	cerr << " rtcp report reception block: 0x"<< endl;
	cerr.setf(ios::hex, ios::basefield);
	cerr << "\tssrc: 0x"<<ssrc<< endl;
	cerr << "\tfraction lost: 0x"<<fraction_lost<< endl;
	cerr << "\tcumulative number lost: 0x"<< cumulative_n_lost << endl;
	cerr << "\tseq_high: 0x"<< seq_high<< endl;
	cerr << "\tjitter: 0x"<< jitter<< endl;
	cerr << "\tlast_sr: 0x"<< last_sr<< endl;
	cerr << "\tdlsr: 0x"<< dlsr<< endl;
	cerr.setf(ios::dec, ios::basefield);
}
#endif


void RtcpReportReceptionBlock::set_fraction_lost(unsigned n){
	fraction_lost = n;
}

unsigned RtcpReportReceptionBlock::get_fraction_lost(){
	return fraction_lost;
}

void RtcpReportReceptionBlock::set_cumulative_n_lost(unsigned n){
	cumulative_n_lost = n;
}

unsigned RtcpReportReceptionBlock::get_cumulative_n_lost(){
	return cumulative_n_lost;
}

void RtcpReportReceptionBlock::set_seq_high(unsigned i){
	seq_high = i;
}

unsigned RtcpReportReceptionBlock::get_seq_high(){
	return seq_high;
}

void RtcpReportReceptionBlock::set_jitter(unsigned i){
	this->jitter = i;
}

unsigned RtcpReportReceptionBlock::get_jitter(){
	return jitter;
}

void RtcpReportReceptionBlock::set_last_sr(unsigned i){
	last_sr = i;
}

unsigned RtcpReportReceptionBlock::get_last_sr(){
	return last_sr;
}

void RtcpReportReceptionBlock::set_dlsr(unsigned i){
	dlsr = i;
}

unsigned RtcpReportReceptionBlock::get_dlsr(){
	return dlsr;
}

