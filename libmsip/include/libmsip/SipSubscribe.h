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
 * 	SipSubscribe.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/



#ifndef SIPSUBSCRIBE_H
#define SIPSUBSCRIBE_H

#include<libmsip/SipMessage.h>
#include<libmsip/SipDialogConfig.h>
#include<vector>

/**
 * Representation of a SIP SUBSCRIBE method.
 * @author Erik Eliasson, eliasson@it.kth.se
 * @version 0.01
 */
class SipSubscribe : public SipMessage{

	public:
		static const int type;
		/**
		 * Parses a SUBSCRIBE packet from a string.
		 */
		SipSubscribe(string &build_from);

		/**
		 * Creates a SUBSCRIBE packet from parameters
		 * @param call_id Call ID of this call
		 * @param tel_no Number(/user) we are trying to contact
		 * @param proxy Remote proxy/user agent
		 * @param local_ip Local IP address of this user agent
		 * @param from_tel_no Local contact (tel no/user)
		 * @param seq_no Sequence number of this packet.
		 */
		SipSubscribe(string branch,
				string call_id, 
				MRef<SipIdentity*> toIdentity,
				MRef<SipIdentity*> fromIdentity,
				//int local_port,
				int32_t seq_no
/*												
				string resource, 
				IPAddress &proxy, 
				string from_tel_no, 
				int32_t seq_no, 
				int32_t local_media_port 
*/				);
		
		virtual ~SipSubscribe();

		virtual std::string getMemObjectType(){return "SipSubscribe";}

		virtual string getString();
		
	private:
/*		string resource; //telephone number for example
		string ip;
		string user_type; //phone or ip
*/

		MRef<SipIdentity *> fromIdentity;
		string toUser; //telephone number for example
		string toDomain;

};

#endif

