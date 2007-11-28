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

#include<libmnetutil/StreamSocket.h>
#include<libmnetutil/IPAddress.h>

#ifdef _MSC_VER
#	include<io.h>
#else
#	include<unistd.h>
#endif

using namespace std;

StreamSocket::StreamSocket(){}

StreamSocket::~StreamSocket(){
}

bool StreamSocket::matchesPeer(const IPAddress& address, int32_t port) const{
	#ifdef DEBUG_OUTPUT
// 	cerr << "StreamSocket:matchesPeer: 1 - " << address.getString() << ":" << port << endl;
// 	cerr << "StreamSocket:matchesPeer: 2 - " << getPeerAddress()->getString() << ":" << getPeerPort() << endl;
	#endif
	return **peerAddress == address && port == peerPort;
}

bool StreamSocket::matchesPeer(const string &address, int32_t port) const{
	#ifdef DEBUG_OUTPUT
// 	cerr << "StreamSocket:matchesPeer: 1 - " << address.getString() << ":" << port << endl;
// 	cerr << "StreamSocket:matchesPeer: 2 - " << getPeerAddress()->getString() << ":" << getPeerPort() << endl;
	#endif
	if (remoteHostUnresolved.size()>0){
		return remoteHostUnresolved==address && port==peerPort;
	}else{
		return **peerAddress == **(IPAddress::create(address)) && port==peerPort;
	}
}

MRef<IPAddress *> StreamSocket::getPeerAddress(){
	return peerAddress;
}

int32_t StreamSocket::getPeerPort() const{
	return peerPort;
}

int32_t StreamSocket::write(string data){
#ifdef _MSC_VER
	return ::_write(fd, data.c_str(), (unsigned int)data.length());
#else
	return ::send(fd, data.c_str(), data.length(), 0);
#endif
}

int32_t StreamSocket::write(const void *buf, int32_t count){
	return ::write(fd, buf, count);
}

int32_t StreamSocket::read(void *buf, int32_t count){
	return ::recv(fd, (char*)buf, count, 0);
}
