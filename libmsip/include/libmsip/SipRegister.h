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

#ifndef SIPREGISTER_H
#define SIPREGISTER_H

#include"SipMessage.h"

/**
 * Representation of a SIP REGISTER method.
 * 
 * @author Erik Eliasson, eliasson@it.kth.se
 * @version 0.01
 */
class SipRegister : public SipMessage{

	public:
		static const int type;
		
		/**
		 * @param call_id Call id of this session.
		 * @param prxy The domain to register to.
		 * @param local_ip Local IP we can be contacted on.
		 * @param sip_listen_port Port on which we are accepting SIP packets.
		 * @param from_tel_no The telephone number/username of the local user agent
		 * @param seq_no Sequence mumber of this packet.
		 */
		SipRegister(string branch,
				string call_id, 
				string domain, 
				string localIp, 
				int32_t sip_listen_port, 
				string from_tel_no, 
				int32_t seq_no,
				string transport
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
				string password
				);

		virtual ~SipRegister();

		virtual std::string getMemObjectType(){return "SipRegister";}
		
		virtual string getString();

	private:
		string domain;
};

#endif

