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

#ifndef IP4ADDRESS_H
#define IP4ADDRESS_H

#include <libmnetutil/IPAddress.h>

#ifdef _MSC_VER
typedef unsigned int uint32_t;
#else
#include<stdint.h>
#endif
struct sockaddr_in;

class IP4Address : public IPAddress{
	public:
		IP4Address(std::string addr);
		IP4Address(struct sockaddr_in *sin);
		IP4Address(const IP4Address&);
		~IP4Address();
		
		uint32_t getBinaryIP();
		
		virtual std::string getString();
		virtual void connect(Socket &socket, int32_t port);
		friend std::ostream& operator<<(std::ostream&, IP4Address &a);
	
		virtual struct sockaddr * getSockaddrptr(int32_t port=0);
		virtual int32_t getSockaddrLength();
		
		virtual bool operator ==(const IP4Address &i4) const;
		virtual bool operator ==(const IPAddress &i) const;

		virtual IP4Address * clone() const{ return new IP4Address(*this);};
		
	private:
		std::string ipaddr;
		struct sockaddr_in * sockaddress;
		uint32_t numIp;
};

#endif
