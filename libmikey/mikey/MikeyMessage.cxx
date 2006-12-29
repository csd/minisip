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

#include<libmikey/MikeyMessage.h>
#include<libmikey/MikeyPayload.h>
#include<libmikey/MikeyPayloadHDR.h>
#include<libmikey/MikeyPayloadKEMAC.h>
#include<libmikey/MikeyPayloadPKE.h>
#include<libmikey/MikeyPayloadDH.h>
#include<libmikey/MikeyPayloadSIGN.h>
#include<libmikey/MikeyPayloadT.h>
#include<libmikey/MikeyPayloadID.h>
#include<libmikey/MikeyPayloadCERT.h>
#include<libmikey/MikeyPayloadCHASH.h>
#include<libmikey/MikeyPayloadV.h>
#include<libmikey/MikeyPayloadSP.h>
#include<libmikey/MikeyPayloadRAND.h>
#include<libmikey/MikeyPayloadERR.h>
#include<libmikey/MikeyPayloadKeyData.h>
#include<libmikey/MikeyPayloadGeneralExtension.h>
#include<libmikey/MikeyException.h>

#include<libmcrypto/aes.h>
#include<libmcrypto/base64.h>
#include<libmcrypto/hmac.h>
#include<libmcrypto/cert.h>
#include<libmcrypto/SipSim.h>

#include<map>

#include"MikeyMessageDH.h"
#include"MikeyMessagePSK.h"
#include"MikeyMessagePKE.h"

using namespace std;

MikeyMessage::MikeyMessage():compiled(false), rawData(NULL){

}

MikeyMessage* MikeyMessage::create( KeyAgreementDH * ka ){
	return new MikeyMessageDH( ka );
}

MikeyMessage* MikeyMessage::create( KeyAgreementPSK * ka,
				    int encrAlg, int macAlg ){
	return new MikeyMessagePSK( ka, encrAlg, macAlg );
}

#ifdef HAVE_OPENSSL
MikeyMessage* MikeyMessage::create( KeyAgreementPKE* ka,
				    int encrAlg, int macAlg,
				    EVP_PKEY* privKeyInitiator ){
	return new MikeyMessagePKE( ka, encrAlg, macAlg, privKeyInitiator );
}
#endif	// HAVE_OPENSSL

/*
 * Alg.
 *  1. Parse HDR payload
 *  2. While not end of packet
 *    2.1 Parse payload (choose right class) and store next payload type.
 *    2.2 Add payload to list of all payloads in message.
*/ 

MikeyMessage* MikeyMessage::parse( byte_t * message, int lengthLimit )
{
	std::list<MikeyPayload *> payloads;

	parse( message, lengthLimit, payloads );

	MikeyPayloadHDR *hdr =
		dynamic_cast<MikeyPayloadHDR*>(*payloads.begin());

	if( !hdr ){
		throw MikeyExceptionMessageContent( 
			"No header in the payload" );
	}

	MikeyMessage* msg = NULL;

	switch( hdr->dataType() ){
		case MIKEY_TYPE_DH_INIT:
		case MIKEY_TYPE_DH_RESP:
			msg = new MikeyMessageDH();
			break;
		case MIKEY_TYPE_PSK_INIT:
		case MIKEY_TYPE_PSK_RESP:
			msg = new MikeyMessagePSK();
			break;
#ifdef HAVE_OPENSSL
		case MIKEY_TYPE_PK_INIT:
		case MIKEY_TYPE_PK_RESP:
			msg = new MikeyMessagePKE();
			break;
#endif	// HAVE_OPENSSL
		case MIKEY_TYPE_ERROR:
			msg = new MikeyMessage();
			break;
		default:
			throw MikeyExceptionUnimplemented(
				"Unimplemented type of message in INVITE" );
	}

	msg->compiled = true;
	msg->rawData = message;
	msg->payloads = payloads;

	return msg;
}

MikeyMessage* MikeyMessage::parse( string b64Message ){

	int messageLength;
	byte_t * messageData;

	messageData = base64_decode( b64Message, &messageLength );

	if( messageData == NULL ){
		throw MikeyExceptionMessageContent( 
				"Invalid B64 input message" );
	}

	return parse( messageData, messageLength );
}
	

