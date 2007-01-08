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
#include"MikeyMessageDHHMAC.h"
#include<libmikey/MikeyPayload.h>
#include<libmikey/MikeyPayloadERR.h>
#include<libmikey/MikeyPayloadDH.h>
#include<libmikey/MikeyPayloadHDR.h>
#include<libmikey/MikeyPayloadID.h>
#include<libmikey/MikeyPayloadKEMAC.h>
#include<libmikey/MikeyPayloadRAND.h>
#include<libmikey/MikeyPayloadT.h>
#include<libmcrypto/rand.h>

#include<map>

using namespace std;

MikeyMessageDHHMAC::MikeyMessageDHHMAC(){
}

MikeyMessageDHHMAC::MikeyMessageDHHMAC( KeyAgreementDHHMAC * ka,
					int macAlg){

	/* generate random a CryptoSessionBundle ID */
	unsigned int csbId = ka->csbId();

	if( !csbId ){
		Rand::randomize( &csbId, sizeof( csbId ));
		ka->setCsbId( csbId );
	}

	addPayload( 
		new MikeyPayloadHDR( HDR_DATA_TYPE_DHHMAC_INIT, 1, 
			HDR_PRF_MIKEY_1, csbId,
			ka->nCs(), ka->getCsIdMapType(), 
			ka->csIdMap() ) );

	addPayload( new MikeyPayloadT() );

	addPolicyToPayload( ka ); //Is in MikeyMessage.cxx

	MikeyPayloadRAND * payload;
	addPayload( payload = new MikeyPayloadRAND() );
	
	//keep a copy of the random value!
	ka->setRand( payload->randData(), 
		     payload->randLength() );

	// TODO add IDr

	addPayload( new MikeyPayloadDH( ka->group(),
					ka->publicKey(),
					ka->keyValidity() ) );

	// Add KEMAC
	byte_t* encrKey = NULL;
	byte_t* iv = NULL;
	unsigned int encrKeyLength = 0;
	int encrAlg = MIKEY_ENCR_NULL;
	MikeyMessage::deriveTranspKeys( ka, encrKey, iv, encrKeyLength,
					encrAlg, macAlg, 0, NULL );
	addKemacPayload( NULL, 0, NULL, NULL, ka->authKey,
			 encrAlg, ka->macAlg );
	
	if( encrKey )
		delete[] encrKey;
	if( iv )
		delete[] iv;
}
//-----------------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------------//

