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
 * 	SipHeaderReferTo.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * 	Johan Bilien, jobi@via.ecp.fr
 * Purpose
 * 
*/

#ifndef SIPHEADERREFERTO_H
#define SIPHEADERREFERTO_H

#ifdef _MSC_VER
#ifdef LIBMSIP_EXPORTS
#define LIBMSIP_API __declspec(dllexport)
#else
#define LIBMSIP_API __declspec(dllimport)
#endif
#else
#define LIBMSIP_API
#endif

#include"SipHeader.h"

/**
 * @author Erik Eliasson
 * @author Johan Bilien
*/

extern SipHeaderFactoryFuncPtr sipHeaderReferToFactory;


class LIBMSIP_API SipHeaderValueReferTo: public SipHeaderValue{

	public:
		
		SipHeaderValueReferTo();
		SipHeaderValueReferTo(const string &build_from);

		virtual ~SipHeaderValueReferTo();
		
                virtual std::string getMemObjectType(){return "SipHeaderReferTo";}
		
		/**
		 * returns string representation of the header
		 */
		string getString(); 

		/**
		 * @return The IP address of the contact header.
		 */
		string getUri();
		
		void setUri(const string &uri);

	private:
		string uri;
};

#endif
