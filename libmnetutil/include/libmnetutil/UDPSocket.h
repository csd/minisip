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

#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include"Socket.h"
#include"IPAddress.h"

using namespace std;

class UDPSocket : public Socket{
	public:
		UDPSocket(bool use_ipv6=false, int32_t port=0);
		virtual ~UDPSocket();
		
		int32_t getPort();
		int32_t sendTo(IPAddress &to_addr, int32_t port, const void *msg, int32_t len);
		int32_t recvFrom(void *buf, int32_t len, IPAddress *& from);
		int32_t recv(void *buf, int32_t len);
		
	private:
		bool use_ipv6;
};

#endif
