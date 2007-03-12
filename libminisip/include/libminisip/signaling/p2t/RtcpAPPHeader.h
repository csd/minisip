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

#ifndef RTCPAPPHEADER_H
#define RTCPAPPHEADER_H

#include<libminisip/libminisip_config.h>

#include<vector>
#include<string>

using namespace std;

/**
 * the header fields for a RTCP APP packet.
 * <p>
 * <p><b>RTCP APP packet:</b><br>
 * The structure of the RTCP APP packet is shown below. The definitions of the fields
 * are found in RFC 3550, section 6.6.
 * <p>
 * <img src=material/RtcpAPP.gif>
 * <p>
 * This class is used by <code>RtcpAPP</code> to store the header fields (all fields
 * above the application-dependent data).
 * @author Florian Maurer, florian.maurer@floHweb.ch
 */

class LIBMINISIP_API RtcpAPPHeader{

	public:
		/**
		 * Constructor. Sets the version=2 and payloadtype=204 (according RFC).
		 */
		RtcpAPPHeader();
		
		/**
		 * set the version value
		 * @param v version
		 */
		void setVersion(int v);
		
		/**
		 * set the padding flag
		 * @param p padding
		 */
		void setPadding(int p);

		/**
		 * set the subtype value
		 * @param st subtype
		 */
		void setSubtype(int st);
		
		/**
		 * set the payload type value
		 * @param pt payload type
		 */
		void setPayloadtype(int pt);
		
		/**
		 * set the length
		 * @param length the length in 4-bytes-word without the first 4 bytes.
		 */
		void setLength(int length);
		
		/**
		 * set the SSRC value
		 * @param ssrc SSRC
		 */
		void setSSRC(int ssrc);
		
		/**
		 * set the name
		 * @param name name defining the set of APP packets
		 */
		void setName( std::string name);
		
		/**
		 * get the version value
		 * @return version value
		 */
		int getVersion();
		
		/**
		 * get the padding flag
		 * @return padding flag
		 */
		int getPadding();
		
		/**
		 * get the subtype value
		 * @return subtype value
		 */
		int getSubtype();
		
		/**
		 * get the payloadtype value
		 * @return payloadtype value
		 */
		int getPayloadtype();
		
		/**
		 * get the length of the header
		 * @return length in 4-byte-words without the first word.
		 */
		int getLength();
		
		/**
		 * get the SSRC value
		 * @return SSRC value
		 */
		int getSSRC();
		
		/**
		 * get the name
		 * @return name
		 */
		 std::string getName();

		/**
		 * get the size of the header fields
		 * @return size in bytes of the header
		 */
		int size();
		
		/**
		 * get the bytes
		 * @return the header according the RFC
		 */
		char *getBytes();
	
	protected:
		///version value
		int version;
		///padding value
		int padding;
		///subtype value
		int subtype;
		///payloadtype value
		int payloadtype;
		///length value
		int length;
		///SSRC
		int SSRC;
		///name
		char name[4];

};

#endif

