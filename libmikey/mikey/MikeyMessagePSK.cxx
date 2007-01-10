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


#include<config.h>

#include<libmikey/MikeyException.h>
#include"MikeyMessagePSK.h"
#include<libmikey/MikeyPayload.h>
#include<libmikey/MikeyPayloadHDR.h>
#include<libmikey/MikeyPayloadT.h>
#include<libmikey/MikeyPayloadRAND.h>
#include<libmikey/MikeyPayloadKEMAC.h>
#include<libmikey/MikeyPayloadV.h>
#include<libmikey/MikeyPayloadERR.h>
#include<libmikey/MikeyPayloadID.h>
#include<libmikey/KeyAgreementPSK.h>
#include<libmikey/MikeyPayloadSP.h>
#include<libmcrypto/rand.h>

using namespace std;

MikeyMessagePSK::MikeyMessagePSK(){
}

MikeyMessagePSK::MikeyMessagePSK( KeyAgreementPSK * ka,
				  int encrAlg, int macAlg ){

	unsigned int csbId = ka->csbId();
	MikeyPayloadT * tPayload;
	MikeyPayloadRAND * randPayload;

	if( !csbId ){
		Rand::randomize( &csbId, sizeof( csbId ));
		ka->setCsbId( csbId );
	}

	addPayload( 
		new MikeyPayloadHDR( HDR_DATA_TYPE_PSK_INIT, 1, 
			HDR_PRF_MIKEY_1, csbId,
			ka->nCs(), ka->getCsIdMapType(), 
			ka->csIdMap() ) );

	addPayload( tPayload = new MikeyPayloadT() );

	addPolicyToPayload( ka ); //Is in MikeyMessage.cxx

	// keep a copy of the time stamp
	uint64_t t = tPayload->ts();
	ka->setTSent( t );

	addPayload( randPayload = new MikeyPayloadRAND() );
	//keep a copy of the random value
	ka->setRand(randPayload->randData(), 
				randPayload->randLength() );

	// Derive the transport keys from the PSK:
	byte_t * encrKey = NULL;
	byte_t * iv = NULL;
	unsigned int encrKeyLength = 0;

	deriveTranspKeys( ka, encrKey, iv, encrKeyLength,
			  encrAlg, macAlg, t, NULL );
	
	MikeyPayloadKeyData * keydata = 
		new MikeyPayloadKeyData( 
				KEYDATA_TYPE_TGK, ka->tgk(),
				ka->tgkLength(), 
				ka->keyValidity() );
	
	byte_t * rawKeyData = new byte_t[ keydata->length() ];
	keydata->writeData( rawKeyData, keydata->length() );
	
	addKemacPayload( rawKeyData,
			 keydata->length(),
			 encrKey, iv, ka->authKey,
			 encrAlg, macAlg );

	if( encrKey != NULL )
		delete [] encrKey;

	if( iv != NULL )
		delete [] iv;
	
	delete keydata;
	delete [] rawKeyData;
}

//-----------------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------------//

