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

#include "MikeyMessagePKE.h"
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
#include<libmcrypto/rand.h>

using namespace std;

MikeyMessagePKE::MikeyMessagePKE(){
}

MikeyMessagePKE::MikeyMessagePKE( KeyAgreementPKE* ka, int encrAlg, int macAlg ){

	unsigned int csbId = ka->csbId();

	if( !csbId ){
		Rand::randomize( &csbId, sizeof( csbId ));
		ka->setCsbId( csbId );
	}

	MikeyPayloadT* tPayload;
	MikeyPayloadRAND* randPayload;

	MRef<CertificateChain*> peerChain =
		ka->peerCertificateChain();
	if( !peerChain || !peerChain->getFirst() ){
		throw MikeyException( "PKE requires peer certificate" );
	}

	//adding header payload
	addPayload(new MikeyPayloadHDR(HDR_DATA_TYPE_PK_INIT, 1, 
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

	// Add certificate chain
	addCertificatePayloads( ka->certificateChain() );

	// Derive the transport keys from the env_key:
	addPkeKemac( ka, encrAlg, macAlg );

	addSignaturePayload( ka->certificateChain()->getFirst() );
}

void MikeyMessagePKE::setOffer(KeyAgreement* kaBase){
	KeyAgreementPKE* ka = dynamic_cast<KeyAgreementPKE*>(kaBase);

	if( !ka ){
		throw MikeyExceptionMessageContent( 
				"Not a PKE keyagreement" );
	}

	MRef<MikeyPayload*> i = extractPayload( MIKEYPAYLOAD_HDR_PAYLOAD_TYPE );
	bool error = false;
	//uint32_t csbId;
	MRef<MikeyCsIdMap*> csIdMap;
	MRef<MikeyMessage*> errorMessage = new MikeyMessage();
	//uint8_t nCs;

	if( i.isNull() || 
		i->payloadType() != MIKEYPAYLOAD_HDR_PAYLOAD_TYPE ){
		throw MikeyExceptionMessageContent( 
				"PKE init message had no HDR payload" );
	}

#define hdr ((MikeyPayloadHDR *)(*i))
	if( hdr->dataType() != HDR_DATA_TYPE_PK_INIT ){
		throw MikeyExceptionMessageContent( 
				"Expected PKE init message" );
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
				"PKE init message had no T payload" );

#define plT ((MikeyPayloadT *)(*i))
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

#define plRand ((MikeyPayloadRAND *)*i)
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
	
	if( !deriveTranspKeys( ka, encrKey, iv, encrKeyLength,
			      encrAlg, macAlg, ka->t_received,
			      *errorMessage ) ){
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
	MRef<MikeyPayloads*> subPayloads = 
		kemac->decodePayloads( MIKEYPAYLOAD_ID_PAYLOAD_TYPE,
				 encrKey, encrKeyLength, iv );

	MRef<MikeyPayload *> plKeyData =
		subPayloads->extractPayload( MIKEYPAYLOAD_KEYDATA_PAYLOAD_TYPE );

	// FIXME check null
	MikeyPayloadKeyData *keyData =
		dynamic_cast<MikeyPayloadKeyData*>(*plKeyData);

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

MRef<MikeyMessage*> MikeyMessagePKE::buildResponse(KeyAgreement* kaBase){
	KeyAgreementPKE* ka = dynamic_cast<KeyAgreementPKE*>(kaBase);

	if( !ka ){
		throw MikeyExceptionMessageContent( 
				"Not a PKE keyagreement" );
	}
	
	if( ka->getV() || ka->getCsIdMapType() == HDR_CS_ID_MAP_TYPE_IPSEC4_ID ){
		// Build the response message
		MRef<MikeyMessage *> result = new MikeyMessage();
		result->addPayload( 
			new MikeyPayloadHDR( HDR_DATA_TYPE_PK_RESP, 0, 
			HDR_PRF_MIKEY_1, ka->csbId(),
			ka->nCs(), ka->getCsIdMapType(), 
			ka->csIdMap() ) );

		result->addPayload( new MikeyPayloadT() );

		// TODO why do we call addPolicyToPayload here?
		addPolicyToPayload( ka ); //Is in MikeyMessage.cxx

		result->addVPayload( ka->macAlg, ka->t_received, 
				ka->authKey, ka->authKeyLength);

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

MRef<MikeyMessage *> MikeyMessagePKE::parseResponse( KeyAgreement * kaBase ){
	KeyAgreementPKE* ka = dynamic_cast<KeyAgreementPKE*>(kaBase);

	if( !ka ){
		throw MikeyExceptionMessageContent( 
				"Not a PKE keyagreement" );
	}

	MRef<MikeyPayload *> i = extractPayload( MIKEYPAYLOAD_HDR_PAYLOAD_TYPE );
	bool error = false;
	MRef<MikeyMessage *> errorMessage = new MikeyMessage();
	MRef<MikeyCsIdMap *> csIdMap;
	uint8_t nCs;
	
	if( i.isNull() ||
		i->payloadType() != MIKEYPAYLOAD_HDR_PAYLOAD_TYPE ){

		throw MikeyExceptionMessageContent( 
				"PKE response message had no HDR payload" );
	}

#define hdr ((MikeyPayloadHDR *)(*i))
	if( hdr->dataType() != HDR_DATA_TYPE_PK_RESP )
		throw MikeyExceptionMessageContent( 
				"Expected PKE response message" );

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

#define plT ((MikeyPayloadT*)*i)
	// FIXME i can be NULL
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

bool MikeyMessagePKE::authenticate(KeyAgreement* kaBase){
	KeyAgreementPKE* ka = dynamic_cast<KeyAgreementPKE*>(kaBase);

	if( !ka ){
		throw MikeyExceptionMessageContent( 
				"Not a PKE keyagreement" );
	}
	
	MRef<MikeyPayload *> payload = *(lastPayload());
	list<MikeyPayload *>::iterator payload_i;
 
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

	if( type() == HDR_DATA_TYPE_PK_INIT )
	{
		if( payload->payloadType() != MIKEYPAYLOAD_SIGN_PAYLOAD_TYPE){
			throw MikeyException( 
			   "PKE init did not end with a SIGN payload" );
		}

		// Fetch peer certificate chain
		MRef<CertificateChain *> peerChain = ka->peerCertificateChain();
		if( peerChain.isNull() || peerChain->getFirst().isNull() ){
			peerChain = extractCertificateChain();

			if( peerChain.isNull() ){
				ka->setAuthError( "No certificate was found" );
				return true;
			}

			ka->setPeerCertificateChain( peerChain );
		}

 		if( !verifySignature( peerChain->getFirst() ) ){
			cout << "Verification of the PKE init message SIGN payload failed!"  << endl;
			cout << "Keypair of the initiator probably mismatch!" << endl;
			return true;
		}

		ka->setCsbId( csbId() );

		if( !extractPkeEnvKey( ka ) ){
			throw MikeyException( "Decryption of envelope key failed" );
		}

		if( !verifyKemac( ka, true ) ){
			return true;
		}

		return false;
	}
	else if( type() == HDR_DATA_TYPE_PK_RESP )
	{
		if( payload->payloadType() != MIKEYPAYLOAD_V_PAYLOAD_TYPE ){
			throw MikeyException( 
				"PKE response did not end with a V payload" );
		}

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
		throw MikeyException( "Invalide type for a PKE message" );
	}

}

bool MikeyMessagePKE::isInitiatorMessage() const{
	return type() == MIKEY_TYPE_PK_INIT;
}

bool MikeyMessagePKE::isResponderMessage() const{
	return type() == MIKEY_TYPE_PK_RESP;
}

int32_t MikeyMessagePKE::keyAgreementType() const{
	return KEY_AGREEMENT_TYPE_PK;
}
