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
 * 	SipHeaderWarning.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#ifndef SIPHEADERWARNING_H
#define SIPHEADERWARNING_H

#include<libmsip/SipHeader.h>
#include<stdint.h>

/**
 * @author Erik Eliasson
*/

class SipHeaderWarning: public SipHeader{
	public:
		SipHeaderWarning(string domainName, uint16_t errorCode, string warning);
		SipHeaderWarning(const string &build_from);

		virtual ~SipHeaderWarning();

                virtual std::string getMemObjectType(){return "SipHeaderWarning";}
		
		/**
		 * returns string representation of the header
		 */
		string getString(); 

		/**
		 * @return The IP address of the contact header.
		 */
		string getWarning();
		
		void setWarning(const string &ua);

		uint16_t getErrorCode();

		void setErrorCode(const uint16_t &errorCodec);

		string getDomainName();

		void setDomainName(const string &domainName);
		
	private:
		string warning;
		uint16_t errorCode;
		string domainName;
};

#endif
