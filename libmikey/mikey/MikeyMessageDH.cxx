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
#include"MikeyMessageDH.h"
#include<libmikey/MikeyPayload.h>
#include<libmikey/MikeyPayloadHDR.h>
#include<libmikey/MikeyPayloadT.h>
#include<libmikey/MikeyPayloadRAND.h>
#include<libmikey/MikeyPayloadCERT.h>
#include<libmikey/MikeyPayloadDH.h>
#include<libmikey/MikeyPayloadERR.h>
#include<libmikey/KeyAgreementDH.h>
#include<libmcrypto/SipSim.h>
#include<libmcrypto/rand.h>

#include<map>

using namespace std;

MikeyMessageDH::MikeyMessageDH(){
}

MikeyMessageDH::MikeyMessageDH( KeyAgreementDH * ka ){

	/* generate random a CryptoSessionBundle ID */
	unsigned int csbId = ka->csbId();

	if( !csbId ){
		if(ka->useSim)
			Rand::randomize( &csbId, sizeof( csbId ), ka->getSim());
		else
			Rand::randomize( &csbId, sizeof( csbId ));
		ka->setCsbId( csbId );
	}

	addPayload( 
		new MikeyPayloadHDR( HDR_DATA_TYPE_DH_INIT, 1, 
			HDR_PRF_MIKEY_1, csbId,
			ka->nCs(), ka->getCsIdMapType(), 
			ka->csIdMap() ) );

	addPayload( new MikeyPayloadT() );

	addPolicyToPayload( ka ); //Is in MikeyMessage.cxx

	MikeyPayloadRAND * payload;

	if(ka->useSim)
		addPayload(payload = new MikeyPayloadRAND(ka->getSim()));
	else
		addPayload( payload = new MikeyPayloadRAND() );
	
	//keep a copy of the random value!
	ka->setRand( payload->randData(), 
		     payload->randLength() );

	/* Include the list of certificates if available */
	addCertificatePayloads( ka->certificateChain() );

	addPayload( new MikeyPayloadDH( ka->group(),
					ka->publicKey(),
					ka->keyValidity() ) );

	if (ka->useSim){
		addSignaturePayload(ka->getSim());
	}else{
		addSignaturePayload( ka->certificateChain()->getFirst() );
	}

}
//-----------------------------------------------------------------------------------------------//
//-----------------------------------------------------------------------------------------------//

void MikeyMessageDH::setOffer( KeyAgreement * kaBase ){
	KeyAgreementDH* ka = dynamic_cast<KeyAgreementDH*>(kaBase);

	if( !ka ){
		throw MikeyExceptionMessageContent( 
				"Not a DH keyagreement" );
	}

	MRef<MikeyPayload *> i = extractPayload( MIKEYPAYLOAD_HDR_PAYLOAD_TYPE );
	bool error = false;
	MRef<MikeyMessage *> errorMessage = new MikeyMessage();

	if( i.isNull() ){
		throw MikeyExceptionMessageContent(
				"DH init message had no HDR payload" );
	}

#define hdr ((MikeyPayloadHDR *)*i)
	if( hdr->dataType() != HDR_DATA_TYPE_DH_INIT )
		throw MikeyExceptionMessageContent( 
				"Expected DH init message" );

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

	// FIXME i can be NULL
	if( ((MikeyPayloadT*)*i)->checkOffset( MAX_TIME_OFFSET ) ){
		error = true;
		errorMessage->addPayload( 
			new MikeyPayloadERR( MIKEY_ERR_TYPE_INVALID_TS ) );
	}
	
	payloads.remove( i );

	addPolicyTo_ka(ka); //Is in MikeyMessage.cxx

	i = extractPayload( MIKEYPAYLOAD_RAND_PAYLOAD_TYPE );

	if( i.isNull() ){
		error = true;
		errorMessage->addPayload( 
			new MikeyPayloadERR( MIKEY_ERR_TYPE_UNSPEC ) );
	}

	ka->setRand( ((MikeyPayloadRAND *)*i)->randData(),
		     ((MikeyPayloadRAND *)*i)->randLength() );

	payloads.remove( i );

	/* If we haven't gotten the peer's certificate chain
	 * (for instance during authentication of the message),
	 * try to get it now */

	// Fetch peer certificate chain
	MRef<CertificateChain *> peerChain = ka->peerCertificateChain();
	if( peerChain.isNull() || peerChain->getFirst().isNull() ){
		peerChain = extractCertificateChain();

		if( !peerChain.isNull() ){
			ka->setPeerCertificateChain( peerChain );
		}
	}

	//FIXME treat the case of an ID payload
	/*
	{
		i = extractPayload( MIKEYPAYLOAD_ID_PAYLOAD_TYPE );
		if( i != NULL ){
			payloads.remove( i );
		}
	}
	*/


	i = extractPayload( MIKEYPAYLOAD_DH_PAYLOAD_TYPE );

	if( i.isNull() ){
		error = true;
		errorMessage->addPayload( 
			new MikeyPayloadERR( MIKEY_ERR_TYPE_UNSPEC ) );
	}

#define dh ((MikeyPayloadDH*)*i)

	// FIXME i can be NULL
	if( ka->group() != dh->group() ){
		ka->setGroup( dh->group() );
	}

	ka->setPeerKey( dh->dhKey(),
		        dh->dhKeyLength() );
	
	ka->setKeyValidity( dh->kv() );
	
	payloads.remove( i );
#undef dh

}
//-----------------------------------------------------------------------------------------------//
//
//-----------------------------------------------------------------------------------------------//

