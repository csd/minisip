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

#ifndef IP6ADDRESS_H
#define IP6ADDRESS_H

#include "IPAddress.h"

#include<iostream>

struct sockaddr_in6;

class IP6Address : public IPAddress{
	public:
		IP6Address(std::string addr);
		IP6Address(const IP6Address&);
		IP6Address(struct sockaddr *);
		~IP6Address();
		
		virtual std::string getString();
		virtual void connect(Socket &socket, int32_t port);
		friend std::ostream& operator<<(std::ostream&, IP6Address &a);

		virtual struct sockaddr *getSockaddrptr(int32_t port=0);
		virtual int32_t getSockaddrLength();

		virtual bool operator ==(const IP6Address &i6) const;
		virtual bool operator ==(const IPAddress &i) const;

		virtual IP6Address * clone() const{return new IP6Address(*this);};
		
	private:
		std::string ipaddr;
		unsigned short num_ip[8];
		struct sockaddr_in6 * sockaddress;
};

#endif
