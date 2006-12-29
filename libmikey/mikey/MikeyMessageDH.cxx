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
#include<libmcrypto/SipSim.h>

#include<map>

using namespace std;

MikeyMessageDH::MikeyMessageDH(){
}

MikeyMessageDH::MikeyMessageDH( KeyAgreementDH * ka ){

	MRef<certificate_chain *> certChain;
	MRef<certificate *> cert;
	/* generate random a CryptoSessionBundle ID */
	unsigned int csbId = rand();
	ka->setCsbId( csbId );

	addPayload( 
		new MikeyPayloadHDR( HDR_DATA_TYPE_DH_INIT, 1, 
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

	/* Include the list of certificates if available */
	certChain = ka->certificateChain();
	if( !certChain.isNull() ){
		ka->certificateChain()->lock();
		certChain->init_index();
		cert = certChain->get_next();
		while( ! cert.isNull() ){
			addPayload( new MikeyPayloadCERT(
				MIKEYPAYLOAD_CERT_TYPE_X509V3SIGN,
				cert) );
			cert = certChain->get_next();
		}
		ka->certificateChain()->unlock();
	}

	addPayload( new MikeyPayloadDH( ka->group(),
					ka->publicKey(),
					ka->keyValidity() ) );

	if (ka->useSim){
		addSignaturePayload(ka->getSim());
	}else{
		addSignaturePayload( ka->certificateChain()->get_first() );
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

	MikeyPayload * i = extractPayload( MIKEYPAYLOAD_HDR_PAYLOAD_TYPE );
	bool error = false;
	MRef<certificate *> peerCert;
	peerCert = NULL;
	MikeyMessage * errorMessage = new MikeyMessage();
	MRef<certificate *> cert;
	MRef<certificate_chain *> certChain;

	if( i == NULL ){
		throw MikeyExceptionMessageContent(
				"DH init message had no HDR payload" );
	}

#define hdr ((MikeyPayloadHDR *)i)
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
	
	payloads.remove( i );

	addPolicyTo_ka(ka); //Is in MikeyMessage.cxx

	i = extractPayload( MIKEYPAYLOAD_RAND_PAYLOAD_TYPE );

	if( i == NULL ){
		error = true;
		errorMessage->addPayload( 
			new MikeyPayloadERR( MIKEY_ERR_TYPE_UNSPEC ) );
	}

	ka->setRand( ((MikeyPayloadRAND *)i)->randData(),
		     ((MikeyPayloadRAND *)i)->randLength() );

	payloads.remove( i );

	/* If we haven't gotten the peer's certificate chain
	 * (for instance during authentication of the message),
	 * try to get it now */

	if( ka->peerCertificateChain()->get_first().isNull() ){
		i = extractPayload( MIKEYPAYLOAD_CERT_PAYLOAD_TYPE );

		while( i != NULL )
		{
			peerCert = certificate::load( 
				((MikeyPayloadCERT *)i)->certData(),
				((MikeyPayloadCERT *)i)->certLength()
				);

			ka->addPeerCertificate( peerCert );
			payloads.remove( i );

			i = extractPayload( MIKEYPAYLOAD_CERT_PAYLOAD_TYPE );
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

	if( i == NULL ){
		error = true;
		errorMessage->addPayload( 
			new MikeyPayloadERR( MIKEY_ERR_TYPE_UNSPEC ) );
	}


	if( ka->group() != ((MikeyPayloadDH *)i)->group() ){
		ka->setGroup( ((MikeyPayloadDH *)i)->group() );
	}

	ka->setPeerKey( ((MikeyPayloadDH *)i)->dhKey(),
		        ((MikeyPayloadDH *)i)->dhKeyLength() );
	
	ka->setKeyValidity( ((MikeyPayloadDH *)i)->kv() );
	
	payloads.remove( i );

}
//-----------------------------------------------------------------------------------------------//
//
//-----------------------------------------------------------------------------------------------//

MikeyMessage * MikeyMessageDH::buildResponse( KeyAgreement * kaBase ){
	KeyAgreementDH* ka = dynamic_cast<KeyAgreementDH*>(kaBase);

	if( !ka ){
		throw MikeyExceptionMessageContent( 
				"Not a DH keyagreement" );
	}

	// Build the response message
	MRef<certificate *> cert;
	MRef<certificate_chain *> certChain;
	MikeyMessage * result = new MikeyMessage();
	result->addPayload( 
			new MikeyPayloadHDR( HDR_DATA_TYPE_DH_RESP, 0, 
			HDR_PRF_MIKEY_1, ka->csbId(),
			ka->nCs(), ka->getCsIdMapType(), 
			ka->csIdMap() ) );

	result->addPayload( new MikeyPayloadT() );

	addPolicyToPayload( ka ); //Is in MikeyMessage.cxx

	/* Include the list of certificates if available */
	certChain = ka->certificateChain();
	if( !certChain.isNull() ){
		ka->certificateChain()->lock();
		certChain->init_index();
		cert = certChain->get_next();
		while( !cert.isNull() ){
			result->addPayload( new MikeyPayloadCERT(
				MIKEYPAYLOAD_CERT_TYPE_X509V3SIGN,
				cert) );
			cert = certChain->get_next();
		}
		ka->certificateChain()->unlock();
	}

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
		result->addSignaturePayload( ka->certificateChain()->get_first() );
	}

	return result;
}

MikeyMessage * MikeyMessageDH::parseResponse( KeyAgreement * kaBase ){
	KeyAgreementDH* ka = dynamic_cast<KeyAgreementDH*>(kaBase);

	if( !ka ){
		throw MikeyExceptionMessageContent( 
				"Not a DH keyagreement" );
	}

	MikeyPayload * i = extractPayload( MIKEYPAYLOAD_HDR_PAYLOAD_TYPE );
	bool error = false;
	bool gotDhi = false;
	certificate * peerCert;
	MikeyMessage * errorMessage = new MikeyMessage();
	MRef<MikeyCsIdMap *> csIdMap;
	uint8_t nCs;
	
	if( i == NULL ){
		throw MikeyExceptionMessageContent(
				"DH resp message had no HDR payload" );
	}

#define hdr ((MikeyPayloadHDR *)(i))
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

	payloads.remove( i );

	addPolicyTo_ka(ka); //Is in MikeyMessage.cxx

	if( ka->peerCertificateChain()->get_first().isNull() ){
		i = extractPayload( MIKEYPAYLOAD_CERT_PAYLOAD_TYPE );

		while( i != NULL )
		{
			peerCert = certificate::load( 
				((MikeyPayloadCERT *)i)->certData(),
				((MikeyPayloadCERT *)i)->certLength()
				);

			ka->addPeerCertificate( peerCert );
			payloads.remove( i );

			i = extractPayload( MIKEYPAYLOAD_CERT_PAYLOAD_TYPE );
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
	
	if( i == NULL ){
		error = true;
		errorMessage->addPayload(
			new MikeyPayloadERR( MIKEY_ERR_TYPE_UNSPEC ) );
	}

#define dh ((MikeyPayloadDH *)i)
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

	if( i == NULL ){
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
			addSignaturePayload(ka->getSim());
		}else{
			errorMessage->addSignaturePayload( ka->certificateChain()->get_first() );
		}
		throw MikeyExceptionMessageContent( errorMessage );
	}

	delete errorMessage;

	return NULL;
}

bool MikeyMessageDH::authenticate( KeyAgreement * kaBase ){
	KeyAgreementDH* ka = dynamic_cast<KeyAgreementDH*>(kaBase);

	if( !ka ){
		throw MikeyExceptionMessageContent( 
				"Not a DH keyagreement" );
	}

	MikeyPayload * sign = (*lastPayload());
	MRef<certificate_chain *> peerCert = ka->peerCertificateChain();

	if( peerCert.isNull() || peerCert->get_first().isNull() )
	{
		/* Try to find the certificate chain in the message */
		list<MikeyPayload *>::iterator i;
		list<MikeyPayload *>::iterator last = lastPayload();

		for( i = firstPayload(); i != last; i++ ){
			MikeyPayload *payload = *i;

			if( payload->payloadType() != MIKEYPAYLOAD_CERT_PAYLOAD_TYPE )
				continue;

			MikeyPayloadCERT * certPayload = (MikeyPayloadCERT*)payload;
			ka->addPeerCertificate(
				certificate::load( 
					certPayload->certData(),
					certPayload->certLength() ));
// 			payloads.remove( certPayload );
		}
	}

	if( peerCert->get_first().isNull() ){
		ka->setAuthError( "No certificate was found" );
		return true;
	}


	if( sign->payloadType() != MIKEYPAYLOAD_SIGN_PAYLOAD_TYPE ){
		ka->setAuthError( "No signature payload found" );
		return true;
	}

	int res; 
	res = peerCert->get_first()->verif_sign( rawMessageData(),
												rawMessageLength() - ((MikeyPayloadSIGN *)sign)->sigLength(),
												((MikeyPayloadSIGN *)sign)->sigData(),
												((MikeyPayloadSIGN *)sign)->sigLength() );
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
