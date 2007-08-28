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

#include<libminisip/signaling/sdp/SdpHeader.h>
#include<libmnetutil/IPAddress.h>

#include<string>

class LIBMINISIP_API SdpHeaderC : public SdpHeader{
	public:
		SdpHeaderC(std::string buildFrom);
		SdpHeaderC(std::string netType, std::string addrType, std::string addr);
		virtual ~SdpHeaderC();
		
		virtual std::string getMemObjectType() const {return "SdpHeaderC";}

		const std::string &getNetType() const;
		void setNetType(std::string netType);

		const std::string &getAddrType() const;
		void setAddrType(std::string addrType);

		const std::string &getAddr() const;
		void setAddr(std::string addr);

		virtual std::string getString();

		MRef<IPAddress*> getIPAdress();

	private:
		std::string netType;
		std::string addrType;
		std::string addr;

		MRef<IPAddress*> ipAddr;
};

#endif
