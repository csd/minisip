/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  
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
*/


#include<config.h>
#include<libmikey/MikeyMessage.h>
#include<libmikey/KeyAgreementPSK.h>
#include<string.h>

KeyAgreementPSK::KeyAgreementPSK():
		KeyAgreement(),t_received(0),authKey(NULL),authKeyLength(0),
		macAlg(0),pskPtr(NULL),pskLengthValue(0),v(0),tSentValue(0){
}

KeyAgreementPSK::KeyAgreementPSK( const unsigned char * pskPtr, int pskLengthValue ):
		KeyAgreement(),t_received(0),authKey(NULL),authKeyLength(0),
		macAlg(0),pskPtr(NULL),pskLengthValue(0),v(0),tSentValue(0){
	//policy = list<Policy_type *>::list();
	this->pskLengthValue = pskLengthValue;
	this->pskPtr = new unsigned char[ pskLengthValue ];
	memcpy( this->pskPtr, pskPtr, pskLengthValue );

}

KeyAgreementPSK::~KeyAgreementPSK(){
	if( pskPtr ){
		delete [] pskPtr;
	}

	if( authKey ){
		delete[] authKey;
		authKey = NULL;
	}
}

int32_t KeyAgreementPSK::type(){
	return KEY_AGREEMENT_TYPE_PSK;
}

void KeyAgreementPSK::generateTgk( uint32_t tgkLength ){
	// Generate random TGK
	setTgk( NULL, tgkLength );
}

void KeyAgreementPSK::genTranspEncrKey( 
		unsigned char * encrKey, int encrKeyLength ){
	keyDeriv( 0xFF, csbId(), pskPtr, pskLengthValue, 
			encrKey, encrKeyLength, KEY_DERIV_TRANS_ENCR );
}
	
void KeyAgreementPSK::genTranspSaltKey( 
		unsigned char * encrKey, int encrKeyLength ){
	keyDeriv( 0xFF, csbId(), pskPtr, pskLengthValue, 
			encrKey, encrKeyLength, KEY_DERIV_TRANS_SALT );
}

void KeyAgreementPSK::genTranspAuthKey( 
		unsigned char * encrKey, int encrKeyLength ){
	keyDeriv( 0xFF, csbId(), pskPtr, pskLengthValue, 
			encrKey, encrKeyLength, KEY_DERIV_TRANS_AUTH );
}

uint64_t KeyAgreementPSK::tSent(){
	return tSentValue;
}

void KeyAgreementPSK::setTSent( uint64_t tSent ){
	this->tSentValue = tSent;
}

MikeyMessage* KeyAgreementPSK::createMessage(){
	return MikeyMessage::create( this );
}

void KeyAgreementPSK::setPSK( const byte_t* psk, int pskLength ){
	if( pskPtr ){
		delete[] pskPtr;
		pskPtr = NULL;
	}

	pskLengthValue = pskLength;
	pskPtr = new byte_t[ pskLength ];
	memcpy( pskPtr, psk, pskLength );
}

int KeyAgreementPSK::getPSKLength(){
	return pskLengthValue;
}

byte_t* KeyAgreementPSK::getPSK(){
	return pskPtr;
}
