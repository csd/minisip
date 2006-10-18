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
 * 	SipHeaderAuthorization.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#ifndef SIPHEADERAUTHORIZATION_H
#define SIPHEADERAUTHORIZATION_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipHeader.h>
#include<libmsip/SipUri.h>

/**
 * @author Erik Eliasson
*/

extern SipHeaderFactoryFuncPtr sipHeaderAuthorizationFactory;


class LIBMSIP_API SipHeaderValueAuthorization: public SipHeaderValue{
	public:
		SipHeaderValueAuthorization(int type, const std::string &typeStr);
		
		SipHeaderValueAuthorization(const std::string &build_from);
		SipHeaderValueAuthorization(int type, const std::string &build_from, const std::string &typeStr);
		
		SipHeaderValueAuthorization(const std::string &sip_method,
				const std::string &username, 
				const std::string &realm, 
				const std::string &nonce, 
				const std::string &opaque,
				const SipUri &uri, 
				const std::string &auth_id, 
				const std::string &password,
				const std::string &auth_method="DIGEST");
		
		SipHeaderValueAuthorization(int type, const std::string &sip_method,
				const std::string &username, 
				const std::string &realm, 
				const std::string &nonce, 
				const std::string &opaque,
				const SipUri &uri, 
				const std::string &auth_id, 
				const std::string &password,
				const std::string &auth_method,
				const std::string &typeStr);

		
		virtual ~SipHeaderValueAuthorization();

                virtual std::string getMemObjectType() const {return "SipHeaderAuthorization";}
		
		/**
		 * returns string representation of the header
		 */
		std::string getString() const; 

		/**
		 * returns the protocol used. This can be either UDP or TCP
		 */
		std::string getSipMethod() const;
		void setSipMethod(const std::string &meth);

		std::string getUsername() const;
		void setUsername(const std::string &username);

		std::string getRealm() const;
		void setRealm(const std::string &r);

		std::string getNonce() const;
		void setNonce(const std::string &n);

		std::string getOpaque() const;
		void setOpaque(const std::string &n);

		SipUri getUri() const;
		void setUri(const SipUri &uri);

		std::string getResponse() const;
		void setResponse(const std::string &resp);

		static std::string md5ToString(unsigned char *md5);
	private:
		std::string calcResponse() const;
		
		std::string sipMethod;
		std::string username;
		std::string realm;
		std::string nonce;
		std::string opaque;
		SipUri uri;
		std::string auth_id;
		std::string password;
		std::string auth_method;
};

#endif
