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
 * 	SdpHeaderO.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#ifndef SDPHEADERO_H
#define SDPHEADERO_H

#include<libminisip/libminisip_config.h>

#include<libminisip/signaling/sdp/SdpHeader.h>


class LIBMINISIP_API SdpHeaderO : public SdpHeader{
	public:
		SdpHeaderO(std::string buildFrom);
		SdpHeaderO(std::string username, std::string session_id, std::string version, std::string net_type, std::string addr_type, std::string addr);
		virtual ~SdpHeaderO();
		
		virtual std::string getMemObjectType() const {return "SdpHeaderO";}

		std::string getUsername();
		void setUsername(std::string username);

		std::string getSessionId();
		void setSessionId(std::string session_id);

		std::string getVersion();
		void setVersion(std::string version);

		std::string getNetType();
		void setNetType(std::string netType);

		std::string getAddrType();
		void setAddrType(std::string addrType);

		std::string getAddr();
		void setAddr(std::string addr);

		virtual std::string getString();

	private:
		std::string username;
		std::string session_id;
		std::string version;
		std::string net_type;
		std::string addr_type;
		std::string addr;
};

#endif
