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

#ifndef IPADDRESS_H
#define IPADDRESS_H

#include<libmnetutil/Socket.h>
#include<sys/types.h>
#include<string>

#define IP_ADDRESS_TYPE_V4 	0
#define IP_ADDRESS_TYPE_V6	1

class IPAddress{
	public:
		virtual ~IPAddress();

		int32_t getType(){return type;};
		
		int getAddressFamily();
		int getProtocolFamily();
		virtual std::string getString()=0;

		virtual void connect(Socket &s, int32_t port)=0;

		virtual struct sockaddr* getSockaddrptr(int32_t port=0)=0;
		virtual int32_t getSockaddrLength()=0;
		
	protected:
		void setAddressFamily(int af);
		void setProtocolFamily(int pf);
		int32_t type;
		
	private:
		int protocol_family;
		int address_family;
};


#endif
