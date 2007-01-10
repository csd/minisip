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

#ifndef GNUTLS_TLSSERVERSOCKET_H
#define GNUTLS_TLSSERVERSOCKET_H

#include<libmcrypto/config.h>

#include<libmcrypto/TlsServerSocket.h>
#include<libmcrypto/gnutls/cert.h>

class LIBMNETUTIL_API GnutlsServerSocket : public TLSServerSocket {

	public:
		GnutlsServerSocket( bool use_ipv6, int32_t listen_port,
				    MRef<gtls_certificate *> cert,
				    MRef<gtls_ca_db *> cert_db=NULL);
		~GnutlsServerSocket();
		virtual std::string getMemObjectType() const {return "GnutlsServerSocket";}

		virtual MRef<StreamSocket *> accept();

	protected:
		virtual void init( bool use_ipv6, int32_t listen_port, 
				   MRef<gtls_certificate *> cert,
				   MRef<gtls_ca_db *> cert_db);
		gnutls_session_t initialize_tls_session();

	private:
		MRef<gtls_ca_db *> m_cert_db;
		MRef<gtls_certificate*> m_cert;

		gnutls_certificate_credentials_t m_xcred;
		gnutls_x509_crt_t* m_ca_list;
		size_t m_ca_list_len;
};
#endif
