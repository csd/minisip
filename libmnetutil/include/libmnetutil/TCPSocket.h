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

#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include"StreamSocket.h"
#include"IPAddress.h"

using namespace std;

class TCPSocket : public StreamSocket{
	public:
		TCPSocket(int32_t fd);
		TCPSocket(string addr,int32_t port=0);
		TCPSocket(IPAddress &addr,int32_t port=0);
		TCPSocket(TCPSocket &sock);
		virtual ~TCPSocket();
		virtual int32_t write(string);
		virtual int32_t write(void *buf, int32_t count);
		virtual int32_t read(void *buf, int32_t count);
		//void flush();

		void useNoDelay(bool noDelay);

		friend std::ostream& operator<<(std::ostream&, TCPSocket&);
		
	private:
//		IPAddress *ipaddr;
};

TCPSocket& operator<<(TCPSocket& sock, string str);

#endif
