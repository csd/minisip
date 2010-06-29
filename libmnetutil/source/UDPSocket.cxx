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

#include<libmnetutil/UDPSocket.h>

#if defined _MSC_VER || __MINGW32__
#include<winsock2.h>
# define USE_WIN32_API
#elif defined HAVE_NETINET_IN_H
#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
//#include<netinet/tcp.h>
#include<netinet/in.h>
//#include<netdb.h>
#endif

#include<libmnetutil/IPAddress.h>
#include<libmnetutil/NetworkException.h>
#include<libmutil/massert.h>
#include<stdlib.h>

#ifndef WIN32
#include<unistd.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
#endif

#ifdef HAVE_WS2TCPIP_H
# include <ws2tcpip.h>
#endif

#include<iostream>

#ifdef USE_WIN32_API
typedef int socklen_t;
#endif

using namespace std;

bool UDPSocket::initUdpSocket( bool use_ipv6_, int32_t port ) {
	type = MSOCKET_TYPE_UDP;
	this->use_ipv6 = use_ipv6_;
	
#ifdef _MSC_VER
	massert(sizeof(SOCKET)==4);
#endif

	if ((fd = (int32_t)socket(use_ipv6? PF_INET6:PF_INET, SOCK_DGRAM, IPPROTO_UDP ))<0){
		#ifdef _WIN32_WCE //wince STLPort has not errno defined ... wcecompat has, though ... 
		throw SocketFailed( -1 );
		#else
		throw SocketFailed( errno );
		#endif
	}

// I believe the following code should be removed, or enabled using a
// parameter. I think we should fail if the port is busy by default. 
//	int32_t on=1;
//#ifndef WIN32
//	setsockopt(fd,SOL_SOCKET,SO_REUSEADDR, (void *) (&on),sizeof(on));
//#else
//	setsockopt(fd,SOL_SOCKET,SO_REUSEADDR, (const char *) (&on),sizeof(on));
//#endif


#ifdef HAVE_IPV6
	if (use_ipv6){
		struct sockaddr_in6 addr;
		int32_t on=1;

#ifdef IPV6_V6ONLY
		setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on));
#endif

		memset(&addr, 0, sizeof(addr));
		addr.sin6_family=PF_INET6;
		addr.sin6_port=htons( (unsigned short)port );
		addr.sin6_addr=in6addr_any;
	
		if (bind(fd, (struct sockaddr *)&addr, sizeof(addr))!=0){
			throw BindFailed( errno );
		}

	}else
#endif // defined(HAVE_IPV6) && defined(HAVE_STRUCT_SOCKADDR_IN6)
	{
		struct sockaddr_in addr;
		addr.sin_family=PF_INET;
		addr.sin_port = htons( (unsigned short)port );
		addr.sin_addr.s_addr=htonl(INADDR_ANY);
	
		if (bind(fd, (struct sockaddr *)&addr, sizeof(addr))!=0){
			throw BindFailed( errno );
		}
	}
	return true;
}

UDPSocket::UDPSocket( int32_t port, bool use_ipv6_ ) {
	initUdpSocket( use_ipv6_, port );
}

/*
UDPSocket::UDPSocket( bool use_ipv6, int32_t port ){
	initUdpSocket( use_ipv6, port );
}
*/
	
UDPSocket::~UDPSocket(){

}

int32_t UDPSocket::sendTo(const IPAddress &to_addr, int32_t port, const void *msg, int32_t len){
	if (use_ipv6 && ( to_addr.getType() != IP_ADDRESS_TYPE_V6)){
		cerr << "Error: trying to send to IPv4 address using IPv6 socket" << endl;
		throw SendFailed( errno );
	}
	if (!use_ipv6 && (to_addr.getType() != IP_ADDRESS_TYPE_V4)){
		cerr << "Error: trying to send to IPv6 address using IPv4 socket" << endl;
		throw SendFailed( errno );
	}
	
//	cerr << "DEBUG: sending UDP message of length "<< len <<" use_ipv6 is "<<use_ipv6<< endl; 

	return sendto(fd, (const char*)msg, len, 0, to_addr.getSockaddrptr(port), to_addr.getSockaddrLength());
}

int32_t UDPSocket::recvFrom(void *buf, int32_t len, MRef<IPAddress *>& from, int32_t &port){
	struct sockaddr_storage sa;
	socklen_t sa_len = sizeof(sa);
	int n;

	n=recvfrom(fd, (char *)buf,len, 0, (struct sockaddr*)&sa, &sa_len);
	from = IPAddress::create((struct sockaddr*)&sa, sa_len);
	port = from->getPort();
	return n;
}

int32_t UDPSocket::recv(void *buf, int32_t len){
#ifndef WIN32
	socklen_t dummy=0;
	return recvfrom(fd,buf,len,0,NULL,&dummy);
#else
	return ::recv(fd,(char *)buf,len,0);
#endif

}

bool UDPSocket::setLowDelay(){
#ifdef USE_WIN32_API
	cerr << "Warning: setting IPTOS_LOWDELAY is not implemented for the Microsoft Visual compiler!"<< endl;
	return false;
#else
	int tos = IPTOS_LOWDELAY;
	if (setsockopt(fd, IPPROTO_IP, IP_TOS, &tos, sizeof(tos))){
		cerr << "WARNING: Could not set type of service to be time chritical"<< endl;
		return false;
	}
	return true;
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

