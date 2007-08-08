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
 *	    Cesc Santasusana, c e s c dot s a n t a A{T g m a i l dot co m; 2005
 * Purpose
 * 
*/

#ifndef SIPHEADERCONTACT_H
#define SIPHEADERCONTACT_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipHeader.h>
#include<libmutil/SipUri.h>

/**
 * @author Erik Eliasson
*/

extern SipHeaderFactoryFuncPtr sipHeaderContactFactory;

class LIBMSIP_API SipHeaderValueContact: public SipHeaderValue{

	public:
		
//		SipHeaderValueContact();
		SipHeaderValueContact(const std::string &build_from);
		SipHeaderValueContact(const SipUri &contactUri,
				int expires=1000);

		virtual ~SipHeaderValueContact();

		virtual std::string getMemObjectType() const {return "SipHeaderContact";}
		
		/**
		 * returns string representation of the header
		 */
		std::string getString() const; 

		/**
		 * returns the protocol used. This can be either UDP or TCP
		 */
		const SipUri &getUri() const;
		void setUri(const SipUri &uri);
		
		/**
		 * can be used to set Caller Preferences for example in the 
		 * SIP REGISTER message
		 * @param featuretag
		 */
		 void setFeatureTag(std::string ft){this->featuretag=ft;}
		
		 /**
		  * Used to get/set the expires for this contact in the registrar.
		  * Using the SipCommands, it can be set to any value (param3),
		  * for example, to zero for de-registration.
		  */
		 int getExpires() const;
		 void setExpires(int _expires);
		 
	private:
		SipUri uri;
		
		//int expires; //now we only store in the params map ... see SipHeader.h
		
		///the featuretag
		std::string featuretag;
};

#endif
