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
#include<libmikey/KeyAgreementDH.h>
#include<libmikey/MikeyException.h>
#include<libmikey/MikeyMessage.h>
#include<libmcrypto/OakleyDH.h>
#include<libmcrypto/SipSim.h>
#include<algorithm>
#include<string.h>

#ifdef SCSIM_SUPPORT
#include<libmcrypto/SipSimSmartCardGD.h>
#endif

using namespace std;

// 
// PeerCertificates
// 
PeerCertificates::PeerCertificates( MRef<CertificateChain *> aCert,
				    MRef<CertificateSet *> aCaDb ):
		certChainPtr( aCert ),
		certDbPtr( aCaDb )
{
	peerCertChainPtr = CertificateChain::create();
}

PeerCertificates::PeerCertificates( MRef<CertificateChain *> aCert,
				    MRef<CertificateChain *> aPeerCert ):
		certChainPtr( aCert ),
		peerCertChainPtr( aPeerCert )
{
}

PeerCertificates::~PeerCertificates(){
}

// 
// KeyAgreementDHBase
//
KeyAgreementDHBase::KeyAgreementDHBase(MRef<SipSim *> s):
	sim(s),
	dh(NULL),
	peerKeyPtr( NULL ),
	peerKeyLengthValue( 0 ),
	publicKeyPtr( NULL ),
	publicKeyLengthValue( 0 )

{
#ifdef SCSIM_SUPPORT
	if (sim && dynamic_cast<SipSimSmartCardGD*>(*sim)){
		// done - DH is implemented on-card
	}
	else
#endif
		{
			dh = new OakleyDH();
			if( dh == NULL )
			{
				throw MikeyException( "Could not create "
						"DH parameters." );
			}
		}
}

KeyAgreementDHBase::~KeyAgreementDHBase(){
	if (dh)
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
KeyAgreementDH::KeyAgreementDH( MRef<CertificateChain *> certChainPtr,
		MRef<CertificateSet *> certDbPtr ):
	KeyAgreement(),
	KeyAgreementDHBase(NULL),
	PeerCertificates( certChainPtr, certDbPtr )
{
}

KeyAgreementDH::KeyAgreementDH( MRef<SipSim*> s ):
	KeyAgreement(s),
	KeyAgreementDHBase(s),
	PeerCertificates( s->getCertificateChain(), s->getCAs() )
{

}

KeyAgreementDH::~KeyAgreementDH(){
}

int32_t KeyAgreementDH::type(){
	return KEY_AGREEMENT_TYPE_DH;
}


int KeyAgreementDHBase::setGroup( int groupValue ){
#ifdef SCSIM_SUPPORT
	if (sim && dynamic_cast<SipSimSmartCardGD*>(*sim)){
		SipSimSmartCardGD* gd = dynamic_cast<SipSimSmartCardGD*>(*sim);
		assert (groupValue==DH_GROUP_OAKLEY5);

		publicKeyPtr = new unsigned char[192];

		unsigned long length;
		gd->getDHPublicValue(length, publicKeyPtr);
	}
	else
#endif
	{
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
	}

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

#ifdef SCSIM_SUPPORT
	if (dynamic_cast<SipSimSmartCardGD*>(*sim)){
		SipSimSmartCardGD *gd = dynamic_cast<SipSimSmartCardGD*>(*sim);
		unsigned long len;
		unsigned char *dhval = new unsigned char[192];	//FIXME: fix API to work with unknown key lengths
		gd->getDHPublicValue(len, dhval);
		gd->genTgk( dhval, len );
		return true;
	}
	else
#endif
	{
		int res = dh->computeSecret( peerKeyPtr, peerKeyLengthValue, tgk(), tgkLength() );
		return res;
	}

}

int KeyAgreementDHBase::group(){
	if( !publicKeyPtr )
		return -1;

	if (sim){
		return DH_GROUP_OAKLEY5;
	}else
		return dh->group();
}

int KeyAgreementDHBase::peerKeyLength(){
	return peerKeyLengthValue;
}

unsigned char * KeyAgreementDHBase::peerKey(){
	return peerKeyPtr;
}

MRef<CertificateChain *> PeerCertificates::certificateChain(){
	return certChainPtr;
}

MRef<CertificateChain *> PeerCertificates::peerCertificateChain(){
	return peerCertChainPtr;
}

void PeerCertificates::setPeerCertificateChain( MRef<CertificateChain *> peerChain ){
	peerCertChainPtr = peerChain;
}

int PeerCertificates::controlPeerCertificate( const std::string &peerUri ){
	if( peerCertChainPtr.isNull() || certDbPtr.isNull() )
		return 0;

	int res = peerCertChainPtr->control( certDbPtr );
	if( !res ){
		return res;
	}

	if( peerUri == "" ){
		return 1;
	}

	MRef<Certificate *> peerCert = peerCertChainPtr->getFirst();
	vector<string> altNames;
	altNames = peerCert->getAltName( Certificate::SAN_URI );
	if( find( altNames.begin(), altNames.end(), peerUri ) != altNames.end() ){
		return 1;
	}

	string id = peerUri;
	size_t pos = peerUri.find(':');

	if( pos != string::npos ){
		id = peerUri.substr( pos + 1 );
	}

	altNames = peerCert->getAltName( Certificate::SAN_RFC822NAME );
	if( find( altNames.begin(), altNames.end(), id ) != altNames.end() ){
		return 1;
	}

	pos = id.find('@');
	if( pos != string::npos ){
		id = id.substr( pos + 1 );
	}

	altNames = peerCert->getAltName( Certificate::SAN_DNSNAME );
	if( find( altNames.begin(), altNames.end(), id ) != altNames.end() ){
		return 1;
	}

	cerr << "Peer URI " << peerUri << " not found in subject alt names." << endl;
	return 0;
}

MikeyMessage* KeyAgreementDH::createMessage(){
	return MikeyMessage::create( this );
}

