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


/* Name
 * 	SipNotify.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/



#ifndef SIPNOTIFY_H
#define SIPNOTIFY_H

#ifdef _MSC_VER
#ifdef LIBMSIP_EXPORTS
#define LIBMSIP_API __declspec(dllexport)
#else
#define LIBMSIP_API __declspec(dllimport)
#endif
#else
#define LIBMSIP_API
#endif

#include<libmsip/SipMessage.h>
#include<libmsip/SipDialogConfig.h>
#include<libmnetutil/IPAddress.h>
#include<vector>
#include<sys/types.h>

/**
 * Representation of a SIP NOTIFY method.
 * @author Erik Eliasson, eliasson@it.kth.se
 */
class LIBMSIP_API SipNotify : public SipMessage{

	public:
		static const int type;
                
		/**
		 * Parses a SUBSCRIBE packet from a string.
		 */
		SipNotify(string &build_from);

		/**
		 * Creates a SUBSCRIBE packet from parameters
		 * @param call_id Call ID of this call
		 * @param tel_no Number(/user) we are trying to contact
		 * @param proxy Remote proxy/user agent
		 * @param local_ip Local IP address of this user agent
		 * @param from_tel_no Local contact (tel no/user)
		 * @param seq_no Sequence number of this packet.
		 */
		SipNotify(string branch,
				string call_id, 
				MRef<SipIdentity*> toIdentity,
				MRef<SipIdentity*> fromId,
				int32_t seq_no
				);
		
		virtual ~SipNotify();

		virtual std::string getMemObjectType(){return "SipNotify";}

		virtual string getString();
		
	private:
		MRef<SipIdentity *> fromIdentity;
		string toUser; //telephone number for example
		string toDomain;
};

#endif

