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


#ifndef TLSSOCKET_H
#define TLSSOCKET_H

#ifdef _MSC_VER
#ifdef LIBMNETUTIL_EXPORTS
#define LIBMNETUTIL_API __declspec(dllexport)
#else
#define LIBMNETUTIL_API __declspec(dllimport)
#endif
#else
#define LIBMNETUTIL_API
#endif

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

#include<libmutil/MemObject.h>

using namespace std;


//Okay - another MSVC thing. Looks like I must explicitely instantiate 
//the MRef template like this to avoid linking errors --Erik.
#ifdef _MSC_VER
template class __declspec(dllexport) MRef<certificate*>;
template class __declspec(dllexport) MRef<ca_db*>;
#endif

class LIBMNETUTIL_API TLSSocket : public StreamSocket{
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
