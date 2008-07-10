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
#include<string.h>

#include"MikeyMessageDH.h"
#include"MikeyMessagePSK.h"
#include"MikeyMessagePKE.h"
#include"MikeyMessageDHHMAC.h"
#include"MikeyMessageRSAR.h"

/// The signature calculation will be factor two faster if this 
/// guess is correct (128 bytes == 1024 bits)
#define GUESSED_SIGNATURE_LENGTH 128

using namespace std;


MikeyPayloads::MikeyPayloads():compiled(false), rawData(NULL){
}

MikeyPayloads::MikeyPayloads( int firstPayloadType, byte_t *message, int lengthLimit )
		:compiled(true), rawData(message)
{
	parse( firstPayloadType, message, lengthLimit, payloads );
}

MikeyMessage::MikeyMessage(){

}

MikeyMessage* MikeyMessage::create( KeyAgreementDH * ka ){
	return new MikeyMessageDH( ka );
}

MikeyMessage* MikeyMessage::create( KeyAgreementPSK * ka,
				    int encrAlg, int macAlg ){
	return new MikeyMessagePSK( ka, encrAlg, macAlg );
}

MikeyMessage* MikeyMessage::create( KeyAgreementDHHMAC * ka,
				    int macAlg ){
	return new MikeyMessageDHHMAC( ka, macAlg );
}

MikeyMessage* MikeyMessage::create( KeyAgreementPKE* ka,
				    int encrAlg, int macAlg ){
	return new MikeyMessagePKE( ka, encrAlg, macAlg );
}

MikeyMessage* MikeyMessage::create( KeyAgreementRSAR* ka ){
	return new MikeyMessageRSAR( ka );
}

/*
 * Alg.
 *  1. Parse HDR payload
 *  2. While not end of packet
 *    2.1 Parse payload (choose right class) and store next payload type.
 *    2.2 Add payload to list of all payloads in message.
*/ 

MikeyMessage* MikeyMessage::parse( byte_t * message, int lengthLimit )
{
	std::list<MRef<MikeyPayload*> > payloads;

	MikeyPayloads::parse( MIKEYPAYLOAD_HDR_PAYLOAD_TYPE,
			      message, lengthLimit, payloads );

	if( payloads.size() == 0 ){
		throw MikeyExceptionMessageContent( 
			"No payloads" );
	}

	MikeyPayloadHDR *hdr =
		dynamic_cast<MikeyPayloadHDR*>(**payloads.begin());

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
		case MIKEY_TYPE_PK_INIT:
		case MIKEY_TYPE_PK_RESP:
			msg = new MikeyMessagePKE();
			break;
		case MIKEY_TYPE_DHHMAC_INIT:
		case MIKEY_TYPE_DHHMAC_RESP:
			msg = new MikeyMessageDHHMAC();
			break;
		case MIKEY_TYPE_RSA_R_INIT:
		case MIKEY_TYPE_RSA_R_RESP:
			msg = new MikeyMessageRSAR();
			break;
		case MIKEY_TYPE_ERROR:
			msg = new MikeyMessage();
			break;
		default:
			throw MikeyExceptionUnimplemented(
				"Unimplemented type of message in INVITE" );
	}

	msg->setRawMessageData( message );
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
}

MikeyPayloads::~MikeyPayloads(){
	
	if( rawData ){
		delete [] rawData;
	}
	
	rawData = NULL;

}

