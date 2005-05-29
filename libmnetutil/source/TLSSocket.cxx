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

#include<libmnetutil/TLSSocket.h>

#ifdef WIN32
#include<winsock2.h>
#elif defined HAVE_SYS_SOCKET_H
#include<sys/types.h>
#include<sys/socket.h>
#endif

#include<libmnetutil/IPAddress.h>
#include<libmnetutil/IP4Address.h>




#include<iostream>

#include<libmnetutil/NetworkException.h>
#include<libmutil/MemObject.h>

int8_t TLSSocket::sslCipherListIndex = 0; /* Set default value ... DEFAULT ciphers */


// When created by a TLS Server
TLSSocket::TLSSocket( TCPSocket * tcp_socket, SSL_CTX * ssl_ctx ):
		tcp_socket(tcp_socket){
	type = SOCKET_TYPE_TLS;
	peerPort = tcp_socket->getPeerPort();
	peerAddress = tcp_socket->getPeerAddress()->clone();

	int error;
	// Copy the SSL parameters, since the server still needs them
	ssl = SSL_new( ssl_ctx );
	this->ssl_ctx = SSL_get_SSL_CTX( ssl );

	SSL_set_fd( ssl, tcp_socket->getFd() );
	fd = tcp_socket->getFd();
	
	error = SSL_accept( ssl );
	if( error <= 0 ){
		cerr << "Could not establish an incoming TLS connection" << endl;
		ERR_print_errors_fp(stderr);
		throw new TLSConnectFailed( error, ssl );
	}
}


TLSSocket::TLSSocket( IPAddress &addr, int32_t port, void * &ssl_ctx,
		MRef<certificate *> cert, 
		MRef<ca_db *> cert_db ){
	TLSSocket::TLSSocket_init( addr, port, ssl_ctx, cert, cert_db);
}

TLSSocket::TLSSocket( string addr, int32_t port, void * &ssl_ctx, 
		MRef<certificate *> cert, 
		MRef<ca_db *> cert_db ){
	TLSSocket::TLSSocket_init( *(new IP4Address(addr)), port, ssl_ctx, cert, cert_db);
}

/* Helper function ... simplify the maintenance of constructors ... */
void TLSSocket::TLSSocket_init( IPAddress &addr, int32_t port, void * &ssl_ctx,
								MRef<certificate *> cert, MRef<ca_db *> cert_db ){
	type = SOCKET_TYPE_TLS;
	const unsigned char * sid_ctx = (const unsigned char *)"Minisip TLS";
	SSLeay_add_ssl_algorithms();
	SSL_METHOD *meth = SSLv23_client_method();
	this->ssl_ctx = (SSL_CTX *)ssl_ctx;
	this->cert_db = cert_db;
	peerPort = port;

	if( this->ssl_ctx == NULL ){
#ifdef DEBUG_OUTPUT
		cerr << "Creating new SSL_CTX" << endl;
#endif
		this->ssl_ctx = SSL_CTX_new( meth );
		
		if( this->ssl_ctx == NULL ){
			throw new TLSInitFailed();
		}
		
		if( sslCipherListIndex != 0 ) 
			setSSLCTXCiphers ( this->ssl_ctx, sslCipherListIndex );
		/* Set options: do not accept SSLv2*/
		SSL_CTX_set_options(this->ssl_ctx, SSL_OP_ALL | SSL_OP_NO_SSLv2);
		
		SSL_CTX_set_verify( this->ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE, 0);
		SSL_CTX_set_verify_depth( this->ssl_ctx, 5);

		if( !cert.isNull() ){
			/* Add a client certificate */
			if( SSL_CTX_use_PrivateKey( this->ssl_ctx, 
			cert->get_openssl_private_key() ) <= 0 ){
				throw new TLSContextInitFailed(); 
			}
			if( SSL_CTX_use_certificate( this->ssl_ctx,
			cert->get_openssl_certificate() ) <= 0 ){
				throw new TLSContextInitFailed(); 
			}
		}

		if( !cert_db.isNull() ){
			/* Use this database for the certificates check */
			SSL_CTX_set_cert_store( this->ssl_ctx, 
						cert_db->get_db());
		}

		//SSL_CTX_set_session_cache_mode( this->ssl_ctx, SSL_SESS_CACHE_BOTH );
		SSL_CTX_set_session_cache_mode( this->ssl_ctx, SSL_SESS_CACHE_SERVER );
		SSL_CTX_set_session_id_context( this->ssl_ctx, sid_ctx, (unsigned int)strlen( (const char *)sid_ctx ) );
		
		ssl_ctx = this->ssl_ctx;
	}
	
	tcp_socket = new TCPSocket( addr, port );
	peerAddress = tcp_socket->getPeerAddress()->clone();

	ssl = SSL_new( this->ssl_ctx );
	
	//FIXME ... this client side cache works?? only if only one host to connect to
	if( this->ssl_ctx->session_cache_head != NULL )
		SSL_set_session( ssl, this->ssl_ctx->session_cache_head );
	
	//SSL_set_verify( this->ssl, SSL_VERIFY_PEER, NULL );

	SSL_set_fd( ssl, tcp_socket->getFd() );
	// FIXME
	fd = tcp_socket->getFd();

	int32_t err = SSL_connect( ssl );

	if( err <= 0 ){
		throw new TLSConnectFailed( err, this->ssl );
	}

	try{
		peer_cert = new certificate( SSL_get_peer_certificate (ssl) );
	}
	catch( certificate_exception *){
		//FIXME
		cerr << "Could not get server certificate" << endl;
		peer_cert = NULL;
	}
	
}


TLSSocket::~TLSSocket(){
#ifdef DEBUG_OUTPUT
	cerr << "TLS: Shutting down TLS Socket" << endl;
#endif	
	SSL_shutdown( ssl );
	SSL_free( ssl );
	//SSL_CTX_free( ssl_ctx );
	delete tcp_socket;
	delete peerAddress;
}

int32_t TLSSocket::write( string data ){
	return SSL_write( ssl, data.c_str(), (int)data.length() );
}

int32_t TLSSocket::write( void *buf, int32_t count ){
	return SSL_write( ssl, buf, count );
}

TLSSocket& operator<<(TLSSocket& sock, string str){
	sock.write(str);
	return sock;
}

int32_t TLSSocket::read( void *buf, int32_t count ){
	//if( SSL_pending( ssl ) == 0 )
	//	return -1;
	int ret;
	ret = SSL_read( ssl, buf, count );
	if( ret == 0 )
//		if( SSL_get_error( ssl, ret ) == SSL_ERROR_ZERO_RETURN )
			// Connection closed
			return 0;
	
//		else
//			return -1;
	else 
		return ret;
}

int32_t TLSSocket::setSSLCTXCiphers ( SSL_CTX *_ctx, int8_t listIdx ) {
	char *ciphers;
	
#ifdef DEBUG_OUTPUT
		cerr << "Modifying SSL_CTX ciphers list" << endl;
#endif	
	
	switch( listIdx ) {
		case 1:
			ciphers = SSL_CIPHERS_AES_HIGH_MEDIUM;
			break;
		case 2:
			ciphers = SSL_CIPHERS_TESTING;
			break;
		default:
			ciphers = SSL_CIPHERS_DEFAULT;
			break;
	}
	if( SSL_CTX_set_cipher_list(_ctx, ciphers) == 0 ) {
#ifdef DEBUG_OUTPUT
		cerr << "ERROR: TLSSocket::setSSLCiphers: failed to set cipher list" << endl;
#endif	
		return 0;
	} else return 1;
}

