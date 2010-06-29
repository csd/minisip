/*
  Copyright (C) 2007  Mikael Magnusson

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

#include<libmnetutil/SctpServerSocket.h>
#include<libmnetutil/SctpSocket.h>

using namespace std;

SctpServerSocket::SctpServerSocket( const IPAddress &addr ):
		ServerSocket( addr.getAddressFamily(),
			      SOCK_STREAM, IPPROTO_SCTP )
{
	type = MSOCKET_TYPE_SCTP;
}

SctpServerSocket *SctpServerSocket::create( int32_t listenport, bool useIpv6,
					  int32_t backlog )
{
	MRef<IPAddress*> addr;

	if( useIpv6 )
		addr = IPAddress::create("::", true);
	else
		addr = IPAddress::create("0.0.0.0", false);

	SctpServerSocket* sock = new SctpServerSocket( **addr );
	sock->listen( **addr, listenport, backlog );
	return sock;
}

MRef<StreamSocket *> SctpServerSocket::createSocket( int32_t fd_,
						    struct sockaddr *sa,
						    int32_t salen )
{
	return new SctpSocket( fd_, sa, salen );
}
