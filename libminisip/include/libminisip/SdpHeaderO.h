/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  
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
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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

#include"SdpHeader.h"

using namespace std;

class SdpHeaderO : public SdpHeader{
	public:
		SdpHeaderO(string buildFrom);
		SdpHeaderO(string username, string session_id, string version, string net_type, string addr_type, string addr);
		virtual ~SdpHeaderO();
		
		virtual std::string getMemObjectType(){return "SdpHeaderO";}

		string getUsername();
		void setUsername(string username);

		string getSessionId();
		void setSessionId(string session_id);

		string getVersion();
		void setVersion(string version);

		string getNetType();
		void setNetType(string netType);

		string getAddrType();
		void setAddrType(string addrType);

		string getAddr();
		void setAddr(string addr);

		virtual string getString();

	private:
		string username;
		string session_id;
		string version;
		string net_type;
		string addr_type;
		string addr;
};

#endif