MikeyMessage::~MikeyMessage(){
	
	if( rawData ){
		delete [] rawData;
	}
	
	rawData = NULL;
	
	list<MikeyPayload *>::iterator i;

	for( i = payloads.begin() ; i != payloads.end() ; i++ ){
		delete *i;
		
	}
}

void MikeyMessage::parse( byte_t * message, int lengthLimit,
			  std::list<MikeyPayload *>& payloads ){
	MikeyPayloadHDR * hdr;
	byte_t * msgpos = message;
	int limit = lengthLimit;
						
	payloads.push_back( hdr = new MikeyPayloadHDR(message, limit) );
	
	limit -=  (int)( hdr->end() - msgpos );
	msgpos = hdr->end();

	int nextPayloadType = hdr->nextPayloadType();

	while( !(msgpos >= message + lengthLimit ) && 
			nextPayloadType != MikeyPayload::LastPayload){
	
		MikeyPayload *payload;
		switch (nextPayloadType){
			case MIKEYPAYLOAD_KEMAC_PAYLOAD_TYPE:
				payload = new MikeyPayloadKEMAC(msgpos, limit);
				break;
			case MIKEYPAYLOAD_PKE_PAYLOAD_TYPE:
				payload = new MikeyPayloadPKE(msgpos, limit);
				break;
			case MIKEYPAYLOAD_DH_PAYLOAD_TYPE:
				payload = new MikeyPayloadDH(msgpos, limit);
				break;
			case MIKEYPAYLOAD_SIGN_PAYLOAD_TYPE:
				payload = new MikeyPayloadSIGN(msgpos, limit);
				break;
			case MIKEYPAYLOAD_T_PAYLOAD_TYPE:
				payload = new MikeyPayloadT(msgpos, limit);
				break;
			case MIKEYPAYLOAD_ID_PAYLOAD_TYPE:
				payload = new MikeyPayloadID(msgpos, limit);
				break;
			case MIKEYPAYLOAD_CERT_PAYLOAD_TYPE:
				payload = new MikeyPayloadCERT(msgpos, limit);
				break;
			case MIKEYPAYLOAD_CHASH_PAYLOAD_TYPE:
				payload = new MikeyPayloadCHASH(msgpos, limit);
				break;
			case MIKEYPAYLOAD_V_PAYLOAD_TYPE:
				payload = new MikeyPayloadV(msgpos, limit);
				break;
			case MIKEYPAYLOAD_SP_PAYLOAD_TYPE:
				payload = new MikeyPayloadSP(msgpos, limit);
				break;
			case MIKEYPAYLOAD_RAND_PAYLOAD_TYPE:
				payload = new MikeyPayloadRAND(msgpos, limit);
				break;
			case MIKEYPAYLOAD_ERR_PAYLOAD_TYPE:
				payload = new MikeyPayloadERR(msgpos, limit);
				break;
			case MIKEYPAYLOAD_KEYDATA_PAYLOAD_TYPE:
				payload = new MikeyPayloadKeyData(msgpos, limit);
				break;
			case MIKEYPAYLOAD_GENERALEXTENSIONS_PAYLOAD_TYPE:
				payload = new MikeyPayloadGeneralExtensions(msgpos, limit);
				break;

			case MIKEYPAYLOAD_LAST_PAYLOAD:
				break;
			default:
				throw MikeyExceptionMessageContent(
					"Payload of unrecognized type." );
		}
		nextPayloadType = payload->nextPayloadType();	
		payloads.push_back( payload );
		
		assert(( payload->end() - msgpos ) == ( payload->length() ));
		limit -= (int)( payload->end() - msgpos );
		msgpos = payload->end();
	}

	if(! (msgpos == message + lengthLimit && 
			nextPayloadType==MIKEYPAYLOAD_LAST_PAYLOAD ) )
		throw MikeyExceptionMessageLengthException(
			"The length of the message did not match"
			"the total length of payloads." );
}

void MikeyMessage::addPayload(MikeyPayload *payload){

	compiled = false;
	// Put the nextPayloadType in the previous payload */
	if( payload->payloadType() != MIKEYPAYLOAD_HDR_PAYLOAD_TYPE ){
		list<MikeyPayload *>::reverse_iterator i = payloads.rbegin();

		(*i)->setNextPayloadType( payload->payloadType() );
	}

	payloads.push_back( payload );
}

