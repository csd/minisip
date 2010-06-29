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

#include<libmnetutil/TCPSocket.h>

#ifdef WIN32
#	include<winsock2.h>
#elif defined HAVE_NETINET_TCP_H
#	include<sys/types.h>
#	include<sys/socket.h>
#	include<netinet/tcp.h>
#	include<netinet/in.h>
#endif

#include<stdlib.h>
#include<stdio.h>
#ifdef _MSC_VER
#	include<io.h>
#	define dup		::_dup
#endif


#include<libmnetutil/IPAddress.h>
#include<libmnetutil/IP4Address.h>
#include<libmnetutil/NetworkException.h>


#ifndef _MSC_VER
#include<unistd.h>
#endif

#include<iostream>

using namespace std;



TCPSocket::TCPSocket(string addr, int32_t port){
	remoteHostUnresolved = addr;
	MRef<IPAddress *> tmp = IPAddress::create(addr);
	initTCPSocket( **tmp, port );
}

TCPSocket::TCPSocket(const IPAddress &ipaddress, int32_t port){
	initTCPSocket( ipaddress, port );
}

void TCPSocket::initTCPSocket(const IPAddress &ipaddress, int32_t port){
	peerAddress = ipaddress.clone();
	peerPort = port;

	type = MSOCKET_TYPE_TCP;
	if ((fd = (int32_t)::socket(ipaddress.getProtocolFamily(), SOCK_STREAM, IPPROTO_TCP ))<0){
		throw SocketFailed( errno );
	}
	int32_t on=1;
#ifndef WIN32
	setsockopt(fd,SOL_SOCKET,SO_REUSEADDR, (void *) (&on),sizeof(on));
#else
	setsockopt(fd,SOL_SOCKET,SO_REUSEADDR, (const char *) (&on),sizeof(on));
#endif

	ipaddress.connect(*this, port);
}

TCPSocket::TCPSocket(int32_t fd_, struct sockaddr * addr, int32_t addr_len){
	type = MSOCKET_TYPE_TCP;
	this->fd=fd_;
	peerAddress = IPAddress::create( addr, addr_len );
	peerPort = peerAddress->getPort();
}

TCPSocket::TCPSocket(TCPSocket &sock){
	type = MSOCKET_TYPE_TCP;

#ifdef _WIN32_WCE
#	pragma message("LIBMUTIL::TCPSocket - COMPILATION Warning: dup(int fd) replacement not found for POCKET PC in EVC")
	this->fd = sock.fd;
#else
	this->fd = dup(sock.fd);
#endif

#ifdef DEBUG_OUTPUT
	cerr << "DEBUG: In TCPSocket copy constructor: First free descriptor number is " << this->fd << endl;
#endif

//	this->ipaddr=0;
}

TCPSocket::~TCPSocket(){
	if (fd!=-1){
		this->close();
		this->fd=-1; //for debugging purpose (make sure error if using deleted pointer)
	}
	//delete peerAddress;
}

//ostream& operator<<(ostream& out, TCPSocket& s){
//	int32_t buf[1024*10];
//	int32_t n = s.do_read(buf,1024*10);
//	out.write(buf,n);
//	return out;
//}

TCPSocket& operator<<(TCPSocket& sock, string str){
	sock.write(str);
	return sock;
}

void TCPSocket::useNoDelay(bool noDelay){
	int on = noDelay?1:0;
#ifndef WIN32
	setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void*)(&on), sizeof(on));
#else
	setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char *)(&on), sizeof(on));
#endif
}
