/*
  Copyright (C) 2005-2007 Mikael Magnusson

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

#ifndef DTLSSOCKET_H
#define DTLSSOCKET_H

#include<libmutil/MemObject.h>
#include<libmnetutil/DatagramSocket.h>
#include<libmcrypto/cert.h>

#define MSOCKET_TYPE_DTLS_UDP     0x21

class LIBMCRYPTO_API DTLSSocket : public DatagramSocket {
	public:
		virtual ~DTLSSocket();

		static DTLSSocket* create( MRef<DatagramSocket *> sock,
					   MRef<Certificate *> cert = NULL,
					   MRef<CertificateSet *> cert_db=NULL );

	protected:
		DTLSSocket();
};
#endif
