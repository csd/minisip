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
 * 	SipCancel.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/



#ifndef SIPCANCEL_H
#define SIPCANCEL_H

#ifdef _MSC_VER
#ifdef LIBMSIP_EXPORTS
#define LIBMSIP_API __declspec(dllexport)
#else
#define LIBMSIP_API __declspec(dllimport)
#endif
#else
#define LIBMSIP_API
#endif

#include"SipMessage.h"

class SipInvite;

/**
 * Implementation of the SIP CANCEL method only to be used
 * with a user agent
 * @author Erik Eliasson, eliasson@it.kth.se
 */
class LIBMSIP_API SipCancel : public SipMessage{
	public:
		static const int type;
                
		/**
		 * Constructor that takes a string representation of a CANCEL packet
		 * that is parsed and converted into a object representation.
		 * @param resp string containing a CANCEL method
		 */
		SipCancel(string &build_from);

		/**
		 * Creates a cancel to an invite message.
		 * @param inv INVITE message to be cancelled.
		 */
		SipCancel(string branch,
				MRef<SipInvite*> inv, 
				string to_uri, 
				string from_uri, 
				string proxy//, 
				//bool local_called
				);

		virtual std::string getMemObjectType(){return "SipCancel";}

		string getString();
		
	private:
		string username;
		string ipaddr;

};

#endif
