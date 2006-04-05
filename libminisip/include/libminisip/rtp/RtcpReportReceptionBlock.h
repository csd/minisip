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

#ifndef RTCPREPORTRECEPTIONBLOCK_H
#define RTCPREPORTRECEPTIONBLOCK_H

#include<libminisip/libminisip_config.h>

class LIBMINISIP_API RtcpReportReceptionBlock{
	public:
		RtcpReportReceptionBlock(unsigned ssrc);
		
		RtcpReportReceptionBlock(void *buildfrom, int max_length);
		int size();
#ifdef DEBUG_OUTPUT
		void debug_print();
#endif

		void set_fraction_lost(unsigned n);
		unsigned get_fraction_lost();

		void set_cumulative_n_lost(unsigned n);
		unsigned get_cumulative_n_lost();

		void set_seq_high(unsigned i);
		unsigned get_seq_high();

		void set_jitter(unsigned i);
		unsigned get_jitter();

		void set_last_sr(unsigned i);
		unsigned get_last_sr();

		void set_dlsr(unsigned i);
		unsigned get_dlsr();

	private:
		unsigned ssrc;
		unsigned fraction_lost;
		unsigned cumulative_n_lost;
		unsigned seq_high;
		unsigned jitter;
		unsigned last_sr;
		unsigned dlsr;

};

#endif
