/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<config.h>
#include<libmikey/MikeyException.h>
#include<libmikey/MikeyMessage.h>

#include<libmikey/MikeyMessage.h>
#include<libmikey/MikeyPayload.h>
#include<libmikey/MikeyPayloadHDR.h>
#include<libmikey/MikeyPayloadT.h>
#include<libmikey/MikeyPayloadRAND.h>
#include<libmikey/MikeyPayloadKEMAC.h>
#include<libmikey/MikeyPayloadV.h>
#include<libmikey/MikeyPayloadID.h>
#include<libmikey/MikeyPayloadERR.h>
#include<libmikey/MikeyException.h>
#include<libmikey/keyagreement_psk.h>

#define MAX_TIME_OFFSET 0xe1000000000LL //1 hour
                                                                                



MikeyMessage::MikeyMessage( KeyAgreementPSK * ka,
		        int encrAlg, int macAlg ):
			compiled(false), 
			rawData(NULL){

	unsigned int csbId = rand();
	ka->setCsbId( csbId );
	MikeyPayloadT * tPayload;
	MikeyPayloadRAND * randPayload;

	addPayload( 
		new MikeyPayloadHDR( HDR_DATA_TYPE_PSK_INIT, 1, 
			HDR_PRF_MIKEY_1, csbId,
			ka->nCs(), HDR_CS_ID_MAP_TYPE_SRTP_ID, 
			ka->csIdMap() ) );

	addPayload( tPayload = new MikeyPayloadT() );

	// keep a copy of the time stamp
	uint64_t t = tPayload->ts();
	ka->setTSent( t );

	addPayload( randPayload = new MikeyPayloadRAND() );
	//keep a copy of the random value
	ka->setRand(randPayload->randData(), 
				randPayload->randLength() );

	// Derive the transport keys from the PSK:
	byte_t * encrKey;
	byte_t * authKey;
	byte_t * saltKey;
	byte_t iv[16];
	int i;

	switch( encrAlg ){
		case MIKEY_ENCR_AES_CM_128:
			encrKey = new byte_t[16];
			ka->genTranspEncrKey( encrKey, 16 );
			saltKey = new byte_t[14];
			ka->genTranspSaltKey( saltKey, 14 );
			iv[0] = saltKey[0];
			iv[1] = saltKey[1];
			for( i = 2; i < 6; i++ ){
				iv[i] = saltKey[i] ^ (csbId >> (5-i)*8) & 0xFF;
			}

			for( i = 6; i < 14; i++ ){
				iv[i] = saltKey[i] ^ (t >> (13-i)) & 0xFF;
			}
			iv[14] = 0x00;
			iv[15] = 0x00;
			break;
		case MIKEY_ENCR_NULL:
			encrKey = NULL;
			saltKey = NULL;
			break;
		case MIKEY_ENCR_AES_KW_128:
			//TODO
		default:
			throw new MikeyException( "Unknown encryption algorithm" );
	}
	switch( macAlg ){
		case MIKEY_MAC_HMAC_SHA1_160:
			authKey = new byte_t[20];
			ka->genTranspAuthKey( authKey, 20 );
			break;
		case MIKEY_MAC_NULL:
			authKey = NULL;
			break;
		default:
			throw new MikeyException( "Unknown MAC algorithm" );
	}
	
	MikeyPayloadKeyData * keydata = 
		new MikeyPayloadKeyData( 
				KEYDATA_TYPE_TGK, ka->tgk(),
				ka->tgkLength(), 
				ka->keyValidity() );
	
	byte_t * rawKeyData = new byte_t[ keydata->length() ];
	keydata->writeData( rawKeyData, keydata->length() );
	
	addKemacPayload( rawKeyData,
			 keydata->length(),
			 encrKey, iv, authKey,
			 encrAlg, macAlg );

	if( encrKey != NULL )
		delete [] encrKey;

	if( saltKey != NULL )
		delete [] saltKey;
	
	if( authKey != NULL )
		delete [] authKey;

	delete keydata;
	delete [] rawKeyData;
}

