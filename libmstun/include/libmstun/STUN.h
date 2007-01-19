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

#ifndef STUN_H
#define STUN_H

#include<libmstun/config.h>

#ifndef _MSC_VER
#include<stdint.h>
#endif

#include<libmnetutil/IPAddress.h>
#include<libmnetutil/UDPSocket.h>
#include<vector>



/**
 * High level API with static methods for determining
 * external NAT address/port mapping and NAT type.
 * @author Erik Eliasson
*/
class LIBMSTUN_API STUN{
	public:
		/// Undefined error.
		static const int STUN_ERROR;
		
		/// The STUN server could not be contacted. 
		static const int STUNTYPE_BLOCKED;
		
		/// No NAT present
		static const int STUNTYPE_OPEN_INTERNET;
		
		/** All packets from this ip/port are mapped
		 * to one external ip/port no matter where
	 	 * they are sent and packets are forwarded
		 * to the client no matter where they come
		 * from.
		*/
		static const int STUNTYPE_FULL_CONE;
		
		/** The external ip/port mapping is unique
		 * only for (localip,localport,remoteip,remoteport).
		 * If any of them is changed, the mapping is different.
		*/
		static const int STUNTYPE_SYMMETRIC_NAT;
		
		/** Packets will only be received if 
		 * the client has previously sent some packet
		 * to the remote ip+port
		*/
		static const int STUNTYPE_PORT_RESTRICTED;

		/** Packets will only be received if the client
		 * has previously sent some packet to the remote
		 * ip.
		*/
		static const int STUNTYPE_RESTRICTED;

		/** Not NAT, but packets are dropped if the
		 * client has not sent a packet to the
		 * remote ip+port.
		*/
		static const int STUNTYPE_SYMMETRIC_FIREWALL;
		

		/**
		 * Returns a number representing what kind of NAT that was 
		 * detected in the network. The integer can be converted 
		 * to a printable message using <STUN::typeToString(int).
		 * @param stunAddr	Address of the STUN server
		 * @param stunPort	Primary port of the STUN server.
		 * @param socket	Socket to use when contacting
		 * 			the STUN server.
		 * @param localAddr	Local IPv4 address used by the
		 * 			socket when contacting the STUN
		 * 			server.
		 * @param localPort	Local port used by the socket
		 * 			when contacting the STUN server.
		 * @param mappedIPBuffer (OUT) Buffer where the external NAT
		 * 			address mapping will be stored.
		 * @param mappedPort	(OUT) The external NAT port mapping
		 * @return 		Integer representing what kind of 
		 * 			NAT is detected in the network.
		*/
		static int getNatType(IPAddress &stunAddr, 
				uint16_t stunPort, 
				UDPSocket &socket,
//				IPAddress &localAddr,
                                std::vector<std::string> localIPs,
				uint16_t localPort,
				char *mappedIPBuffer,
				uint16_t &mappedPort);

		/**
		 * Same as above, but the mapping arguments are left out.
		 * @see getNatType(stunAddr, stunPort, socket, localAddr, localPort, mappedIPBuffer, mappedPort)
		*/
		static int getNatType(IPAddress &stunAddr, 
				uint16_t stunPort, 
				UDPSocket &socket,
//				IPAddress &localAddr,
                                std::vector<std::string> localIPs,
				uint16_t localPort);
		

		
		/**
		 * The external IP/port mapping done by a firewall for 
		 * a specific local IP/port can be found using this function.
		*/
		static void getExternalMapping(IPAddress &stunAddr, 
				uint16_t stunPort, 
				UDPSocket &socket, 
				char *mappedIPBuffer, 
				uint16_t &mappedPort);


		/**
		 * Converts the integer NAT type returned from the <getNatType> 
		 * functions to a string.
		*/
		static const char *typeToString(int);

};


#endif
