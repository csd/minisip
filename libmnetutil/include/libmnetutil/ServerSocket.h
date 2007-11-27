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

#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include<libmnetutil/libmnetutil_config.h>

#include<libmnetutil/TCPSocket.h>
#include<libmnetutil/IPAddress.h>


class LIBMNETUTIL_API ServerSocket : public Socket {
	public:
		virtual MRef<StreamSocket *> accept();
		void listen( const IPAddress &addr, int32_t listenPort,
			     int32_t backlog );

	protected:
		ServerSocket();
		ServerSocket( int32_t domain, int32_t type, int32_t protocol );

		/**
		 * Used in accept to create a socket for
		 * the incoming connection.
		 */
		virtual MRef<StreamSocket *> createSocket( int32_t sd,
							   struct sockaddr *sa,
							   int32_t salen )=0;
};
#endif