void MikeyMessageDHHMAC::setOffer( KeyAgreement * kaBase ){
	KeyAgreementDHHMAC* ka = dynamic_cast<KeyAgreementDHHMAC*>(kaBase);

	if( !ka ){
		throw MikeyExceptionMessageContent( 
				"Not a DHHMAC keyagreement" );
	}

	MRef<MikeyPayload *> i = extractPayload( MIKEYPAYLOAD_HDR_PAYLOAD_TYPE );
	bool error = false;
	MRef<MikeyMessage *> errorMessage = new MikeyMessage();

	if( i.isNull() ){
		throw MikeyExceptionMessageContent(
				"DHHMAC init message had no HDR payload" );
	}

#define hdr ((MikeyPayloadHDR *)*i)
	if( hdr->dataType() != HDR_DATA_TYPE_DHHMAC_INIT )
		throw MikeyExceptionMessageContent( 
				"Expected DHHMAC init message" );

	ka->setnCs( hdr->nCs() );
	ka->setCsbId( hdr->csbId() );
	

	if( hdr->csIdMapType() == HDR_CS_ID_MAP_TYPE_SRTP_ID || hdr->csIdMapType() == HDR_CS_ID_MAP_TYPE_IPSEC4_ID){ 
		ka->setCsIdMap( hdr->csIdMap() );
		ka->setCsIdMapType( hdr->csIdMapType() );
	}
	else{
		throw MikeyExceptionMessageContent( 
				"Unknown type of CS ID map" );
	}
	payloads.remove( i );
#undef hdr


	errorMessage->addPayload(
			new MikeyPayloadHDR( HDR_DATA_TYPE_ERROR, 0,
			HDR_PRF_MIKEY_1, ka->csbId(),
			ka->nCs(), ka->getCsIdMapType(), 
			ka->csIdMap() ) );
	
	i = extractPayload( MIKEYPAYLOAD_T_PAYLOAD_TYPE );

	if( i.isNull() ){
		error = true;
		errorMessage->addPayload( 
			new MikeyPayloadERR( MIKEY_ERR_TYPE_UNSPEC ) );
	}

#define plT ((MikeyPayloadT*)*i)
	if( plT->checkOffset( MAX_TIME_OFFSET ) ){
		error = true;
		errorMessage->addPayload( 
			new MikeyPayloadERR( MIKEY_ERR_TYPE_INVALID_TS ) );
	}
	
	payloads.remove( i );
#undef plT

	addPolicyTo_ka(ka); //Is in MikeyMessage.cxx

	i = extractPayload( MIKEYPAYLOAD_RAND_PAYLOAD_TYPE );

	if( i.isNull() ){
		error = true;
		errorMessage->addPayload( 
			new MikeyPayloadERR( MIKEY_ERR_TYPE_UNSPEC ) );
	}

#define plRand ((MikeyPayloadRAND*)*i)
	ka->setRand( plRand->randData(),
		     plRand->randLength() );

	payloads.remove( i );
#undef plRand


	//FIXME treat the case of an ID payload
	i = extractPayload( MIKEYPAYLOAD_ID_PAYLOAD_TYPE );
	if( !i.isNull() ){
		payloads.remove( i );
	}

	i = extractPayload( MIKEYPAYLOAD_DH_PAYLOAD_TYPE );

	if( i.isNull() ){
		error = true;
		errorMessage->addPayload( 
			new MikeyPayloadERR( MIKEY_ERR_TYPE_UNSPEC ) );
	}

#define plDH ((MikeyPayloadDH*)*i)
	if( ka->group() != plDH->group() ){
		if( ka->setGroup( plDH->group() ) ){
			error = true;
			errorMessage->addPayload(
				new MikeyPayloadERR( MIKEY_ERR_TYPE_INVALID_DH ) );
		}
	}

	ka->setPeerKey( plDH->dhKey(),
		        plDH->dhKeyLength() );
	
	ka->setKeyValidity( plDH->kv() );
	
	payloads.remove( i );
#undef plDH

	i = extractPayload( MIKEYPAYLOAD_KEMAC_PAYLOAD_TYPE );

#define plKemac ((MikeyPayloadKEMAC*)*i)
	if( i.isNull() ){
		error = true;
		errorMessage->addPayload( 
			new MikeyPayloadERR( MIKEY_ERR_TYPE_UNSPEC ) );
	}	
	else{
		ka->macAlg = plKemac->macAlg();;
	}
#undef plKemac

	if( error ){
		throw MikeyExceptionMessageContent( errorMessage );
	}

// 	ka->computeTgk();
}
//-----------------------------------------------------------------------------------------------//
//
//-----------------------------------------------------------------------------------------------//

MRef<MikeyMessage *> MikeyMessageDHHMAC::buildResponse( KeyAgreement * kaBase ){
	KeyAgreementDHHMAC* ka = dynamic_cast<KeyAgreementDHHMAC*>(kaBase);

	if( !ka ){
		throw MikeyExceptionMessageContent( 
				"Not a DHHMAC keyagreement" );
	}

	// Build the response message
	MRef<MikeyMessage *> result = new MikeyMessage();
	result->addPayload( 
			new MikeyPayloadHDR( HDR_DATA_TYPE_DHHMAC_RESP, 0, 
			HDR_PRF_MIKEY_1, ka->csbId(),
			ka->nCs(), ka->getCsIdMapType(), 
			ka->csIdMap() ) );

	result->addPayload( new MikeyPayloadT() );

	// FIXME add IDi

	result->addPayload( new MikeyPayloadDH( 
				    ka->group(),
                                    ka->publicKey(),
				    ka->keyValidity() ) );

	result->addPayload( new MikeyPayloadDH( 
				    ka->group(),
                                    ka->peerKey(),
				    ka->keyValidity() ) );

	// KEMAC
	byte_t * encrKey = NULL;
	byte_t * iv = NULL;
	unsigned int encrKeyLength = 0;
	int encrAlg = MIKEY_ENCR_NULL;

	deriveTranspKeys( ka, encrKey, iv, encrKeyLength,
			  encrAlg, ka->macAlg, 0, NULL );

	result->addKemacPayload( NULL, 0, NULL, NULL, ka->authKey,
				 encrAlg, ka->macAlg );

	if( encrKey )
		delete[] encrKey;
	if( iv )
		delete[] iv;

	return result;
}

