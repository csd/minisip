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

/* Name
 * 	SipResponse.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/


#ifndef SIPRESPONSE_H
#define SIPRESPONSE_H

#include"SipMessage.h"
#include"SipInvite.h"
#include<libmsip/SdpPacket.h>


/**
 * 
 * @author Erik Eliasson, eliasson@it.kth.se
 * @version 0.01
 */
class SipResponse : public SipMessage{

	public:
		static const int type;

		SipResponse(string branch, int32_t status, string status_desc, MRef<SipMessage*> inv);


		virtual std::string getMemObjectType(){return "SipResponse";}
		
		/**
		 * Parses response packet from string representation.
		 * @param respstr string representation of response packet.
		 */
		SipResponse(string &respstr);

		/**
		 * @returns Status code of this response.
		 */
		int32_t getStatusCode();

		/**
		 * @resutns Returns status description of this response.
		 */
		string getStatusDesc();

		/**
		 * @returns Nonce in this repsonse if available.
		 */ 
		string getNonce();
		void setNonce(string n){nonce=n;};

		/**
		 * @returns Realm in this response if available.
		 */
		string getRealm();
		void setRealm(string r){realm=r;}

		MRef<SdpPacket*> getSdp();

		string getString();

	private:
		int32_t status_code;
		string status_desc;

		string realm;
		string nonce;

		string tag;
};


#endif
