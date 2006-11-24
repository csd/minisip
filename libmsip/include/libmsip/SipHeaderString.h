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
*/


/* Name
 * 	SipHeaderString.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 	Represents a header value that internally is stored
 * 	as a string, such as the Subject or Content-Type header 
 * 	values.
*/

#ifndef SIPHEADERSTRING_H
#define SIPHEADERSTRING_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipHeader.h>

/**
 * @author Erik Eliasson
*/

extern SipHeaderFactoryFuncPtr sipHeaderSubjectFactory;

class LIBMSIP_API SipHeaderValueString: public SipHeaderValue{
	public:

		/**
		 * @param type	Header type identifier as defined
		 * 		in SipHeader.h. This is needed since
		 * 		this does not represent a SIP header
		 * 		value by it self, and is expected to
		 * 		be used as base class for "Subject",
		 * 		"Content-type" etc.
		 * @param build_from	String that is the header value.
		 */
		SipHeaderValueString(int type, const std::string& typeStr, const std::string& build_from);

		virtual ~SipHeaderValueString();
		
		/**
		 * returns string representation of the header. This
		 * 	is the string passed to the constructor
		 * 	with any white space in the ends removed.
		 */
		std::string getString() const; 

		void setString(const std::string &newStr);

	protected:
		std::string str;
};

#endif
