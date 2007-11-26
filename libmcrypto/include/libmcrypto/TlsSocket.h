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

#ifndef TLSSOCKET_H
#define TLSSOCKET_H

#include<libmcrypto/config.h>

#include<libmutil/mtypes.h>
#include<libmutil/MemObject.h>
#include<libmnetutil/IPAddress.h>
#include<libmnetutil/StreamSocket.h>
#include<libmcrypto/cert.h>

class LIBMCRYPTO_API TLSSocket : public StreamSocket {
	public:
		virtual ~TLSSocket();

		static TLSSocket* connect( MRef<StreamSocket*> ssock,
					   MRef<Certificate *> cert=NULL,
					   MRef<CertificateSet *> cert_db=NULL,
					   std::string serverName="" );

	protected:
		TLSSocket();
};

#endif
