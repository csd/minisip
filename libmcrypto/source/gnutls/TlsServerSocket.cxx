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

#include<libmcrypto/gnutls/TlsServerSocket.h>
#include<libmcrypto/gnutls/TlsSocket.h>
#include<libmcrypto/TlsException.h>

#include<sys/socket.h>

using namespace std;

TLSServerSocket::TLSServerSocket( int32_t domain, int32_t listen_port )
		:ServerSocket( domain, listen_port )
{
}

TLSServerSocket::~TLSServerSocket()
{
}

ServerSocket *TLSServerSocket::create( bool use_ipv6, int32_t listen_port, MRef<certificate *> cert, MRef<ca_db *> cert_db ){
	MRef<gtls_certificate*> gtls_cert;
	MRef<gtls_ca_db*> gtls_db;

	if( cert )
		gtls_cert = (gtls_certificate*)*cert;

	if( cert_db )
		gtls_db = (gtls_ca_db*)*cert_db;

	return new GnutlsServerSocket( use_ipv6, listen_port,
				       gtls_cert, gtls_db );
}

ServerSocket *TLSServerSocket::create(int32_t listen_port, MRef<certificate *> cert, MRef<ca_db *> cert_db ){
	return create( false, listen_port, cert, cert_db );
}


GnutlsServerSocket::GnutlsServerSocket( bool use_ipv6, int32_t listen_port, 
					MRef<gtls_certificate *> cert,
					MRef<gtls_ca_db *> cert_db):TLSServerSocket(use_ipv6?AF_INET6:AF_INET, listen_port)
{
	init(use_ipv6, listen_port, cert, cert_db);
}

GnutlsServerSocket::~GnutlsServerSocket(){ 
	if( m_xcred ){
		gnutls_certificate_free_credentials( m_xcred );
		m_xcred = NULL;
	}

	if( m_ca_list ){
		delete[] m_ca_list;
		m_ca_list = NULL;
	}
}

gnutls_session_t GnutlsServerSocket::initialize_tls_session(){
	gnutls_session_t session;

	gnutls_init (&session, GNUTLS_SERVER);

	/* avoid calling all the priority functions, since the defaults
	 * are adequate.
	 */
	gnutls_set_default_priority (session);

	gnutls_credentials_set (session, GNUTLS_CRD_CERTIFICATE, m_xcred);

	/* request client certificate if any.
	 */
	gnutls_certificate_server_set_request (session, GNUTLS_CERT_REQUEST);

// 	gnutls_dh_set_prime_bits (session, DH_BITS);

	return session;
}

void GnutlsServerSocket::init( bool use_ipv6, int32_t listen_port, 
			       MRef<gtls_certificate *> cert,
			       MRef<gtls_ca_db *> cert_db)
{
	cerr << "GnutlsServerSocket::init" << endl;
	m_cert = cert;
	m_cert_db = cert_db;

	int32_t backlog = 25;
	
	gnutls_certificate_allocate_credentials (&m_xcred);

	if( !cert_db->getDb(&m_ca_list, &m_ca_list_len) ){
		cerr << "ca db failed" << endl;
		throw TLSContextInitFailed();
	}

	gnutls_certificate_set_x509_trust(m_xcred, m_ca_list, m_ca_list_len);

	// FIXME support chained certs.
	gnutls_x509_crt_t gcert = cert->get_certificate();
	gnutls_x509_privkey_t gkey = NULL;
	
	MRef<gtls_priv_key*> gtls_pk =
		dynamic_cast<gtls_priv_key*>( *cert->get_pk() );

	if( gtls_pk ){
		gkey = gtls_pk->get_private_key();
	}

	gnutls_certificate_set_x509_key(m_xcred, &gcert, 1, gkey);

	if( use_ipv6 )
		listen("::", listen_port, backlog);
	else
		listen("0.0.0.0", listen_port, backlog);

	cerr << "GnutlsServerSocket::init ends" << endl;
}

MRef<StreamSocket *> GnutlsServerSocket::accept(){
	MRef<StreamSocket *> ssocket = ServerSocket::accept();

	gnutls_session_t session = initialize_tls_session();

	return new GnutlsSocket( ssocket, session );
}
