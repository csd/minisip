/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/
#ifdef HAVE_CONFIG_H
#include<config.h>
#endif

#ifdef WIN32
#include<winsock2.h>
#elif defined HAVE_NETINET_IN_H
#include<sys/socket.h>
#include<stdio.h>
//#include<netinet/tcp.h>
#include<netinet/in.h>
//#include<netdb.h>
#endif

#include<libmnetutil/UDPSocket.h>
#include<libmnetutil/IPAddress.h>
#include<libmnetutil/IP4Address.h>
#include<libmnetutil/IP6Address.h>
#include<libmnetutil/NetworkException.h>
#include<stdlib.h>
#include<errno.h>



#include<unistd.h>
#include<iostream>
#include<errno.h>

using namespace std;

UDPSocket::UDPSocket(bool use_ipv6, int32_t port){

	type = SOCKET_TYPE_UDP;
	this->use_ipv6 = use_ipv6;
	
	if ((fd = socket(use_ipv6? PF_INET6:PF_INET, SOCK_DGRAM, IPPROTO_UDP ))<0){
		throw new SocketFailed( errno );
	}
	int32_t on=1;
#ifndef WIN32
	setsockopt(fd,SOL_SOCKET,SO_REUSEADDR, (void *) (&on),sizeof(on));
#else
	setsockopt(fd,SOL_SOCKET,SO_REUSEADDR, (const char *) (&on),sizeof(on));
#endif
#ifndef WIN32
	if (use_ipv6){
		struct sockaddr_in6 addr;
		addr.sin6_family=PF_INET6;
		addr.sin6_port=htons(port);
		addr.sin6_addr=in6addr_any;
	
		if (bind(fd, (struct sockaddr *)&addr, sizeof(addr))!=0){
			throw new BindFailed( errno );
		}

	}else
#endif
	{
		struct sockaddr_in addr;
		addr.sin_family=PF_INET;
		addr.sin_port=htons(port);
		addr.sin_addr.s_addr=htonl(INADDR_ANY);
	
		if (bind(fd, (struct sockaddr *)&addr, sizeof(addr))!=0){
			throw new BindFailed( errno );
		}


	}
}
	
UDPSocket::~UDPSocket(){

}

int32_t UDPSocket::getPort(){
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

int32_t UDPSocket::sendTo(IPAddress &to_addr, int32_t port, const void *msg, int32_t len){
	if (use_ipv6 && ( to_addr.getType() != IP_ADDRESS_TYPE_V6)){
		cerr << "Error: trying to send to IPv6 address using IPv4 socket" << endl;
		throw new SendFailed( errno );
	}
	if (!use_ipv6 && (to_addr.getType() != IP_ADDRESS_TYPE_V4)){
		cerr << "Error: trying to send to IPv4 address using IPv6 socket" << endl;
		throw new SendFailed( errno );
	}
	
//	cerr << "DEBUG: sending UDP message of length "<< len <<" use_ipv6 is "<<use_ipv6<< endl; 

#ifndef WIN32
	return sendto(fd, msg, len, 0, to_addr.getSockaddrptr(port), to_addr.getSockaddrLength());
#else
	return sendto(fd, (const char*)msg, len, 0, to_addr.getSockaddrptr(port), to_addr.getSockaddrLength());
#endif
}

int32_t UDPSocket::recvFrom(void *buf, int32_t len, IPAddress *& from){
	struct sockaddr_in from4;
	int n, addr_len;
#ifndef WIN32
	if (use_ipv6){
		struct sockaddr_in6 from6;
		addr_len=sizeof(struct sockaddr_in6);
		n=recvfrom(fd, (char*)buf,len, 0, (struct sockaddr*) &from6, (socklen_t*)&addr_len);
//		from = new IP6Address(from6);
		from=NULL;
	}else
#endif
	{
		addr_len = sizeof(struct sockaddr_in);
#ifdef WIN32
		n=recvfrom(fd, (char*)buf,len, 0, (struct sockaddr*)&from4, &addr_len);
#else
		n=recvfrom(fd, (char*)buf,len, 0, (struct sockaddr*)&from4, (socklen_t*)&addr_len);
#endif
//		from = new IP4Address(from4);
		from=NULL;
	}
	return n;
}

int32_t UDPSocket::recv(void *buf, int32_t len){
#ifndef WIN32
	int32_t dummy=0;
	return recvfrom(fd,buf,len,0,NULL,(socklen_t*)&dummy);
#else
	return ::recv(fd,(char *)buf,len,0);
#endif

}
	
/*
ostream& operator<<(ostream& out, UDPSocket& s){
//	int32_t buf[1024*10];
//	int32_t n = s.read(buf,1024*10);
//	out.write(buf,n);
	return out;
}
*/

/*
UDPSocket& operator<<(UDPSocket& sock, string str){
//	sock.write(str);
	return sock;
}
*/

