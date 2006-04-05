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

#ifndef RTCPREPORT_H
#define RTCPREPORT_H

#include<libminisip/libminisip_config.h>

#include<vector>

#define PACKET_TYPE_SR 200
#define PACKET_TYPE_RR 201
#define PACKET_TYPE_SDES 202
#define PACKET_TYPE_BYE 203
#define PACKET_TYPE_APP 204

#define PACKET_TYPE_XR 207

class LIBMINISIP_API RtcpReport{
	public:
		//		RtcpReport(void *buildfrom, int max_length);
		RtcpReport(unsigned packet_type);

		static RtcpReport *build_from(void *buildfrom, int max_length);

		virtual ~RtcpReport(){};
//		virtual vector<unsigned char> get_packet_bytes()=0;

		virtual int size() = 0;
		
#ifdef DEBUG_OUTPUT
		virtual void debug_print()=0;
#endif
	protected:
		void parse_header(void *build_from, int max_length);
		
		unsigned version;
		unsigned padding;
		unsigned rc_sc;
		unsigned packet_type;
		unsigned length;
	private:

	
};

#endif