void MikeyMessagePSK::setOffer( KeyAgreement * kaBase ){
	KeyAgreementPSK* ka = dynamic_cast<KeyAgreementPSK*>(kaBase);

	if( !ka ){
		throw MikeyExceptionMessageContent( 
				"Not a PSK keyagreement" );
	}

	MRef<MikeyPayload *> i = extractPayload( MIKEYPAYLOAD_HDR_PAYLOAD_TYPE );
	bool error = false;
	//uint32_t csbId;
	MRef<MikeyCsIdMap *> csIdMap;
	MRef<MikeyMessage *> errorMessage = new MikeyMessage();
	//uint8_t nCs;

	if( i.isNull() || 
		i->payloadType() != MIKEYPAYLOAD_HDR_PAYLOAD_TYPE ){
		throw MikeyExceptionMessageContent( 
				"PSK init message had no HDR payload" );
	}

#define hdr ((MikeyPayloadHDR *)(*i))
	if( hdr->dataType() != HDR_DATA_TYPE_PSK_INIT ){
		throw MikeyExceptionMessageContent( 
				"Expected PSK init message" );
	}

	ka->setnCs( hdr->nCs() );
	ka->setCsbId( hdr->csbId() );
	ka->setV(hdr->v());

	if( hdr->csIdMapType() == HDR_CS_ID_MAP_TYPE_SRTP_ID || hdr->csIdMapType() == HDR_CS_ID_MAP_TYPE_IPSEC4_ID ){
		ka->setCsIdMap( hdr->csIdMap() );
		ka->setCsIdMapType( hdr->csIdMapType() );
	}
	else{
		throw MikeyExceptionMessageContent( 
				"Unknown type of CS ID map" );
	}
	

#undef hdr
	errorMessage->addPayload(
			new MikeyPayloadHDR( HDR_DATA_TYPE_ERROR, 0,
			HDR_PRF_MIKEY_1, ka->csbId(),
			ka->nCs(), ka->getCsIdMapType(), 
			ka->csIdMap() ) );

	//FIXME look at the other fields!

	remove( i );
	i = extractPayload( MIKEYPAYLOAD_T_PAYLOAD_TYPE );

	if( i.isNull() )
		throw MikeyExceptionMessageContent( 
				"PSK init message had no T payload" );

#define plT ((MikeyPayloadT*)*i)
	if( plT->checkOffset( MAX_TIME_OFFSET ) ){
		error = true;
		errorMessage->addPayload( 
			new MikeyPayloadERR( MIKEY_ERR_TYPE_INVALID_TS ) );
	}	

	ka->t_received = plT->ts();
	
	remove( i );
#undef plT

	addPolicyTo_ka(ka); //Is in MikeyMessage.cxx

	i = extractPayload( MIKEYPAYLOAD_RAND_PAYLOAD_TYPE );

	if( i.isNull() ){
		error = true;
		errorMessage->addPayload( 
			new MikeyPayloadERR( MIKEY_ERR_TYPE_UNSPEC ) );
	}	
#define plRand ((MikeyPayloadRAND*)*i)

	// FIXME i can be NULL
	ka->setRand( plRand->randData(),
			plRand->randLength() );

	remove( i );
#undef plRand

	i = extractPayload( MIKEYPAYLOAD_ID_PAYLOAD_TYPE );

	//FIXME treat the case of an ID payload
	if( !i.isNull() ){
		remove( i );
	}

	i = extractPayload( MIKEYPAYLOAD_KEMAC_PAYLOAD_TYPE );

	if( i.isNull() ){
		error = true;
		errorMessage->addPayload( 
			new MikeyPayloadERR( MIKEY_ERR_TYPE_UNSPEC ) );
	}	

	// FIXME i can be NULL
#define kemac ((MikeyPayloadKEMAC *)*i)
	int encrAlg = kemac->encrAlg();
	int macAlg  = kemac->macAlg();
	ka->macAlg = macAlg;

	// Derive the transport keys
	byte_t * encrKey=NULL;
	byte_t * iv=NULL;
	unsigned int encrKeyLength = 0;
	
	if( !deriveTranspKeys( ka, encrKey, iv, encrKeyLength, encrAlg,
			       macAlg, ka->t_received, *errorMessage ) ){
		if( encrKey != NULL )
			delete [] encrKey;
		if( iv != NULL )
			delete [] iv;
		
		// We always build the error message with HMAC_SHA1, not
		// sure about this

		unsigned int authKeyLength = 20;
		byte_t* authKey = new byte_t[20];
		ka->genTranspAuthKey( authKey, 20 );
		
		errorMessage->addVPayload( MIKEY_MAC_HMAC_SHA1_160, 
				ka->t_received, authKey, authKeyLength  );
		
		delete [] authKey;
		throw MikeyExceptionMessageContent( errorMessage );
	}

	// decrypt the TGK
	// TODO handle parse failure.
	MRef<MikeyPayloads*> subPayloads = 
		kemac->decodePayloads( MIKEYPAYLOAD_KEYDATA_PAYLOAD_TYPE,
				 encrKey, encrKeyLength, iv );
	list<MRef<MikeyPayload *> >::iterator iPayload =
		subPayloads->firstPayload();
	MikeyPayloadKeyData *keyData =
		dynamic_cast<MikeyPayloadKeyData*>(**iPayload);
	// FIXME: assume only one KeyData subpayload, I don't know what
	// to do of more keys. Ask Ericsson
	int tgkLength = keyData->keyDataLength();
	byte_t * tgk = keyData->keyData();

	ka->setTgk( tgk, tgkLength );
	ka->setKeyValidity( keyData->kv() );
#undef kemac

	if( encrKey != NULL )
		delete [] encrKey;
	if( iv != NULL )
		delete [] iv;
}

//-----------------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------------//

MRef<MikeyMessage *> MikeyMessagePSK::buildResponse( KeyAgreement * kaBase ){
	KeyAgreementPSK* ka = dynamic_cast<KeyAgreementPSK*>(kaBase);

	if( !ka ){
		throw MikeyExceptionMessageContent( 
				"Not a PSK keyagreement" );
	}
	
	if( ka->getV() || ka->getCsIdMapType() == HDR_CS_ID_MAP_TYPE_IPSEC4_ID ){
		// Build the response message
		MRef<MikeyMessage *> result = new MikeyMessage();
		result->addPayload( 
			new MikeyPayloadHDR( HDR_DATA_TYPE_PSK_RESP, 0, 
			HDR_PRF_MIKEY_1, ka->csbId(),
			ka->nCs(), ka->getCsIdMapType(), 
			ka->csIdMap() ) );

		result->addPayload( new MikeyPayloadT() );

		addPolicyToPayload( ka ); //Is in MikeyMessage.cxx

		result->addVPayload( ka->macAlg, ka->t_received, 
				ka->authKey, ka->authKeyLength );

		if( ka->authKey != NULL ){
			delete [] ka->authKey;
			ka->authKey = NULL;
		}

		return result;
	}
	
	if( ka->authKey != NULL ){
		delete [] ka->authKey;
		ka->authKey = NULL;
	}
	
	return NULL;
}

