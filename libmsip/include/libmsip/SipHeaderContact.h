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
 * 	SipHeaderContact.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#ifndef SIPHEADERCONTACT_H
#define SIPHEADERCONTACT_H

#include<libmsip/SipHeader.h>
#include<libmsip/SipURI.h>

/**
 * @author Erik Eliasson
*/


class SipHeaderValueContact: public SipHeaderValue{

	public:
		
		SipHeaderValueContact();
		SipHeaderValueContact(const string &build_from);
		SipHeaderValueContact(const string &username, 
				const string &ip, 
				int32_t port, 
				const string &usertype="phone", 
				const string &transport="");

		virtual ~SipHeaderValueContact();

                virtual std::string getMemObjectType(){return "SipHeaderContact";}
		
		/**
		 * returns string representation of the header
		 */
		string getString(); 

		/**
		 * returns the protocol used. This can be either UDP or TCP
		 */
		SipURI getUri();
		void setUri(const SipURI &uri);
		
		/**
		 * can be used to set Caller Preferences for example in the 
		 * SIP REGISTER message
		 * @param featuretag
		 */
		 void setFeatureTag(string featuretag){this->featuretag=featuretag;}
		
	private:
		SipURI uri;
		
		///the featuretag
		string featuretag;
};

#endif
