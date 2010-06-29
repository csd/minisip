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

#include<libmnetutil/NetworkException.h>
#include<libmnetutil/SctpSocket.h>

using namespace std;

SctpSocket::SctpSocket( const IPAddress &addr, int32_t port ){
	type = MSOCKET_TYPE_SCTP;
	peerAddress = addr.clone();
	peerPort = port;

	if ((fd = (int32_t)::socket(addr.getProtocolFamily(), SOCK_STREAM, IPPROTO_SCTP ))<0){
		throw SocketFailed( errno );
	}
	int32_t on=1;
#ifndef WIN32
	setsockopt(fd,SOL_SOCKET,SO_REUSEADDR, (void *) (&on),sizeof(on));
#else
	setsockopt(fd,SOL_SOCKET,SO_REUSEADDR, (const char *) (&on),sizeof(on));
#endif

	addr.connect(*this, port);	
}

SctpSocket::SctpSocket(int32_t fd_, struct sockaddr * addr, int32_t addr_len){
	type = MSOCKET_TYPE_SCTP;
	fd = fd_;
	peerAddress = IPAddress::create( addr, addr_len );
	peerPort = peerAddress->getPort();
}

SctpSocket::~SctpSocket(){
}
