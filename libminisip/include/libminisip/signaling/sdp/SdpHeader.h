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

/* Name
 * 	SdpHeader.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/


#ifndef SDPHEADER_H
#define SDPHEADER_H

#include<libminisip/libminisip_config.h>

#include<libmutil/MemObject.h>

#define SDP_HEADER_TYPE_A	0
#define SDP_HEADER_TYPE_C	1
#define SDP_HEADER_TYPE_M	2
#define SDP_HEADER_TYPE_O	3
#define SDP_HEADER_TYPE_S	4
#define SDP_HEADER_TYPE_T	5
#define SDP_HEADER_TYPE_V	6
#define SDP_HEADER_TYPE_I	7

#include<string>

class LIBMINISIP_API SdpHeader : public MObject{
	public:
                SdpHeader(int type, int priority);
		
		/**
		 * A SDP header is a one line string. the getString method
		 * returns this string and all header subclasses MUST
		 * implement it.
		 * 
		 * @return The string that will be one line header in the 
		 * SDP message
		 */
		virtual std::string getString()=0;

		/**
		 * The headers in an SDP message will be ordered depending
		 * based on a priority value. A lower value means higher
		 * priority. The value MUST be in the interval [0..15]. 
		 * Headers with any other value will be discarded and not
		 * included in the SDP message.
		 * 
		 * @return An integer that determines where in the
		 *         list of headers this header will be in the
		 *         SDP message. 		 */
		int32_t getPriority() const {return priority;};

		/**
		 * All headers have a type identifier (integer) that is 
		 * defined in SdpHeader.h. This method returns this
		 * identifier, and can be used to determine which 
		 * of the subclasses an object is.
		 * @return Type id that is defined in SdpHeader.h
		 */
		int32_t getType(){return type;};
		
		void set_priority(int32_t prio){this->priority=prio;}	
	private:
		int32_t type;
		int32_t priority;
};

#endif
