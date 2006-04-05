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
 * 	SdpHeaderC.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#ifndef SDPHEADERC_H
#define SDPHEADERC_H

#include<libminisip/libminisip_config.h>

#include<libminisip/sdp/SdpHeader.h>

#include<string>

class SdpHeaderC : public SdpHeader{
	public:
		SdpHeaderC(std::string buildFrom);
		SdpHeaderC(std::string netType, std::string addrType, std::string addr);
		virtual ~SdpHeaderC();
		
		virtual std::string getMemObjectType(){return "SdpHeaderC";}

		std::string getNetType();
		void setNetType(std::string netType);

		std::string getAddrType();
		void setAddrType(std::string addrType);

		std::string getAddr();
		void setAddr(std::string addr);

		virtual std::string getString();

	private:
		std::string netType;
		std::string addrType;
		std::string addr;
};

#endif
