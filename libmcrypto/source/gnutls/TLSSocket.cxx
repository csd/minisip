/*
 *  Copyright (C) 2004-2006 the Minisip Team
 * 
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 * 
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 * 
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *  */

/* Copyright (C) 2006
 *
 * Authors: Erik Ehrlund <eehrlund@kth.se>
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/


#include <gnutls/gnutls.h>

#include <libmnetutil/TCPSocket.h>

#include <libmcrypto/gnutls/init.h>
#include <libmcrypto/TLSSocket.h>
#include <libmcrypto/openssl/TLSException.h>

using namespace std;

#define session ((gnutls_session_t)priv)

/************************************************************************/
void checkErr(int a)
{
	if(a<0)
	{	
		perror("An error has occured");
		throw TLSInitFailed();
		return;
	}  
}

/*********************************************************************************/
/* constructor*/
// When created by a TLS Server
TLSSocket::TLSSocket( MRef<StreamSocket *> tcp_socket, SSL_CTX * ssl_ctx )
		: sock(tcp_socket)
{
	type = SOCKET_TYPE_TLS;
	peerPort = tcp_socket->getPeerPort();
	peerAddress = tcp_socket->getPeerAddress()->clone();

	throw Exception( "TLSSocket not fully implemented" );

// 	int error;
// 	// Copy the SSL parameters, since the server still needs them
// 	// Initialize ssl in priv
// 	priv = SSL_new( ssl_ctx );
// 	this->ssl_ctx = SSL_get_SSL_CTX( ssl );

// 	SSL_set_fd( ssl, tcp_socket->getFd() );
// 	fd = tcp_socket->getFd();
	
// 	error = SSL_accept( ssl );
// 	if( error <= 0 ){
// 		cerr << "Could not establish an incoming TLS connection" << endl;
// 		ERR_print_errors_fp(stderr);
// 		throw TLSConnectFailed( error, ssl );
// 	}	
}

TLSSocket::TLSSocket(string addr, int32_t port, void * &ssl_ctx,
		     MRef<certificate *> cert,
		     MRef<ca_db *> cert_db )
{
	TLSSocket::TLSSocket_init(new TCPSocket(addr, port),
				  ssl_ctx, cert, cert_db);
}

TLSSocket::TLSSocket(IPAddress &addr, int32_t port, void * &ssl_ctx,
		     MRef<certificate *> cert,
		     MRef<ca_db *> cert_db )
{
	TLSSocket::TLSSocket_init(new TCPSocket(addr, port),
				  ssl_ctx, cert, cert_db);
}

/*********************************************************************************/
TLSSocket::~TLSSocket()
{  
	gnutls_bye (session, GNUTLS_SHUT_WR);
	gnutls_deinit (session);
	//gnutls_anon_free_client_credentials (anoncred);

// 	gnutls_global_deinit ();
}

const int g_cert_type_priority[3] = { GNUTLS_CRT_X509, GNUTLS_CRT_OPENPGP, 0 };
gnutls_certificate_credentials_t g_xcred;
MRef<StreamSocket *> g_sock;

//#define CAFILE "/etc/ssl/certs/ca-certificates.crt"
#define CAFILE "/etc/ssl/certs/ca.hem.za.org"

#define MSG "\r\n\r\n"

/*********************************************************************************/
void TLSSocket::TLSSocket_init(MRef<StreamSocket*> ssock, void * &ssl_ctx,
			       MRef<certificate *> cert,
			       MRef<ca_db *> cert_db)
{
	int err=0;

	/* init gnutls */
	libmcryptoGnutlsInit();

	/* X509 stuff */
	gnutls_certificate_allocate_credentials (&g_xcred);

	/* sets the trusted cas file
	 */
	err = gnutls_certificate_set_x509_trust_file (g_xcred, CAFILE,
						      GNUTLS_X509_FMT_PEM);
	checkErr(err);

	// Initialize session in priv
	err = gnutls_init ((gnutls_session_t*)&priv, GNUTLS_CLIENT);
	checkErr(err);

	/* Use default priorities */
	err = gnutls_set_default_priority (session);
	checkErr(err);

	err = gnutls_certificate_type_set_priority (session, g_cert_type_priority);
	checkErr(err);


	/* put the x509 credentials to the current session
	 */
	err = gnutls_credentials_set (session, GNUTLS_CRD_CERTIFICATE, g_xcred);
	checkErr(err);

	gnutls_transport_set_ptr (session,
				  (gnutls_transport_ptr_t) ssock->getFd());

	err = gnutls_handshake (session);
	if (err<0)
	{
		perror("****** HANDSHAKE FAILED ********");
		gnutls_perror(err);
		throw Exception("handshake failed");
	}

	err = gnutls_record_send (session, MSG, strlen (MSG));
	checkErr(err);

	sock = ssock;
	fd = ssock->getFd();
	peerPort = ssock->getPeerPort();
	peerAddress = ssock->getPeerAddress();
	type = SOCKET_TYPE_TLS;

	cerr << "TLSSocket::TLSSocket_init success";
	return;
}

/********************************************************************************/

int32_t TLSSocket::write(const void *msg, int length)
{
	int a ;
	cerr << "TLSSocket::write ";
	cerr.write((const char*)msg, length);
	cerr << endl;
	a = gnutls_record_send (session, msg , length);
	return a;
}
/*********************************************************************************/
int32_t TLSSocket::write(string msg)
{   
	cerr << "TLSSocket::write str " << msg << endl;
	return TLSSocket::write(msg.c_str(), msg.size());
}

/*********************************************************************************/
int32_t TLSSocket::read (void *buf, int maxlength)
{  
	int recv;
	recv = gnutls_record_recv (session, buf, maxlength);
	return recv;
}
