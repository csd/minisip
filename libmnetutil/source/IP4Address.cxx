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

#include<libmnetutil/IP4Address.h>
#include<libmnetutil/NetworkException.h>

#include<iostream>

#ifdef HAVE_NETINET_IN_H
#include<netdb.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#endif

#ifdef WIN32
#include"winsock2.h"

// borrowed from tcpdump
int
inet_aton(const char *cp, struct in_addr *addr)
{
  addr->s_addr = inet_addr(cp);
  return (addr->s_addr == INADDR_NONE) ? 0 : 1;
}
#endif


#include<stdio.h>
#include<unistd.h>
#include<assert.h>
#include<libmutil/itoa.h>
#include<strings.h>

IP4Address::IP4Address(struct sockaddr_in *sin){
        sockaddress = new sockaddr_in;
	type = IP_ADDRESS_TYPE_V4;
	setAddressFamily(AF_INET);
	setProtocolFamily(PF_INET);
	
	unsigned char *ip_bytes = (unsigned char *)&sin->sin_addr;
	
	//bcopy((char *)&sockaddress, (char *)sin, sizeof(struct sockaddr_in));
	//bcopy((char *)num_ip, (char *)&sin->sin_addr, 4);
	memcpy( sin, sockaddress, sizeof(struct sockaddr_in));
	memcpy(&sin->sin_addr,num_ip, 4);
} 

uint32_t IP4Address::getBinaryIP(){
	uint32_t bip;
//	bcopy((char *)&bip, (char *)&(sockaddress.sin_addr), 4);
	
//	bcopy((char *)&(sockaddress.sin_addr), (char*)&bip, 4);
	memcpy(&bip, &(sockaddress->sin_addr), 4);
	bip = htonl(bip);
	return bip;
}


/*
**
** Alg:	1. Convert address to binary format
**	2. 
**	3.
*/
IP4Address::IP4Address(string addr){
        sockaddress = new sockaddr_in;
	type = IP_ADDRESS_TYPE_V4;
	ipaddr = addr;

	setAddressFamily(AF_INET);
	setProtocolFamily(PF_INET);
	unsigned char *ip;
	unsigned long int ip_data;
	if (inet_aton(addr.c_str(),(struct in_addr *)&ip_data)){
		ip = (unsigned char *)&ip_data;
	}else{

		
#ifndef WIN32
		struct hostent *hp= gethostbyname2(ipaddr.c_str(), AF_INET);	
#else
		struct hostent *hp= gethostbyname(ipaddr.c_str());	
//		struct hostent *hp= gethostbyaddr(ipaddr.c_str(), 4, AF_INET);	

		if (WSAGetLastError() != 0) {
			if (WSAGetLastError() == 11001)
				throw new HostNotFound( addr );
		}
#endif

		
		if (!hp){ //throw host not found exception here
			throw new HostNotFound( addr );
		}
		ip = (unsigned char *)hp->h_addr;
		assert(hp->h_length==4);
	}
	
	for (int32_t i=0; i<4; i++)
		num_ip[i] = ip[i];

	memset(sockaddress, '\0', sizeof(sockaddress));
	sockaddress->sin_family=AF_INET;
	memcpy(&(sockaddress->sin_addr), ip, sizeof(ip_data));
	sockaddress->sin_port=0;
	unsigned char *ip_bytes = (unsigned char *)ip;
}

 struct sockaddr * IP4Address::getSockaddrptr(int32_t port){
	sockaddress->sin_port=htons(port);
	return (sockaddr *)sockaddress;
}

int32_t IP4Address::getSockaddrLength(){
	return sizeof(struct sockaddr_in);
}

string IP4Address::getString(){
	return ipaddr;
}

void IP4Address::connect(Socket &socket, int32_t port){

	unsigned char *ip;
	unsigned long int ip_data;
	if (inet_aton(ipaddr.c_str(),(struct in_addr *)&ip_data)){
		ip = (unsigned char *)&ip_data;
	}else{
	
#ifndef WIN32
		struct hostent *hp= gethostbyname2(ipaddr.c_str(), AF_INET);	
#else
		struct hostent *hp= gethostbyname(ipaddr.c_str());	
#endif
		if (!hp){ //throw host not found exception here
			throw new HostNotFound( ipaddr );
		}
		ip = (unsigned char *)hp->h_addr;
		assert(hp->h_length==4);
	}
	
	struct sockaddr_in sin;
	memset(&sin, '\0', sizeof(sin));
	sin.sin_family = AF_INET;
	memcpy(&sin.sin_addr, ip, sizeof(ip_data));
	sin.sin_port = htons(port);
	
	int error = ::connect(socket.getFd(), (struct sockaddr *)&sin, sizeof(sin));
	if (error < 0){
		perror("connect");
		socket.close();
		throw new ConnectFailed( error );
	}

}

std::ostream& operator<<(std::ostream& out, IP4Address &a){
	out << a.ipaddr;
	
	unsigned char *ip = (unsigned char*)&a.num_ip;
	cerr << " (";

	for (int32_t i=0; i<4; i++){
		if (i>0)
			cerr << ".";

		cerr << (unsigned)ip[i];

	}
	cerr << ")";

	return out;
}

