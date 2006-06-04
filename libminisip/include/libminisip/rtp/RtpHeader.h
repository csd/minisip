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

#ifndef RTPHEADER_H
#define RTPHEADER_H

#include<libminisip/libminisip_config.h>

#include<vector>

class LIBMINISIP_API RtpHeader{

	public:
		RtpHeader();
		void setVersion(int v);
		void setExtension(int x);
		int getExtension();
		void setCSRCCount(int cc);
		void setMarker(int m);
		bool getMarker();
		void setPayloadType(int pt);
		int getPayloadType();
		void setSeqNo(uint16_t seq_no);
		uint16_t getSeqNo();
		void setTimestamp(uint32_t timestamp);
		uint32_t getTimestamp();
		void setSSRC(uint32_t ssrc);
		uint32_t getSSRC();
		void addCSRC(int csrc);

#ifdef TCP_FRIENDLY
		void setRttEstimate(uint32_t rtt){tcpFriendlyMode=true; rttestimate=rtt;}
		uint32_t getRttEstimate(){return rttestimate;}
		
		void setSendingTimestamp(uint32_t ts){tcpFriendlyMode=true; sending_timestamp=ts;}
		uint32_t getSendingTimestamp(){return sending_timestamp;}
#endif

#ifdef DEBUG_OUTPUT
		void printDebug();
#endif

		int size();
		char *getBytes();

		int CSRC_count;
		int version;
		int extension;
		int marker;
		int payload_type;
		uint16_t sequence_number;
		uint32_t timestamp;
		uint32_t SSRC;
		std::vector<int> CSRC; 
		
	private:
#ifdef TCP_FRIENDLY 
		//code to support the variable bandwidth experiments in the
		//thesis by David Tlahuetl
		bool tcpFriendlyMode;
		uint32_t sending_timestamp;
		uint32_t rttestimate;
#endif

};

#endif