MRef<MikeyMessage*> MikeyMessagePSK::parseResponse( KeyAgreement * kaBase ){
	KeyAgreementPSK* ka = dynamic_cast<KeyAgreementPSK*>(kaBase);

	if( !ka ){
		throw MikeyExceptionMessageContent( 
				"Not a PSK keyagreement" );
	}

	MRef<MikeyPayload *> i = extractPayload( MIKEYPAYLOAD_HDR_PAYLOAD_TYPE );
	bool error = false;
	MRef<MikeyMessage *> errorMessage = new MikeyMessage();
	MRef<MikeyCsIdMap *> csIdMap;
	uint8_t nCs;
	
	if( i.isNull() ||
		i->payloadType() != MIKEYPAYLOAD_HDR_PAYLOAD_TYPE ){

		throw MikeyExceptionMessageContent( 
				"PSK response message had no HDR payload" );
	}

#define hdr ((MikeyPayloadHDR *)(*i))
	if( hdr->dataType() != HDR_DATA_TYPE_PSK_RESP )
		throw MikeyExceptionMessageContent( 
				"Expected PSK response message" );

	if( hdr->csIdMapType() == HDR_CS_ID_MAP_TYPE_SRTP_ID || hdr->csIdMapType() == HDR_CS_ID_MAP_TYPE_IPSEC4_ID){
		csIdMap = hdr->csIdMap();
	}
	else{
		throw MikeyExceptionMessageContent( 
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

	if( i.isNull() ){
		error = true;
		errorMessage->addPayload( 
			new MikeyPayloadERR( MIKEY_ERR_TYPE_UNSPEC ) );
	}	

	// FIXME i can be NULL
#define plT ((MikeyPayloadT*)*i)
	if( plT->checkOffset( MAX_TIME_OFFSET ) ){
		error = true;
		errorMessage->addPayload( 
			new MikeyPayloadERR( MIKEY_ERR_TYPE_INVALID_TS ) );
	}	

	uint64_t t_received = plT->ts();
#undef plT

	if( error ){
		byte_t authKey[20];
		unsigned int authKeyLength = 20;

		ka->genTranspAuthKey( authKey, 20 );
		
		errorMessage->addVPayload( MIKEY_MAC_HMAC_SHA1_160, 
				t_received, authKey, authKeyLength  );

		throw MikeyExceptionMessageContent( errorMessage );
	}
	addPolicyTo_ka(ka); //Is in MikeyMessage.cxx
	return NULL;
}

bool MikeyMessagePSK::authenticate( KeyAgreement * kaBase ){
	KeyAgreementPSK* ka = dynamic_cast<KeyAgreementPSK*>(kaBase);

	if( !ka ){
		throw MikeyExceptionMessageContent( 
				"Not a PSK keyagreement" );
	}

	MRef<MikeyPayload *> payload = *(lastPayload());
 
	if( ka->rand() == NULL ){
		MRef<MikeyPayload *> pl =
			extractPayload(MIKEYPAYLOAD_RAND_PAYLOAD_TYPE );
		
		if( pl.isNull() ){
			ka->setAuthError(
				"The MIKEY init has no"
				"RAND payload."
			);
			
			return true;
		}

		MikeyPayloadRAND * randPayload;
		randPayload = (MikeyPayloadRAND*)*pl;
		
		ka->setRand( randPayload->randData(), 
			     randPayload->randLength() );
	}

	if( isInitiatorMessage() )
	{
		if( payload->payloadType() != MIKEYPAYLOAD_KEMAC_PAYLOAD_TYPE){
			throw MikeyException( 
			   "PSK init did not end with a KEMAC payload" );
		}
		
		ka->setCsbId( csbId() );

		if( !verifyKemac( ka, false ) ){
			return true;
		}

		return false;

	}
	else if( isResponderMessage() )
	{
		if( ka->csbId() != csbId() ){
			ka->setAuthError( "CSBID mismatch\n" );
			return true;
		}

		if( !verifyV( ka ) ){
			return true;
		}

		return false;
	}
	else{
		throw MikeyException( "Invalide type for a PSK message" );
	}
}

bool MikeyMessagePSK::isInitiatorMessage() const{
	return type() == MIKEY_TYPE_PSK_INIT;
}

bool MikeyMessagePSK::isResponderMessage() const{
	return type() == MIKEY_TYPE_PSK_RESP;
}

int32_t MikeyMessagePSK::keyAgreementType() const{
	return KEY_AGREEMENT_TYPE_PSK;
}
