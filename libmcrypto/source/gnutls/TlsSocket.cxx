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
#include <libmcrypto/gnutls/TlsSocket.h>
#include <libmcrypto/TlsException.h>

using namespace std;

/************************************************************************/
static void checkErr(int a)
{
	if(a<0)
	{	
		perror("An error has occured");
		throw TLSInitFailed();
		return;
	}  
}


TLSSocket::TLSSocket()
{
}

TLSSocket::~TLSSocket()
{
}

TLSSocket* TLSSocket::connect( MRef<StreamSocket *> sock,
			       MRef<Certificate *> cert,
			       MRef<CertificateSet *> cert_db,
			       string serverName )
{
	MRef<GtlsCertificateSet*> Gtlsdb;
	MRef<GtlsCertificate*> Gtlscert;

	if( cert_db )
		Gtlsdb = (GtlsCertificateSet*)*cert_db;

	if( cert )
		Gtlscert = (GtlsCertificate*)*cert;

	return new GnutlsSocket( sock, Gtlsdb, Gtlscert );
}


/*********************************************************************************/
/* constructor*/
// When created by a TLS Server
GnutlsSocket::GnutlsSocket( MRef<StreamSocket *> tcp_socket,
			    gnutls_session_t session )
		: sock(tcp_socket)
{
	type = MSOCKET_TYPE_TLS;
	peerPort = tcp_socket->getPeerPort();
	peerAddress = tcp_socket->getPeerAddress()->clone();
 	fd = tcp_socket->getFd();
	m_session = session;

	gnutls_transport_set_ptr (m_session, (gnutls_transport_ptr_t) fd);
	
	int ret = gnutls_handshake (m_session);
	if (ret < 0){
		gnutls_deinit (m_session);
		m_session = NULL;
		fprintf(stderr, "*** Handshake has failed (%s)\n\n",
			gnutls_strerror (ret));
		throw TLSInitFailed();
	}
	printf("- Handshake was completed\n");
}

GnutlsSocket::GnutlsSocket(MRef<StreamSocket *> sock,
			   MRef<GtlsCertificateSet *> cert_db,
			   MRef<GtlsCertificate *> cert)
{
	GnutlsSocket::GnutlsSocket_init(sock,
					cert_db, cert);
}

/*********************************************************************************/
GnutlsSocket::~GnutlsSocket()
{  
	gnutls_bye (m_session, GNUTLS_SHUT_WR);
	gnutls_deinit (m_session);
	if( m_xcred ){
		gnutls_certificate_free_credentials( m_xcred );
		m_xcred = NULL;
	}

	if( m_ca_list ){
		delete[] m_ca_list;
		m_ca_list = NULL;
	}

// 	gnutls_global_deinit ();
}

const int g_cert_type_priority[3] = { GNUTLS_CRT_X509, GNUTLS_CRT_OPENPGP, 0 };

/*********************************************************************************/
void GnutlsSocket::GnutlsSocket_init( MRef<StreamSocket*> ssock,
				      MRef<GtlsCertificateSet *> cert_db,
				      MRef<GtlsCertificate *> cert )
{
	int err=0;

	/* init gnutls */
	libmcryptoGnutlsInit();

	/* X509 stuff */
	err = gnutls_certificate_allocate_credentials (&m_xcred);
	checkErr(err);

	if( cert_db ){
		if( !cert_db->getDb(&m_ca_list, &m_ca_list_len) ){
			cerr << "ca db failed" << endl;
			throw TLSContextInitFailed();
		}

		err = gnutls_certificate_set_x509_trust(m_xcred, m_ca_list, m_ca_list_len);
		checkErr(err);
	}

	if( cert ){
		// FIXME support chained certs.
		gnutls_x509_crt_t gcert = cert->getCertificate();
		gnutls_x509_privkey_t gkey = NULL;
	
		MRef<GtlsPrivateKey*> Gtlspk =
			dynamic_cast<GtlsPrivateKey*>( *cert->getPk() );

		if( Gtlspk ){
			gkey = Gtlspk->getPrivateKey();
		}

		err = gnutls_certificate_set_x509_key(m_xcred, &gcert, 1, gkey);
		checkErr(err);
	}

	// Initialize session in priv
	err = gnutls_init (&m_session, GNUTLS_CLIENT);
	checkErr(err);

	/* Use default priorities */
	err = gnutls_set_default_priority (m_session);
	checkErr(err);

	err = gnutls_certificate_type_set_priority (m_session, g_cert_type_priority);
	checkErr(err);


	/* put the x509 credentials to the current session
	 */
	err = gnutls_credentials_set (m_session, GNUTLS_CRD_CERTIFICATE, m_xcred);
	checkErr(err);

	gnutls_transport_set_ptr (m_session,
				  (gnutls_transport_ptr_t) ssock->getFd());

	err = gnutls_handshake (m_session);
	if (err<0)
	{
		perror("****** HANDSHAKE FAILED ********");
		gnutls_perror(err);
		throw Exception("handshake failed");
	}

	sock = ssock;
	fd = ssock->getFd();
	peerPort = ssock->getPeerPort();
	peerAddress = ssock->getPeerAddress();
	type = MSOCKET_TYPE_TLS;

	return;
}

/********************************************************************************/

int32_t GnutlsSocket::write(const void *msg, int length)
{
	int a ;
	a = gnutls_record_send (m_session, msg , length);
	return a;
}
/*********************************************************************************/
int32_t GnutlsSocket::write(string msg)
{   
	return GnutlsSocket::write(msg.c_str(), msg.size());
}

/*********************************************************************************/
int32_t GnutlsSocket::read (void *buf, int maxlength)
{  
	int recv;
	recv = gnutls_record_recv (m_session, buf, maxlength);
	return recv;
}
