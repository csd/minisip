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
#include<libmikey/keyagreement_psk.h>

KeyAgreementPSK::KeyAgreementPSK( unsigned char * pskPtr, int pskLengthValue )
	:KeyAgreement(), tSentValue( 0 ){
	//policy = list<Policy_type *>::list();
	typeValue = KEY_AGREEMENT_TYPE_PSK;
	this->pskLengthValue = pskLengthValue;
	this->pskPtr = new unsigned char[ pskLengthValue ];
	memcpy( this->pskPtr, pskPtr, pskLengthValue );

}

KeyAgreementPSK::~KeyAgreementPSK(){
	if( pskPtr ){
		delete [] pskPtr;
	}

}

void KeyAgreementPSK::generateTgk( uint32_t tgkLength ){
	typeValue = KEY_AGREEMENT_TYPE_PSK;
	this->tgkLengthValue = tgkLength;
	if( tgkPtr ){
		delete [] tgkPtr;
	}
	
	tgkPtr = new unsigned char[ tgkLength ];
	RAND_bytes( tgkPtr, tgkLength );
}

void KeyAgreementPSK::genTranspEncrKey( 
		unsigned char * encrKey, int encrKeyLength ){
	keyDeriv( 0xFF, csbIdValue, pskPtr, pskLengthValue, 
			encrKey, encrKeyLength, KEY_DERIV_TRANS_ENCR );
}
	
void KeyAgreementPSK::genTranspSaltKey( 
		unsigned char * encrKey, int encrKeyLength ){
	keyDeriv( 0xFF, csbIdValue, pskPtr, pskLengthValue, 
			encrKey, encrKeyLength, KEY_DERIV_TRANS_SALT );
}

void KeyAgreementPSK::genTranspAuthKey( 
		unsigned char * encrKey, int encrKeyLength ){
	keyDeriv( 0xFF, csbIdValue, pskPtr, pskLengthValue, 
			encrKey, encrKeyLength, KEY_DERIV_TRANS_AUTH );
}

uint64_t KeyAgreementPSK::tSent(){
	return tSentValue;
}

void KeyAgreementPSK::setTSent( uint64_t tSent ){
	this->tSentValue = tSent;
}


MikeyMessage * KeyAgreementPSK::parseResponse( MikeyMessage * response)
{
	return response->parseResponse( this );
}

void KeyAgreementPSK::setOffer( MikeyMessage * offer )
{
	offer->setOffer( this );
	return;
}

MikeyMessage * KeyAgreementPSK::buildResponse( MikeyMessage * offer)
{
	return offer->buildResponse( this );
}

bool KeyAgreementPSK::authenticate( MikeyMessage * msg)
{
	return msg->authenticate( this );
}
