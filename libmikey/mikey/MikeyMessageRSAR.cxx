
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

#include <config.h>

#include "MikeyMessageRSAR.h"
#include <libmikey/MikeyPayloadHDR.h>
#include <libmikey/MikeyPayloadT.h>
#include <libmikey/MikeyPayloadRAND.h>
#include <libmikey/MikeyException.h>
#include <libmikey/MikeyPayloadCERT.h>
#include <libmikey/MikeyPayloadKeyData.h>
#include <libmikey/MikeyPayloadERR.h>
#include <libmikey/MikeyPayloadID.h>
#include <libmikey/MikeyPayloadKEMAC.h>
#include <libmikey/MikeyPayloadV.h>
#include <libmikey/MikeyPayloadPKE.h>

using namespace std;

MikeyMessageRSAR::MikeyMessageRSAR(){
}

MikeyMessageRSAR::MikeyMessageRSAR( KeyAgreementRSAR* ka ){

	unsigned int csbId = rand();
	ka->setCsbId(csbId);
	MikeyPayloadT* tPayload;
	MikeyPayloadRAND* randPayload;

	//adding header payload
	addPayload(new MikeyPayloadHDR(HDR_DATA_TYPE_RSA_R_INIT, 1, 
											HDR_PRF_MIKEY_1, csbId, ka->nCs(),
											ka->getCsIdMapType(), ka->csIdMap()));

	//adding timestamp payload
	addPayload(tPayload = new MikeyPayloadT());

	//adding security policy
	addPolicyToPayload(ka); //Is in MikeyMessage.cxx

	//keep a copy of the time stamp
	uint64_t t = tPayload->ts();
	ka->setTSent(t);

	//adding random payload
	addPayload(randPayload = new MikeyPayloadRAND());
	
	//keep a copy of the random value
	ka->setRand(randPayload->randData(), randPayload->randLength());

	// Add certificate chain (SIGN)
	addCertificatePayloads( ka->certificateChain() );

	// Add signature (T)
	addSignaturePayload( ka->certificateChain()->get_first() );
}

