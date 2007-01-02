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
#include<libmikey/keyagreement_dh.h>
#include<libmikey/MikeyException.h>
#include<libmcrypto/OakleyDH.h>
#include<libmcrypto/SipSim.h>

using namespace std;

KeyAgreementDH::KeyAgreementDH( MRef<certificate_chain *> certChainPtr,
		MRef<ca_db *> certDbPtr ):
	KeyAgreement(),
	useSim(false),
	peerKeyPtr( NULL ),
	peerKeyLengthValue( 0 ),
	certChainPtr( certChainPtr ),
	certDbPtr( certDbPtr )
{
	//policy = list<Policy_type *>::list();
	typeValue = KEY_AGREEMENT_TYPE_DH;
	dh = new OakleyDH();
	peerCertChainPtr = certificate_chain::create();

}

KeyAgreementDH::KeyAgreementDH( MRef<SipSim*> s ):
	KeyAgreement(),
	useSim(true),
	peerKeyPtr( NULL ),
	peerKeyLengthValue( 0 ),
	certChainPtr( NULL ),
	certDbPtr( NULL ),
	sim(s)
{
	//policy = list<Policy_type *>::list();
	typeValue = KEY_AGREEMENT_TYPE_DH;
	dh = new OakleyDH();
	peerCertChainPtr = certificate_chain::create();

}

KeyAgreementDH::~KeyAgreementDH(){
	delete dh;
	if( peerKeyPtr != NULL ){
		delete [] peerKeyPtr;
		peerKeyPtr = NULL;
	}
}

KeyAgreementDH::KeyAgreementDH( MRef<certificate_chain *> certChainPtr,
		MRef<ca_db *> certDbPtr, int groupValue ):
	useSim(false),
	peerKeyPtr( NULL ),
	peerKeyLengthValue( 0 ),
	certChainPtr( certChainPtr ),
	peerCertChainPtr( NULL ),
	certDbPtr( certDbPtr ){
	//policy = list<Policy_type *>::list();
	typeValue = KEY_AGREEMENT_TYPE_DH;
	dh = new OakleyDH();
	if( dh == NULL )
	{
		throw MikeyException( "Could not create "
				          "DH parameters." );
	}

	if( setGroup( groupValue ) ){
		throw MikeyException( "Could not set the  "
				      "DH group." );
	}
	peerCertChainPtr = certificate_chain::create();
}


KeyAgreementDH::KeyAgreementDH( MRef<SipSim*> s, int groupValue ):
	useSim(true),
	peerKeyPtr( NULL ),
	peerKeyLengthValue( 0 ),
	certChainPtr( NULL ),
	peerCertChainPtr( NULL ),
	certDbPtr( NULL ),
	sim(s)
{
	//policy = list<Policy_type *>::list();
	typeValue = KEY_AGREEMENT_TYPE_DH;
	dh = new OakleyDH();
	if( dh == NULL )
	{
		throw MikeyException( "Could not create "
				          "DH parameters." );
	}

	if( setGroup( groupValue ) ){
		throw MikeyException( "Could not set the  "
				      "DH group." );
	}
	peerCertChainPtr = certificate_chain::create();
}

int KeyAgreementDH::setGroup( int groupValue ){
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
	
void KeyAgreementDH::setPeerKey( unsigned char * peerKeyPtr,
			      int peerKeyLengthValue ){
	if( this->peerKeyPtr )
		delete[] this->peerKeyPtr;

	this->peerKeyPtr = new unsigned char[ peerKeyLengthValue ];
	this->peerKeyLengthValue = peerKeyLengthValue;
	memcpy( this->peerKeyPtr, peerKeyPtr, peerKeyLengthValue );

}

int KeyAgreementDH::publicKeyLength(){
	return dh->publicKeyLength();
}

unsigned char * KeyAgreementDH::publicKey(){
	unsigned char * publicKey;
	uint32_t length = publicKeyLength();
	publicKey = new unsigned char[ length ];
	dh->getPublicKey( publicKey, length );
	return publicKey;

}

int KeyAgreementDH::computeTgk(){
	assert( peerKeyPtr );

	int res = dh->computeSecret( peerKeyPtr, peerKeyLengthValue, tgkPtr, tgkLengthValue );
	return res;
}

int KeyAgreementDH::group(){
	return dh->group();

}

int KeyAgreementDH::peerKeyLength(){
	return peerKeyLengthValue;
}

unsigned char * KeyAgreementDH::peerKey(){
	return peerKeyPtr;
}

MRef<certificate_chain *> KeyAgreementDH::certificateChain(){
	if (useSim){
		return sim->getCertificateChain();
	}else{
		return certChainPtr;
	}
}

MRef<certificate_chain *> KeyAgreementDH::peerCertificateChain(){
	return peerCertChainPtr;
}

void KeyAgreementDH::addPeerCertificate( MRef<certificate *> peerCertPtr ){
	if( this->peerCertChainPtr.isNull() ){
		this->peerCertChainPtr = certificate_chain::create();
	}
	
	this->peerCertChainPtr->lock();
	this->peerCertChainPtr->add_certificate( peerCertPtr );
	this->peerCertChainPtr->unlock();
}

int KeyAgreementDH::controlPeerCertificate(){
	if( peerCertChainPtr.isNull() || certDbPtr.isNull() )
		return 0;
	return peerCertChainPtr->control( certDbPtr );
}

MikeyMessage* KeyAgreementDH::createMessage(){
	return MikeyMessage::create( this );
}

MRef<SipSim*> KeyAgreementDH::getSim(){
	return sim;
}
