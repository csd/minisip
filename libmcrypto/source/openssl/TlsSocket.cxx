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

#include<config.h>

#include<libmcrypto/openssl/TlsSocket.h>
#include<libmcrypto/openssl/cert.h>

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#ifdef WIN32
#include<winsock2.h>
#elif defined HAVE_SYS_SOCKET_H
#include<sys/types.h>
#include<sys/socket.h>
#endif

#include<libmnetutil/IPAddress.h>
#include<libmnetutil/TCPSocket.h>

#include<iostream>

#include<libmcrypto/TlsException.h>
#include<libmcrypto/openssl/TlsException.h>
#include<libmutil/MemObject.h>

using namespace std;

TLSSocket::TLSSocket()
{
}

TLSSocket::~TLSSocket()
{
}

TLSSocket* TLSSocket::connect( IPAddress &addr, int32_t port,
			       MRef<Certificate *> cert,
			       MRef<CertificateSet *> cert_db,
			       string serverName )
{
	void *ssl_ctx = NULL;
	MRef<OsslCertificate*> ssl_cert;
	MRef<OsslCertificateSet*> ssl_db;

	if( cert )
		ssl_cert = (OsslCertificate*)*cert;

	if( cert_db )
		ssl_db = (OsslCertificateSet*)*cert_db;

	return new OsslSocket( addr, port, ssl_ctx, ssl_cert, ssl_db );
}


int8_t OsslSocket::sslCipherListIndex = 0; /* Set default value ... DEFAULT ciphers */


#define ssl ((SSL*)priv)



// When created by a TLS Server
OsslSocket::OsslSocket( MRef<StreamSocket *> tcp_socket, SSL_CTX * ssl_ctx_ ):
		sock(tcp_socket){
	type = SOCKET_TYPE_TLS;
	peerPort = tcp_socket->getPeerPort();
	peerAddress = tcp_socket->getPeerAddress()->clone();

	int error;
	// Copy the SSL parameters, since the server still needs them
	// Initialize ssl in priv
	priv = SSL_new( ssl_ctx_ );
	this->ssl_ctx = SSL_get_SSL_CTX( ssl );

	SSL_set_fd( ssl, tcp_socket->getFd() );
	fd = tcp_socket->getFd();
	
	error = SSL_accept( ssl );
	if( error <= 0 ){
		cerr << "Could not establish an incoming TLS connection" << endl;
		ERR_print_errors_fp(stderr);
		throw TLSConnectFailed( error, ssl );
	}
}


OsslSocket::OsslSocket( IPAddress &addr, int32_t port, void * &ssl_ctx_,
			      MRef<OsslCertificate *> cert, 
			      MRef<OsslCertificateSet *> cert_db_ ){
	MRef<TCPSocket*> tcp_sock = new TCPSocket( addr, port );
	OsslSocket::OsslSocket_init( *tcp_sock, ssl_ctx_, cert, cert_db_);
}

OsslSocket::OsslSocket( string addr, int32_t port, void * &ssl_ctx_, 
			      MRef<OsslCertificate *> cert, 
			      MRef<OsslCertificateSet *> cert_db_ ){
	MRef<TCPSocket*> tcp_sock = new TCPSocket( addr, port );
	OsslSocket::OsslSocket_init( *tcp_sock, ssl_ctx_, cert, cert_db_);
}

