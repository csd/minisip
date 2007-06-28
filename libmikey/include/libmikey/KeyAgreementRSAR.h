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

#ifndef KEYAGREEMENTRSAR_H
#define KEYAGREEMENTRSAR_H

#include <libmikey/KeyAgreementPKE.h>
#include <libmcrypto/cert.h>

/**
 * Instances of this class are used to store the necessary information about
 * the keys used in the security protocol SRTP
 * It contains the necessary methods to derive the keys used
 */
class LIBMIKEY_API KeyAgreementRSAR : public KeyAgreementPKE{
	public:
	
		KeyAgreementRSAR( MRef<CertificateChain *> cert, 
				  MRef<CertificateSet *> CertificateSet );
	    
		/**
		 * Destructor deletes some objects to prevent memory leaks
		 */
		~KeyAgreementRSAR();
	    
		MikeyMessage* createMessage();
};
#endif //KEYAGREEMENTRSAR_H
