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


#ifndef STREAMSOCKET_H
#define STREAMSOCKET_H

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

#include<string>
using namespace std;

class IPAddress;
class IP4Address;

class LIBMNETUTIL_API StreamSocket : public Socket{
	public:
		virtual ~StreamSocket();
		virtual int32_t write(std::string)=0;
		virtual int32_t write(void *buf, int32_t count)=0;
		virtual int32_t read(void *buf, int32_t count)=0;

		// Buffer of the received data;
		string received;

		bool matchesPeer(IPAddress& address, uint16_t port);

		IPAddress * getPeerAddress();
		uint16_t getPeerPort();
	protected:
		IPAddress * peerAddress;
		uint16_t peerPort;

};

#endif