/* Helper function ... simplify the maintenance of constructors ... */
void OsslSocket::OsslSocket_init( MRef<StreamSocket*> ssock, void * &ssl_ctx_,
					MRef<OsslCertificate *> cert,
					MRef<OsslCertificateSet *> cert_db_ ){
	type = SOCKET_TYPE_TLS;
	const unsigned char * sid_ctx = (const unsigned char *)"Minisip TLS";
	SSLeay_add_ssl_algorithms();
	SSL_METHOD *meth = SSLv23_client_method();
	this->ssl_ctx = (SSL_CTX *)ssl_ctx_;
	this->cert_db = cert_db_;
	peerPort = ssock->getPeerPort();
	MRef<OsslCertificate*> ssl_cert;
	MRef<OsslCertificateSet*> ssl_db;

	if( cert )
		ssl_cert = (OsslCertificate*)*cert;

	if( cert_db )
		ssl_db = (OsslCertificateSet*)*cert_db;

	if( this->ssl_ctx == NULL ){
#ifdef DEBUG_OUTPUT
		cerr << "Creating new SSL_CTX" << endl;
#endif
		this->ssl_ctx = SSL_CTX_new( meth );
		
		if( this->ssl_ctx == NULL ){
			cerr << "Could not create SSL session" << endl;
			ERR_print_errors_fp(stderr);
			throw TLSInitFailed();
		}
		
		if( sslCipherListIndex != 0 ) 
			setSSLCTXCiphers ( this->ssl_ctx, sslCipherListIndex );
		/* Set options: do not accept SSLv2*/
		long options = SSL_OP_NO_SSLv2 | SSL_OP_ALL;
		
#if OPENSSL_VERSION_NUMBER >= 0x00908000
		// Disable SSL_OP_TLS_BLOCK_PADDING_BUG in 0.9.8, buggy
		options &= ~SSL_OP_TLS_BLOCK_PADDING_BUG;
#endif
		SSL_CTX_set_options(this->ssl_ctx, options);
		
		SSL_CTX_set_verify( this->ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE, 0);
		SSL_CTX_set_verify_depth( this->ssl_ctx, 5);

		if( !cert.isNull() ){
			/* Add a client Certificate */
			MRef<PrivateKey*> pk = ssl_cert->getPk();
			MRef<OsslPrivateKey*> ssl_pk =
				dynamic_cast<OsslPrivateKey*>(*pk);

			if( !ssl_pk || SSL_CTX_use_PrivateKey( this->ssl_ctx, 
			ssl_pk->getOpensslPrivateKey() ) <= 0 ){
				cerr << "SSL: Could not use private key" << endl;
				ERR_print_errors_fp(stderr);
				throw TLSContextInitFailed(); 
			}
			if( SSL_CTX_use_certificate( this->ssl_ctx,
			ssl_cert->getOpensslCertificate() ) <= 0 ){
				cerr << "SSL: Could not use Certificate" << endl;
				ERR_print_errors_fp(stderr);
				throw TLSContextInitFailed(); 
			}
		}

		if( !cert_db.isNull() ){
			/* Use this database for the Certificates check */
			SSL_CTX_set_cert_store( this->ssl_ctx, 
						ssl_db->getDb());
		}

		//SSL_CTX_set_session_cache_mode( this->ssl_ctx, SSL_SESS_CACHE_BOTH );
		SSL_CTX_set_session_cache_mode( this->ssl_ctx, SSL_SESS_CACHE_SERVER );
		SSL_CTX_set_session_id_context( this->ssl_ctx, sid_ctx, (unsigned int)strlen( (const char *)sid_ctx ) );
		
		ssl_ctx = this->ssl_ctx;
	}
	
	sock = ssock;
	peerAddress = sock->getPeerAddress()->clone();

	// Initialize ssl in priv
	priv = SSL_new( this->ssl_ctx );
	
	//FIXME ... this client side cache works?? only if only one host to connect to
	if( this->ssl_ctx->session_cache_head != NULL )
		SSL_set_session( ssl, this->ssl_ctx->session_cache_head );
	
	//SSL_set_verify( this->ssl, SSL_VERIFY_PEER, NULL );

	SSL_set_fd( ssl, sock->getFd() );
	// FIXME
	fd = sock->getFd();

	int32_t err = SSL_connect( ssl );

	if( err <= 0 ){
		cerr << "SSL: connect failed" << endl;
		ERR_print_errors_fp(stderr);
		throw TLSConnectFailed( err, ssl );
	}

	try{
		peer_cert = new OsslCertificate( SSL_get_peer_certificate (ssl) );
	}
	catch( CertificateException &){
		//FIXME
		cerr << "Could not get server Certificate" << endl;
		peer_cert = NULL;
	}
	
}


OsslSocket::~OsslSocket(){
#ifdef DEBUG_OUTPUT
	cerr << "TLS: Shutting down TLS Socket" << endl;
#endif	
	SSL_shutdown( ssl );
	SSL_free( ssl );
	//SSL_CTX_free( ssl_ctx );
	//delete tcp_socket;
	//delete peerAddress;
}

int32_t OsslSocket::write( string data ){
	return SSL_write( ssl, data.c_str(), (int)data.length() );
}

int32_t OsslSocket::write( const void *buf, int32_t count ){
	return SSL_write( ssl, buf, count );
}

OsslSocket& operator<<(OsslSocket& sock, string str){
	sock.write(str);
	return sock;
}

int32_t OsslSocket::read( void *buf, int32_t count ){
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

int32_t OsslSocket::setSSLCTXCiphers ( SSL_CTX *_ctx, int8_t listIdx ) {
	const char *ciphers;
	
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
		cerr << "ERROR: OsslSocket::setSSLCiphers: failed to set cipher list" << endl;
#endif	
		return 0;
	} else return 1;
}

