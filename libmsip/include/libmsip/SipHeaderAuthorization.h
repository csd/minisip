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
 * 	SipHeaderAuthorization.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#ifndef SIPHEADERAUTHORIZATION_H
#define SIPHEADERAUTHORIZATION_H


#include"SipHeader.h"
#include<libmsip/SipURI.h>

/**
 * @author Erik Eliasson
*/


class SipHeaderAuthorization: public SipHeader{
	public:
		SipHeaderAuthorization();
		SipHeaderAuthorization(int type);
		
		SipHeaderAuthorization(const string &build_from);
		SipHeaderAuthorization(int type, const string &build_from);
		
		SipHeaderAuthorization(const string &sip_method,
				const string &username, 
				const string &realm, 
				const string &nonce, 
				const SipURI &uri, 
				const string &auth_id, 
				const string &password,
				const string &auth_method="DIGEST");
		
		SipHeaderAuthorization(int type, const string &sip_method,
				const string &username, 
				const string &realm, 
				const string &nonce, 
				const SipURI &uri, 
				const string &auth_id, 
				const string &password,
				const string &auth_method="DIGEST");

		
		virtual ~SipHeaderAuthorization();

                virtual std::string getMemObjectType(){return "SipHeaderAuthorization";}
		
		/**
		 * returns string representation of the header
		 */
		string getString(); 

		/**
		 * returns the protocol used. This can be either UDP or TCP
		 */
		string getSipMethod();
		void setSipMethod(const string &meth);

		string getUsername();
		void setUsername(const string &username);

		string getRealm();
		void setRealm(const string &r);

		string getNonce();
		void setNonce(const string &n);

		SipURI getUri();
		void setUri(const SipURI &uri);

		string getResponse();
		void setResponse(const string &resp);

		static string md5ToString(unsigned char *md5);
	private:
		string calcResponse();
		
		string sipMethod;
		string username;
		string realm;
		string nonce;
		SipURI uri;
		string auth_id;
		string password;
		string auth_method;
};

#endif
