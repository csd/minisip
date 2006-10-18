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
 * 	SipHeaderVia.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#ifndef SIPHEADERVIA_H
#define SIPHEADERVIA_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipHeader.h>

/**
 * @author Erik Eliasson
*/

extern SipHeaderFactoryFuncPtr sipHeaderViaFactory;

// Ex: Via: SIP/2.0/UDP 10.0.0.2:5043
// 
class LIBMSIP_API SipHeaderValueVia: public SipHeaderValue{
	public:
		SipHeaderValueVia();
		SipHeaderValueVia(const std::string &build_from);
		SipHeaderValueVia(const std::string &proto, const std::string &ip, int32_t port);

		virtual ~SipHeaderValueVia();

                virtual std::string getMemObjectType() const {return "SipHeaderVia";}
		
		/**
		 * returns string representation of the header
		 */
		std::string getString() const; 

		/**
		 * returns the protocol used. This can be either UDP or TCP
		 */
		std::string getProtocol() const;
		void setProtocol(const std::string &protocol);
		
		/**
		 * @return The IP address of the contact header.
		 */
		std::string getIp() const;
		
		void setIp(const std::string &ip);


		/**
		 *
		 */
		int32_t getPort() const;
		void setPort(int32_t port);
		
	private:
		std::string protocol;
		std::string ip;
		int32_t port;
};

#endif
