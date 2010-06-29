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

TLSServerSocket::TLSServerSocket()
		:ServerSocket()
{
}

TLSServerSocket::~TLSServerSocket()
{
}

TLSServerSocket *TLSServerSocket::create( MRef<ServerSocket*> sock, MRef<Certificate *> cert, MRef<CertificateSet *> cert_db ){
	MRef<GtlsCertificate*> Gtlscert;
	MRef<GtlsCertificateSet*> Gtlsdb;

	if( cert )
		Gtlscert = (GtlsCertificate*)*cert;

	if( cert_db )
		Gtlsdb = (GtlsCertificateSet*)*cert_db;

	return new GnutlsServerSocket( sock, Gtlscert, Gtlsdb );
}


GnutlsServerSocket::GnutlsServerSocket( MRef<ServerSocket*> sock,
					MRef<GtlsCertificate *> cert,
					MRef<GtlsCertificateSet *> cert_db)
{
	init( sock, cert, cert_db );
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

	/* request client Certificate if any.
	 */
	gnutls_certificate_server_set_request (session, GNUTLS_CERT_REQUEST);

// 	gnutls_dh_set_prime_bits (session, DH_BITS);

	return session;
}

void GnutlsServerSocket::init( MRef<ServerSocket*> sock, 
			       MRef<GtlsCertificate *> cert,
			       MRef<GtlsCertificateSet *> cert_db)
{
	type = MSOCKET_TYPE_TLS;
	this->sock = sock;
	cerr << "GnutlsServerSocket::init" << endl;
	m_cert = cert;
	m_cert_db = cert_db;

	gnutls_certificate_allocate_credentials (&m_xcred);

	if( !cert_db->getDb(&m_ca_list, &m_ca_list_len) ){
		cerr << "ca db failed" << endl;
		throw TLSContextInitFailed();
	}

	gnutls_certificate_set_x509_trust(m_xcred, m_ca_list, m_ca_list_len);

	// FIXME support chained certs.
	gnutls_x509_crt_t gcert = cert->getCertificate();
	gnutls_x509_privkey_t gkey = NULL;
	
	MRef<GtlsPrivateKey*> Gtlspk =
		dynamic_cast<GtlsPrivateKey*>( *cert->getPk() );

	if( Gtlspk ){
		gkey = Gtlspk->getPrivateKey();
	}

	gnutls_certificate_set_x509_key(m_xcred, &gcert, 1, gkey);

	cerr << "GnutlsServerSocket::init ends" << endl;
}

MRef<StreamSocket *> GnutlsServerSocket::accept(){
	MRef<StreamSocket *> ssocket = ServerSocket::accept();

	gnutls_session_t session = initialize_tls_session();

	return new GnutlsSocket( ssocket, session );
}

MRef<StreamSocket *> GnutlsServerSocket::createSocket( int32_t sd,
						     struct sockaddr *sa,
						     int32_t salen ){
	// Unused
	return NULL;
}

int32_t GnutlsServerSocket::getFd(){
	return sock->getFd();
}

int32_t GnutlsServerSocket::getPort(){
	return sock->getPort();
}

int GnutlsServerSocket::getAddressFamily(){
	return sock->getAddressFamily();
}

MRef<IPAddress *> GnutlsServerSocket::getLocalAddress() const{
	return sock->getLocalAddress();
}
