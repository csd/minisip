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

#ifndef RTPMONITOR_H
#define RTPMONITOR_H

#include"RtpPacket.h"
#include"RtcpPacket.h"
#include<config.h>

class RtpMonitor{
	public:
		RtpMonitor(unsigned ssrc);
		~RtpMonitor();
		void packet_sent(RtpPacket &pack);
		void packet_received(RtpPacket &pack);
		void packet_discarded(RtpPacket &pack);
		
		void packet_generated(unsigned rtp_seq_no);
		

		RtcpPacket *generate_rtcp_packet();

		string get_string();
		
	private:
		char *seq_no_lookback;
		int seq_lookback_size;
		int lookback_used;
		unsigned ssrc;
		unsigned n_received;
		unsigned n_sent;
		unsigned n_discarded;
///		RtcpReportRR receiver_report;
///		RtcpReportSR sender_report;

		//RtcpReportSDES sdes; generated dynamically

///		RtcpReportSenderInfo sender_info;
///
		//		RtcpReport
		
		int get_number_lost();

		int get_fraction_lost();
		int get_fraction_discard();
		int get_burst_count();
		int get_burst_density();
		
};

#endif
