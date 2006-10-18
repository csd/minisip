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
 * 	SipHeaderContentLength.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#ifndef SIPHEADERCONTENTLENGTH_H
#define SIPHEADERCONTENTLENGTH_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipHeader.h>

/**
 * @author Erik Eliasson
*/

extern SipHeaderFactoryFuncPtr sipHeaderContentLengthFactory;

class LIBMSIP_API SipHeaderValueContentLength: public SipHeaderValue{
	public:
		SipHeaderValueContentLength(int32_t length);
		SipHeaderValueContentLength(const std::string &build_from);

		virtual ~SipHeaderValueContentLength();
		

                virtual std::string getMemObjectType() const {return "SipHeaderContentLength";}
		
		/**
		 * returns string representation of the header
		 */
		virtual std::string getString() const; 

		/**
		 * @return The IP address of the contact header.
		 */
		int32_t getContentLength() const;
		
		void setContentLength(int32_t content_length);

	private:
		int32_t content_length;
};

#endif
