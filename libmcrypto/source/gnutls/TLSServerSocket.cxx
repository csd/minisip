/*
  Copyright (C) 2006 Mikael Magnusson
  
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


#include<config.h>

#include<libmcrypto/openssl/TLSServerSocket.h>

#include<sys/socket.h>

using namespace std;

TLSServerSocket::TLSServerSocket( int32_t listen_port, MRef<certificate *> cert, MRef<ca_db *> cert_db):ServerSocket(AF_INET, listen_port)
{
	init(false, listen_port, cert, cert_db);
}

TLSServerSocket::TLSServerSocket( bool use_ipv6, int32_t listen_port, 
				 MRef<certificate *> cert,
				  MRef<ca_db *> cert_db):ServerSocket(use_ipv6?AF_INET6:AF_INET, listen_port)
{
	init(use_ipv6, listen_port, cert, cert_db);
}

void TLSServerSocket::init( bool use_ipv6, int32_t listen_port, 
			    MRef<certificate *> cert,
			    MRef<ca_db *> cert_db)
{
	throw Exception("TLSServerSocket unimplemented");
}

MRef<StreamSocket *> TLSServerSocket::accept(){
	MRef<StreamSocket *> ssocket = ServerSocket::accept();

	return new TLSSocket( ssocket, ssl_ctx );
}