MikeyMessage * MikeyMessage::buildResponse( KeyAgreementPSK * ka ){

	MikeyPayload * i = extractPayload( MIKEYPAYLOAD_HDR_PAYLOAD_TYPE );
	bool error = false;
	int v = false;
	uint32_t csbId;
	uint64_t t_received;
	MRef<MikeyCsIdMap *> csIdMap;
	MikeyMessage * errorMessage = new MikeyMessage();
	uint8_t nCs;

	if( i == NULL || 
		i->payloadType() != MIKEYPAYLOAD_HDR_PAYLOAD_TYPE ){
		throw new MikeyExceptionMessageContent( 
				"PSK init message had no HDR payload" );
	}

#define hdr ((MikeyPayloadHDR *)(i))
	if( hdr->dataType() != HDR_DATA_TYPE_PSK_INIT ){
		throw new MikeyExceptionMessageContent( 
				"Expected PSK init message" );
	}


	v = hdr->v(); 
	csbId = hdr->csbId();
	ka->setCsbId( csbId );
	if( hdr->csIdMapType() == HDR_CS_ID_MAP_TYPE_SRTP_ID ){
		csIdMap = hdr->csIdMap();
	}
	else{
		throw new MikeyExceptionMessageContent( 
				"Unknown type of CS ID map" );
	}
	
	ka->setCsIdMap( csIdMap );
	nCs = hdr->nCs();

#undef hdr
	errorMessage->addPayload(
			new MikeyPayloadHDR( HDR_DATA_TYPE_ERROR, 0,
			HDR_PRF_MIKEY_1, csbId,
			nCs, HDR_CS_ID_MAP_TYPE_SRTP_ID, 
			csIdMap ) );

	//FIXME look at the other fields!

	remove( i );
	i = extractPayload( MIKEYPAYLOAD_T_PAYLOAD_TYPE );

	if( i == NULL )
		throw new MikeyExceptionMessageContent( 
				"PSK init message had no T payload" );

	if( ((MikeyPayloadT*)i)->checkOffset( MAX_TIME_OFFSET ) ){
		error = true;
		errorMessage->addPayload( 
			new MikeyPayloadERR( MIKEY_ERR_TYPE_INVALID_TS ) );
	}	

	t_received = ((MikeyPayloadT*)i)->ts();
	
	remove( i );
	i = extractPayload( MIKEYPAYLOAD_RAND_PAYLOAD_TYPE );

	if( i == NULL ){
		error = true;
		errorMessage->addPayload( 
			new MikeyPayloadERR( MIKEY_ERR_TYPE_UNSPEC ) );
	}	

	ka->setRand( ((MikeyPayloadRAND *)i)->randData(),
			((MikeyPayloadRAND *)i)->randLength() );

	remove( i );
	i = extractPayload( MIKEYPAYLOAD_ID_PAYLOAD_TYPE );

	//FIXME treat the case of an ID payload
	if( i != NULL ){
		remove( i );
	}

	i = extractPayload( MIKEYPAYLOAD_KEMAC_PAYLOAD_TYPE );

	if( i == NULL ){
		error = true;
		errorMessage->addPayload( 
			new MikeyPayloadERR( MIKEY_ERR_TYPE_UNSPEC ) );
	}	

#define kemac ((MikeyPayloadKEMAC *)i)
	int encrAlg = kemac->encrAlg();
	int macAlg  = kemac->macAlg();

	// Derive the transport keys
	byte_t * encrKey;
	byte_t * authKey;
	byte_t * saltKey;
	byte_t iv[16];
	unsigned int authKeyLength = 0;
	unsigned int encrKeyLength = 0;
	int j;
	
	switch( encrAlg ){
		case MIKEY_ENCR_AES_CM_128:
			encrKeyLength = 16;
			encrKey = new byte_t[16];
			ka->genTranspEncrKey( encrKey, 16 );
			saltKey = new byte_t[14];
			ka->genTranspSaltKey( saltKey, 14 );
			iv[0] = saltKey[0];
			iv[1] = saltKey[1];
			for( j = 2; j < 6; j++ ){
				iv[j] = saltKey[j] ^ (csbId >> (5-j)*8) & 0xFF;
			}

			for( j = 6; j < 14; j++ ){
				iv[j] = saltKey[j] ^ (t_received >> (13-j)) & 0xFF;
			}
			iv[14] = 0x00;
			iv[15] = 0x00;
			break;
		case MIKEY_ENCR_NULL:
			encrKey = NULL;
			saltKey = NULL;
			break;
		case MIKEY_ENCR_AES_KW_128:
			//TODO
		default:
			error = true;
			errorMessage->addPayload( 
				new MikeyPayloadERR( MIKEY_ERR_TYPE_INVALID_EA ) );
	}
	
	switch( macAlg ){
		case MIKEY_MAC_HMAC_SHA1_160:
			authKeyLength = 20;
			authKey = new byte_t[20];
			ka->genTranspAuthKey( authKey, 20 );
			break;
		case MIKEY_MAC_NULL:
			authKey = NULL;
			break;
		default:
			error = true;
			errorMessage->addPayload( 
				new MikeyPayloadERR( MIKEY_ERR_TYPE_INVALID_HA ) );
	}

	if( error ){
		if( authKey != NULL )
			delete [] authKey;
		if( encrKey != NULL )
			delete [] encrKey;
		if( saltKey != NULL )
			delete [] saltKey;
		
		// We always build the error message with HMAC_SHA1, not
		// sure about this

		authKeyLength = 20;
		authKey = new byte_t[20];
		ka->genTranspAuthKey( authKey, 20 );
		
		errorMessage->addVPayload( MIKEY_MAC_HMAC_SHA1_160, 
				t_received, authKey, authKeyLength  );
		
		delete [] authKey;
		throw new MikeyExceptionMessageContent( errorMessage );
	}

	// decrypt the TGK
	list<MikeyPayloadKeyData *>::iterator iKeyData = 
		kemac->keyData( encrKey, encrKeyLength, iv ).begin();
	// FIXME: assume only one KeyData subpayload, I don't know what
	// to do of more keys. Ask Ericsson
	int tgkLength = (*iKeyData)->keyDataLength();
	byte_t * tgk = (*iKeyData)->keyData();

	ka->setTgk( tgk, tgkLength );
	ka->setKeyValidity( (*iKeyData)->kv() );
#undef kemac


	if( v ){
		// Build the response message
		MikeyMessage * result = new MikeyMessage();
		result->addPayload( 
			new MikeyPayloadHDR( HDR_DATA_TYPE_PSK_RESP, 0, 
			HDR_PRF_MIKEY_1, csbId,
			nCs, HDR_CS_ID_MAP_TYPE_SRTP_ID, 
			csIdMap ) );

		result->addPayload( new MikeyPayloadT() );

		result->addVPayload( macAlg, t_received, 
				authKey, authKeyLength );

		if( authKey != NULL )
			delete [] authKey;
		if( encrKey != NULL )
			delete [] encrKey;
		if( saltKey != NULL )
			delete [] saltKey;

		return result;
	}
	
	if( authKey != NULL )
		delete [] authKey;
	if( encrKey != NULL )
		delete [] encrKey;
	if( saltKey != NULL )
		delete [] saltKey;
	
	return NULL;
}

