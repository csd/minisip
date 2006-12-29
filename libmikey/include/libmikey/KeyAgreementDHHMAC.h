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


#ifndef KEYAGREEMENT_DHHMAC_H
#define KEYAGREEMENT_DHHMAC_H

#include<libmikey/libmikey_config.h>

#include<libmikey/keyagreement.h>
#include<libmcrypto/OakleyDH.h>
#include<libmutil/mtypes.h>

/**
 * HMAC-Authenticated Diffie-Hellman
 * for Multimedia Internet KEYing (MIKEY)
 * RFC 4650
 */
class LIBMIKEY_API KeyAgreementDHHMAC : public KeyAgreement{
	public:
		KeyAgreementDHHMAC( byte_t * psk, int pskLength,
				    int groupValue = -1 );
		~KeyAgreementDHHMAC();

		int setGroup( int group );
		int group();

		void setAuthKey( int macAlg, byte_t *authKey,
				 unsigned int authKeyLength );
		int getMacAlg();

		void setPeerKey( byte_t * peerKey, int peerKeyLength );
		int peerKeyLength();
		byte_t * peerKey();

		int publicKeyLength();
		byte_t * publicKey();

		int computeTgk();

		void genTranspAuthKey( byte_t * encrKey, int encrKeyLength );

		uint64_t tSent();
		void setTSent( uint64_t tSent );

		MikeyMessage* createMessage();

	private:
		byte_t * pskPtr;
		int pskLengthValue;
		OakleyDH * dh;

		byte_t * peerKeyPtr;
		int peerKeyLengthValue;

		uint64_t tSentValue;
		uint64_t t_received;
		byte_t * m_authKey;
		unsigned int m_authKeyLength;
		int m_macAlg;
};

#endif
