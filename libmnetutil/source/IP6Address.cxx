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


#include<libmnetutil/IP6Address.h>
#include<libmnetutil/NetworkException.h>
#ifdef HAVE_NETDB_H
#include<sys/socket.h>
#include<netdb.h>
#endif

#ifdef WIN32
#include<winsock2.h>
#include<ws2tcpip.h>
const struct in6_addr in6addr_any = {{IN6ADDR_ANY_INIT}};
#endif
#include<stdio.h>
#include<unistd.h>

#include<iostream>

using namespace std;

/*
**
** Alg:	1. Convert address to binary format
**	2. 
**	3.
*/
IP6Address::IP6Address(string addr){
        sockaddress = new sockaddr_in6;
	type = IP_ADDRESS_TYPE_V6;
	ipaddr = addr;

	setAddressFamily(AF_INET6);
	setProtocolFamily(PF_INET6);
#ifndef WIN32
	hostent *hp= gethostbyname2(ipaddr.c_str(), AF_INET6);	
#else
	hostent *hp= gethostbyname(ipaddr.c_str());	
#endif
	if (!hp){ //throw host not found exception here
#ifdef DEBUG_OUTPUT
		cerr << "ERROR:(in IP6Address) Unknown host: <" << ipaddr.c_str() <<">"<< endl;
#endif
		throw new HostNotFound( ipaddr );

	}
	unsigned short *ip = (unsigned short *)hp->h_addr;
	for (int32_t i=0; i<8; i++)
		num_ip[i] = ip[i];

	//bzero((char*)&sockaddress, sizeof(sockaddress));
	memset(sockaddress, '\0', sizeof(sockaddress));
	sockaddress->sin6_family=PF_INET6;
	sockaddress->sin6_port=0;
	//bcopy(hp->h_addr,(char *)&sockaddress.sin6_addr, hp->h_length);
	memcpy(&sockaddress->sin6_addr,hp->h_addr, hp->h_length);
}


struct sockaddr *IP6Address::getSockaddrptr(int32_t port){
	sockaddress->sin6_port = htons(port);
	return (struct sockaddr *) sockaddress;
}

int32_t IP6Address::getSockaddrLength(){
	return sizeof(struct sockaddr_in6);
}

string IP6Address::getString(){
	return "not_implemented";
}

void IP6Address::connect(Socket &socket, int32_t port){
#ifndef WIN32
	struct hostent *hp = gethostbyname2(ipaddr.c_str(), AF_INET6);
#else
	struct hostent *hp = gethostbyname(ipaddr.c_str());
#endif
	if (!hp){ //throw host not found exception here
#ifdef DEBUG_OUTPUT
		cerr << "ERROR:(in IP6Address::connect) Unknown host: " << ipaddr << endl;
#endif

		throw new HostNotFound( ipaddr );
	}
	
	struct sockaddr_in6 sin;
	//bzero((char*)&sin, sizeof(sin));
	memset(&sin,'\0', sizeof(sin));
	sin.sin6_family = AF_INET6;
	//bcopy(hp->h_addr, (char *)&sin.sin6_addr, hp->h_length);
	memcpy( &sin.sin6_addr,hp->h_addr, hp->h_length);
	sin.sin6_port = htons(port);
	
	if (::connect(socket.getFd(), (struct sockaddr *)&sin, sizeof(sin)) < 0){
		perror("(in IP6Address::connect()): connect");
		socket.close();
		throw new ConnectFailed( errno );
	}

}

ostream& operator<<(ostream& out, IP6Address &a){
	out << a.ipaddr;
	
	unsigned short *ip = (unsigned short*)&a.num_ip;
	out << " (";
	out.setf( ios::hex , ios::basefield );
	for (int32_t i=0; i<8; i++){
		if (i>0)
			out << ":";
		out << (unsigned short)(ntohs(ip[i]));
	}
	out.setf( ios::dec , ios::basefield );
	out << ")";
	
	return out;
}

