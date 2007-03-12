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

#ifndef XRVOIPREPORTBLOCK_H 
#define XRVOIPREPORTBLOCK_H 

#include<libminisip/libminisip_config.h>

#include<libminisip/media/rtp/XRReportBlock.h>

class LIBMINISIP_API XRVoIPReportBlock : public XRReportBlock{
	public:
		XRVoIPReportBlock(void *build_from, int max_length);
		
#ifdef DEBUG_OUTPUT
		virtual void debug_print();
#endif
		virtual int size();

	private:	
		unsigned block_type;
		unsigned reserved;
		unsigned block_length;

		unsigned loss_rate;
		unsigned discard_rate;
		unsigned burst_density;
		unsigned gap_density;

		unsigned burst_duration;
		unsigned gap_duration;

		unsigned round_trip_delay;	
		unsigned end_system_delay;

		unsigned signal_power;
		unsigned RERL;
		unsigned noise_level;
		unsigned Gmin;

		unsigned R_factor;
		unsigned ext_R_factor;
		unsigned MOS_LQ;
		unsigned MOS_CQ;

		unsigned RX_config;
		unsigned JB_nominal;
		unsigned JB_maximum;
		unsigned JB_abs_max;	

};


#endif
