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

#ifndef IP6SERVERSOCKET_H
#define IP6SERVERSOCKET_H

#include<libmnetutil/libmnetutil_config.h>

#include"ServerSocket.h"
#include"TCPSocket.h"

class LIBMNETUTIL_API IP6ServerSocket : public ServerSocket {
	public:
		IP6ServerSocket(int32_t listenport, int32_t backlog=25);

		virtual std::string getMemObjectType() const {return "IP6ServerSocket";};

		//inherited: TCPSocket *accept();
		virtual struct sockaddr *getSockaddrStruct(int32_t &ret_length);
		virtual MRef<TCPSocket *> createSocket(int32_t fd, struct sockaddr_in6 *saddr);
};
#endif