MRef<MikeyMessage *> MikeyMessageDHHMAC::parseResponse( KeyAgreement * kaBase ){
	KeyAgreementDHHMAC* ka = dynamic_cast<KeyAgreementDHHMAC*>(kaBase);

	if( !ka ){
		throw MikeyExceptionMessageContent( 
				"Not a DHHMAC keyagreement" );
	}

	MRef<MikeyPayload *> i = extractPayload( MIKEYPAYLOAD_HDR_PAYLOAD_TYPE );
	bool error = false;
	bool gotDhi = false;
	MRef<MikeyMessage *> errorMessage = new MikeyMessage();
	MRef<MikeyCsIdMap *> csIdMap;
	uint8_t nCs;
	
	if( i.isNull() ){
		throw MikeyExceptionMessageContent(
				"DHHMAC resp message had no HDR payload" );
	}

#define hdr ((MikeyPayloadHDR *)(*i))
	if( hdr->dataType() != HDR_DATA_TYPE_DHHMAC_RESP ){
		throw MikeyExceptionMessageContent( 
				"Expected DHHMAC resp message" );
	}

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
	//FIXME look at the other fields!
	
	errorMessage->addPayload(
			new MikeyPayloadHDR( HDR_DATA_TYPE_ERROR, 0,
			HDR_PRF_MIKEY_1, ka->csbId(),
			nCs, ka->getCsIdMapType(),
			csIdMap ) );
	
	payloads.remove( i );
	i = extractPayload( MIKEYPAYLOAD_T_PAYLOAD_TYPE );

	if( i.isNull() ){
		error = true;
		errorMessage->addPayload( 
			new MikeyPayloadERR( MIKEY_ERR_TYPE_UNSPEC ) );
	}

#define plT ((MikeyPayloadT *)(*i))
	if( plT->checkOffset( MAX_TIME_OFFSET ) ){
		error = true;
		errorMessage->addPayload( 
			new MikeyPayloadERR( MIKEY_ERR_TYPE_INVALID_TS ) );
	}

	payloads.remove( i );
#undef plT

	addPolicyTo_ka(ka); //Is in MikeyMessage.cxx

	i = extractPayload( MIKEYPAYLOAD_ID_PAYLOAD_TYPE );
	if( !i.isNull() ){
		payloads.remove( i );
	}

	i = extractPayload( MIKEYPAYLOAD_DH_PAYLOAD_TYPE );
	
	if( i.isNull() ){
		error = true;
		errorMessage->addPayload(
			new MikeyPayloadERR( MIKEY_ERR_TYPE_UNSPEC ) );
	}

#define dh ((MikeyPayloadDH *)*i)
	if( string( (const char *)dh->dhKey(), 
				  dh->dhKeyLength() ) ==
	    string( (const char *)ka->publicKey(), 
		    		  ka->publicKeyLength() ) ){
		// This is the DHi payload
		gotDhi = true;
	}
	else{
		// This is the DHr payload
		ka->setPeerKey( dh->dhKey(),
				dh->dhKeyLength() );
	}

	payloads.remove( i );
	i = extractPayload( MIKEYPAYLOAD_DH_PAYLOAD_TYPE );

	if( i.isNull() ){
		error = true;
		errorMessage->addPayload(
			new MikeyPayloadERR( MIKEY_ERR_TYPE_UNSPEC ) );
	}

	if( gotDhi ){
		// this one should then be DHr
		ka->setPeerKey( dh->dhKey(), dh->dhKeyLength() );
	}
	else{
		if( string( (const char *)dh->dhKey(), 
					  dh->dhKeyLength() ) !=
	    	    string( (const char *)ka->publicKey(), 
		    	    ka->publicKeyLength() ) ){
			error = true;
			errorMessage->addPayload(
				new MikeyPayloadERR( MIKEY_ERR_TYPE_UNSPEC ) );
		}
	}
#undef dh

	if( error ){
		throw MikeyExceptionMessageContent( errorMessage );
	}

// 	ka->computeTgk();

	return NULL;
}

bool MikeyMessageDHHMAC::authenticate( KeyAgreement * kaBase ){
	KeyAgreementDHHMAC* ka = dynamic_cast<KeyAgreementDHHMAC*>(kaBase);

	if( !ka ){
		throw MikeyExceptionMessageContent( 
				"Not a DHHMAC keyagreement" );
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

	
	if( isInitiatorMessage() || isResponderMessage() ){
		if( payload->payloadType() != MIKEYPAYLOAD_KEMAC_PAYLOAD_TYPE){
			cerr << "Last payload type = " << (int)payload->payloadType() << endl;
			throw MikeyException( 
			   "DHHMAC init did not end with a KEMAC payload" );
		}
		
		ka->setCsbId( csbId() );

		if( !verifyKemac( ka, false ) ){
			return true;
		}
		return false;
	}
	else{
		throw MikeyException( "Invalide type for a DHHMAC message" );
	}

}

bool MikeyMessageDHHMAC::isInitiatorMessage() const{
	return type() == MIKEY_TYPE_DHHMAC_INIT;
}

bool MikeyMessageDHHMAC::isResponderMessage() const{
	return type() == MIKEY_TYPE_DHHMAC_RESP;
}

int32_t MikeyMessageDHHMAC::keyAgreementType() const{
	return KEY_AGREEMENT_TYPE_DHHMAC;
}
