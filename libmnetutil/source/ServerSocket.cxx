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


#ifdef HAVE_CONFIG_H
#include<config.h>
#endif

#ifdef WIN32
#include<winsock2.h>
#elif defined HAVE_ARPA_INET_H
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#endif

#include<libmnetutil/ServerSocket.h>
#include<libmnetutil/TCPSocket.h>
#include<libmnetutil/NetworkException.h>



#include<stdio.h>
#include <errno.h>

#include<iostream>
#include<errno.h>


ServerSocket::ServerSocket(int32_t domain, int32_t listenport){
	this->domain=domain;
	this->listen_port=listenport;
	fd = (int32_t)::socket(domain, SOCK_STREAM, IPPROTO_TCP);
	if (fd<0){
		throw new SocketFailed( errno );
	}
	int32_t on=1;
#ifdef WIN32
	setsockopt(fd,SOL_SOCKET,SO_REUSEADDR, (const char *) (&on),sizeof(on));
#else
	setsockopt(fd,SOL_SOCKET,SO_REUSEADDR, (void *) (&on),sizeof(on));
#endif

}


void ServerSocket::listen(struct sockaddr *saddr, int32_t sockaddr_length, int32_t backlog){
	
	if (bind(fd,saddr, sockaddr_length )!=0){
		throw new BindFailed( errno );
	}

	if (::listen(fd, backlog)!=0){
		throw new ListenFailed( errno );
	}
}

void ServerSocket::listen(string local_ip, int32_t local_port, int32_t backlog){
	struct sockaddr_in local_addr;

	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons((int16_t)local_port);
	local_addr.sin_addr.s_addr = inet_addr(local_ip.c_str());
	memset(&(local_addr.sin_zero), '\0', 8);

	if (bind(fd,(struct sockaddr*)&local_addr, sizeof(struct sockaddr))!=0){
		throw new BindFailed( errno );
	}

	if (::listen(fd, backlog)!=0){
		throw new ListenFailed( errno );
	}
}

StreamSocket *ServerSocket::accept(){
	int32_t cli;
	struct sockaddr sin;
	int32_t sinlen=sizeof(struct sockaddr);
	//sin = get_sockaddr_struct(sinlen);
	
#ifndef WIN32
	if ((cli=::accept(fd, &sin, (socklen_t*)&sinlen))<0){
#else
	if ((cli=(int32_t)::accept(fd, &sin, (int*)&sinlen))<0){
#endif
		perror("in ServerSocket::accept(): accept:");
	}
	return new TCPSocket(cli,&sin);
}

int32_t ServerSocket::getPort(){
		struct sockaddr_in addr;
		int sz = sizeof(addr);
#ifdef WIN32
		if (getsockname(fd, (struct sockaddr *)&addr, &sz)){
#else
		if (getsockname(fd, (struct sockaddr *)&addr, (socklen_t*)&sz)){
#endif
			throw new GetSockNameFailed( errno );
		}
		
		int32_t port2 = ntohs(addr.sin_port);
		return port2;
}

