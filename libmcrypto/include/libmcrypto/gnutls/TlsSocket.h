/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
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
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Mikael Magnusson <mikma@users.sourceforge.net>
 */

#ifndef GNUTLS_TLSSOCKET_H
#define GNUTLS_TLSSOCKET_H

#include<libmcrypto/config.h>

#include<libmnetutil/StreamSocket.h>

#include<libmcrypto/gnutls/cert.h>
#include<libmutil/mtypes.h>

#include<libmnetutil/IPAddress.h>

#include<libmutil/MemObject.h>
#include<libmcrypto/TlsSocket.h>

#include<gnutls/gnutls.h>

class LIBMCRYPTO_API GnutlsSocket : public TLSSocket {
	public:
		GnutlsSocket( MRef<StreamSocket *> sock,
			      MRef<GtlsCertificateSet *> cert_db=NULL,
			      MRef<GtlsCertificate *> cert=NULL);

		GnutlsSocket( MRef<StreamSocket *> sock,
			      gnutls_session_t session );
		
		virtual ~GnutlsSocket();

		virtual std::string getMemObjectType() const {return "GnutlsSocket";};

		virtual int32_t write(std::string);
		
		virtual int32_t write(const void *buf, int32_t count);
		
		virtual int32_t read(void *buf, int32_t count);

	private:
		void GnutlsSocket_init( MRef<StreamSocket*> ssock,
					MRef<GtlsCertificateSet *> cert_db,
					MRef<GtlsCertificate *> cert);

		gnutls_certificate_credentials_t m_xcred;
		gnutls_session_t m_session;
		
		MRef<StreamSocket *> sock;
		
		MRef<Certificate *> peer_cert;
		
		/** CA db */
		MRef<CertificateSet *> cert_db;

		gnutls_x509_crt_t* m_ca_list;
		size_t m_ca_list_len;
};
#endif
