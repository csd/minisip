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

/*
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 */

#ifndef IP6ADDRESS_H
#define IP6ADDRESS_H

#include<libmnetutil/libmnetutil_config.h>

#include "IPAddress.h"

#include<iostream>

struct sockaddr_in6;

class LIBMNETUTIL_API IP6Address : public IPAddress {
	public:
		IP6Address(std::string addr);
		IP6Address(const IP6Address&);
		IP6Address(struct sockaddr_in6 *);
		~IP6Address();

		virtual int32_t getPort() const;
		virtual std::string getString() const;
		virtual void connect(Socket &socket, int32_t port) const;
		friend std::ostream& operator<<(std::ostream&, const IP6Address &a);

		virtual struct sockaddr *getSockaddrptr(int32_t port=0) const;
		virtual int32_t getSockaddrLength() const;

		virtual bool operator ==(const IP6Address &i6) const;
		virtual bool operator ==(const IPAddress &i) const;

		virtual MRef<IPAddress *> clone() const;

	private:
		std::string ipaddr;
		unsigned short num_ip[8];
		struct sockaddr_in6 * sockaddress;
};
#endif
