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
 * 	SipBye.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/


#ifndef SIPBYE_H
#define SIPBYE_H

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
#include<sys/types.h>

class SipInvite;

/**
 * Implementation of the BYE method in sip only for a user agent.
 * @author Erik Eliasson, eliasson@it.kth.se
 */
class LIBMSIP_API SipBye : public SipMessage{
	public:
		static const int type;

		/**
		 * Constructor that takes a string containing a SIP BYE packet 
		 * that is parsed to create a representation from.
		 */
		SipBye(string &build_from);

		/**
		 * 
		 */
		SipBye(string branch,
				MRef<SipInvite*> inv, 
				string to_uri, //string to_tag,
				string from_uri,// string from_tag,
				string proxyAddr, 
				int32_t seq_no//, 
				//bool made_call
				);
                

		virtual std::string getMemObjectType(){return "SipBye";}
		
		string getString();

		
	private:
		bool made_call;
		string username;
		string domain;
};	

#endif
