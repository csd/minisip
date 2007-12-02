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

#include<libmnetutil/IP4Address.h>

/*
#ifdef _WIN32_WCE
	#include<wcecompat/stdlib.h>
	#include<wcecompat/stdio.h>
	#ifndef _MSC_VER
		#include<wcecompat/strings.h>
	#endif
#endif
*/

#include<iostream>

#include<exception>
#include<typeinfo>

#ifdef HAVE_NETDB_H
	#include<netdb.h>
#endif
#ifdef HAVE_NETINET_IN_H
	#include<netinet/in.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
	#include<sys/socket.h>
#endif
#ifdef HAVE_ARPA_INET_H
	#include<arpa/inet.h>
#endif

#ifndef HAVE_INET_ATON
	#include<inet_aton.h>
#endif

#include<libmnetutil/NetworkException.h>

#include<libmutil/merror.h>
#include<libmutil/massert.h>
#include<libmutil/stringutils.h>

using namespace std;

static std::string toStr(uint32_t numIp){
	unsigned char ip[4];
	ip[0] = (unsigned char)(numIp >> 24);
	ip[1] = (unsigned char)(numIp >> 16);
	ip[2] = (unsigned char)(numIp >>  8);
	ip[3] = (unsigned char)(numIp);
	std::string ret="(";
	for (int32_t i=0; i<4; i++){
		if (i>0)
			ret=ret+".";
		ret = ret + itoa( (unsigned)ip[i] );
	}
	ret = ret+")";
	return ret;
}

std::ostream& operator<<(std::ostream& out, IP4Address &a){
	return out<<toStr(a.numIp);
}


Dbg& operator<<(Dbg& out, IP4Address &a){
	return out << toStr(a.numIp);
}


IP4Address::IP4Address(struct sockaddr_in *sin){
	sockaddress = new sockaddr_in;
	type = IP_ADDRESS_TYPE_V4;
	setAddressFamily(AF_INET);
	setProtocolFamily(PF_INET);
	
	memcpy(sockaddress, sin, sizeof(struct sockaddr_in));
	numIp = ntoh32(sin->sin_addr.s_addr);
	ipaddr = string(inet_ntoa(in_addr((sin->sin_addr))));
} 

uint32_t IP4Address::getBinaryIP(){
	return numIp;
}

IP4Address::IP4Address(string addr){
	sockaddress = new sockaddr_in;
	type = IP_ADDRESS_TYPE_V4;
	ipaddr = addr;

	setAddressFamily(AF_INET);
	setProtocolFamily(PF_INET);
	struct in_addr ip_data;
	if (inet_aton(addr.c_str(),&ip_data)){
		numIp = ntoh32(ip_data.s_addr);
	}else{

		//unsigned char *ip;
		
#ifndef WIN32
		struct hostent *hp= gethostbyname2(ipaddr.c_str(), AF_INET);	
#else
		struct hostent *hp= gethostbyname(ipaddr.c_str());	
//		struct hostent *hp= gethostbyaddr(ipaddr.c_str(), 4, AF_INET);	

		if (WSAGetLastError() != 0) {
			if (WSAGetLastError() == 11001)
				throw HostNotFound( addr );
		}
#endif

		
		if (!hp){
			throw HostNotFound( addr );
		}
		
		numIp = ntoh32(*((uint32_t*)(hp->h_addr)));

		massert(hp->h_length==4);
		#ifdef DEBUG_OUTPUT
		mdbg("net") << "IP4Address(string): " << *this << endl;
		#endif
	}

	memset(sockaddress, '\0', sizeof(struct sockaddr_in));
	sockaddress->sin_family=AF_INET;
	sockaddress->sin_addr.s_addr = hton32(numIp);
	sockaddress->sin_port=0;
}

IP4Address::IP4Address(const IP4Address& other){
	type = IP_ADDRESS_TYPE_V4;
	setAddressFamily(AF_INET);
	setProtocolFamily(PF_INET);
	ipaddr = other.ipaddr;
	numIp = other.numIp;
	sockaddress = new sockaddr_in;
	memcpy(sockaddress, other.sockaddress, sizeof(struct sockaddr_in));
}

IP4Address::~IP4Address(){
	delete sockaddress;
}

MRef<IPAddress *> IP4Address::clone() const{ 
	return new IP4Address(*this); 
}

struct sockaddr * IP4Address::getSockaddrptr(int32_t port) const{
	sockaddress->sin_port=hton16(port);
	return (sockaddr *)sockaddress;
}

int32_t IP4Address::getSockaddrLength() const{
	return sizeof(struct sockaddr_in);
}

string IP4Address::getString() const{
	return ipaddr;
}

int32_t IP4Address::getPort() const
{
	return ntoh16(sockaddress->sin_port);
}

void IP4Address::connect(Socket &socket, int32_t port) const{

	unsigned char *ip;
	unsigned long int ip_data;
	if (inet_aton(ipaddr.c_str(),(struct in_addr *)((void*)&ip_data) )){
		ip = (unsigned char *)&ip_data;
	}else{
	
#ifndef WIN32
		struct hostent *hp= gethostbyname2(ipaddr.c_str(), AF_INET);	
#else
		struct hostent *hp= gethostbyname(ipaddr.c_str());	
#endif
		if (!hp){ //throw host not found exception here
			throw HostNotFound( ipaddr );
		}
		ip = (unsigned char *)hp->h_addr;
		massert(hp->h_length==4);
	}
	
	struct sockaddr_in sin;
	memset(&sin, '\0', sizeof(sin));
	sin.sin_family = AF_INET;
	memcpy(&sin.sin_addr, ip, sizeof(ip_data));
	sin.sin_port = htons( (unsigned short)port );
	
	int error = ::connect(socket.getFd(), (struct sockaddr *)&sin, sizeof(sin));
	if (error < 0){
		int failedErrno = errno;
		merror("connect");
		socket.close();
		throw ConnectFailed( failedErrno );
	}

}

bool IP4Address::operator ==(const IP4Address &i4) const{
	return this->numIp == i4.numIp;
}

bool IP4Address::operator ==(const IPAddress &i) const{

	try{
		const IP4Address &i4 = dynamic_cast<const IP4Address&>(i);
		return (*this == i4);
	}
	catch(std::bad_cast &){
		// Comparing IPv6 and IPv4 addresses
		return false;
	}
}
