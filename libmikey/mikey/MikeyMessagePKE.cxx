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

#include <config.h>

#include<openssl/rsa.h>

#include <libmikey/MikeyMessage.h>
#include <libmikey/MikeyPayloadHDR.h>
#include <libmikey/MikeyPayloadT.h>
#include <libmikey/MikeyPayloadRAND.h>
#include <libmikey/MikeyException.h>
#include <libmikey/MikeyPayloadKeyData.h>
#include <libmikey/MikeyPayloadERR.h>
#include <libmikey/MikeyPayloadID.h>
#include <libmikey/MikeyPayloadKEMAC.h>
#include <libmikey/MikeyPayloadV.h>
#include <libmikey/MikeyPayloadPKE.h>
#include <libmutil/aes.h>
#include <libmutil/hmac.h>

MikeyMessage::MikeyMessage(KeyAgreementPKE* ka, int encrAlg, int macAlg, EVP_PKEY* privKeyInitiator):
												compiled(false), rawData(NULL){

	unsigned int csbId = rand();
	ka->setCsbId(csbId);
	MikeyPayloadT* tPayload;
	MikeyPayloadRAND* randPayload;

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

	// Derive the transport keys from the env_key:
	byte_t* encrKey;
	byte_t* authKey;
	byte_t* saltKey;
	byte_t iv[16];
	int i;

	switch( encrAlg ){
		case MIKEY_ENCR_AES_CM_128:
			encrKey = new byte_t[16];
			ka->genTranspEncrKey(encrKey, 16);
			saltKey = new byte_t[14];
			ka->genTranspSaltKey(saltKey, 14);
			iv[0] = saltKey[0];
			iv[1] = saltKey[1];
			for( i = 2; i < 6; i++ ){
				iv[i] = saltKey[i] ^ (csbId >> (5-i)*8) & 0xFF;
			}

			for( i = 6; i < 14; i++ ){
				iv[i] = (byte_t)(saltKey[i] ^ (t >> (13-i)) & 0xFF);
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
			throw new MikeyException( "Unknown encryption algorithm (MikeyMessage::MikeyMessage)" );
	}
	switch( macAlg ){
		case MIKEY_MAC_HMAC_SHA1_160:
			ka->authKeyLength = 20;
			authKey = new byte_t[ka->authKeyLength];
			ka->genTranspAuthKey(authKey, ka->authKeyLength);
			ka->authKey = authKey;
			break;
		case MIKEY_MAC_NULL:
			authKey = NULL;
			break;
		default:
			throw new MikeyException( "Unknown MAC algorithm (MikeyMessage::MikeyMessage)" );
	}
	
	//adding KEMAC payload
	MikeyPayloadKeyData* keydata = 
		new MikeyPayloadKeyData(KEYDATA_TYPE_TGK, ka->tgk(),
							ka->tgkLength(), ka->keyValidity());
	
	byte_t* rawKeyData = new byte_t[ keydata->length() ];
	keydata->writeData(rawKeyData, keydata->length());
	
	addKemacPayloadPKE(rawKeyData, keydata->length(), encrKey, iv, authKey, encrAlg, macAlg);
	
	//adding PKE payload
	RSA* pubKeyResponderRsa = EVP_PKEY_get1_RSA(ka->getPublicKey());
	byte_t* env_key = ka->getEnvelopeKey();
	unsigned char* encEnvKey = new unsigned char[RSA_size(pubKeyResponderRsa)];
	RSA_public_encrypt(ka->getEnvelopeKeyLength(), env_key, encEnvKey, pubKeyResponderRsa, RSA_PKCS1_PADDING);
	
	addPayload(new MikeyPayloadPKE(2, encEnvKey, RSA_size(pubKeyResponderRsa)));
	
	
	//adding SIGN payload
	if(privKeyInitiator == NULL)
		throw new MikeyException("Adding SIGN payload: No private key found!");
	
	unsigned int* computedMacLength = new unsigned int;
	*computedMacLength = EVP_PKEY_size(privKeyInitiator);
	unsigned char* computedMac = new unsigned char[*computedMacLength];
	
	MikeyPayloadSIGN* sig = new MikeyPayloadSIGN(*computedMacLength, computedMac,
											MIKEYPAYLOAD_SIGN_TYPE_RSA_PKCS);
	addPayload(sig);
	
	EVP_MD_CTX* ctxt = EVP_MD_CTX_create();
	EVP_SignInit(ctxt, EVP_sha1());
	EVP_SignUpdate(ctxt, rawMessageData(), (rawMessageLength() - sig->length()));
	if(!EVP_SignFinal(ctxt, computedMac, computedMacLength, privKeyInitiator)){
		EVP_MD_CTX_destroy(ctxt);
		throw new MikeyException("Create SIGN payload of the Init message failed!");
	}
	EVP_MD_CTX_destroy(ctxt);
	
	sig->setSigData(computedMac);

	//remove garbage
	if( encrKey != NULL )
		delete [] encrKey;

	if( saltKey != NULL )
		delete [] saltKey;
	
	if( authKey != NULL )
		delete [] authKey;

	delete keydata;
	delete [] rawKeyData;
	delete [] encEnvKey;
	delete [] computedMac;
	delete computedMacLength;
}

void MikeyMessage::addKemacPayloadPKE(byte_t* tgk, int tgkLength, byte_t* encrKey, byte_t* iv,
										byte_t* authKey, int encrAlg, int macAlg ){
	
											
	byte_t* encrData = new byte_t[tgkLength];
	MikeyPayloadKEMAC * payload;
	
	switch(encrAlg){
		case MIKEY_PAYLOAD_KEMAC_ENCR_AES_CM_128: {
			AES* aes = new AES(encrKey, 16);
			aes->ctr_encrypt(tgk, tgkLength, encrData, iv);
			delete aes;
			break;
		}
		case MIKEY_PAYLOAD_KEMAC_ENCR_NULL:
			memcpy(encrData, tgk, tgkLength);
			break;
		case MIKEY_PAYLOAD_KEMAC_ENCR_AES_KW_128:
			//TODO
		default:
			throw new MikeyException("No transport encrytption algorithm selected");
			break;
	}
	
	//generating MAC for KEMAC payload
	switch(macAlg){
		case MIKEY_PAYLOAD_KEMAC_MAC_HMAC_SHA1_160:
		{
			unsigned int computedMacLength;
			unsigned char* computedMac = new unsigned char[20];
			
			payload = new MikeyPayloadKEMAC(encrAlg, tgkLength, encrData, macAlg, computedMac);
		 	this->addPayload(payload);
		 	
			const int macInputLength = (payload->encrDataLength() + 4);
			unsigned char* macInput = new unsigned char[macInputLength];
			macInput[0] = (uint8_t)payload->encrAlg();
			uint16_t ip = (uint16_t)payload->encrDataLength();
			memcpy(macInput + 1, &ip, 2);
			macInput[1] = 0;  //TODO
			macInput[2] = 0x22;
			memcpy(macInput + 3, payload->encrData(), payload->encrDataLength());
			macInput[3 + payload->encrDataLength()] = (uint8_t)payload->macAlg();
						
			hmac_sha1(authKey, 20, macInput, macInputLength, computedMac, &computedMacLength );
			payload->setMac(computedMac);
			
			delete [] computedMac;
			delete [] macInput;
			break;
		}
		case MIKEY_PAYLOAD_KEMAC_MAC_NULL:
			payload = new MikeyPayloadKEMAC(encrAlg, tgkLength, encrData, macAlg, NULL);
			addPayload(payload);
			break;
		default:
			throw new MikeyException("No transport mac algorithm selected");
			break;
	}
	this->compiled = false;								
	delete [] encrData;
}

void MikeyMessage::setOffer(KeyAgreementPKE* ka){

	MikeyPayload* i = extractPayload( MIKEYPAYLOAD_HDR_PAYLOAD_TYPE );
	bool error = false;
	//uint32_t csbId;
	MRef<MikeyCsIdMap*> csIdMap;
	MikeyMessage* errorMessage = new MikeyMessage();
	//uint8_t nCs;

	if( i == NULL || 
		i->payloadType() != MIKEYPAYLOAD_HDR_PAYLOAD_TYPE ){
		throw new MikeyExceptionMessageContent( 
				"PKE init message had no HDR payload" );
	}

#define hdr ((MikeyPayloadHDR *)(i))
	if( hdr->dataType() != HDR_DATA_TYPE_PK_INIT ){
		throw new MikeyExceptionMessageContent( 
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
		throw new MikeyExceptionMessageContent( 
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
		throw new MikeyExceptionMessageContent( 
				"PKE init message had no T payload" );

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
	ka->macAlg = macAlg;

	// Derive the transport keys
	byte_t * encrKey=NULL;
	byte_t * authKey=NULL;
	byte_t * saltKey=NULL;
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
				iv[j] = saltKey[j] ^ (ka->csbId() >> (5-j)*8) & 0xFF;
			}

			for( j = 6; j < 14; j++ ){
				iv[j] = (byte_t)(saltKey[j] ^ (ka->t_received >> (13-j)) & 0xFF);
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
			ka->authKey = authKey;
			ka->authKeyLength = authKeyLength;
			break;
		case MIKEY_MAC_NULL:
			authKey = NULL;
			ka->authKey = NULL;
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

		authKeyLength = 20;
		authKey = new byte_t[20];
		ka->genTranspAuthKey( authKey, 20 );
		
		errorMessage->addVPayload( MIKEY_MAC_HMAC_SHA1_160, 
				ka->t_received, authKey, authKeyLength  );
		
		delete [] authKey;
		throw new MikeyExceptionMessageContent( errorMessage );
	}
	
	// decrypt the TGK
	list<MikeyPayloadKeyData *>::iterator iKeyData = 
		kemac->keyData( encrKey, encrKeyLength, iv ).begin();
	
	int tgkLength = (*iKeyData)->keyDataLength();
	byte_t * tgk = (*iKeyData)->keyData();

	ka->setTgk( tgk, tgkLength );
	ka->setKeyValidity( (*iKeyData)->kv() );
#undef kemac

	if( encrKey != NULL )
		delete [] encrKey;
	if( saltKey != NULL )
		delete [] saltKey;
}

MikeyMessage* MikeyMessage::buildResponse(KeyAgreementPKE* ka){
	
	if( ka->getV() || ka->getCsIdMapType() == HDR_CS_ID_MAP_TYPE_IPSEC4_ID ){
		// Build the response message
		MikeyMessage * result = new MikeyMessage();
		result->addPayload( 
			new MikeyPayloadHDR( HDR_DATA_TYPE_PK_RESP, 0, 
			HDR_PRF_MIKEY_1, ka->csbId(),
			ka->nCs(), ka->getCsIdMapType(), 
			ka->csIdMap() ) );

		result->addPayload( new MikeyPayloadT() );

		addPolicyToPayload( ka ); //Is in MikeyMessage.cxx

		result->addVPayload( ka->macAlg, ka->t_received, 
				ka->authKey, ka->authKeyLength);

		if( ka->authKey != NULL )
			delete [] ka->authKey;

		return result;
	}
	
	if( ka->authKey != NULL )
		delete [] ka->authKey;
	
	return NULL;
}

MikeyMessage * MikeyMessage::parseResponse( KeyAgreementPKE * ka ){
	MikeyPayload * i = extractPayload( MIKEYPAYLOAD_HDR_PAYLOAD_TYPE );
	bool error = false;
	MikeyMessage * errorMessage = new MikeyMessage();
	MRef<MikeyCsIdMap *> csIdMap;
	uint8_t nCs;
	
	if( i == NULL ||
		i->payloadType() != MIKEYPAYLOAD_HDR_PAYLOAD_TYPE ){

		throw new MikeyExceptionMessageContent( 
				"PKE response message had no HDR payload" );
	}

#define hdr ((MikeyPayloadHDR *)(i))
	if( hdr->dataType() != HDR_DATA_TYPE_PK_RESP )
		throw new MikeyExceptionMessageContent( 
				"Expected PKE response message" );

	if( hdr->csIdMapType() == HDR_CS_ID_MAP_TYPE_SRTP_ID || hdr->csIdMapType() == HDR_CS_ID_MAP_TYPE_IPSEC4_ID){
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
	addPolicyTo_ka(ka); //Is in MikeyMessage.cxx
	return NULL;
}

bool MikeyMessage::authenticate(KeyAgreementPKE* ka){
	
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
			
			return false;
		}

		ka->setRand( randPayload->randData(), 
			     randPayload->randLength() );
	}

	if( type() == HDR_DATA_TYPE_PK_INIT )
	{
		MikeyPayloadKEMAC * kemac;
		if( payload->payloadType() != MIKEYPAYLOAD_SIGN_PAYLOAD_TYPE){
			throw new MikeyException( 
			   "PKE init did not end with a SIGN payload" );
		}
		
		MikeyPayloadSIGN* sig = (MikeyPayloadSIGN*)extractPayload(MIKEYPAYLOAD_SIGN_PAYLOAD_TYPE);
		
		EVP_MD_CTX* ctx = EVP_MD_CTX_create();
		EVP_VerifyInit(ctx, EVP_sha1());
		EVP_VerifyUpdate(ctx, rawMessageData(), (rawMessageLength() - sig->length()));
		int err = 1;
		err = EVP_VerifyFinal(ctx, sig->sigData(), sig->sigLength(), ka->getPublicKey());
		if(!err){
			cout << "Verification of the PKE init message SIGN payload failed! Code: "  << err << endl;
			cout << "Keypair of the initiator probably mismatch!" << endl;
			EVP_MD_CTX_destroy(ctx);
			return false;
		}
		EVP_MD_CTX_destroy(ctx);
		

		kemac = (MikeyPayloadKEMAC *) extractPayload(MIKEYPAYLOAD_KEMAC_PAYLOAD_TYPE);
		macAlg = kemac->macAlg();
		receivedMac = kemac->macData();
		
		macInputLength = (kemac->encrDataLength() + 4);
		macInput = new byte_t[macInputLength];
		macInput[0] = (uint8_t)kemac->encrAlg();
		uint16_t ip = (uint16_t)kemac->encrDataLength();
		memcpy(macInput + 1, &ip, 2);
		macInput[1] = 0;   //TODO
		macInput[2] = 0x22;
		memcpy(macInput + 3, kemac->encrData(), kemac->encrDataLength());
		macInput[3 + kemac->encrDataLength()] = (uint8_t)kemac->macAlg();
		
		ka->setCsbId( csbId() );
	}
	else if( type() == HDR_DATA_TYPE_PK_RESP )
	{
		if( ka->csbId() != csbId() ){
			ka->setAuthError( "CSBID mismatch\n" );
			return false;
		}
		MikeyPayloadV * v;
		uint64_t t_sent = ka->tSent();
		if( payload->payloadType() != MIKEYPAYLOAD_V_PAYLOAD_TYPE ){
			throw new MikeyException( 
			   "PKE response did not end with a V payload" );
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
				(byte_t)((t_sent >> (i*8))&0xFF);
		}
	}
	else{
		throw new MikeyException( "Invalide type for a PKE message" );
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
					return false;
				}
			}
			return true;
		case MIKEY_MAC_NULL:
			return false;
		default:
			throw new MikeyException( "Unknown MAC algorithm (MikeyMessage::authenticate)" );
	}
}