void MikeyMessage::operator +=( MikeyPayload * payload ){
	addPayload( payload );
}


void MikeyMessage::addSignaturePayload( MRef<SipSim*> sim ){
	byte_t signature[4096];
	int signatureLength;
	MikeyPayloadSIGN * sign;
	MikeyPayload * last;
	
	// set the previous nextPayloadType to signature
	last = *lastPayload();
	last->setNextPayloadType( MIKEYPAYLOAD_SIGN_PAYLOAD_TYPE );

	if( sim->getSignature( (unsigned char*)rawMessageData(), (int)rawMessageLength(),
			 (unsigned char*)signature, signatureLength, true ) ){
		throw MikeyException( "Could not perform digital signature of the message" );
	}

	addPayload( ( sign = new MikeyPayloadSIGN( signatureLength, signature,
				MIKEYPAYLOAD_SIGN_TYPE_RSA_PKCS ) ) );

	sim->getSignature( rawMessageData(), 
			 rawMessageLength() - signatureLength,
			 signature, signatureLength, true );
	sign->setSigData( signature );
	compiled = false;
}


void MikeyMessage::addSignaturePayload( MRef<certificate *> cert ){
	byte_t signature[4096];
	int signatureLength;
	MikeyPayloadSIGN * sign;
	MikeyPayload * last;
	
	// set the previous nextPayloadType to signature
	last = *lastPayload();
	last->setNextPayloadType( MIKEYPAYLOAD_SIGN_PAYLOAD_TYPE );

	if( cert->sign_data( rawMessageData(), rawMessageLength(),
			 signature, &signatureLength ) ){
		throw MikeyException( "Could not perform digital signature of the message" );
	}

	addPayload( ( sign = new MikeyPayloadSIGN( signatureLength, signature,
				MIKEYPAYLOAD_SIGN_TYPE_RSA_PKCS ) ) );

	cert->sign_data( rawMessageData(), 
			 rawMessageLength() - signatureLength,
			 signature, &signatureLength );
	sign->setSigData( signature );
	compiled = false;
}

void MikeyMessage::addKemacPayload( byte_t * tgk, int tgkLength,
		                      byte_t * encrKey,
				      byte_t * iv,
				      byte_t * authKey,
				      int encrAlg, int macAlg ){
	byte_t * encrData = new byte_t[ tgkLength ];
	AES * aes;
	MikeyPayload * last;
	
	// set the previous nextPayloadType to KEMAC
	last = * lastPayload();
	last->setNextPayloadType( MIKEYPAYLOAD_KEMAC_PAYLOAD_TYPE );
	
	switch( encrAlg ){
		case MIKEY_PAYLOAD_KEMAC_ENCR_AES_CM_128:
			aes = new AES( encrKey, 16 );
			aes->ctr_encrypt( tgk, tgkLength, encrData, iv );
			delete aes;
			break;
		case MIKEY_PAYLOAD_KEMAC_ENCR_NULL:
			memcpy( encrData, tgk, tgkLength );
			break;
		case MIKEY_PAYLOAD_KEMAC_ENCR_AES_KW_128:
			//TODO
		default:
			delete [] encrData;
			throw MikeyException( "Unknown encryption algorithm" );
	}
	
	MikeyPayloadKEMAC * payload;
	byte_t macData[20];
	unsigned int macDataLength;
	
	switch( macAlg ){
		case MIKEY_PAYLOAD_KEMAC_MAC_HMAC_SHA1_160:
			addPayload( payload = new MikeyPayloadKEMAC( encrAlg, 
				tgkLength, encrData, macAlg, macData ) );
			
			
			hmac_sha1( authKey, 20,
				rawMessageData(),
				// Compute the MAC over the whole message,
				// MAC field excluded
				rawMessageLength() - 20,
				macData, &macDataLength );
			
			//assert( macDataLength == 20 );
			payload->setMac( macData );
			break;
		case MIKEY_PAYLOAD_KEMAC_MAC_NULL:
			addPayload( payload = new MikeyPayloadKEMAC( encrAlg, 
				tgkLength, encrData, macAlg, NULL ) );
			break;
		default:
			delete [] encrData;
			throw MikeyException( "Unknown MAC algorithm" );
	}
	compiled = false;
	delete [] encrData;
}

