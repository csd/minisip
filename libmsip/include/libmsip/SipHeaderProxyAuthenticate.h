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
 * 	SipHeaderProxyAuthenticate.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#ifndef SIPHEADERPROXYAUTHENTICATE_H
#define SIPHEADERPROXYAUTHENTICATE_H

#include<libmsip/libmsip_config.h>


#include<libmsip/SipHeader.h>
#include<libmutil/SipUri.h>

/**
 * @author Erik Eliasson
*/

extern SipHeaderFactoryFuncPtr sipHeaderProxyAuthenticateFactory;

/**
 *
 * Note that a Proxy-Authenticate header contains several
 * header values where each value is a key-value pair
 * (one header value can for example be nonce="123")
 *
 * In this implementation the "key" is called "property"
 * and according to RFC3261 it can be for example username, 
 * realm, nonce,...
 *
 */
class LIBMSIP_API SipHeaderValueProxyAuthenticate: public SipHeaderValue{
	public:
		
		SipHeaderValueProxyAuthenticate(int type, const std::string &typeStr, const std::string &build_from);
		
		SipHeaderValueProxyAuthenticate(const std::string &build_from);

		virtual ~SipHeaderValueProxyAuthenticate();

                virtual std::string getMemObjectType() const {return "SipHeaderProxyAuthenticate";}
		
		/**
		 * returns string representation of the header
		 */
		std::string getString() const; 

		std::string getAuthMethod() const;

	protected:
		char getFirstParameterSeparator() const {return ' ';}
		char getParameterSeparator() const {return ',';}

	private:
		void init(const std::string& build_from);

		std::string authMethod;
};

#endif
