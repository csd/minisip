/*
  Copyright (C) 2007  Mikael Magnusson

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
 * Authors: Mikael Magnusson <mikma@users.sourceforge.net>
 */

#ifndef SCTP_SERVER_SOCKET_H
#define SCTP_SERVER_SOCKET_H

#include<libmnetutil/libmnetutil_config.h>

#include<libmnetutil/ServerSocket.h>
#include<libmnetutil/StreamSocket.h>

class LIBMNETUTIL_API SctpServerSocket : public ServerSocket {
	public:
		static SctpServerSocket *create( int32_t listenport,
						bool useIpv6=false,
						int32_t backlog=25 );

		// ServerSocket
		virtual MRef<StreamSocket *> createSocket( int32_t fd,	
							   struct sockaddr *sa,
							   int32_t salen );

		// MObject
		virtual std::string getMemObjectType() const {return "SctpServerSocket";}

	protected:
		SctpServerSocket( const IPAddress &addr );
};

#endif
