/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien, Joachim Orrblad
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
 *	    Joachim Orrblad <joachim@orrblad.com>
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/

#ifndef MIKEYMESSAGEPKE_H
#define MIKEYMESSAGEPKE_H

#include<libmikey/libmikey_config.h>
#include<libmikey/MikeyMessage.h>
#include<libmikey/KeyAgreementPKE.h>
#include<libmcrypto/cert.h>

class LIBMIKEY_API MikeyMessagePKE: public MikeyMessage{
	public:
		MikeyMessagePKE();
 		MikeyMessagePKE(KeyAgreementPKE* ka,
				int encrAlg = MIKEY_ENCR_AES_CM_128,
				int macAlg = MIKEY_MAC_HMAC_SHA1_160 );

		MRef<MikeyMessage *> parseResponse( KeyAgreement  * ka );
		void setOffer( KeyAgreement * ka );
		MRef<MikeyMessage *> buildResponse( KeyAgreement * ka );
		bool authenticate( KeyAgreement  * ka );

		bool isInitiatorMessage() const;
		bool isResponderMessage() const;
		int32_t keyAgreementType() const;

	private:
};

#endif
