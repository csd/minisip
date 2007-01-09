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
#include<libmikey/MikeyMessage.h>
#include<libmcrypto/OakleyDH.h>
#include<libmcrypto/SipSim.h>

using namespace std;

// 
// PeerCertificates
// 
PeerCertificates::PeerCertificates( MRef<certificate_chain *> aCert,
				    MRef<ca_db *> aCaDb ):
		certChainPtr( aCert ),
		certDbPtr( aCaDb )
{
	peerCertChainPtr = certificate_chain::create();
}

PeerCertificates::PeerCertificates( MRef<certificate_chain *> aCert,
				    MRef<certificate_chain *> aPeerCert ):
		certChainPtr( aCert ),
		peerCertChainPtr( aPeerCert )
{
}

PeerCertificates::~PeerCertificates(){
}

// 
// KeyAgreementDHBase
//
KeyAgreementDHBase::KeyAgreementDHBase():
	peerKeyPtr( NULL ),
	peerKeyLengthValue( 0 ),
	publicKeyPtr( NULL ),
	publicKeyLengthValue( 0 )
{
	dh = new OakleyDH();
	if( dh == NULL )
	{
		throw MikeyException( "Could not create "
				          "DH parameters." );
	}
}

KeyAgreementDHBase::~KeyAgreementDHBase(){
	delete dh;
	if( peerKeyPtr != NULL ){
		delete [] peerKeyPtr;
		peerKeyPtr = NULL;
	}
	if( publicKeyPtr != NULL ){
		delete [] publicKeyPtr;
		publicKeyPtr = NULL;
	}
}

// 
// KeyAgreementDH
// 
KeyAgreementDH::KeyAgreementDH( MRef<certificate_chain *> certChainPtr,
		MRef<ca_db *> certDbPtr ):
	KeyAgreement(),
	PeerCertificates( certChainPtr, certDbPtr ),
	useSim(false)
{
}

KeyAgreementDH::KeyAgreementDH( MRef<SipSim*> s ):
	PeerCertificates( s->getCertificateChain(), s->getCAs() ),
	useSim(true),
	sim(s)
{

}

KeyAgreementDH::~KeyAgreementDH(){
}

int32_t KeyAgreementDH::type(){
	return KEY_AGREEMENT_TYPE_DH;
}

int KeyAgreementDHBase::setGroup( int groupValue ){
	if( !dh->setGroup( groupValue ) )
		return 1;

	uint32_t len = dh->secretLength();

	if( len != tgkLength() || !tgk() ){
		setTgk( NULL, len );
	}

	int32_t length = dh->publicKeyLength();
	if( length != publicKeyLengthValue ){
		if( publicKeyPtr ){
			delete[] publicKeyPtr;
		}
		publicKeyLengthValue = length;
		publicKeyPtr = new unsigned char[ length ];
	}
	dh->getPublicKey( publicKeyPtr, length );

	return 0;
}
	
void KeyAgreementDHBase::setPeerKey( unsigned char * peerKeyPtr,
			      int peerKeyLengthValue ){
	if( this->peerKeyPtr )
		delete[] this->peerKeyPtr;

	this->peerKeyPtr = new unsigned char[ peerKeyLengthValue ];
	this->peerKeyLengthValue = peerKeyLengthValue;
	memcpy( this->peerKeyPtr, peerKeyPtr, peerKeyLengthValue );

}

int KeyAgreementDHBase::publicKeyLength(){
	return publicKeyLengthValue;
}

unsigned char * KeyAgreementDHBase::publicKey(){
	return publicKeyPtr;
}

int KeyAgreementDHBase::computeTgk(){
	assert( peerKeyPtr );

	int res = dh->computeSecret( peerKeyPtr, peerKeyLengthValue, tgk(), tgkLength() );
	return res;
}

int KeyAgreementDHBase::group(){
	if( !publicKeyPtr )
		return -1;

	return dh->group();
}

int KeyAgreementDHBase::peerKeyLength(){
	return peerKeyLengthValue;
}

unsigned char * KeyAgreementDHBase::peerKey(){
	return peerKeyPtr;
}

MRef<certificate_chain *> PeerCertificates::certificateChain(){
	return certChainPtr;
}

MRef<certificate_chain *> PeerCertificates::peerCertificateChain(){
	return peerCertChainPtr;
}

void PeerCertificates::setPeerCertificateChain( MRef<certificate_chain *> peerChain ){
	peerCertChainPtr = peerChain;
}

int PeerCertificates::controlPeerCertificate(){
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
