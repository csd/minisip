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

#include<netinet/in.h>


#include<libmnetutil/IPAddress.h>
#include<libmnetutil/IP4Address.h>
#include<libmnetutil/IP6Address.h>

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

IPAddress * IPAddress::create( sockaddr * addr ){
	if( ((sockaddr_in*)addr)->sin_family == AF_INET ){
		return new IP4Address( (sockaddr_in *)addr );
	}
	else if( ((sockaddr_in*)addr)->sin_family == AF_INET6 ){
		return new IP6Address( addr );
	}
	// FIXME exception
	else return NULL;
}
