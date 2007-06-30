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

#include<libmnetutil/IPAddress.h>
#include<libmnetutil/IP4Address.h>
#ifdef HAVE_IPV6
#include<libmnetutil/IP6Address.h>
#endif
#include<libmnetutil/NetworkException.h>

#ifdef WIN32
# include<winsock2.h>
# include<ws2tcpip.h>
#elif defined HAVE_NETDB_H
# include<sys/types.h>
# include<sys/socket.h>
# include<arpa/inet.h>
#endif
#ifndef HAVE_INET_PTON
# include<inet_pton.h>
#endif

using namespace std;

IPAddress::~IPAddress(){

}

int IPAddress::getAddressFamily() const{
	return address_family;
}

void IPAddress::setAddressFamily(int af){
	address_family=af;
}

int IPAddress::getProtocolFamily() const{
	return protocol_family;
}

void IPAddress::setProtocolFamily(int pf){
	protocol_family=pf;
}

MRef<IPAddress *> IPAddress::create( sockaddr * addr, int32_t addr_len ){
	if( addr->sa_family == AF_INET &&
	    addr_len >= (int32_t) sizeof(struct sockaddr_in)){
		return new IP4Address( (sockaddr_in *)addr );
	}
#ifdef HAVE_IPV6
	else if( addr->sa_family == AF_INET6 &&
		 addr_len >= (int32_t)sizeof(struct sockaddr_in6)){
		return new IP6Address( (sockaddr_in6 *)addr );
	}
#endif
	else
		throw UnknownAddressFamily(addr->sa_family);
}

MRef<IPAddress *> IPAddress::create(const string &addr){
	try {
		return new IP4Address( addr );
	} catch( HostNotFound & ){
	}
#ifdef HAVE_IPV6
	try {
		return new IP6Address( addr );
	} catch( HostNotFound & ){
	}
#endif
	return NULL;
}

MRef<IPAddress *> IPAddress::create(const std::string &addr, bool use_ipv6){
	try {
		if( !use_ipv6 )
			return new IP4Address( addr );
#ifdef HAVE_IPV6
		else
			return new IP6Address( addr );
#endif
	} catch( HostNotFound & ){
	}

	return NULL;
}

bool IPAddress::isNumeric(const string &addr){
	char tmp[sizeof(struct in6_addr)];

	if( (inet_pton(AF_INET, addr.c_str(), &tmp) > 0) ||
	    (inet_pton(AF_INET6, addr.c_str(), &tmp) > 0) ){
		return true;
	}

	return false;
}