static MRef<MikeyPayload*> parsePayload( int payloadType,
				   byte_t * msgpos, int limit ){
	MRef<MikeyPayload*> payload = NULL;

	switch (payloadType){
		case MIKEYPAYLOAD_HDR_PAYLOAD_TYPE:
			payload = new MikeyPayloadHDR(msgpos, limit);
			break;
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

	return payload;
}

void MikeyPayloads::parse( int firstPayloadType,
			   byte_t * message, int lengthLimit,
			  std::list<MRef<MikeyPayload*> >& payloads ){
	MRef<MikeyPayload*> hdr;
	byte_t * msgpos = message;
	int limit = lengthLimit;
						
	hdr = parsePayload( firstPayloadType,
			    message, limit );
	
	payloads.push_back( hdr );	

	limit -=  (int)( hdr->end() - msgpos );
	msgpos = hdr->end();

	int nextPayloadType = hdr->nextPayloadType();

	while( !(msgpos >= message + lengthLimit ) && 
			nextPayloadType != MikeyPayload::LastPayload){
	
		MRef<MikeyPayload*>payload = parsePayload( nextPayloadType,
						      msgpos, limit );

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

void MikeyPayloads::addPayload(MRef<MikeyPayload*>payload){

	compiled = false;
	// Put the nextPayloadType in the previous payload */
	if( payload->payloadType() != MIKEYPAYLOAD_HDR_PAYLOAD_TYPE ){
		list<MRef<MikeyPayload*> >::reverse_iterator i = payloads.rbegin();

		if( i != payloads.rend() ){
			(*i)->setNextPayloadType( payload->payloadType() );
		}
	}

	payloads.push_back( payload );
}

void MikeyPayloads::operator +=( MRef<MikeyPayload*> payload ){
	addPayload( payload );
}

static vector<byte_t> tsToVec( uint64_t ts ){
	vector<byte_t> vec;

	vec.resize( 8 );
	for( int i = 0; i < 8; i++ ){
		vec[ 8 - i - 1 ] = 
			(byte_t)((ts >> (i*8))&0xFF);
	}

	return vec;
}

vector<byte_t> MikeyPayloads::buildSignData( size_t sigLength,
					     bool useIdsT ){
	vector<byte_t> signData;

	byte_t * start = rawMessageData();
	byte_t * end = start;
	int diff=rawMessageLength() - (int)sigLength;
	assert(diff>=0);
	end+=diff;

	signData.insert( signData.end(), start, end);

	if( useIdsT ){
		vector<byte_t> vecIDi = extractIdVec( 0 );
		vector<byte_t> vecIDr = extractIdVec( 1 );
		MRef<MikeyPayload*> i;

		i = extractPayload( MIKEYPAYLOAD_T_PAYLOAD_TYPE );
		if( !i ){
			throw MikeyException( "Could not perform digital signature of the message, no T" );
		}

		MRef<MikeyPayloadT*> plT = dynamic_cast<MikeyPayloadT*>(*i);
		vector<byte_t> vecTs = tsToVec( plT->ts() );
	
		signData.insert( signData.end(), vecIDi.begin(), vecIDi.end() );
		signData.insert( signData.end(), vecIDr.begin(), vecIDr.end() );
		signData.insert( signData.end(), vecTs.begin(), vecTs.end() );
	}

	return signData;
}


void MikeyPayloads::addSignaturePayload( MRef<SipSim*> sim,
					 bool addIdsAndT ){
	byte_t signature[4096];
	int signatureLength=4096;
	MikeyPayloadSIGN * sign;
	MRef<MikeyPayload*> last;
	vector<byte_t> signData;
	
	// set the previous nextPayloadType to signature
	last = *lastPayload();
	last->setNextPayloadType( MIKEYPAYLOAD_SIGN_PAYLOAD_TYPE );

	// See the comment in addSignaturePayload(cert) for explanation of
	// the following steps.

	addPayload( ( sign = new MikeyPayloadSIGN( GUESSED_SIGNATURE_LENGTH, 
						   MIKEYPAYLOAD_SIGN_TYPE_RSA_PKCS ) ) );

	signData = buildSignData( GUESSED_SIGNATURE_LENGTH, addIdsAndT );

	if (!sim->getSignature( &signData.front(), (int)signData.size(), 
			 signature, signatureLength, true )){
		throw MikeyException( "Could not perform digital signature of the message" );
	}

	if (signatureLength!=GUESSED_SIGNATURE_LENGTH){	// if the length field in the signature payload was 
							// wrong, we have to redo the signature
		sign->setSigData(signature, signatureLength); // the length needs to be set to the correct value
		signData = buildSignData( signatureLength, addIdsAndT );

		sim->getSignature( &signData.front(), (int)signData.size(),
				signature, signatureLength, true );
	}

	sign->setSigData( signature, signatureLength );
	compiled = false;
}


void MikeyPayloads::addSignaturePayload( MRef<Certificate *> cert,
					 bool addIdsAndT ){
	byte_t signature[4096];
	int signatureLength = 128;
	MikeyPayloadSIGN * sign;
	MRef<MikeyPayload*> last;
	vector<byte_t> signData;
	
	// set the previous nextPayloadType to signature
	last = *lastPayload();
	last->setNextPayloadType( MIKEYPAYLOAD_SIGN_PAYLOAD_TYPE );

	//The SIGN payload constructor can not take the signature as
	//parameter. This is because it can not be computed before
	//the SIGN payload has been added to the MIKEY message (the next
	//payload field in the payload before SIGN is not set).
	//
	//We guess that the length of the signature is 1024 bits. We then
	//calculate the signature, and if it turns out that it was not
	//1024, we have to re-do the signature calculation with the correct
	//length.
	//
	//Double-signatures would be avoided if the Certificate had a 
	//method to find out the length of the signature.
	
	addPayload( ( sign = new MikeyPayloadSIGN(GUESSED_SIGNATURE_LENGTH, MIKEYPAYLOAD_SIGN_TYPE_RSA_PKCS ) ) );

	signData = buildSignData( GUESSED_SIGNATURE_LENGTH, addIdsAndT );

	if (cert->signData( &signData.front(), (int)signData.size(),
			 signature, &signatureLength )){
		throw MikeyException( "Could not perform digital signature of the message" );
	}


	if (signatureLength!=GUESSED_SIGNATURE_LENGTH){	// if the length field in the signature payload was 
							// wrong, we have to redo the signature
		sign->setSigData(signature, signatureLength); // the length needs to be set to the correct value
		signData = buildSignData( signatureLength, addIdsAndT );

		cert->signData( &signData.front(), (int)signData.size(),
				 signature, &signatureLength );
	}

	sign->setSigData( signature, signatureLength ); // the payload signature is a dummy value until we do this
	compiled = false;
}

void MikeyPayloads::addKemacPayload( byte_t * tgk, int tgkLength,
		                      byte_t * encrKey,
				      byte_t * iv,
				      byte_t * authKey,
				     int encrAlg, int macAlg,
				     bool kemacOnly ){
	byte_t * encrData = new byte_t[ tgkLength ];
	AES * aes;
	MRef<MikeyPayload*> last;
	
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
	byte_t* macInput = NULL;
	byte_t* macInputPtr = NULL;
	unsigned int macInputLength = 0;
	
	payload = new MikeyPayloadKEMAC( encrAlg, tgkLength, encrData,
					 macAlg, macData );
	addPayload( payload );

	if( kemacOnly ){
		macInputLength = payload->length();
		macInput = new byte_t[ macInputLength ];
		payload->writeData( macInput, macInputLength );
		macInput[0] = MIKEYPAYLOAD_LAST_PAYLOAD;
		macInputPtr = macInput;
	}
	else{
		macInputPtr = rawMessageData();
		macInputLength = rawMessageLength();
	}

	switch( macAlg ){
		case MIKEY_PAYLOAD_KEMAC_MAC_HMAC_SHA1_160:{
			hmac_sha1( authKey, 20,
				macInputPtr,
				// Compute the MAC over the mac input,
				// MAC field excluded
				macInputLength - 20,
				macData, &macDataLength );
			
			//assert( macDataLength == 20 );
			payload->setMac( macData );
			break;
		}
		case MIKEY_PAYLOAD_KEMAC_MAC_NULL:
			break;
		default:
			delete [] encrData;
			throw MikeyException( "Unknown MAC algorithm" );
	}
	compiled = false;
	delete [] encrData;
	if( macInput ){
		delete[] macInput;
		macInput = NULL;
	}
}

void MikeyPayloads::addVPayload( int macAlg, uint64_t t,
		byte_t * authKey, uint32_t authKeyLength ){
		MikeyPayloadV * payload;
		unsigned int hmacOutputLength;
		byte_t hmacOutput[20];
		byte_t * hmacInput;
		unsigned int messageLength;
		byte_t * messageData;

	MRef<MikeyPayload*> last;
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


void MikeyPayloads::compile(){
	if (compiled){
		throw MikeyExceptionMessageContent("BUG: trying to compile already compiled message.");
	}
	if( rawData )
		delete [] rawData;

	rawData = new byte_t[ rawMessageLength() ];	
	
	list<MRef<MikeyPayload*> >::iterator i;
	byte_t *pos = rawData;
	for (i=payloads.begin(); i!=payloads.end(); i++){
		int len = (*i)->length();
		(*i)->writeData(pos,len);
		pos+=len;
	}

}

byte_t *MikeyPayloads::rawMessageData(){
	
	if (!compiled)
		compile();
	return rawData;
}

int MikeyPayloads::rawMessageLength(){
	list<MRef<MikeyPayload*> >::iterator i;
	int length=0;
	for (i=payloads.begin(); i!=payloads.end(); i++){
		length+=(*i)->length();
	}

	return length;
}

void MikeyPayloads::setRawMessageData( byte_t *data ){
	if( rawData ){
		delete[] rawData;
		rawData = NULL;
	}

	rawData = data;
	compiled = true;
}

string MikeyPayloads::debugDump(){
	string ret="";
	list<MRef<MikeyPayload*> >::iterator i;
	for (i=payloads.begin(); i!=payloads.end(); i++)
	{
		ret=ret+"\n\n"+(*i)->debugDump();
	}
	
	return ret;
}

list<MRef<MikeyPayload*> >::const_iterator MikeyPayloads::firstPayload() const{
	return payloads.begin();
}

list<MRef<MikeyPayload*> >::const_iterator MikeyPayloads::lastPayload() const{
	return --payloads.end();
}

list<MRef<MikeyPayload*> >::iterator MikeyPayloads::firstPayload(){
	return payloads.begin();
}

list<MRef<MikeyPayload*> >::iterator MikeyPayloads::lastPayload(){
	return --payloads.end();
}

string MikeyPayloads::b64Message(){
	return base64_encode( rawMessageData(), rawMessageLength() );
}

uint32_t MikeyMessage::csbId(){
	MRef<MikeyPayload*> hdr = * firstPayload();
	if( hdr->payloadType() != MIKEYPAYLOAD_HDR_PAYLOAD_TYPE ){
		throw MikeyExceptionMessageContent( 
				"First payload was not a header" );
	}
	return dynamic_cast<MikeyPayloadHDR *>(*hdr)->csbId();
}

int MikeyMessage::type() const{
	MRef<const MikeyPayload*> hdr = extractPayload( MIKEYPAYLOAD_HDR_PAYLOAD_TYPE );
	if( hdr.isNull() ){
		throw MikeyExceptionMessageContent( 
			"No header in the payload" );
	}

	return dynamic_cast<const MikeyPayloadHDR *>(*hdr)->dataType();
}

MRef<MikeyPayload*> MikeyPayloads::extractPayload( int payloadType ){
	list<MRef<MikeyPayload*> >::iterator i;

	for( i = payloads.begin(); i != payloads.end(); i++ ){
		if( (*i)->payloadType() == payloadType ){
			return *i;
		}
	}
	return NULL;
}

MRef<const MikeyPayload*> MikeyPayloads::extractPayload( int payloadType ) const{
	list<MRef<MikeyPayload*> >::const_iterator i;

	for( i = payloads.begin(); i != payloads.end(); i++ ){
		if( (*i)->payloadType() == payloadType ){
			return **i;
		}
	}
	return NULL;
}

void MikeyPayloads::remove( MRef<MikeyPayload*> payload ){
	list<MRef<MikeyPayload*> >::iterator i;

	for( i = payloads.begin(); i != payloads.end(); i++ ){
		if( *i == payload ){
			payloads.erase( i ); 
			return;
		}
	}
}

void MikeyPayloads::addPolicyToPayload(KeyAgreement * ka){
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

void MikeyPayloads::addPolicyTo_ka(KeyAgreement * ka){
#define SP dynamic_cast<MikeyPayloadSP *>(*i)
	// Adding policy to ka
	int policy_i, policy_j;
	MikeyPolicyParam * PParam;
	MRef<MikeyPayload*> i;
	while ( 1 ){
		i = extractPayload( MIKEYPAYLOAD_SP_PAYLOAD_TYPE );
		if( i.isNull() ){
			break;
		}
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

MRef<MikeyMessage *> MikeyMessage::parseResponse( KeyAgreement  * ka ){
	throw MikeyExceptionUnimplemented( "parseResponse not implemented" );
}

void MikeyMessage::setOffer( KeyAgreement * ka ){
	throw MikeyExceptionUnimplemented( "setOffer not implemented" );
}

MRef<MikeyMessage *> MikeyMessage::buildResponse( KeyAgreement * ka ){
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

bool MikeyPayloads::deriveTranspKeys( KeyAgreementPSK* ka,
					byte_t*& encrKey, byte_t *& iv,
					unsigned int& encrKeyLength,
					int encrAlg, int macAlg,
					uint64_t t,
					MikeyMessage* errorMessage ){
	// Derive the transport keys from the env_key:
	byte_t* authKey = NULL;
	bool error = false;
	unsigned int authKeyLength = 0;
	int i;

	encrKey = NULL;
	iv = NULL;
	encrKeyLength = 0;

	switch( encrAlg ){
		case MIKEY_ENCR_AES_CM_128: {
			byte_t saltKey[14];
			encrKeyLength = 16;
			encrKey = new byte_t[ encrKeyLength ];
			ka->genTranspEncrKey(encrKey, encrKeyLength);
			ka->genTranspSaltKey(saltKey, sizeof(saltKey));
			iv = new byte_t[ encrKeyLength ];
			iv[0] = saltKey[0];
			iv[1] = saltKey[1];
			for( i = 2; i < 6; i++ ){
				iv[i] = saltKey[i] ^ (ka->csbId() >> (5-i)*8) & 0xFF;
			}

			for( i = 6; i < 14; i++ ){
				iv[i] = (byte_t)(saltKey[i] ^ (t >> (13-i)*8) & 0xFF);
			}
			iv[14] = 0x00;
			iv[15] = 0x00;
			break;
		}
		case MIKEY_ENCR_NULL:
			break;
		case MIKEY_ENCR_AES_KW_128:
			//TODO
		default:
			error = true;
			if( errorMessage ){
				errorMessage->addPayload( 
					new MikeyPayloadERR( MIKEY_ERR_TYPE_INVALID_EA ) );
			}
			else{
				throw MikeyException( "Unknown encryption algorithm" );
			}
	}
	switch( macAlg ){
		case MIKEY_MAC_HMAC_SHA1_160:
			authKeyLength = 20;
			authKey = new byte_t[ authKeyLength ];
			ka->genTranspAuthKey(authKey, authKeyLength);
			break;
		case MIKEY_MAC_NULL:
			authKey = NULL;
			break;
		default:
			error = true;
			if( errorMessage ){
				errorMessage->addPayload( 
					new MikeyPayloadERR( MIKEY_ERR_TYPE_INVALID_HA ) );
			}
			else{
				throw MikeyException( "Unknown MAC algorithm" );
			}
	}

	ka->macAlg = macAlg;
	if( ka->authKey ){
		delete[] ka->authKey;
	}
	ka->authKey = authKey;
	ka->authKeyLength = authKeyLength;
	return !error;
}

void MikeyPayloads::addCertificatePayloads( MRef<CertificateChain *> certChain ){
	if( certChain.isNull() ){
		cerr << "No Certificates" << endl;
		return;
	}

	certChain->lock();
	certChain->initIndex();
	MRef<Certificate*> cert = certChain->getNext();
	while( ! cert.isNull() ){
		MRef<MikeyPayload*> payload =
			new MikeyPayloadCERT( MIKEYPAYLOAD_CERT_TYPE_X509V3SIGN,
					      cert);
		addPayload( payload );
		cert = certChain->getNext();
	}

	certChain->unlock();
}


MRef<CertificateChain*> MikeyPayloads::extractCertificateChain() const{
	MRef<CertificateChain *> peerChain;

	/* Try to find the Certificate chain in the message */
	list<MRef<MikeyPayload*> >::const_iterator i;
	list<MRef<MikeyPayload*> >::const_iterator last = lastPayload();

	for( i = firstPayload(); i != last; i++ ){
		MRef<MikeyPayload*> payload = *i;

		if( payload->payloadType() != MIKEYPAYLOAD_CERT_PAYLOAD_TYPE )
			continue;

		MikeyPayloadCERT * certPayload =
			dynamic_cast<MikeyPayloadCERT*>(*payload);
		MRef<Certificate*> peerCert = 
			Certificate::load( certPayload->certData(),
					   certPayload->certLength() );

		if( peerChain.isNull() ){
			peerChain = CertificateChain::create();
		}

// 		cerr << "Add Certificate: " << peerCert->get_name() << endl;

		peerChain->addCertificate( peerCert );
	}

	return peerChain;
}

bool MikeyPayloads::verifySignature( MRef<Certificate*> cert,
				     bool addIdsAndT ){
	MRef<MikeyPayload*> payload =
		extractPayload(MIKEYPAYLOAD_SIGN_PAYLOAD_TYPE);

	if( !payload ){
		throw MikeyExceptionMessageContent( "No SIGN payload" );
	}

	MikeyPayloadSIGN* sig = dynamic_cast<MikeyPayloadSIGN*>(*payload);
	vector<byte_t> signData;

	signData = buildSignData( sig->sigLength(), addIdsAndT );

	int res = cert->verifSign( &signData.front(), (int)signData.size(),
				    sig->sigData(),
				    sig->sigLength() );
	return res > 0;
}

bool MikeyPayloads::verifyKemac( KeyAgreementPSK* ka,
				 bool kemacOnly ){
	int macAlg;
	byte_t * receivedMac;
	byte_t * macInput;
	unsigned int macInputLength;

	MRef<MikeyPayload*> payload =
		extractPayload(MIKEYPAYLOAD_KEMAC_PAYLOAD_TYPE);

	if( !payload ){
		throw MikeyExceptionMessageContent("No KEMAC payload");
	}

	MikeyPayloadKEMAC * kemac;

	kemac = dynamic_cast<MikeyPayloadKEMAC *>(*payload);

	macAlg = kemac->macAlg();
	receivedMac = kemac->macData();
		
	if( kemacOnly ){
		macInputLength = kemac->length();
		macInput = new byte_t[macInputLength];
		kemac->writeData( macInput, macInputLength );
		macInput[0] = MIKEYPAYLOAD_LAST_PAYLOAD;
	}
	else{
		macInputLength = rawMessageLength();
		macInput = new byte_t[macInputLength];
		memcpy( macInput, rawMessageData(), rawMessageLength() );
	}

	macInputLength -= 20; // Subtract mac data
	bool ret = verifyMac( ka, macAlg, receivedMac,
			      macInput, macInputLength );

	delete[] macInput;
	return ret;
}

bool MikeyPayloads::verifyV( KeyAgreementPSK* ka ){
	int macAlg;
	byte_t * receivedMac;
	byte_t * macInput;
	unsigned int macInputLength;
	MikeyPayloadV * v;
	uint64_t t_sent = ka->tSent();
	MRef<MikeyPayload*> payload =
		extractPayload(MIKEYPAYLOAD_V_PAYLOAD_TYPE);

	if( !payload ){
		throw MikeyExceptionMessageContent("No V payload");
	}

	v = dynamic_cast<MikeyPayloadV*>(*payload);

	macAlg = v->macAlg();
	receivedMac = v->verData();
	// macInput = raw_messsage without mac / sent_t
	macInputLength = rawMessageLength() - 20 + 8;
	macInput = new byte_t[macInputLength];
	memcpy( macInput, rawMessageData(), rawMessageLength() - 20 );
	
	for( int i = 0; i < 8; i++ ){
		macInput[ macInputLength - i - 1 ] = 
			(byte_t)((t_sent >> (i*8))&0xFF);
	}

	bool ret = verifyMac( ka, macAlg, receivedMac,
			      macInput, macInputLength );

	delete[] macInput;
	return ret;
}

bool MikeyPayloads::verifyMac( KeyAgreementPSK* ka, int macAlg,
			       const byte_t* receivedMac,
			       const byte_t* macInput,
			       unsigned int macInputLength ) const{
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

			for( int i = 0; i < 20; i++ ){
				if( computedMac[i] != receivedMac[i] ){
					ka->setAuthError(
						"MAC mismatch."
					);
					return false;
				}
			}
			return true;
		case MIKEY_MAC_NULL:
			return true;
		default:
			throw MikeyException( "Unknown MAC algorithm" );
	}
}

void MikeyPayloads::addPkeKemac( KeyAgreementPKE* ka,
				    int encrAlg, int macAlg ){

	// Derive the transport keys from the env_key:
	byte_t* encrKey = NULL;
	byte_t* iv = NULL;
	unsigned int encrKeyLength = 0;
	
	deriveTranspKeys( ka, encrKey, iv, encrKeyLength,
			  encrAlg, macAlg, ka->tSent(),
			  NULL );

	//adding KEMAC payload
	MikeyPayloads* subPayloads = new MikeyPayloads();
	MikeyPayloadKeyData* keydata = 
		new MikeyPayloadKeyData(KEYDATA_TYPE_TGK, ka->tgk(),
							ka->tgkLength(), ka->keyValidity());

	subPayloads->addId( ka->uri() );
 	subPayloads->addPayload( keydata );
	keydata = NULL;

	unsigned int rawKeyDataLength = subPayloads->rawMessageLength();
	byte_t* rawKeyData = new byte_t[ rawKeyDataLength ];
	memcpy( rawKeyData, subPayloads->rawMessageData(), rawKeyDataLength );

	addKemacPayload(rawKeyData, rawKeyDataLength,
			encrKey, iv, ka->authKey, encrAlg, macAlg, true );

	if( encrKey ){
		delete[] encrKey;
		encrKey = NULL;
	}

	if( iv ){
		delete[] iv;
		iv = NULL;
	}

	delete subPayloads;
	subPayloads = NULL;

 	delete [] rawKeyData;
	rawKeyData = NULL;

	//adding PKE payload
	MRef<Certificate*> certResponder =
		ka->peerCertificateChain()->getFirst();

	byte_t* env_key = ka->getEnvelopeKey();
	int encEnvKeyLength = 8192; // TODO autodetect?
	unsigned char* encEnvKey = new unsigned char[ encEnvKeyLength ];

	if( !certResponder->publicEncrypt( env_key, ka->getEnvelopeKeyLength(),
					    encEnvKey, &encEnvKeyLength ) ){
		throw MikeyException( "PKE encryption of envelope key failed" );
	}

	addPayload(new MikeyPayloadPKE(2, encEnvKey, encEnvKeyLength));

 	delete [] encEnvKey;
	encEnvKey = NULL;
}

bool MikeyPayloads::extractPkeEnvKey( KeyAgreementPKE* ka ) const{
	MRef<const MikeyPayload*> payloadPke =
		extractPayload( MIKEYPAYLOAD_PKE_PAYLOAD_TYPE );
	if( !payloadPke ){
		throw MikeyException( "PKE init did not contain PKE payload" );
	}

	const MikeyPayloadPKE *pke =
		dynamic_cast<const MikeyPayloadPKE*>( *payloadPke );

	if( !pke ){
		throw MikeyException( "PKE init did not contain PKE payload" );
	}

	MRef<Certificate*> cert = ka->certificateChain()->getFirst();
	int envKeyLength = pke->dataLength();
	byte_t *envKey = new byte_t[ envKeyLength ];
		
	if( !cert->privateDecrypt( pke->data(), pke->dataLength(),
				    envKey, &envKeyLength ) ){
		throw MikeyException( "Decryption of envelope key failed" );
	}

	ka->setEnvelopeKey( envKey, envKeyLength );

	delete[] envKey;
	envKey = NULL;
	return true;
}

void MikeyPayloads::addId( const string &theId ){
 	int type = MIKEYPAYLOAD_ID_TYPE_URI;
	string id = theId;

	if( id.substr( 0, 4 ) == "nai:" ){
		type = MIKEYPAYLOAD_ID_TYPE_NAI;
		id = id.substr( 4 );
	}

	MikeyPayloadID* initId =
		new MikeyPayloadID( type, (int)id.size(), (byte_t*)id.c_str() );
	addPayload( initId );
}

const MikeyPayloadID* MikeyPayloads::extractId( int index ) const{
	const MikeyPayloadID *id = NULL;
	list<MRef<MikeyPayload*> >::const_iterator i;
	list<MRef<MikeyPayload*> >::const_iterator last = lastPayload();
	int j;
	
	for( i = firstPayload(), j = 0; i != last; i++ ){
		MRef<MikeyPayload*> payload = *i;

		if( payload->payloadType() == MIKEYPAYLOAD_ID_PAYLOAD_TYPE ){
			if( j == index ){
				id = dynamic_cast<const MikeyPayloadID*>(*payload);
				break;
			}

			j++;
		}
	}

	return id;
}


string MikeyPayloads::extractIdStr( int index ) const{
	const MikeyPayloadID *id = extractId( index );

	if( !id ){
		return "";
	}

	string idData = string( (const char*)id->idData(), id->idLength() );
	string idStr;

	switch( id->idType() ){
		case MIKEYPAYLOAD_ID_TYPE_NAI:
			idStr = "nai:" + idData;
			break;

		case MIKEYPAYLOAD_ID_TYPE_URI:
			idStr = idData;
			break;
			
		default:
			return "";
	}

	return idStr;
}


vector<byte_t> MikeyPayloads::extractIdVec( int index ) const{
	const MikeyPayloadID *id = extractId( index );
	vector<byte_t> result;

	if( !id ){
		return result;
	}

	result.resize( id->idLength() );
	memcpy( &result.front(), id->idData(), id->idLength() );
	return result;
}
