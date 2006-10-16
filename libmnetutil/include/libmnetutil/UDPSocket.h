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

#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include<libmnetutil/libmnetutil_config.h>

#include<libmnetutil/DatagramSocket.h>
#include<libmnetutil/IPAddress.h>


class LIBMNETUTIL_API UDPSocket : public DatagramSocket {
	public:
	
		UDPSocket( int32_t port=0, bool use_ipv6=false );
//		UDPSocket( int32_t port );
		
		virtual ~UDPSocket();

		virtual std::string getMemObjectType() const {return "UDPSocket";}

		int32_t sendTo(const IPAddress &to_addr, int32_t port, const void *msg, int32_t len);
		
		int32_t recvFrom(void *buf, int32_t len, MRef<IPAddress *>& from, int32_t &port);
		
		int32_t recv(void *buf, int32_t len);

		bool setLowDelay();

	private:
		bool initUdpSocket( bool use_ipv6, int32_t port );
		bool use_ipv6;
};
#endif
