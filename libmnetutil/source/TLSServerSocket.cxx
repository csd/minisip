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


#ifdef HAVE_CONFIG_H
#include<config.h>
#endif

#ifdef WIN32
#include<winsock2.h>
#elif defined HAVE_ARPA_INET_H
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#endif

#include<libmnetutil/TLSServerSocket.h>
#include<libmutil/massert.h>


#ifdef DEBUG_OUTPUT
#include<iostream>
#endif

#ifdef WIN32
typedef int socklen_t
#endif


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
	int32_t backlog = 25;
	SSL_METHOD * meth;
	const unsigned char * sid_ctx = (const unsigned char *)"Minisip TLS";
	
	if( use_ipv6 )
		listen("::", listen_port, backlog);
	else
		listen("0.0.0.0", listen_port, backlog);

	SSL_load_error_strings();
	SSLeay_add_ssl_algorithms();
	meth = SSLv23_server_method();
	this->ssl_ctx = SSL_CTX_new( meth );
	this->cert_db = cert_db;

	if( ssl_ctx == NULL ){
#ifdef DEBUG_OUTPUT
		cerr << "Could not initialize SSL context" << endl;
#endif

		exit( 1 );
	}

	if( TLSSocket::sslCipherListIndex != 0 ) 
		TLSSocket::setSSLCTXCiphers ( this->ssl_ctx, TLSSocket::sslCipherListIndex );
	/* Set options: do not accept SSLv2*/
	SSL_CTX_set_options(ssl_ctx, SSL_OP_NO_SSLv2);
	
	SSL_CTX_set_verify( ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE, 0);
	//SSL_CTX_set_verify( ssl_ctx, SSL_VERIFY_NONE, 0);
	SSL_CTX_set_verify_depth( ssl_ctx, 5);
	
	//SSL_CTX_set_session_cache_mode( ssl_ctx, SSL_SESS_CACHE_BOTH );
	SSL_CTX_set_session_cache_mode( ssl_ctx, SSL_SESS_CACHE_SERVER );
	SSL_CTX_set_session_id_context( ssl_ctx, sid_ctx, (unsigned int)strlen( (const char *)sid_ctx ) );

	if( !cert_db.isNull() ){
		/* Use this database for the certificates check */
		SSL_CTX_set_cert_store( this->ssl_ctx, this->cert_db->get_db());
	}
	
		
	if( SSL_CTX_use_PrivateKey( ssl_ctx, cert->get_openssl_private_key() ) <= 0 ){
#ifdef DEBUG_OUTPUT
		cerr << "Could not use the given private key" << endl;
#endif

		ERR_print_errors_fp(stderr);
		exit( 1 );
	}
	
		
	if( SSL_CTX_use_certificate( ssl_ctx, cert->get_openssl_certificate() ) <= 0 ){
#ifdef DEBUG_OUTPUT
		cerr << "Could not use the given certificate" << endl;
#endif

		ERR_print_errors_fp(stderr);
		exit( 1 );
	}

	if( !SSL_CTX_check_private_key( ssl_ctx ) ){
#ifdef DEBUG_OUTPUT
		cerr << "Given private key does not match the certificate"<<endl;
#endif

		exit( 1 );
	}
}

MRef<StreamSocket *> TLSServerSocket::accept(){
	int32_t cli;
	struct sockaddr_storage sin;
	socklen_t sinlen=sizeof(sin);
	//sin = get_sockaddr_struct(sinlen);
	
#ifdef WIN32
	massert(sizeof(SOCKET)==4);
#endif
	if ((cli=(int32_t)::accept(fd, (struct sockaddr*)&sin, &sinlen))<0){
		perror("in ServerSocket::accept(): accept:");
	}

	return new TLSSocket( new TCPSocket( cli, (struct sockaddr*)&sin, sinlen), ssl_ctx );
}