MikeyMessage * MikeyMessage::parseResponse( KeyAgreementPSK * ka ){
	MikeyPayload * i = extractPayload( MIKEYPAYLOAD_HDR_PAYLOAD_TYPE );
	bool error = false;
	MikeyMessage * errorMessage = new MikeyMessage();
	MRef<MikeyCsIdMap *> csIdMap;
	uint8_t nCs;
	
	if( i == NULL ||
		i->payloadType() != MIKEYPAYLOAD_HDR_PAYLOAD_TYPE ){

		throw new MikeyExceptionMessageContent( 
				"PSK response message had no HDR payload" );
	}

#define hdr ((MikeyPayloadHDR *)(i))
	if( hdr->dataType() != HDR_DATA_TYPE_PSK_RESP )
		throw new MikeyExceptionMessageContent( 
				"Expected PSK response message" );

	if( hdr->csIdMapType() == HDR_CS_ID_MAP_TYPE_SRTP_ID ){
		csIdMap = hdr->csIdMap();
	}
	else{
		throw new MikeyExceptionMessageContent( 
				"Unknown type of CS ID map" );
	}

	nCs = hdr->nCs();
#undef hdr
	ka->setCsIdMap( csIdMap );

	errorMessage->addPayload(
			new MikeyPayloadHDR( HDR_DATA_TYPE_ERROR, 0,
			HDR_PRF_MIKEY_1, ka->csbId(),
			nCs, HDR_CS_ID_MAP_TYPE_SRTP_ID,
			csIdMap ) );


	remove( i );
	i = extractPayload( MIKEYPAYLOAD_T_PAYLOAD_TYPE );

	if( i == NULL ){
		error = true;
		errorMessage->addPayload( 
			new MikeyPayloadERR( MIKEY_ERR_TYPE_UNSPEC ) );
	}	

	if( ((MikeyPayloadT*)i)->checkOffset( MAX_TIME_OFFSET ) ){
		error = true;
		errorMessage->addPayload( 
			new MikeyPayloadERR( MIKEY_ERR_TYPE_INVALID_TS ) );
	}	

	uint64_t t_received = ((MikeyPayloadT*)i)->ts();

	if( error ){
		byte_t authKey[20];
		unsigned int authKeyLength = 20;

		ka->genTranspAuthKey( authKey, 20 );
		
		errorMessage->addVPayload( MIKEY_MAC_HMAC_SHA1_160, 
				t_received, authKey, authKeyLength  );

		throw new MikeyExceptionMessageContent( errorMessage );
	}

	return NULL;
}

