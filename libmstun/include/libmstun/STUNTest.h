/*
 Copyright (C) 2004-2006 the Minisip Team
 
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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef STUNTEST_H
#define STUNTEST_H

#include<libmstun/config.h>

#include<libmstun/STUNMessage.h>

#include<libmnetutil/UDPSocket.h>
#include<libmnetutil/IPAddress.h>

/**
 * Declares the test used by the STUN client to determine the type of NAT.
 * @author Erik Eliasson
*/
class LIBMSTUN_API STUNTest{
	public:
		/**
		 * Provides a reliable transport for sending a request to
		 * the server and receiving a response.
		 * TODO: If the server returns that the port is unreachable
		 * 	we should detect this and not retransmit. This should
		 * 	be done by connecting/unconnecting the UDP socket.
		 * @param addr	IP4 address of the STUN server.
		 * @param port	STUN server primary port.
		 * @param sock	Socket to use when communicating with the
		 * 		STUN server.
		 * @param changeIP Argument to the STUN server indicating
		 * 		from which IP the response should be sent.
		 * @param changePort Argument to the STUN server indicating
		 * 		from which port the response should be sent.
		 * @return	Returns the STUN message received from the
		 * 		server.
		*/
		static STUNMessage *test(IPAddress *addr, uint16_t port, UDPSocket &sock, bool changeIP, bool changePort);
};

#endif
