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
#include"TCPSocket.h"
#include"IPAddress.h"

using namespace std;

class ServerSocket : public Socket{

	public:
		ServerSocket(int32_t domain, int32_t listen_port);
		virtual StreamSocket *accept();

		
		/**
		 * get the port where the socket is listening to incoming
		 * connections.
		 * @return the port number
		 */
		int32_t getPort();

	protected:
		void listen(struct sockaddr *saddr, int32_t sockaddr_length, int32_t backlog);
		void listen(string local_ip, int32_t local_port, int32_t backlog);
		
	private:
		int32_t domain, listen_port;
};


#endif
