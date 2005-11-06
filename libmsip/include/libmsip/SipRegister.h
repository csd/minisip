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


#ifndef SIPREGISTER_H
#define SIPREGISTER_H

#ifdef _MSC_VER
#ifdef LIBMSIP_EXPORTS
#define LIBMSIP_API __declspec(dllexport)
#else
#define LIBMSIP_API __declspec(dllimport)
#endif
#else
#define LIBMSIP_API
#endif

#include<libmsip/SipRequest.h>

/**
 * Representation of a SIP REGISTER method.
 * 
 * @author Erik Eliasson, eliasson@it.kth.se
 * @version 0.01
 */
class LIBMSIP_API SipRegister : public SipRequest{

	public:
		static const int type;
		
		/**
		 * @param call_id Call id of this session.
		 * @param prxy The domain to register to.
		 * @param local_ip Local IP we can be contacted on.
		 * @param sip_listen_port Port on which we are accepting SIP packets.
		 * @param from_tel_no The telephone number/username of the local user agent
		 * @param seq_no Sequence mumber of this packet.
		 * @param expires Number of seconds for the register to expire (0 to de-register)
		 */
		SipRegister(string branch,
				string call_id, 
				string domain, 
				string localIp, 
				int32_t sip_listen_port, 
				string from_tel_no, 
				int32_t seq_no,
				string transport,
				int expires
				);
		
		/**
		 * @param call_id Call id of this session.
		 * @param prxy Address of proxy to connect to.
		 * @param local_ip Local IP we can be contacted on.
		 * @param sip_listen_port Port on which we are accepting SIP packets.
		 * @param from_tel_no The telephone number/username of the local user agent
		 * @param seq_no Sequence mumber of this packet.
		 * @param username Username to use in authentication
		 * @param realm Realm to use in authentication. Typically received in a 407 repsonse previously.
		 * @param nonce Nonce to use in authentication. Typically received in a 407 response previously.
		 * @param password Password to use in authentication.
		 * @param expires Number of seconds for the register to expire (0 to de-register)
		 */
		SipRegister(string branch,
				string call_id, 
				string domain, 
				string localIp, 
				int32_t sip_listen_port, 
				string from_tel_no, 
				int32_t seq_no, 
				string transport,
				string auth_id, 
				string realm, 
				string nonce, 
				string password,
				int expires
				);

		virtual ~SipRegister();

		virtual std::string getMemObjectType(){return "SipRegister";}
		
	private:
		string domain;
};

#endif