void MikeyMessage::addVPayload( int macAlg, uint64_t t,
		byte_t * authKey, uint32_t authKeyLength ){
		MikeyPayloadV * payload;
		unsigned int hmacOutputLength;
		byte_t hmacOutput[20];
		byte_t * hmacInput;
		unsigned int messageLength;
		byte_t * messageData;

	MikeyPayload * last;
	// set the previous nextPayloadType to V
	last = *lastPayload();
	last->setNextPayloadType( MIKEYPAYLOAD_V_PAYLOAD_TYPE );
	
	switch( macAlg ){
		case MIKEY_PAYLOAD_V_MAC_HMAC_SHA1_160:
			addPayload( payload = 
				new MikeyPayloadV( macAlg, hmacOutput ) );
			
			messageLength = rawMessageLength();
			messageData = rawMessageData();

			//hmacInput = [ message t_received ]
			hmacInput = new byte_t[ messageLength + 8 -20 ];

			memcpy( hmacInput, messageData, messageLength-20 );

			hmacInput[ messageLength - 20 ] = (byte_t)((t >> 56)&0xFF);
			hmacInput[ messageLength - 19 ] = (byte_t)((t >> 48)&0xFF);
			hmacInput[ messageLength - 18 ] = (byte_t)((t >> 40)&0xFF);
			hmacInput[ messageLength - 17 ] = (byte_t)((t >> 32)&0xFF);
			hmacInput[ messageLength - 16 ] = (byte_t)((t >> 24)&0xFF);
			hmacInput[ messageLength - 15 ] = (byte_t)((t >> 16)&0xFF);
			hmacInput[ messageLength - 14 ] = (byte_t)((t >>  8)&0xFF);
			hmacInput[ messageLength - 13 ] = (byte_t)((t      )&0xFF);
			
			hmac_sha1( authKey, authKeyLength,
			   hmacInput, messageLength + 8 -20,
			   hmacOutput, &hmacOutputLength );

			payload->setMac( hmacOutput );
			delete [] hmacInput;
			break;

		case MIKEY_PAYLOAD_V_MAC_NULL:
			addPayload( new MikeyPayloadV( 
						macAlg, NULL ) );
			break;
		default:
			throw MikeyException( "Unknown MAC algorithm" );
	}
	compiled = false;
}


void MikeyMessage::compile(){
	if (compiled){
		throw MikeyExceptionMessageContent("BUG: trying to compile already compiled message.");
	}
	if( rawData )
		delete [] rawData;

	rawData = new byte_t[ rawMessageLength() ];	
	
	list<MikeyPayload *>::iterator i;
	byte_t *pos = rawData;
	for (i=payloads.begin(); i!=payloads.end(); i++){
		int len = (*i)->length();
		(*i)->writeData(pos,len);
		pos+=len;
	}

}

byte_t *MikeyMessage::rawMessageData(){
	
	if (!compiled)
		compile();
	return rawData;
}

int MikeyMessage::rawMessageLength(){
	list<MikeyPayload *>::iterator i;
	int length=0;
	for (i=payloads.begin(); i!=payloads.end(); i++){
		length+=(*i)->length();
	}

	return length;
}

string MikeyMessage::debugDump(){
	string ret="";
	list<MikeyPayload *>::iterator i;
	for (i=payloads.begin(); i!=payloads.end(); i++)
	{
		ret=ret+"\n\n"+(*i)->debugDump();
	}
	
	return ret;
}

list<MikeyPayload *>::iterator MikeyMessage::firstPayload(){
	return payloads.begin();
}

list<MikeyPayload *>::iterator MikeyMessage::lastPayload(){
	return --payloads.end();
}

string MikeyMessage::b64Message(){
	return base64_encode( rawMessageData(), rawMessageLength() );
}

uint32_t MikeyMessage::csbId(){
	MikeyPayload * hdr = * firstPayload();
	if( hdr->payloadType() != MIKEYPAYLOAD_HDR_PAYLOAD_TYPE ){
		throw MikeyExceptionMessageContent( 
				"First payload was not a header" );
	}
	return ((MikeyPayloadHDR *)hdr)->csbId();
}

