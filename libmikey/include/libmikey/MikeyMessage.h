/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien, Joachim Orrblad
  
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
*/


#ifndef MIKEYMESSAGE_H
#define MIKEYMESSAGE_H

#include<list>
#include<iostream>
#include<assert.h>
#include<libmikey/MikeyDefs.h>
#include<libmikey/MikeyPayload.h>
#include<libmikey/MikeyPayloadSIGN.h>
//#include"../util/cert.h"
//#include"../util/aes.h"
//#include"../util/hmac.h"
//#include"../util/base64.h"
#include<libmikey/keyagreement.h>
#include<libmikey/keyagreement_dh.h>
#include<libmikey/keyagreement_psk.h>

#define MIKEY_TYPE_PSK_INIT    0
#define MIKEY_TYPE_PSK_RESP    1
#define MIKEY_TYPE_PK_INIT     2 
#define MIKEY_TYPE_PK_RESP     3
#define MIKEY_TYPE_DH_INIT     4
#define MIKEY_TYPE_DH_RESP     5
#define MIKEY_TYPE_ERROR       6

#define MIKEY_ENCR_AES_CM_128 0
#define MIKEY_ENCR_NULL       1
#define MIKEY_ENCR_AES_KW_128 2

#define MIKEY_MAC_HMAC_SHA1_160 0
#define MIKEY_MAC_NULL          1

/**
 * @author Erik Eliasson, Johan Bilien
*/

class aes;
class certificate;
class certificate_db;


class MikeyMessage{
	public:
		MikeyMessage();
		MikeyMessage( byte_t *message, int lengthLimit );
		MikeyMessage( std::string b64Message );
		MikeyMessage( KeyAgreementDH * ka );
		MikeyMessage( KeyAgreementPSK * ka,
			      int encrAlg = MIKEY_ENCR_AES_CM_128, 
			      int macAlg  = MIKEY_MAC_HMAC_SHA1_160 );
		~MikeyMessage();
		
		void addPayload( MikeyPayload * payload );
		void operator+=( MikeyPayload * payload );
		void addSignaturePayload( MRef<certificate *> cert );
		void addVPayload( int macAlg, uint64_t receivedT,
			byte_t * authKey, uint32_t authKeyLength);
		void addKemacPayload(
				byte_t * tgk, int tgkLength,
				byte_t * encrKey, byte_t * iv,
				byte_t * authKey,
				int encrAlg, int macAlg );
		
		std::string debugDump();
		byte_t * rawMessageData();
		int rawMessageLength();
		
		list<MikeyPayload *>::iterator firstPayload();
		list<MikeyPayload *>::iterator lastPayload();
		
		MikeyPayload * extractPayload( int type );
		void remove( MikeyPayload * );

		int type();
		uint32_t csbId();

		std::string b64Message();

		MikeyMessage * parseResponse( KeyAgreementDH  * ka );
		MikeyMessage * parseResponse( KeyAgreementPSK * ka );

		void setOffer( KeyAgreementDH * ka );
		void setOffer( KeyAgreementPSK * ka );

		MikeyMessage * buildResponse( KeyAgreementDH  * ka );
		MikeyMessage * buildResponse( KeyAgreementPSK * ka );


		bool authenticate( KeyAgreementDH  * ka );
		bool authenticate( KeyAgreementPSK * ka );

	protected:
		list<MikeyPayload *> payloads;

	private:
		void parse( byte_t *message, int lengthLimit );
		void compile();
		void addPolicyToPayload(KeyAgreement * ka);
		void addPolicyTo_ka(KeyAgreement * ka);
		bool compiled;
		byte_t *rawData;
		
};


#endif
