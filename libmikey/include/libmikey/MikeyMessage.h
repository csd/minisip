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

#include<libmikey/libmikey_config.h>

#include<libmikey/MikeyDefs.h>

#include<assert.h>

#include<libmikey/MikeyPayload.h>
#include<libmikey/MikeyPayloadSIGN.h>

#include<libmikey/keyagreement.h>
#include<libmikey/keyagreement_dh.h>
#include<libmikey/keyagreement_psk.h>
#include<libmikey/KeyAgreementPKE.h>
#include<libmikey/KeyAgreementDHHMAC.h>

#include<list>
#include<iostream>

#define MIKEY_TYPE_PSK_INIT    0
#define MIKEY_TYPE_PSK_RESP    1
#define MIKEY_TYPE_PK_INIT     2 
#define MIKEY_TYPE_PK_RESP     3
#define MIKEY_TYPE_DH_INIT     4
#define MIKEY_TYPE_DH_RESP     5
#define MIKEY_TYPE_ERROR       6
#define MIKEY_TYPE_DHHMAC_INIT 7
#define MIKEY_TYPE_DHHMAC_RESP 8

#define MIKEY_ENCR_NULL       0
#define MIKEY_ENCR_AES_CM_128 1
#define MIKEY_ENCR_AES_KW_128 2

#define MIKEY_MAC_NULL          0
#define MIKEY_MAC_HMAC_SHA1_160 1

#define MAX_TIME_OFFSET (int64_t)(0xe100000<<16) //1 hour

class aes;
class SipSim;
class certificate;
class certificate_db;

/**
 * MikeyMessages can be created in three different ways.
 * 1. new MikeyMessages creats an empty message
 * 2. MikeyMessage::create creats a message from a keyagreement
 * 3. MikeyMessage::parse creats a message from a binary representation
 */
class LIBMIKEY_API MikeyMessage{
	public:
 		MikeyMessage();
		static MikeyMessage* create( KeyAgreementDH * ka );
		static MikeyMessage* create( KeyAgreementDHHMAC * ka,
					     int macAlg = MIKEY_MAC_HMAC_SHA1_160);
		static MikeyMessage* create( KeyAgreementPSK * ka,
			      int encrAlg = MIKEY_ENCR_AES_CM_128, 
			      int macAlg  = MIKEY_MAC_HMAC_SHA1_160 );
		
		//added by choehn
		static MikeyMessage* create(KeyAgreementPKE* ka,
				  int encrAlg = MIKEY_ENCR_AES_CM_128,
				  int macAlg = MIKEY_MAC_HMAC_SHA1_160,
				  EVP_PKEY* privKeyInitiator = NULL);

		static MikeyMessage* parse( byte_t *message, int lengthLimit );
		static MikeyMessage* parse( std::string b64Message );

		virtual ~MikeyMessage();
		
		void addPayload( MikeyPayload * payload );
		void operator+=( MikeyPayload * payload );
		void addSignaturePayload( MRef<SipSim*> sim );
		void addSignaturePayload( MRef<certificate *> cert );
		void addVPayload( int macAlg, uint64_t receivedT,
			byte_t * authKey, uint32_t authKeyLength);
		virtual void addKemacPayload(
				byte_t * tgk, int tgkLength,
				byte_t * encrKey, byte_t * iv,
				byte_t * authKey,
				int encrAlg, int macAlg );
				
		std::string debugDump();
		byte_t * rawMessageData();
		int rawMessageLength();
		
		std::list<MikeyPayload *>::iterator firstPayload();
		std::list<MikeyPayload *>::iterator lastPayload();
		
		MikeyPayload * extractPayload( int type );
		const MikeyPayload * extractPayload( int type ) const;
		void remove( MikeyPayload * );

		int type() const;
		uint32_t csbId();

		std::string b64Message();

		virtual MikeyMessage * parseResponse( KeyAgreement  * ka );
		virtual void setOffer( KeyAgreement * ka );
		virtual MikeyMessage * buildResponse( KeyAgreement * ka );
		virtual bool authenticate( KeyAgreement  * ka );

		virtual bool isInitiatorMessage() const;
		virtual bool isResponderMessage() const;
		virtual int32_t keyAgreementType() const;

	protected:
		void addPolicyToPayload(KeyAgreement * ka);
		void addPolicyTo_ka(KeyAgreement * ka);

		std::list<MikeyPayload *> payloads;

	private:
		static void parse( byte_t *message,
				   int lengthLimit,
				   std::list<MikeyPayload *>& payloads);
		void compile();
		bool compiled;
		byte_t *rawData;
		
};

#endif
