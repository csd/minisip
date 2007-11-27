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

#ifndef STREAMSOCKET_H
#define STREAMSOCKET_H

#include<libmnetutil/libmnetutil_config.h>

#include<libmnetutil/Socket.h>

#include<string>

#include<libmnetutil/IPAddress.h>

class LIBMNETUTIL_API StreamSocket : public Socket {
	public:
		StreamSocket();
		virtual ~StreamSocket();
		virtual int32_t write(std::string);
		virtual int32_t write(const void *buf, int32_t count);
		virtual int32_t read(void *buf, int32_t count);

		// Buffer of the received data;
		std::string received;

		bool matchesPeer(const IPAddress& address, int32_t port) const;
		bool matchesPeer(const std::string &address, int32_t port) const;

		MRef<IPAddress *> getPeerAddress();
		int32_t getPeerPort() const;
	protected:
		MRef<IPAddress *> peerAddress;
		std::string remoteHostUnresolved;
		int32_t peerPort;

};
#endif
