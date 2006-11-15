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

#include<libmnetutil/Socket.h>
#include<libmnetutil/NetworkException.h>
#include<libmnetutil/IPAddress.h>

#if defined _MSC_VER || defined __MINGW32__	// was: if defined WIN32 && not defined __CYGWIN__
#include<winsock2.h>
# define USE_WIN32_API
#else
#include<unistd.h>
#include <sys/socket.h>
#endif

#include<iostream>
using namespace std;

#ifdef USE_WIN32_API
typedef int socklen_t;
#endif

Socket::Socket(){

}

Socket::~Socket(){
	if (fd!=-1){
		this->close();
	}
}

int32_t Socket::getFd(){
	return fd;
}

int32_t Socket::getType(){
	return type;
}

int Socket::getAddressFamily()
{
	struct sockaddr_storage sa;
	socklen_t salen = sizeof(sa);

	if( getsockname(fd, (struct sockaddr*)&sa, &salen) < 0 ){
		return -1;
	}

	return sa.ss_family;
}

int32_t Socket::getPort(){
	MRef<IPAddress *> addr = getLocalAddress();
	int32_t port2 = addr->getPort();
	return port2;
}

MRef<IPAddress *> Socket::getLocalAddress() const{
	struct sockaddr_storage sa;
	socklen_t sz = sizeof(sa);
	if (getsockname(fd, (struct sockaddr *)&sa, &sz)){
		throw GetSockNameFailed( errno );
	}

	MRef<IPAddress *> addr = IPAddress::create((struct sockaddr*)&sa, sz);
	return addr;
}



void Socket::close( void ){
//#if defined WIN32 && not defined __CYGWIN__
#ifdef USE_WIN32_API
	closesocket(fd);
#else
	::close(fd);
#endif
	fd = -1;
}

