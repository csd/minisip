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

#include<libmikey/MikeyPayload.h>
#include<libmikey/MikeyPayloadHDR.h>
#include<libmikey/MikeyPayloadT.h>
#include<libmikey/MikeyPayloadRAND.h>
#include<libmikey/MikeyPayloadCERT.h>
#include<libmikey/MikeyPayloadID.h>
#include<libmikey/MikeyPayloadDH.h>
#include<libmikey/MikeyPayloadSIGN.h>
#include<libmikey/MikeyPayloadERR.h>


#define MAX_TIME_OFFSET 0xe1000000000LL //1 hour



MikeyMessage::MikeyMessage( KeyAgreementDH * ka ):compiled(false), rawData(NULL){

	MRef<certificate_chain *> certChain;
	MRef<certificate *> cert;
	/* generate random a CryptoSessionBundle ID */
	unsigned int csbId = rand();
	ka->setCsbId( csbId );

	addPayload( 
		new MikeyPayloadHDR( HDR_DATA_TYPE_DH_INIT, 1, 
			HDR_PRF_MIKEY_1, csbId,
			ka->nCs(), HDR_CS_ID_MAP_TYPE_SRTP_ID, 
			ka->csIdMap() ) );

	addPayload( new MikeyPayloadT() );

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

	addSignaturePayload( ka->certificateChain()->get_first() );

}

MikeyMessage * MikeyMessage::buildResponse( KeyAgreementDH * ka ){

	MikeyPayload * i = extractPayload( MIKEYPAYLOAD_HDR_PAYLOAD_TYPE );
	bool error = false;
	MRef<MikeyCsIdMap *> csIdMap;
	MRef<certificate *> peerCert;
	peerCert = NULL;
	MikeyMessage * errorMessage = new MikeyMessage();
	MRef<certificate *> cert;
	MRef<certificate_chain *> certChain;
	uint8_t nCs;

	if( i == NULL ){
		throw new MikeyExceptionMessageContent(
				"DH init message had no HDR payload" );
	}

#define hdr ((MikeyPayloadHDR *)i)
	if( hdr->dataType() != HDR_DATA_TYPE_DH_INIT )
		throw new MikeyExceptionMessageContent( 
				"Expected DH init message" );

	unsigned int csbId = hdr->csbId();
	nCs = hdr->nCs();
	ka->setCsbId( csbId );
	

	if( hdr->csIdMapType() == HDR_CS_ID_MAP_TYPE_SRTP_ID ){
		csIdMap = hdr->csIdMap();
	}
	else{
		throw new MikeyExceptionMessageContent( 
				"Unknown type of CS ID map" );
	}
	payloads.remove( i );
#undef hdr

	ka->setCsIdMap( csIdMap );

	errorMessage->addPayload(
			new MikeyPayloadHDR( HDR_DATA_TYPE_ERROR, 0,
			HDR_PRF_MIKEY_1, csbId,
			nCs, HDR_CS_ID_MAP_TYPE_SRTP_ID, 
			csIdMap ) );
	
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
			peerCert = new certificate( 
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

	// Build the response message
	MikeyMessage * result = new MikeyMessage();
	result->addPayload( 
			new MikeyPayloadHDR( HDR_DATA_TYPE_DH_RESP, 0, 
			HDR_PRF_MIKEY_1, csbId,
			nCs, HDR_CS_ID_MAP_TYPE_SRTP_ID, 
			csIdMap ) );

	result->addPayload( new MikeyPayloadT() );

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

	result->addSignaturePayload( ka->certificateChain()->get_first() );

	return result;
}

MikeyMessage * MikeyMessage::parseResponse( KeyAgreementDH * ka ){
	MikeyPayload * i = extractPayload( MIKEYPAYLOAD_HDR_PAYLOAD_TYPE );
	bool error = false;
	bool gotDhi = false;
	certificate * peerCert;
	MikeyMessage * errorMessage = new MikeyMessage();
	MRef<MikeyCsIdMap *> csIdMap;
	uint8_t nCs;
	
	if( i == NULL ){
		throw new MikeyExceptionMessageContent(
				"DH resp message had no HDR payload" );
	}

#define hdr ((MikeyPayloadHDR *)(i))
	if( hdr->dataType() != HDR_DATA_TYPE_DH_RESP ){
		throw new MikeyExceptionMessageContent( 
				"Expected DH resp message" );
	}

	if( hdr->csIdMapType() == HDR_CS_ID_MAP_TYPE_SRTP_ID ){
		csIdMap = hdr->csIdMap();
	}
	else{
		throw new MikeyExceptionMessageContent( 
				"Unknown type of CS ID map" );
	}
	nCs = hdr->nCs();
#undef hdr
	//FIXME look at the other fields!
	
	errorMessage->addPayload(
			new MikeyPayloadHDR( HDR_DATA_TYPE_ERROR, 0,
			HDR_PRF_MIKEY_1, ka->csbId(),
			nCs, HDR_CS_ID_MAP_TYPE_SRTP_ID,
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
	
	if( ka->peerCertificateChain()->get_first().isNull() ){
		i = extractPayload( MIKEYPAYLOAD_CERT_PAYLOAD_TYPE );

		while( i != NULL )
		{
			peerCert = new certificate( 
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
		errorMessage->addSignaturePayload( 
				ka->certificateChain()->get_first() );
		throw new MikeyExceptionMessageContent( errorMessage );
	}

	delete errorMessage;

	return NULL;
}

bool MikeyMessage::authenticate( KeyAgreementDH * ka ){

	MikeyPayload * sign = (*lastPayload());
	list<MikeyPayload *>::iterator i;
	MRef<certificate_chain *> peerCert = ka->peerCertificateChain();

	if( peerCert.isNull() || peerCert->get_first().isNull() )
	{
		/* Try to find the certificate chain in the message */
		MikeyPayloadCERT * certPayload;
		while( ( ((MikeyPayload *)certPayload) = 
				extractPayload( MIKEYPAYLOAD_CERT_PAYLOAD_TYPE)
				) != NULL ){
			ka->addPeerCertificate(
				new certificate( 
					certPayload->certData(),
					certPayload->certLength() ));
			payloads.remove( certPayload );
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

	return peerCert->get_first()->verif_sign( rawMessageData(),
			rawMessageLength() - 
			((MikeyPayloadSIGN *)sign)->sigLength(),
			((MikeyPayloadSIGN *)sign)->sigData(),
			((MikeyPayloadSIGN *)sign)->sigLength() );

}