int MikeyMessage::type() const{
	const MikeyPayload * hdr = extractPayload( MIKEYPAYLOAD_HDR_PAYLOAD_TYPE );
	if( hdr == NULL ){
		throw MikeyExceptionMessageContent( 
			"No header in the payload" );
	}

	return ((MikeyPayloadHDR *)hdr)->dataType();
}

MikeyPayload * MikeyMessage::extractPayload( int payloadType ){
	list<MikeyPayload *>::iterator i;

	for( i = payloads.begin(); i != payloads.end(); i++ ){
		if( (*i)->payloadType() == payloadType ){
			return *i;
		}
	}
	return NULL;
}

const MikeyPayload * MikeyMessage::extractPayload( int payloadType ) const{
	list<MikeyPayload *>::const_iterator i;

	for( i = payloads.begin(); i != payloads.end(); i++ ){
		if( (*i)->payloadType() == payloadType ){
			return *i;
		}
	}
	return NULL;
}

void MikeyMessage::remove( MikeyPayload * payload ){
	list<MikeyPayload *>::iterator i;

	for( i = payloads.begin(); i != payloads.end(); i++ ){
		if( *i == payload ){
			payloads.erase( i ); 
			return;
		}
	}
}

void MikeyMessage::addPolicyToPayload(KeyAgreement * ka){
#define comp (uint16_t)((*iter)->policy_No) << 8 | (uint16_t)((*iter)->prot_type)
	// Adding policy to payload
	MikeyPayloadSP *PSP;
	list <Policy_type *> * policy = ka->getPolicy();
	list <Policy_type *>::iterator iter;
	map <uint16_t, MikeyPayloadSP*> existingSPpayloads;
	map <uint16_t, MikeyPayloadSP*>::iterator mapiter;
	for( iter = (*policy).begin(); iter != (*policy).end()  ; iter++ ){
		mapiter = existingSPpayloads.find(comp);
		if (mapiter == existingSPpayloads.end()){
			existingSPpayloads.insert( pair<int, MikeyPayloadSP*>(comp, PSP = new MikeyPayloadSP((*iter)->policy_No, (*iter)->prot_type)));
			addPayload(PSP);
			PSP->addMikeyPolicyParam((*iter)->policy_type, (*iter)->length, (*iter)->value);
		}
		else
			(mapiter->second)->addMikeyPolicyParam((*iter)->policy_type, (*iter)->length, (*iter)->value);
	}
	existingSPpayloads.empty();
#undef comp
}

void MikeyMessage::addPolicyTo_ka(KeyAgreement * ka){
#define SP ((MikeyPayloadSP *)i)
	// Adding policy to ka
	int policy_i, policy_j;
	MikeyPolicyParam * PParam;
	MikeyPayload * i;
	while ((i = extractPayload( MIKEYPAYLOAD_SP_PAYLOAD_TYPE )) != NULL){
		policy_i = 0;
		policy_j = 0;
		while (policy_i < SP->noOfPolicyParam()){
			if((PParam = SP->getParameterType(policy_j++)) != NULL ){
				assert (policy_j-1 == PParam->type);
				ka->setPolicyParamType( SP-> policy_no, SP-> prot_type, PParam->type, PParam->length, PParam->value);
				policy_i++;
			}	
		}
		payloads.remove( i );
	}
#undef SP
}

MikeyMessage * MikeyMessage::parseResponse( KeyAgreement  * ka ){
	throw MikeyExceptionUnimplemented( "parseResponse not implemented" );
}

void MikeyMessage::setOffer( KeyAgreement * ka ){
	throw MikeyExceptionUnimplemented( "setOffer not implemented" );
}

MikeyMessage * MikeyMessage::buildResponse( KeyAgreement * ka ){
	throw MikeyExceptionUnimplemented( "buildResponse not implemented" );
}

bool MikeyMessage::authenticate( KeyAgreement  * ka ){
	throw MikeyExceptionUnimplemented( "authenticate not implemented" );
}

bool MikeyMessage::isInitiatorMessage() const{
	return false;
}

bool MikeyMessage::isResponderMessage() const{
	return false;
}

int32_t MikeyMessage::keyAgreementType() const{
	throw MikeyExceptionUnimplemented(
		"Unimplemented type of MIKEY message" );
}
