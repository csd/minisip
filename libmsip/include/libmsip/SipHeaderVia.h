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

#ifdef _MSC_VER
#ifdef LIBMSIP_EXPORTS
#define LIBMSIP_API __declspec(dllexport)
#else
#define LIBMSIP_API __declspec(dllimport)
#endif
#else
#define LIBMSIP_API
#endif

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
		SipHeaderValueVia(const string &build_from);
		SipHeaderValueVia(const string &proto, const string &ip, int32_t port, const string &branch);

		virtual ~SipHeaderValueVia();

                virtual std::string getMemObjectType(){return "SipHeaderVia";}
		
		/**
		 * returns string representation of the header
		 */
		string getString(); 

		/**
		 * returns the protocol used. This can be either UDP or TCP
		 */
		string getProtocol();
		void setProtocol(const string &protocol);
		
		/**
		 * @return The IP address of the contact header.
		 */
		string getIp();
		
		void setIp(const string &ip);


		string getBranch();
		void setBranch(const string &branch);
		

		/**
		 *
		 */
		int32_t getPort();
		void setPort(int32_t port);
		
	private:
		string protocol;
		string ip;
		string branch;
		int32_t port;
};

#endif