MRef<MikeyMessage *> MikeyMessageDH::buildResponse( KeyAgreement * kaBase ){
	KeyAgreementDH* ka = dynamic_cast<KeyAgreementDH*>(kaBase);

	if( !ka ){
		throw MikeyExceptionMessageContent( 
				"Not a DH keyagreement" );
	}

	// Build the response message
	MRef<MikeyMessage *> result = new MikeyMessage();
	result->addPayload( 
			new MikeyPayloadHDR( HDR_DATA_TYPE_DH_RESP, 0, 
			HDR_PRF_MIKEY_1, ka->csbId(),
			ka->nCs(), ka->getCsIdMapType(), 
			ka->csIdMap() ) );

	result->addPayload( new MikeyPayloadT() );

	addPolicyToPayload( ka ); //Is in MikeyMessage.cxx

	/* Include the list of certificates if available */
	result->addCertificatePayloads( ka->certificateChain() );

	result->addPayload( new MikeyPayloadDH( 
				    ka->group(),
                                    ka->publicKey(),
				    ka->keyValidity() ) );

	result->addPayload( new MikeyPayloadDH( 
				    ka->group(),
                                    ka->peerKey(),
				    ka->keyValidity() ) );

	if (ka->useSim){
		result->addSignaturePayload(ka->getSim());
	}else{
		result->addSignaturePayload( ka->certificateChain()->getFirst() );
	}

	return result;
}

MRef<MikeyMessage *> MikeyMessageDH::parseResponse( KeyAgreement * kaBase ){
	KeyAgreementDH* ka = dynamic_cast<KeyAgreementDH*>(kaBase);

	if( !ka ){
		throw MikeyExceptionMessageContent( 
				"Not a DH keyagreement" );
	}

	MRef<MikeyPayload *> i = extractPayload( MIKEYPAYLOAD_HDR_PAYLOAD_TYPE );
	bool error = false;
	bool gotDhi = false;
	MRef<MikeyMessage *> errorMessage = new MikeyMessage();
	MRef<MikeyCsIdMap *> csIdMap;
	uint8_t nCs;
	
	if( i.isNull() ){
		throw MikeyExceptionMessageContent(
				"DH resp message had no HDR payload" );
	}

#define hdr ((MikeyPayloadHDR *)(*i))
	if( hdr->dataType() != HDR_DATA_TYPE_DH_RESP ){
		throw MikeyExceptionMessageContent( 
				"Expected DH resp message" );
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

	// FIXME i can be NULL
	if( ((MikeyPayloadT*)*i)->checkOffset( MAX_TIME_OFFSET ) ){
		error = true;
		errorMessage->addPayload( 
			new MikeyPayloadERR( MIKEY_ERR_TYPE_INVALID_TS ) );
	}

	payloads.remove( i );

	addPolicyTo_ka(ka); //Is in MikeyMessage.cxx

	// Fetch peer certificate chain
	MRef<CertificateChain *> peerChain = ka->peerCertificateChain();
	if( peerChain.isNull() || peerChain->getFirst().isNull() ){
		peerChain = extractCertificateChain();

		if( !peerChain.isNull() ){
			ka->setPeerCertificateChain( peerChain );
		}
	}

	//FIXME treat the case of an ID payload
	/*{
		i = extractPayload( MIKEYPAYLOAD_ID_PAYLOAD_TYPE );
		if( i != NULL ){
			payloads.remove( i );
		}
	}*/

	i = extractPayload( MIKEYPAYLOAD_DH_PAYLOAD_TYPE );
	
	if( i.isNull() ){
		error = true;
		errorMessage->addPayload(
			new MikeyPayloadERR( MIKEY_ERR_TYPE_UNSPEC ) );
	}

	// FIXME i can be NULL
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

        //FIXME handle key validity information
	
	if( error ){
		if (ka->useSim){
			errorMessage->addSignaturePayload(ka->getSim());
		}else{
			errorMessage->addSignaturePayload( ka->certificateChain()->getFirst() );
		}
		throw MikeyExceptionMessageContent( errorMessage );
	}

	return NULL;
}

bool MikeyMessageDH::authenticate( KeyAgreement * kaBase ){
	KeyAgreementDH* ka = dynamic_cast<KeyAgreementDH*>(kaBase);
	if( !ka ){
		throw MikeyExceptionMessageContent( 
				"Not a DH keyagreement" );
	}

	MRef<MikeyPayload *> sign = (*lastPayload());

	// Fetch peer certificate chain
	MRef<CertificateChain *> peerCert = ka->peerCertificateChain();
	if( peerCert.isNull() || peerCert->getFirst().isNull() ){
		peerCert = extractCertificateChain();

		if( peerCert.isNull() ){
			ka->setAuthError( "No certificate was found" );
			return true;
		}

		ka->setPeerCertificateChain( peerCert );
	}

	if( sign->payloadType() != MIKEYPAYLOAD_SIGN_PAYLOAD_TYPE ){
		ka->setAuthError( "No signature payload found" );
		return true;
	}

#define signPl ((MikeyPayloadSIGN*)*sign)
	int res; 
	res = peerCert->getFirst()->verifSign( rawMessageData(),
						rawMessageLength() - signPl->sigLength(),
						signPl->sigData(),
						signPl->sigLength() );
	if( res > 0 ) return false;
	else return true;
}

bool MikeyMessageDH::isInitiatorMessage() const{
	return type() == MIKEY_TYPE_DH_INIT;
}

bool MikeyMessageDH::isResponderMessage() const{
	return type() == MIKEY_TYPE_DH_RESP;
}

int32_t MikeyMessageDH::keyAgreementType() const{
	return KEY_AGREEMENT_TYPE_DH;
}
