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

#include<libminisip/media/rtp/XRVoIPReportBlock.h>

#include<libmutil/massert.h>
//#include<netinet/in.h>
#include<iostream>

using namespace std;

/*
struct voip_metrics_report{
	unsigned block_type:8;
	unsigned reserved:8;
	unsigned block_length:16;
	
	unsigned loss_rate:8;
	unsigned discard_rate:8;
	unsigned burst_density:8;
	unsigned gap_density:8;

	unsigned burst_duration:16;
	unsigned gap_duration:16;
	
	unsigned round_trip_delay:16;	
	unsigned end_system_delay:16;
	
	unsigned signal_power:8;
	unsigned RERL:8;
	unsigned noise_level:8;
	unsigned Gmin:8;

	unsigned R_factor:8;
	unsigned ext_R_factor:8;
	unsigned MOS_LQ:8;
	unsigned MOS_CQ:8;

	unsigned RX_config:8;
	unsigned JB_nominal:8;
	unsigned JB_maximum:8;
	unsigned JB_abs_max:8;	
};
*/

XRVoIPReportBlock::XRVoIPReportBlock(void *build_from, int max_length){
	//struct voip_metrics_report *vmrp = (struct voip_metrics_report *)build_from;
	uint8_t * bytearray = (uint8_t *)build_from;
	massert(bytearray[1]==0);
	massert(max_length>=7);
		
	//this->block_type = vmrp->block_type;
	this->block_type = bytearray[0];
	massert(this->block_type==VOIP_METRICS_REPORT);
	//this->block_length = ntohs(vmrp->block_length);
	this->block_length = U16_AT( bytearray + 2 );
	massert(this->block_length==6);
	//this->loss_rate = vmrp->loss_rate;
	this->loss_rate = bytearray[4];
	//this->discard_rate = vmrp->discard_rate;
	this->discard_rate = bytearray[5];
	//this->burst_density = vmrp->burst_density;
	this->burst_density = bytearray[6];
	this->gap_density = bytearray[7];
	//this->gap_density = vmrp->gap_density;
	//this->burst_duration = ntohs(vmrp->burst_duration);
	this->burst_duration = U16_AT( bytearray + 8 );
	//this->gap_duration = ntohs(vmrp->gap_duration);
	this->gap_duration = U16_AT( bytearray + 10 );
	//this->round_trip_delay = ntohs(vmrp->round_trip_delay);
	this->round_trip_delay = U16_AT( bytearray + 12 );
	//this->end_system_delay = ntohs(vmrp->end_system_delay);
	this->end_system_delay = U16_AT( bytearray + 14 );
	//this->signal_power = vmrp->signal_power;
	this->signal_power = bytearray[16];
	//this->RERL = vmrp->RERL;
	this->RERL = bytearray[17];
	//this->noise_level = vmrp->noise_level;
	this->noise_level = bytearray[18];
	//this->Gmin = vmrp->Gmin;
	this->Gmin = bytearray[19];
	//this->R_factor = vmrp->R_factor;
	this->R_factor = bytearray[20];
	//this->ext_R_factor = vmrp->ext_R_factor;
	this->ext_R_factor = bytearray[21];
	//this->MOS_LQ = vmrp->MOS_LQ;
	this->MOS_LQ = bytearray[22];
	//this->MOS_CQ = vmrp->MOS_CQ;
	this->MOS_CQ = bytearray[23];
	//this->RX_config = vmrp->RX_config;
	this->RX_config = bytearray[24];
	//this->JB_nominal = vmrp->JB_nominal;
	this->JB_nominal = bytearray[25];
	//this->JB_maximum = vmrp->JB_maximum;
	this->JB_maximum = bytearray[26];
	//this->JB_abs_max = vmrp->JB_abs_max;
	this->JB_abs_max = bytearray[27];
}

#ifdef DEBUG_OUTPUT
void  XRVoIPReportBlock::debug_print(){
	cerr.setf( ios::hex, ios::basefield );
	cerr <<"\tblock_type=0x"<<this->block_type<<endl;
	cerr <<"\tblock_length=0x"<< this->block_length << endl;
	cerr <<"\tloss_rate=0x"<<this->loss_rate << endl;
	cerr <<"\tdiscard_rate=0x"<<this->discard_rate << endl;
	cerr <<"\tburst_density=0x"<<this->burst_density << endl;
	cerr <<"\tgap_density=0x"<<this->gap_density << endl;
	cerr <<"\tburst_duration=0x"<<this->burst_duration << endl;
	cerr <<"\tgap_duration=0x"<<this->gap_duration << endl;
	cerr <<"\tround_trip_delay=0x"<<this->round_trip_delay << endl;
	cerr <<"\tend_system_delay=0x"<<this->end_system_delay << endl;
	cerr <<"\tsignal_power=0x"<<this->signal_power << endl;
	cerr <<"\tRERL=0x"<<this->RERL << endl;
	cerr <<"\tnoise_level=0x"<<this->noise_level <<endl;
	cerr <<"\tGmin=0x"<<this->Gmin << endl;
	cerr <<"\tR_factor=0x"<<this->R_factor << endl;
	cerr <<"\text_R_factor=0x"<<this->ext_R_factor << endl;
	cerr <<"\tMOS_LQ=0x"<<this->MOS_LQ << endl;
	cerr <<"\tMOS_CQ=0x"<<this->MOS_CQ << endl;
	cerr <<"\tRX_config=0x"<<this->RX_config << endl;
	cerr <<"\tJB_nominal=0x"<<this->JB_nominal << endl;
	cerr <<"\tJB_maximum=0x"<<this->JB_maximum << endl;
	cerr <<"\tJB_abs_max=0x"<<this->JB_abs_max << endl;
	
	cerr.setf( ios::dec, ios::basefield );
}
#endif

int XRVoIPReportBlock::size(){
	return 28;
}

