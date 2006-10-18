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
 * 	SipHeaderWarning.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#ifndef SIPHEADERWARNING_H
#define SIPHEADERWARNING_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipHeader.h>

#include<libmutil/mtypes.h>
/**
 * @author Erik Eliasson
*/

extern SipHeaderFactoryFuncPtr sipHeaderWarningFactory;

class LIBMSIP_API SipHeaderValueWarning: public SipHeaderValue{
	public:
		SipHeaderValueWarning(std::string domainName, uint16_t errorCode, std::string warning);
		SipHeaderValueWarning(const std::string &build_from);

		virtual ~SipHeaderValueWarning();

                virtual std::string getMemObjectType() const {return "SipHeaderWarning";}
		
		/**
		 * returns string representation of the header
		 */
		std::string getString() const; 

		/**
		 * @return The IP address of the contact header.
		 */
		std::string getWarning() const;
		
		void setWarning(const std::string &ua);

		uint16_t getErrorCode() const;

		void setErrorCode(const uint16_t &errorCodec);

		std::string getDomainName() const;

		void setDomainName(const std::string &domainName);
		
	private:
		std::string warning;
		uint16_t errorCode;
		std::string domainName;
};

#endif
