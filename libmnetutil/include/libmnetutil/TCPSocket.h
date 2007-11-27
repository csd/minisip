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

#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include<libmnetutil/libmnetutil_config.h>

#include<libmnetutil/StreamSocket.h>
#include<libmnetutil/IPAddress.h>

class LIBMNETUTIL_API TCPSocket : public StreamSocket {
	public:
		TCPSocket(int32_t fd, sockaddr * addr, int32_t addr_len);
		
		TCPSocket(std::string addr,int32_t port=0);
		TCPSocket(const IPAddress &addr,int32_t port=0);

		TCPSocket(TCPSocket &sock);
		virtual ~TCPSocket();

		virtual std::string getMemObjectType() const {return "TCPSocket";};

//		void flush();

		void useNoDelay(bool noDelay);

		friend std::ostream& operator<<(std::ostream&, TCPSocket&);

	private:
		void initTCPSocket(const IPAddress &ipaddress, int32_t port);
};

TCPSocket& operator<<(TCPSocket& sock, std::string str);
#endif
