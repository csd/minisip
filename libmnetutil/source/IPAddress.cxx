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
#include<libmnetutil/IP6Address.h>
#include<libmnetutil/NetworkException.h>

using namespace std;

IPAddress::~IPAddress(){

}

int IPAddress::getAddressFamily(){
	return address_family;
}

void IPAddress::setAddressFamily(int af){
	address_family=af;
}

int IPAddress::getProtocolFamily(){
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
	else if( addr->sa_family == AF_INET6 &&
		 addr_len >= (int32_t)sizeof(struct sockaddr_in6)){
		return new IP6Address( (sockaddr_in6 *)addr );
	}
	// FIXME exception
	else return NULL;
}

MRef<IPAddress *> IPAddress::create(const string &addr){
	try {
		return new IP4Address( addr );
	} catch( HostNotFound & ){
	}

	try {
		return new IP6Address( addr );
	} catch( HostNotFound & ){
	}

	return NULL;
}
