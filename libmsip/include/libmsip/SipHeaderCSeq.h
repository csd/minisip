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
 * 	SipHeaderCSeq.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#ifndef SIPHEADERCSEQ_H
#define SIPHEADERCSEQ_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipHeader.h>

/**
 * @author Erik Eliasson
*/

extern SipHeaderFactoryFuncPtr sipHeaderCSeqFactory;

class LIBMSIP_API SipHeaderValueCSeq: public SipHeaderValue{

	public:
		
		SipHeaderValueCSeq(const std::string &method, int seq);
		SipHeaderValueCSeq(const std::string &build_from);

		virtual ~SipHeaderValueCSeq();
		
                virtual std::string getMemObjectType() const {return "SipHeaderCSeq";}
		
		/**
		 * returns string representation of the header
		 */
		std::string getString() const; 

		
		/**
		 * @return The IP address of the contact header.
		 */
		int32_t getCSeq() const;
		void setCSeq(int32_t n);


		std::string getMethod() const;
		void setMethod(const std::string &method);

	private:
		std::string method;
		int32_t seq;
};

#endif
