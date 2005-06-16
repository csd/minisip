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

#ifdef _MSC_VER		// was: if defined WIN32 && not defined __CYGWIN__
#include<winsock2.h>
#else
#include<unistd.h>
#endif

#include<iostream>
using namespace std;

Socket::Socket(){

}

Socket::~Socket(){
	if (fd!=-1){
		close();
	}
}

int32_t Socket::getFd(){
	return fd;
}

int32_t Socket::getType(){
	return type;
}

void Socket::close(){
//#if defined WIN32 && not defined __CYGWIN__
#ifdef _MSC_VER
	closesocket(fd);
#else
	::close(fd);
#endif
	fd = -1;
}

