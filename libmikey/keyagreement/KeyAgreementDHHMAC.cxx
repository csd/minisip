/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
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
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/

#include<config.h>
#include<libmcrypto/rand.h>
#include<libmikey/MikeyMessage.h>
#include<libmikey/MikeyException.h>
#include<libmikey/KeyAgreementDHHMAC.h>
#include<libmcrypto/OakleyDH.h>

#include<string>

using namespace std;

KeyAgreementDHHMAC::KeyAgreementDHHMAC( unsigned char * pskPtr,
					int pskLengthValue,
					int groupValue )
		:KeyAgreement(), pskPtr( NULL ), pskLengthValue( 0 ),
		 dh( NULL ), peerKeyPtr( NULL ), peerKeyLengthValue( 0 ),
		 tSentValue( 0 ), t_received( 0 ), m_authKey( NULL ),
		 m_authKeyLength( 0 ), m_macAlg( 0 ){
	typeValue = KEY_AGREEMENT_TYPE_DHHMAC;
	this->pskLengthValue = pskLengthValue;
	this->pskPtr = new unsigned char[ pskLengthValue ];
	memcpy( this->pskPtr, pskPtr, pskLengthValue );
	dh = new OakleyDH();
	if( dh == NULL )
	{
		throw MikeyException( "Could not create "
				          "DH parameters." );
	}

	if( groupValue >= 0 && setGroup( groupValue ) ){
		throw MikeyException( "Could not set the  "
				      "DH group." );
	}
}

KeyAgreementDHHMAC::~KeyAgreementDHHMAC(){
	if( pskPtr ){
		delete [] pskPtr;
	}
}

int KeyAgreementDHHMAC::setGroup( int groupValue ){
	if( !dh->setGroup( groupValue ) )
		return 1;

	uint32_t len = dh->secretLength();

	if( len != tgkLengthValue || !tgkPtr ){
		if( tgkPtr )
			delete[] tgkPtr;
		tgkPtr = new unsigned char[ len ];
	}

	tgkLengthValue = len;
	return 0;
}

int KeyAgreementDHHMAC::group(){
	return dh->group();

}

void KeyAgreementDHHMAC::setAuthKey( int macAlg, byte_t *authKey,
				     unsigned int authKeyLength ){
	m_macAlg = macAlg;
	m_authKey = authKey;
	m_authKeyLength = authKeyLength;
}

int KeyAgreementDHHMAC::getMacAlg(){
	return m_macAlg;
}

void KeyAgreementDHHMAC::setPeerKey( unsigned char * peerKeyPtr,
			      int peerKeyLengthValue ){
	if( this->peerKeyPtr )
		delete[] this->peerKeyPtr;

	this->peerKeyPtr = new unsigned char[ peerKeyLengthValue ];
	this->peerKeyLengthValue = peerKeyLengthValue;
	memcpy( this->peerKeyPtr, peerKeyPtr, peerKeyLengthValue );

}

int KeyAgreementDHHMAC::peerKeyLength(){
	return peerKeyLengthValue;
}

unsigned char * KeyAgreementDHHMAC::peerKey(){
	return peerKeyPtr;
}

int KeyAgreementDHHMAC::publicKeyLength(){
	return dh->publicKeyLength();
}

unsigned char * KeyAgreementDHHMAC::publicKey(){
	unsigned char * publicKey;
	uint32_t length = publicKeyLength();
	publicKey = new unsigned char[ length ];
	dh->getPublicKey( publicKey, length );
	return publicKey;

}

int KeyAgreementDHHMAC::computeTgk(){
	assert( peerKeyPtr );

	int res = dh->computeSecret( peerKeyPtr, peerKeyLengthValue,
				     tgkPtr, tgkLengthValue );
	return res;
}

void KeyAgreementDHHMAC::genTranspAuthKey( 
		unsigned char * encrKey, int encrKeyLength ){
	keyDeriv( 0xFF, csbIdValue, pskPtr, pskLengthValue, 
			encrKey, encrKeyLength, KEY_DERIV_TRANS_AUTH );
}

uint64_t KeyAgreementDHHMAC::tSent(){
	return tSentValue;
}

void KeyAgreementDHHMAC::setTSent( uint64_t tSent ){
	this->tSentValue = tSent;
}

MikeyMessage* KeyAgreementDHHMAC::createMessage(){
	return MikeyMessage::create( this );
}
