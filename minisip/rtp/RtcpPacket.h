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

#ifndef RTCPPACKET_H
#define RTCPPACKET_H



#include<vector>
#include"RtcpReport.h"

#include<config.h>

#define RTCP_TYPE_SR		0
#define RTCP_TYPE_RR		1
#define RTCP_TYPE_SDES		2
#define RTCP_TYPE_BYE		3
#define RTCP_TYPE_APP		4

using namespace std;

class RtcpPacket{
	public:
		RtcpPacket();
		RtcpPacket(void *buildfrom, int length);
		~RtcpPacket();
		vector<RtcpReport *> &get_reports();
		void add_report(RtcpReport *report);

#ifdef DEBUG_OUTPUT
		void debug_print();
#endif
		int get_type();
	private:
		vector<RtcpReport *> reports;
		int type;
		
};

#endif
