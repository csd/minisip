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

#ifndef TLSSOCKET_H
#define TLSSOCKET_H

//#include<config.h>

//#ifndef NO_SECURITY

#include"StreamSocket.h"
#include"TCPSocket.h"
#include"IPAddress.h"
#include<libmutil/cert.h>


#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>


using namespace std;

class TLSSocket : public StreamSocket{
	public:
		TLSSocket(string addr, int32_t port, void * &ssl_ctx, 
		          MRef<certificate *> cert = NULL, 
			  MRef<ca_db *> cert_db=NULL );
		TLSSocket(IPAddress &addr, int32_t port, void * &ssl_ctx, 
			  MRef<certificate *> cert=NULL, 
			  MRef<ca_db *> cert_db=NULL );
		TLSSocket( TCPSocket * tcpSock, SSL_CTX * ssl_ctx );
		virtual ~TLSSocket();
		virtual int32_t write(string);
		virtual int32_t write(void *buf, int32_t count);
		virtual int32_t read(void *buf, int32_t count);

		friend std::ostream& operator<<(std::ostream&, TLSSocket&);

	private:
		TCPSocket * tcp_socket;
		SSL_CTX* ssl_ctx;
		SSL*     ssl;
		MRef<certificate *> peer_cert;
		/* CA db */
		MRef<ca_db *> cert_db;
};

TLSSocket& operator<<(TLSSocket& sock, string str);

//#endif //NO_SECURITY

#endif
