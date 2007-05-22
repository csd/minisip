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

#include<libmnetutil/IP6ServerSocket.h>

#ifdef WIN32
#include<winsock2.h>
#include<ws2tcpip.h>
#elif defined HAVE_NETDB_H
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#endif

#include<libmnetutil/ServerSocket.h>



#include<stdio.h>

#ifndef _MSC_VER
#include<unistd.h>
#endif


IP6ServerSocket::IP6ServerSocket(int32_t listenport, int32_t backlog): ServerSocket(PF_INET6,listenport){
	struct sockaddr_in6 sin;
	//bzero((char*)&sin, sizeof(sin));
	memset(&sin, '0', sizeof(sin));
	sin.sin6_family = AF_INET6;
	sin.sin6_addr=in6addr_any;
	sin.sin6_port = htons( (unsigned short)listenport );
	this->listen((struct sockaddr *)&sin,sizeof(sin),backlog);	 
}

struct sockaddr *IP6ServerSocket::getSockaddrStruct(int32_t &ret_length){
	ret_length=sizeof(struct sockaddr_in6);
	struct sockaddr_in6 *sin = (struct sockaddr_in6*)malloc(sizeof(struct sockaddr_in6));
	return (struct sockaddr *)sin;
}

MRef<TCPSocket *> IP6ServerSocket::createSocket(int32_t fd_, struct sockaddr_in6 *saddr){
	return new TCPSocket(fd_,(struct sockaddr*)saddr, sizeof(struct sockaddr_in6));
}

