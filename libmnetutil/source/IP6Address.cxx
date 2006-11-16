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

#include<libmnetutil/IP6Address.h>

#ifdef WIN32
#include<winsock2.h>
#include<ws2tcpip.h>
const struct in6_addr in6addr_any = {{IN6ADDR_ANY_INIT}};
#elif defined HAVE_NETDB_H
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<netinet/in.h>
#endif

#include<libmnetutil/NetworkException.h>
#include<libmutil/merror.h>

#include<stdio.h>

#ifndef _MSC_VER
#include<unistd.h>
#endif

#include<iostream>

#include<exception>
#include<typeinfo>

using namespace std;

#ifdef HAVE_GETNAMEINFO
string buildAddressString(const struct sockaddr *sa, socklen_t salen)
{
	char buf[INET6_ADDRSTRLEN+1] = "";

	memset(buf, 0, sizeof(buf));
	int res = getnameinfo( sa, salen, buf, sizeof(buf),
			       NULL, 0, NI_NUMERICHOST );

	if( res < 0 ){
		throw ResolvError( res );
	}
	else {
		return buf;
	}
}
#else
#error getnameinfo required when enabling ipv6
#endif	// HAVE_GETNAMEINFO

/*
**
** Alg:	1. Convert address to binary format
**	2. 
**	3.
*/
IP6Address::IP6Address(string addr){
	sockaddress = new sockaddr_in6;
	type = IP_ADDRESS_TYPE_V6;

	setAddressFamily(AF_INET6);
	setProtocolFamily(PF_INET6);
#ifndef WIN32
	hostent *hp= gethostbyname2(addr.c_str(), AF_INET6);	
#else
	hostent *hp= gethostbyname(addr.c_str());	
#endif
	if (!hp){ //throw host not found exception here
#ifdef DEBUG_OUTPUT
		cerr << "ERROR:(in IP6Address) Unknown host: <" << addr.c_str() <<">"<< endl;
#endif
		throw HostNotFound( addr );

	}
	unsigned short *ip = (unsigned short *)hp->h_addr;
	for (int32_t i=0; i<8; i++)
		num_ip[i] = ip[i];

	//bzero((char*)&sockaddress, sizeof(sockaddress));
	memset(sockaddress, '\0', sizeof(*sockaddress));
	sockaddress->sin6_family=PF_INET6;
	sockaddress->sin6_port=0;
	//bcopy(hp->h_addr,(char *)&sockaddress.sin6_addr, hp->h_length);
	memcpy(&sockaddress->sin6_addr,hp->h_addr, hp->h_length);

 	ipaddr = buildAddressString((struct sockaddr*)sockaddress, sizeof(struct sockaddr_in6));
}

IP6Address::IP6Address(struct sockaddr_in6 * addr){
	type = IP_ADDRESS_TYPE_V6;
	setAddressFamily(AF_INET6);
	setProtocolFamily(PF_INET6);
	sockaddress = new sockaddr_in6;
	memcpy(sockaddress, addr, sizeof(sockaddr_in6));
	for (int32_t i=0; i<8; i++)

#ifdef _MSC_VER
		num_ip[i] = ((sockaddr_in6 *)sockaddress)->sin6_addr.u.Word[i];
#else
#ifdef WIN32	// Should be __CYGWIN__?
		num_ip[i] = ((sockaddr_in6 *)sockaddress)->sin6_addr._S6_un._S6_u16[i];
#else
#ifdef DARWIN
		num_ip[i] = ((sockaddr_in6 *)sockaddress)->sin6_addr.__u6_addr.__u6_addr16[i];
#else
		num_ip[i] = ((sockaddr_in6 *)sockaddress)->sin6_addr.in6_u.u6_addr16[i];
#endif
#endif
#endif

	ipaddr = buildAddressString((struct sockaddr*)addr, sizeof(struct sockaddr_in6));
}


IP6Address::IP6Address(const IP6Address& other){
	type = IP_ADDRESS_TYPE_V6;
	setAddressFamily(AF_INET6);
	setProtocolFamily(PF_INET6);
	ipaddr = other.ipaddr;
	memcpy( num_ip, other.num_ip, sizeof(num_ip) );
	sockaddress = new sockaddr_in6;
	memcpy(sockaddress, other.sockaddress, sizeof(sockaddr_in6));
}


IP6Address::~IP6Address(){
	delete sockaddress;
}

struct sockaddr *IP6Address::getSockaddrptr(int32_t port) const{
	sockaddress->sin6_port = htons( (unsigned short)port );
	return (struct sockaddr *) sockaddress;
}

int32_t IP6Address::getSockaddrLength() const{
	return sizeof(struct sockaddr_in6);
}

string IP6Address::getString() const{
	return ipaddr;
}

int32_t IP6Address::getPort() const
{
	return ntoh16(sockaddress->sin6_port);
}

void IP6Address::connect(Socket &socket, int32_t port){
	struct sockaddr_in6 sin;
	memcpy(&sin, sockaddress, sizeof(sin));
	sin.sin6_port = htons( (unsigned short)port );

	if (::connect(socket.getFd(), (struct sockaddr *)&sin, sizeof(sin)) < 0){
		merror("(in IP6Address::connect()): connect");
		socket.close();
		throw ConnectFailed( errno );
	}

}

ostream& operator<<(ostream& out, const IP6Address &a){
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

bool IP6Address::operator ==(const IP6Address &i) const{
	uint8_t j;

	for(j=0; j<8; j++){
		if(num_ip[j] != i.num_ip[j]){
			return false;
		}
	}
	return true;
}

bool IP6Address::operator ==(const IPAddress &i) const{

	try{
		const IP6Address &i6 = dynamic_cast<const IP6Address&>(i);
		return (*this == i6);
	}
	catch(std::bad_cast &){
		// Comparing IPv6 and IPv4 addresses
		return false;
	}
}

