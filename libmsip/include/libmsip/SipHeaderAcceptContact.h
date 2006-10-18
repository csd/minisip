/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien, Florian Mauer
  
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
 *          Florian Maurer, <florian.maurer@floHweb.ch>
*/


/* Name
 * 	SipHeaderAcceptContact.h
 * Author
 * 	Florian Maurer, florian.maurer@floHweb.ch
 * Purpose
 *	The Accept-Contact header field allows the UAC to specify that a UA
 *	should be contacted if it matches some or all of the values of the
 *	header field.  Each value of the Accept-Contact header field contains
 *	a "*" and is parameterized by a set of feature parameters.  Any UA
 *	whose capabilities match the feature set described by the feature
 *	parameters matches the value. The precise behavior depends heavily on
 *	whether the "require" and "explicit" feature parameters are present.
 *	When both of them are present, a proxy will only forward the request
 *	to contacts which have explicitly indicated that they support the
 *	desired feature set. Any others are discarded.
 *	http://www.ietf.org/internet-drafts/draft-ietf-sip-callerprefs-10.txt
 *	
 *	At the moment the Accept-Contact header is used to distinguish in 
 *	a INVITE message between a regular call and a P2T call.
 * Version
 *	2004-05-28 
*/

#ifndef SIPHEADERACCEPTCONTACT_H
#define SIPHEADERACCEPTCONTACT_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipHeader.h>


extern SipHeaderFactoryFuncPtr sipHeaderAcceptContactFactory;

/**
 * @author Florian Maurer
*/
class LIBMSIP_API SipHeaderValueAcceptContact: public SipHeaderValue{

	public:

		SipHeaderValueAcceptContact(std::string build_from);
		SipHeaderValueAcceptContact(std::string featuretag, 
				bool set_require, 
				bool set_explicit);

		virtual ~SipHeaderValueAcceptContact();

		std::string getMemObjectType() const {return "SipHeaderAcceptContact";}
		
		/**
		 * returns string representation of the header
		 */
		std::string getString() const ; 
		
		/**
		 * returns the featuretag
		 * @return  a string containing the feature tag
		 */
		std::string getFeaturetag() const {return featuretag;}

	private:
		
		/**
		 * the feature that should be supported
		 */		
		std::string featuretag;
		
		/**
		 * set the require flag
		 */
		bool set_require;
		
		/**
		 * set the explicit flag
		 */		
		bool set_explicit;

};


#endif
