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


#include<libmnetutil/TCPSocket.h>
#include<libmnetutil/IPAddress.h>
#include<libmnetutil/IP4Address.h>
#include<libmnetutil/NetworkException.h>
#include<stdlib.h>
#include<stdio.h>

#ifdef LINUX
#include<sys/socket.h>
#include<netinet/tcp.h>
#include<netinet/in.h>
#endif

#ifdef WIN32
#include<winsock2.h>
#endif

#include<unistd.h>
#include<errno.h>

#include<iostream>
using namespace std;

TCPSocket::TCPSocket(string addr, int32_t port){
//	this->ipaddr = &ipaddress;
	type = SOCKET_TYPE_TCP;
	IP4Address ipaddress(addr);
	
	if ((fd = ::socket(ipaddress.getProtocolFamily(), SOCK_STREAM, IPPROTO_TCP ))<0){
		throw new SocketFailed( errno );
	}
	int32_t on=1;
#ifndef WIN32
	setsockopt(fd,SOL_SOCKET,SO_REUSEADDR, (void *) (&on),sizeof(on));
#else
	setsockopt(fd,SOL_SOCKET,SO_REUSEADDR, (const char *) (&on),sizeof(on));
#endif

	ipaddress.connect(*this, port);
}

TCPSocket::TCPSocket(IPAddress &ipaddress, int32_t port){
	
	type = SOCKET_TYPE_TCP;
	if ((fd = ::socket(ipaddress.getProtocolFamily(), SOCK_STREAM, IPPROTO_TCP ))<0){
		throw new SocketFailed( errno );
	}
	int32_t on=1;
#ifndef WIN32
	setsockopt(fd,SOL_SOCKET,SO_REUSEADDR, (void *) (&on),sizeof(on));
#else
	setsockopt(fd,SOL_SOCKET,SO_REUSEADDR, (const char *) (&on),sizeof(on));
#endif

	ipaddress.connect(*this, port);
}

TCPSocket::TCPSocket(int32_t fd){
	type = SOCKET_TYPE_TCP;
	this->fd=fd;
//	this->ipaddr=0;
}

TCPSocket::TCPSocket(TCPSocket &sock){
	type = SOCKET_TYPE_TCP;
	this->fd = dup(sock.fd);
#ifdef DEBUG_OUTPUT
	cerr << "DEBUG: In TCPSocket copy constructor: First free descriptor number is " << this->fd << endl;
#endif

//	this->ipaddr=0;
}

TCPSocket::~TCPSocket(){
	if (fd!=-1){
		close();
		this->fd=-1; //for debugging purpose (make sure error if using deleted pointer)
	}
	
}

int32_t TCPSocket::write(string data){
	return ::write(fd, data.c_str(), data.length());
}

int32_t TCPSocket::write(void *buf, int32_t count){
	return ::write(fd, buf, count);
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

int32_t TCPSocket::read(void *buf, int32_t count){
	return ::read(fd, buf, count);
}


void TCPSocket::useNoDelay(bool noDelay){
	int on = noDelay?1:0;
#ifndef WIN32
	setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void*)(&on), sizeof(on));
#else
	setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char *)(&on), sizeof(on));
#endif
}
