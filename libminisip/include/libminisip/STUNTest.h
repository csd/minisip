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


/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef STUNTEST_H
#define STUNTEST_H

#ifdef _MSC_VER
#ifdef LIBMINISIP_EXPORTS
#define LIBMINISIP_API __declspec(dllexport)
#else
#define LIBMINISIP_API __declspec(dllimport)
#endif
#else
#define LIBMINISIP_API
#endif


#ifdef _MSC_VER
typedef unsigned short uint16_t;
#else
#include<stdint.h>
#endif

#include<libminisip/STUNMessage.h>

#include<libmnetutil/UDPSocket.h>
#include<libmnetutil/IP4Address.h>

/**
 * Declares the test used by the STUN client to determine the type of NAT.
 * @author Erik Eliasson
*/
class LIBMINISIP_API STUNTest{
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
		static STUNMessage *test(IP4Address *addr, uint16_t port, UDPSocket &sock, bool changeIP, bool changePort);
};

#endif
