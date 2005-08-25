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


#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#ifdef _MSC_VER
#ifdef LIBMNETUTIL_EXPORTS
#define LIBMNETUTIL_API __declspec(dllexport)
#else
#define LIBMNETUTIL_API __declspec(dllimport)
#endif
#else
#define LIBMNETUTIL_API
#endif

#include<libmnetutil/Socket.h>
#include<libmnetutil/IPAddress.h>

using namespace std;

class LIBMNETUTIL_API UDPSocket : public Socket{
	public:
		UDPSocket(bool use_ipv6=false, int32_t port=0);
		virtual ~UDPSocket();

		virtual std::string getMemObjectType(){return "UDPSocket";}
		
		int32_t getPort();
		int32_t sendTo(IPAddress &to_addr, int32_t port, const void *msg, int32_t len);
		int32_t recvFrom(void *buf, int32_t len, IPAddress *& from);
		int32_t recv(void *buf, int32_t len);

		bool setLowDelay();
		
	private:
		bool use_ipv6;
};

#endif
