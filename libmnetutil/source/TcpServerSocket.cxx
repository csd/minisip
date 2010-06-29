/*
  Copyright (C) 2007 Mikael Magnusson

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
 * Authors: Mikael Magnusson <mikma@users.sourceforge.net>
 */

#include<config.h>

#include<libmnetutil/TcpServerSocket.h>

#ifdef WIN32
#include<winsock2.h>
#elif defined HAVE_NETDB_H
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#endif

#ifndef _MSC_VER
#include<unistd.h>
#endif

using namespace std;

TcpServerSocket::TcpServerSocket( int32_t domain ):
		ServerSocket( domain, SOCK_STREAM, IPPROTO_TCP )
{
	type = MSOCKET_TYPE_TCP;
}

TcpServerSocket *TcpServerSocket::create( int32_t listenport, bool useIpv6,
					  int32_t backlog )
{
	MRef<IPAddress*> addr;

	if( useIpv6 )
		addr = IPAddress::create("::", true);
	else
		addr = IPAddress::create("0.0.0.0", false);

	int32_t domain = addr->getAddressFamily();

	TcpServerSocket* sock =
		new TcpServerSocket( domain );
	sock->listen( **addr, listenport, backlog );
	return sock;
}

MRef<StreamSocket *> TcpServerSocket::createSocket( int32_t fd_,
						    struct sockaddr *sa,
						    int32_t salen )
{
	return new TCPSocket( fd_, sa, salen );
}

