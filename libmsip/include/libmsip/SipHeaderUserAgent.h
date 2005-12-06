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
 * 	SipHeaderUserAgent.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#ifndef SIPHEADERUSERAGENT_H
#define SIPHEADERUSERAGENT_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipHeader.h>

#define HEADER_USER_AGENT_DEFAULT	"Minisip"

/**
 * @author Erik Eliasson
*/

extern SipHeaderFactoryFuncPtr sipHeaderUserAgentFactory;

class LIBMSIP_API SipHeaderValueUserAgent: public SipHeaderValue{
	public:
		SipHeaderValueUserAgent();
		SipHeaderValueUserAgent(const string &build_from);

		virtual ~SipHeaderValueUserAgent();

                virtual std::string getMemObjectType(){return "SipHeaderUserAgent";}
		
		/**
		 * returns string representation of the header
		 */
		string getString(); 

		/**
		 * @return The IP address of the contact header.
		 */
		string getUserAgent();
		
		void setUserAgent(const string &ua);
		
	private:
		string user_agent;
};

#endif
