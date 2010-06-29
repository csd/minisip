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

#ifndef SOCKET_H
#define SOCKET_H

#include<libmnetutil/libmnetutil_config.h>

#define MSOCKET_TYPE_STREAM      0x10
#define MSOCKET_TYPE_TCP         0x11
#define MSOCKET_TYPE_TLS         0x12
#define MSOCKET_TYPE_TLSSRP      0x13
#define MSOCKET_TYPE_SCTP        0x14
#define MSOCKET_TYPE_TLS_SCTP    0x15

#define MSOCKET_TYPE_UDP         0x20

#include<libmutil/MemObject.h>
#include<libmutil/mtypes.h>

//order of io.h and socket.h is important!
//note (cesc): io.h is not really needed here ... but in wince, we need to make 
//	sure that we are the first ones to include io.h and undefine close, read and write ... 
//	otherwise, the stupid evc4.0 compiler says that Socket->_close is not a member ... of course not!
#ifdef _MSC_VER
#	ifdef _WIN32_WCE
#		include<io.h>
#		undef _read
#		undef _write
#		undef _close
#	endif
#endif

class IPAddress;

class LIBMNETUTIL_API Socket : public MObject {
	public:
		Socket();
		virtual ~Socket();
		virtual int32_t getFd();
		int32_t getType();

		/**
		* get the port where the socket is listening to incoming
		* connections.
		* @return the port number
		*/
		virtual int32_t getPort(); // The method is virtual to support exotic sub-classes.
					   // Example: To support incoming TLS/TCP connections behind 
					   // NAT, someone can create a server socket that accepts 
					   // connections via a proxy (it would connect as client to 
					   // this proxy).

		virtual int getAddressFamily();

		virtual MRef<IPAddress *> getLocalAddress() const;

//#ifdef _WIN32_WCE
/* Undef this ... it causes a link problem ... */
//#undef close
//#endif
		virtual void close( void );

	protected:
		int32_t type;
		int32_t fd;
};
#endif
