/*
  Copyright (C) 2005, 2004 Erik Eliasson and Johan Bilien
  
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


/* Name
 * 	SipAck.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/


#ifndef SIPACK_H
#define SIPACK_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipRequest.h>

/**
 * Implementation of the ACK method in SIP for (only a user agent).
 * @author Erik Eliasson
 */
class LIBMSIP_API SipAck : public SipRequest{

	public:
		static const int type;
                
		/**
		 * Creates an ACK object from a string.
		 * This string is typically received from another application and is
		 * parsed by this class.
		 * @param resp	string representation of a SIP/ACK packet to be parsed
		 * into an instance of this class.
		 */
		SipAck(string &build_from);

		/**
		 * Creates an ACK packet object from a packet that will be acknowledged and
		 * info about the local settings.
		 * @param pack Packet that should be received.
		 * @param local_tel_no Telephone number of the local user agent
		 * @param proxy The proxy or user agent to sent this ACK to.
		 * @param local_sip_port Port on which the local user agent is 
		 *                       accepting packets.
		 */
		SipAck(string branch, 
				MRef<SipMessage*> pack, 
				string local_tel_no, 
				string proxy);


		virtual std::string getMemObjectType(){return "SipAck";}
		
		void set_Conf();
		bool is_Conf();

	private:
		string username;
		string ipaddr;
		int32_t port;
		bool Conf;

};

#endif