bool MikeyMessage::authenticate( KeyAgreementPSK * ka ){

	MikeyPayload * payload = *(lastPayload());
	int i;
	int macAlg;
	byte_t * receivedMac;
	byte_t * macInput;
	unsigned int macInputLength;
	list<MikeyPayload *>::iterator payload_i;
 
	if( ka->rand() == NULL ){
		
		MikeyPayloadRAND * randPayload;
		
		randPayload = (MikeyPayloadRAND*) extractPayload(MIKEYPAYLOAD_RAND_PAYLOAD_TYPE );
		
		if( randPayload == NULL ){
			ka->setAuthError(
				"The MIKEY init has no"
				"RAND payload."
			);
			
			return true;
		}

		ka->setRand( randPayload->randData(), 
			     randPayload->randLength() );
	}

	if( type() == HDR_DATA_TYPE_PSK_INIT )
	{
		MikeyPayloadKEMAC * kemac;
		if( payload->payloadType() != MIKEYPAYLOAD_KEMAC_PAYLOAD_TYPE){
			throw new MikeyException( 
			   "PSK init did not end with a KEMAC payload" );
		}
		

		kemac = (MikeyPayloadKEMAC *)payload;
		macAlg = kemac->macAlg();
		receivedMac = kemac->macData();
		macInput = rawMessageData();
		macInputLength = rawMessageLength() - 20;
		ka->setCsbId( csbId() );
	}
	else if( type() == HDR_DATA_TYPE_PSK_RESP )
	{
		if( ka->csbId() != csbId() ){
			ka->setAuthError( "CSBID mismatch\n" );
			return false;
		}
		MikeyPayloadV * v;
		uint64_t t_sent = ka->tSent();
		if( payload->payloadType() != MIKEYPAYLOAD_V_PAYLOAD_TYPE ){
			throw new MikeyException( 
			   "PSK response did not end with a V payload" );
		}

		v = (MikeyPayloadV *)payload;
		macAlg = v->macAlg();
		receivedMac = v->verData();
		// macInput = raw_messsage without mac / sent_t
		macInputLength = rawMessageLength() - 20 + 8;
		macInput = new byte_t[macInputLength];
		memcpy( macInput, rawMessageData(), rawMessageLength() - 20 );
		
		for( i = 0; i < 8; i++ ){
			macInput[ macInputLength - i - 1 ] = 
				(t_sent >> (i*8))&0xFF;
		}
	}
	else{
		throw new MikeyException( "Invalide type for a PSK message" );
	}

	byte_t authKey[20];
	byte_t computedMac[20];
	unsigned int computedMacLength;
	
	switch( macAlg ){
		case MIKEY_MAC_HMAC_SHA1_160:
			ka->genTranspAuthKey( authKey, 20 );

			hmac_sha1( authKey, 20,
				   macInput,
				   macInputLength,
				   computedMac, &computedMacLength );

			for( i = 0; i < 20; i++ ){
				if( computedMac[i] != receivedMac[i] ){
					ka->setAuthError(
						"MAC mismatch: the shared"
						"key probably differs."
					);
					return true;
				}
			}
			return false;
		case MIKEY_MAC_NULL:
			return false;
		default:
			throw new MikeyException( "Unknown MAC algorithm" );
	}

}