void MikeyMessageRSAR::setOffer(KeyAgreement* kaBase){
	KeyAgreementRSAR* ka = dynamic_cast<KeyAgreementRSAR*>(kaBase);

	if( !ka ){
		throw MikeyExceptionMessageContent( 
				"Not a RSAR keyagreement" );
	}

	MikeyPayload* i = extractPayload( MIKEYPAYLOAD_HDR_PAYLOAD_TYPE );
	bool error = false;
	//uint32_t csbId;
	MRef<MikeyCsIdMap*> csIdMap;
	MikeyMessage* errorMessage = new MikeyMessage();
	//uint8_t nCs;

	if( i == NULL || 
		i->payloadType() != MIKEYPAYLOAD_HDR_PAYLOAD_TYPE ){
		throw MikeyExceptionMessageContent( 
				"RSAR init message had no HDR payload" );
	}

#define hdr ((MikeyPayloadHDR *)(i))
	if( hdr->dataType() != HDR_DATA_TYPE_RSA_R_INIT ){
		throw MikeyExceptionMessageContent( 
				"Expected RSAR init message" );
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

	if( i == NULL )
		throw MikeyExceptionMessageContent( 
				"RSAR init message had no T payload" );

	// FIXME i can be NULL
	if( ((MikeyPayloadT*)i)->checkOffset( MAX_TIME_OFFSET ) ){
		error = true;
		errorMessage->addPayload( 
			new MikeyPayloadERR( MIKEY_ERR_TYPE_INVALID_TS ) );
	}	

	ka->t_received = ((MikeyPayloadT*)i)->ts();
	
	remove( i );

	addPolicyTo_ka(ka); //Is in MikeyMessage.cxx

	i = extractPayload( MIKEYPAYLOAD_RAND_PAYLOAD_TYPE );

	if( i == NULL ){
		error = true;
		errorMessage->addPayload( 
			new MikeyPayloadERR( MIKEY_ERR_TYPE_UNSPEC ) );
	}	

	// FIXME i can be NULL
	ka->setRand( ((MikeyPayloadRAND *)i)->randData(),
			((MikeyPayloadRAND *)i)->randLength() );

	remove( i );
	i = extractPayload( MIKEYPAYLOAD_ID_PAYLOAD_TYPE );

	//FIXME treat the case of an ID payload
	if( i != NULL ){
		remove( i );
	}

}

MikeyMessage* MikeyMessageRSAR::buildResponse(KeyAgreement* kaBase){
	KeyAgreementRSAR* ka = dynamic_cast<KeyAgreementRSAR*>(kaBase);

	if( !ka ){
		throw MikeyExceptionMessageContent( 
				"Not a RSAR keyagreement" );
	}
	
	// Build the response message
	MikeyMessageRSAR * result = new MikeyMessageRSAR();
	result->addPayload( 
			   new MikeyPayloadHDR( HDR_DATA_TYPE_RSA_R_RESP, 0, 
						HDR_PRF_MIKEY_1, ka->csbId(),
						ka->nCs(), ka->getCsIdMapType(), 
						ka->csIdMap() ) );

	MikeyPayloadT* tPayload = new MikeyPayloadT();
	result->addPayload( tPayload );

	//keep a copy of the time stamp
	uint64_t t = tPayload->ts();
	ka->setTSent(t);

	//adding random payload
	MikeyPayloadRAND* randPayload = NULL;
	result->addPayload(randPayload = new MikeyPayloadRAND());

	// Add certificate chain
	result->addCertificatePayloads( ka->certificateChain() );

	// TODO move encrAlg and macAlg to method or ctor parameter
	int encrAlg = MIKEY_ENCR_AES_CM_128;
	int macAlg = MIKEY_MAC_HMAC_SHA1_160;

	result->addPkeKemac( ka, encrAlg, macAlg );

	result->addSignaturePayload( ka->certificateChain()->get_first() );

	return result;
}

MikeyMessage * MikeyMessageRSAR::parseResponse( KeyAgreement * kaBase ){
	KeyAgreementRSAR* ka = dynamic_cast<KeyAgreementRSAR*>(kaBase);

	if( !ka ){
		throw MikeyExceptionMessageContent( 
				"Not a RSAR keyagreement" );
	}

	MikeyPayload * i = extractPayload( MIKEYPAYLOAD_HDR_PAYLOAD_TYPE );
	bool error = false;
	MikeyMessage * errorMessage = new MikeyMessage();
	MRef<MikeyCsIdMap *> csIdMap;
	uint8_t nCs;
	
	if( i == NULL ||
		i->payloadType() != MIKEYPAYLOAD_HDR_PAYLOAD_TYPE ){

		throw MikeyExceptionMessageContent( 
				"RSAR response message had no HDR payload" );
	}

#define hdr ((MikeyPayloadHDR *)(i))
	if( hdr->dataType() != HDR_DATA_TYPE_RSA_R_RESP )
		throw MikeyExceptionMessageContent( 
				"Expected RSAR response message" );

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

	if( i == NULL ){
		error = true;
		errorMessage->addPayload( 
			new MikeyPayloadERR( MIKEY_ERR_TYPE_UNSPEC ) );
	}	

	// FIXME i can be NULL
	if( ((MikeyPayloadT*)i)->checkOffset( MAX_TIME_OFFSET ) ){
		error = true;
		errorMessage->addPayload( 
			new MikeyPayloadERR( MIKEY_ERR_TYPE_INVALID_TS ) );
	}	

	uint64_t t_received = ((MikeyPayloadT*)i)->ts();

	i = extractPayload( MIKEYPAYLOAD_KEMAC_PAYLOAD_TYPE );

	if( i == NULL ){
		error = true;
		errorMessage->addPayload( 
			new MikeyPayloadERR( MIKEY_ERR_TYPE_UNSPEC ) );
	}	

	// FIXME handle i == NULL
#define kemac ((MikeyPayloadKEMAC *)i)
	int encrAlg = kemac->encrAlg();
	int macAlg  = kemac->macAlg();
	ka->macAlg = macAlg;

	// Derive the transport keys
	byte_t * encrKey=NULL;
	byte_t * iv=NULL;
	unsigned int encrKeyLength = 0;
	
	if( !deriveTranspKeys( ka, encrKey, iv, encrKeyLength,
			      encrAlg, macAlg, t_received,
			      errorMessage ) ){
		if( encrKey != NULL )
			delete [] encrKey;
		if( iv != NULL )
			delete [] iv;

		unsigned int authKeyLength = 20;
		byte_t* authKey = new byte_t[ authKeyLength ];
		ka->genTranspAuthKey( authKey, authKeyLength );
		
		errorMessage->addVPayload( MIKEY_MAC_HMAC_SHA1_160, 
				ka->t_received, authKey, authKeyLength  );
		
		delete [] authKey;
		throw MikeyExceptionMessageContent( errorMessage );
	}
	
	// decrypt the TGK
	MikeyPayloads* subPayloads = 
		kemac->decodePayloads( MIKEYPAYLOAD_ID_PAYLOAD_TYPE,
				 encrKey, encrKeyLength, iv );
	
	if( encrKey != NULL ){
		delete [] encrKey;
		encrKey = NULL;
	}
	if( iv != NULL ){
		delete [] iv;
		iv = NULL;
	}

	MikeyPayloadKeyData *keyData =
		dynamic_cast<MikeyPayloadKeyData*>(subPayloads->extractPayload( MIKEYPAYLOAD_KEYDATA_PAYLOAD_TYPE ));

	int tgkLength = keyData->keyDataLength();
	byte_t * tgk = keyData->keyData();

	ka->setTgk( tgk, tgkLength );
	ka->setKeyValidity( keyData->kv() );
#undef kemac

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

bool MikeyMessageRSAR::authenticate(KeyAgreement* kaBase){
	KeyAgreementRSAR* ka = dynamic_cast<KeyAgreementRSAR*>(kaBase);

	if( !ka ){
		throw MikeyExceptionMessageContent( 
				"Not a RSAR keyagreement" );
	}
	
	MikeyPayload * payload = *(lastPayload());
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

	if( isInitiatorMessage() || isResponderMessage() ){
		if( payload->payloadType() != MIKEYPAYLOAD_SIGN_PAYLOAD_TYPE){
			throw MikeyException( 
			   "RSAR init did not end with a SIGN payload" );
		}

		if( isResponderMessage() &&
		    ka->csbId() != csbId() ){
			ka->setAuthError( "CSBID mismatch\n" );
			return true;
		}

		// Fetch peer certificate chain
		MRef<certificate_chain *> peerChain = ka->peerCertificateChain();
		if( peerChain.isNull() || peerChain->get_first().isNull() ){
			peerChain = extractCertificateChain();

			if( peerChain.isNull() ){
				ka->setAuthError( "No certificate was found" );
				return true;
			}

			ka->setPeerCertificateChain( peerChain );
		}

 		if( !verifySignature( peerChain->get_first() ) ){
			cout << "Verification of the RSAR init message SIGN payload failed!"  << endl;
			cout << "Keypair of the initiator probably mismatch!" << endl;
			return true;
		}

		ka->setCsbId( csbId() );

		if( isResponderMessage() ){
			if( !extractPkeEnvKey( ka ) ){
				throw MikeyException( "Decryption of envelope key failed" );
			}

			if( !verifyKemac( ka, true ) ){
				return true;
			}
		}

		return false;
	}
	else{
		throw MikeyException( "Invalide type for a RSAR message" );
	}

}

bool MikeyMessageRSAR::isInitiatorMessage() const{
	return type() == MIKEY_TYPE_RSA_R_INIT;
}

bool MikeyMessageRSAR::isResponderMessage() const{
	return type() == MIKEY_TYPE_RSA_R_RESP;
}

int32_t MikeyMessageRSAR::keyAgreementType() const{
	return KEY_AGREEMENT_TYPE_RSA_R;
}

