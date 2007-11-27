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


#include<config.h>

#include<libmnetutil/ServerSocket.h>

#ifdef WIN32
#include<winsock2.h>
#elif defined HAVE_ARPA_INET_H
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#endif

#include<libmutil/merror.h>
#include<libmnetutil/NetworkException.h>

#include<stdio.h>

#include<iostream>

using namespace std;

#ifdef WIN32
typedef int socklen_t;
#endif

ServerSocket::ServerSocket(){
}

ServerSocket::ServerSocket(int32_t domain, int32_t type, int32_t protocol){
	fd = (int32_t)::socket( domain, type, protocol );
	if (fd<0){
		throw SocketFailed( errno );
	}
	int32_t on=1;
#ifdef WIN32
	setsockopt(fd,SOL_SOCKET,SO_REUSEADDR, (const char *) (&on),sizeof(on));
#else
	setsockopt(fd,SOL_SOCKET,SO_REUSEADDR, (void *) (&on),sizeof(on));
#endif

#ifdef IPV6_V6ONLY
	if( domain == PF_INET6 )
		setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on));
#endif
}


void ServerSocket::listen( const IPAddress &addr, int32_t port, int32_t backlog){
	socklen_t salen = addr.getSockaddrLength();
	struct sockaddr *sa = addr.getSockaddrptr( port );
	
	if (bind(fd, sa, salen )!=0){
		throw BindFailed( errno );
	}

	if (::listen(fd, backlog)!=0){
		throw ListenFailed( errno );
	}
}

MRef<StreamSocket *>ServerSocket::accept(){
	int32_t cli;
	struct sockaddr_storage sin;
	socklen_t sinlen=sizeof(sin);
	//sin = get_sockaddr_struct(sinlen);

	if ((cli=(int32_t)::accept(fd, (struct sockaddr*)&sin, &sinlen))<0){
		merror("in ServerSocket::accept(): accept:");
	}
	
	return createSocket( cli, (struct sockaddr*)&sin, sinlen );
}
