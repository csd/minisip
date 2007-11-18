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

#ifndef IP4ADDRESS_H
#define IP4ADDRESS_H

#include<libmnetutil/libmnetutil_config.h>

#include <libmnetutil/IPAddress.h>

#include<libmutil/mtypes.h>
#include<libmutil/dbg.h>

#include<string>

struct sockaddr_in;

class LIBMNETUTIL_API IP4Address : public IPAddress {
	public:
		IP4Address(std::string addr);
		IP4Address(struct sockaddr_in *sin);
		IP4Address(const IP4Address&);
		~IP4Address();

		uint32_t getBinaryIP();

		virtual int32_t getPort() const;
		virtual std::string getString() const;
		virtual void connect(Socket &socket, int32_t port) const;
		friend std::ostream& operator<<(std::ostream&, IP4Address &a);
		friend Dbg& operator<<(Dbg&, IP4Address &a);

		virtual struct sockaddr * getSockaddrptr(int32_t port=0) const;
		virtual int32_t getSockaddrLength() const;

		virtual bool operator ==(const IP4Address &i4) const;
		virtual bool operator ==(const IPAddress &i) const;
		
		virtual MRef<IPAddress *> clone() const;

	private:
		std::string ipaddr;
		struct sockaddr_in * sockaddress;
		uint32_t numIp;
};
#endif
