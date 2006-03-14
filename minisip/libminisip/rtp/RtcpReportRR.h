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

#ifndef RTCPREPORTRR_H
#define RTCPREPORTRR_H

#include"RtcpReport.h"
#include"RtcpReportReceptionBlock.h"
#include<vector>
#include<config.h>

using namespace std;

class RtcpReportRR : public RtcpReport{
	public:
		
		RtcpReportRR(unsigned sender_ssrc);
		
		RtcpReportRR(void * build_from, int max_length);
		
		virtual ~RtcpReportRR();
		
//		virtual vector<unsigned char> get_packet_bytes();

		int size();

#ifdef DEBUG_OUTPUT
		virtual void debug_print();
#endif

		int get_n_report_blocks();
		RtcpReportReceptionBlock &get_reception_block(int i);

		void add_reception_block(RtcpReportReceptionBlock block);
		
	private:
		unsigned sender_ssrc;
		//-------------
		vector<RtcpReportReceptionBlock>reception_blocks;
};

#endif
