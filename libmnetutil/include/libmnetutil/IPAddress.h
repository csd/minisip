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

#ifndef IPADDRESS_H
#define IPADDRESS_H

#include<libmnetutil/libmnetutil_config.h>

#include<libmnetutil/Socket.h>

#define IP_ADDRESS_TYPE_V4      0
#define IP_ADDRESS_TYPE_V6      1

#include<string>
#include<libmutil/MemObject.h>

#ifdef WIN32
	#include <winsock2.h>
	#ifdef HAVE_WS2TCPIP_H
		# include<ws2tcpip.h>
	#endif
#elif defined HAVE_NETINET_IN_H
	#include<sys/types.h>
	#include<netinet/in.h>
	#include<sys/socket.h>
#endif

class LIBMNETUTIL_API IPAddress : public MObject{
	public:
		virtual ~IPAddress();

		int32_t getType() const { return type; };

		int getAddressFamily() const;
		int getProtocolFamily() const;
		virtual int32_t getPort() const=0;
		virtual std::string getString() const=0;

		virtual void connect(Socket &s, int32_t port) const=0;

		virtual struct sockaddr* getSockaddrptr(int32_t port=0) const=0;
		virtual int32_t getSockaddrLength() const=0;

		virtual bool operator ==(const IPAddress &i) const =0;
		virtual MRef<IPAddress *> clone() const =0;

		static MRef<IPAddress *> create(sockaddr * addr, int32_t addr_len);

		/**
		 * Tries to create IPv4 or IPv6 address. First v4 is tried
		 * and if that fails, v6 is tried.
		 * NOTE: The method returns NULL if the host can not be
		 * resolved, and does not throw an exception.
		 */
		static MRef<IPAddress *> create(const std::string &addr);

		static MRef<IPAddress *> create(const std::string &addr, bool use_ipv6);

		static bool isNumeric(const std::string &addr);

	protected:
		void setAddressFamily(int af);
		void setProtocolFamily(int pf);
		int32_t type;

	private:
		int protocol_family;
		int address_family;
};
#endif
