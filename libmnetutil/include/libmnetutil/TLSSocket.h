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

#include<libmutil/mtypes.h>

#include<libmnetutil/StreamSocket.h>
#include<libmnetutil/TCPSocket.h>
#include<libmnetutil/IPAddress.h>
#include<libmutil/cert.h>

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include<libmutil/MemObject.h>

using namespace std;

/**
   Various lists of ciphers. It includes the default list used,
   as well as a strong cipher list (AES+HIGH+MEDIUM:!aNULL);
   a testing list (with the null encryption ciphers).
   set this with TLSSocket::setSSLCiphers( int idx ), where
	 idx = 1 is AES.HIGH.MEDIUM
	 idx = 2 is TESTING
	 all others DEFAULT
   */
#define SSL_CIPHERS_DEFAULT "DHE-RSA-AES256-SHA:DHE-DSS-AES256-SHA:AES256-SHA:EDH-RSA-DES-CBC3-SHA:EDH-DSS-DES-CBC3-SHA:DES-CBC3-SHA:DES-CBC3-MD5:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA:AES128-SHA:RC2-CBC-MD5:DHE-DSS-RC4-SHA:RC4-SHA:RC4-MD5:RC4-MD5:RC4-64-MD5:EXP1024-DHE-DSS-DES-CBC-SHA:EXP1024-DES-CBC-SHA:EXP1024-RC2-CBC-MD5:EDH-RSA-DES-CBC-SHA:EDH-DSS-DES-CBC-SHA:DES-CBC-SHA:DES-CBC-MD5:EXP1024-DHE-DSS-RC4-SHA:EXP1024-RC4-SHA:EXP1024-RC4-MD5:EXP-EDH-RSA-DES-CBC-SHA:EXP-EDH-DSS-DES-CBC-SHA:EXP-DES-CBC-SHA:EXP-RC2-CBC-MD5:EXP-RC2-CBC-MD5:EXP-RC4-MD5:EXP-RC4-MD5"
#define SSL_CIPHERS_AES_HIGH_MEDIUM "DHE-RSA-AES256-SHA:DHE-DSS-AES256-SHA:AES256-SHA:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA:AES128-SHA"
#define SSL_CIPHERS_TESTING "NULL-SHA:NULL-MD5:AES256-SHA:AES128-SHA"

/**
   List of ciphers ... openssl ciphers 'ALL:eNULL:!LOW:!EXPORT'
   This is, all ciphers (included the null encryption ones) except the
       low security and export ones.
   eNULL ciphers are listed for testing purposes. DON't use in production environment!
ADH-AES256-SHA:DHE-RSA-AES256-SHA:DHE-DSS-AES256-SHA:AES256-SHA:\ 
ADH-AES128-SHA:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA:AES128-SHA:\ 
DHE-DSS-RC4-SHA:\ 
EDH-RSA-DES-CBC3-SHA:EDH-DSS-DES-CBC3-SHA:DES-CBC3-SHA:\ 
RC4-SHA:RC4-MD5:\ 
ADH-DES-CBC3-SHA:\ 
ADH-RC4-MD5:\ 
DES-CBC3-MD5:\ 
RC2-CBC-MD5:RC4-MD5:\ 
NULL-SHA:NULL-MD5
*/

//Okay - another MSVC thing. Looks like I must explicitely instantiate
//the MRef template like this to avoid linking errors --Erik.
#ifdef _MSC_VER
template class __declspec(dllexport) MRef<certificate*>;
template class __declspec(dllexport) MRef<ca_db*>;
#endif

class LIBMNETUTIL_API TLSSocket : public StreamSocket {
	public:
		TLSSocket(string addr, int32_t port, void * &ssl_ctx,
			MRef<certificate *> cert = NULL,
			MRef<ca_db *> cert_db=NULL );
		
		TLSSocket(IPAddress &addr, int32_t port, void * &ssl_ctx,
			MRef<certificate *> cert=NULL,
			MRef<ca_db *> cert_db=NULL );
		
		TLSSocket( TCPSocket * tcpSock, SSL_CTX * ssl_ctx );
		
		virtual ~TLSSocket();

		virtual std::string getMemObjectType(){return "TLSSocket";};

		virtual int32_t write(string);
		
		virtual int32_t write(void *buf, int32_t count);
		
		virtual int32_t read(void *buf, int32_t count);
		static int32_t setSSLCTXCiphers ( SSL_CTX *_ctx, int8_t listIdx );
		
		/* Must be initialized ... now at Minisip.cxx::tls_server_thread*/
		static int8_t sslCipherListIndex;  

		friend std::ostream& operator<<(std::ostream&, TLSSocket&);

	private:
		void TLSSocket_init( IPAddress &addr, int32_t port, void * &ssl_ctx,
			MRef<certificate *> cert, MRef<ca_db *> cert_db );
		
		TCPSocket * tcp_socket;
		
		SSL_CTX* ssl_ctx;
		
		SSL*     ssl;
		
		MRef<certificate *> peer_cert;
		
		/** CA db */
		MRef<ca_db *> cert_db;
};

TLSSocket& operator<<(TLSSocket& sock, string str);
#endif
