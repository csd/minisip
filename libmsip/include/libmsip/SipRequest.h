/*
  Copyright (C) 2005 Mikael Magnusson
  
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
 * Author(s): Mikael Magnusson <mikma@users.sourceforge.net>
 *
*/


/* Name
 * 	SipRequest.h
 * Author
 * 	Mikael Magnusson, mikma@users.sourceforge.net
 * Purpose
 *	Base class of classes representing sip request messages, and
 *      used when parsing unsupported request methods.
*/



#ifndef SIPREQUEST_H
#define SIPREQUEST_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipMessage.h>
#include<libmutil/MemObject.h>

class LIBMSIP_API SipRequest : public SipMessage{

	public:
		static const int type;
                
		SipRequest(string branch, int type, const string &method,
			   const string &uri = "");

		SipRequest(string &build_from);

		virtual ~SipRequest();

		virtual std::string getMemObjectType(){return "SipRequest";}

		virtual string getString();

		virtual void setMethod(const string &method);
		virtual string getMethod();

		virtual void setUri(const string &uri);
		virtual string getUri();

		/**
		 * Insert a route first in the list of routes, forcing
		 * the request through the proxy.
		 */
		void addRoute(const string &route);

		/**
		 * Insert a loose route first in the list of routes,
		 * forcing the request through the proxy with the
		 * specified address. Using the default transport if
		 * it's an empty string, and the default port if port
		 * is set to zero.
		 */
		void addRoute(const string &addr, int32_t port,
			      const string &transport);

	protected:
		SipRequest(int type, string &build_from);

		virtual void init(string &build_from);

	private:
		string method;
		string uri;
};

#endif
