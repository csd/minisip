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
#include<libmikey/keyagreement_dh.h>
#include<libmikey/MikeyException.h>

KeyAgreementDH::KeyAgreementDH( MRef<certificate_chain *> certChainPtr,
		MRef<ca_db *> certDbPtr ):
	KeyAgreement(),
	peerKeyPtr( NULL ),
	peerKeyLengthValue( 0 ),
	certChainPtr( certChainPtr ),
	certDbPtr( certDbPtr ){
	//policy = list<Policy_type *>::list();
	typeValue = KEY_AGREEMENT_TYPE_DH;
	opensslDhPtr = DH_new();
	peerCertChainPtr = new certificate_chain();

}

KeyAgreementDH::~KeyAgreementDH(){
	DH_free( opensslDhPtr );
	if( peerKeyPtr == NULL )
		delete [] peerKeyPtr;

}

KeyAgreementDH::KeyAgreementDH( MRef<certificate_chain *> certChainPtr,
		MRef<ca_db *> certDbPtr, int groupValue ):
	peerKeyPtr( NULL ),
	peerKeyLengthValue( 0 ),
	certChainPtr( certChainPtr ),
	peerCertChainPtr( NULL ),
	certDbPtr( certDbPtr ){
	//policy = list<Policy_type *>::list();
	typeValue = KEY_AGREEMENT_TYPE_DH;
	opensslDhPtr = DH_new();
	if( opensslDhPtr == NULL )
	{
		throw new MikeyException( "Could not create openssl "
				          "DH parameters." );
	}

	if( setGroup( groupValue ) ){
		throw new MikeyException( "Could not set the  "
				          "DH group." );
	}
	peerCertChainPtr = new certificate_chain();
}

int KeyAgreementDH::setGroup( int groupValue ){
	this->groupValue = groupValue;
	switch( groupValue ) {
		case DH_GROUP_OAKLEY5:
			BN_hex2bn( &opensslDhPtr->p, OAKLEY5_P );
			BN_hex2bn( &opensslDhPtr->g, OAKLEY5_G );
			tgkLengthValue = OAKLEY5_L;
			break;
		case DH_GROUP_OAKLEY1:
			BN_hex2bn( &opensslDhPtr->p, OAKLEY1_P );
			BN_hex2bn( &opensslDhPtr->g, OAKLEY1_G );
			tgkLengthValue = OAKLEY1_L;
			break;
		case DH_GROUP_OAKLEY2:
			BN_hex2bn( &opensslDhPtr->p, OAKLEY2_P );
			BN_hex2bn( &opensslDhPtr->g, OAKLEY2_G );
			tgkLengthValue = OAKLEY2_L;
			break;
		default:
			return 1;
	}
	if( !DH_generate_key( opensslDhPtr ) )
	{
		return 1;
	}
			
	tgkPtr = new unsigned char[ tgkLengthValue ];

	return 0;
}
	
void KeyAgreementDH::setPeerKey( unsigned char * peerKeyPtr,
			      int peerKeyLengthValue ){
	this->peerKeyPtr = new unsigned char[ peerKeyLengthValue ];
	this->peerKeyLengthValue = peerKeyLengthValue;
	memcpy( this->peerKeyPtr, peerKeyPtr, peerKeyLengthValue );

}

int KeyAgreementDH::publicKeyLength(){
	return BN_num_bytes( opensslDhPtr->pub_key );
}

unsigned char * KeyAgreementDH::publicKey(){
	unsigned char * publicKey;
	publicKey = new unsigned char[ publicKeyLength() ];
	BN_bn2bin( opensslDhPtr->pub_key, publicKey );
	return publicKey;

}

int KeyAgreementDH::computeTgk(){
	BIGNUM * bn_peerKeyPtr =  BN_new();;
	
	assert( peerKeyPtr );

	BN_bin2bn( peerKeyPtr, peerKeyLengthValue, bn_peerKeyPtr );

	if( DH_compute_key( tgkPtr, bn_peerKeyPtr, opensslDhPtr ) < 0 )
	{
		BN_clear_free( bn_peerKeyPtr );
		throw new MikeyException( "Could not create the TGK." );
	}
	return 0;

}

int KeyAgreementDH::group(){
	return groupValue;

}

int KeyAgreementDH::peerKeyLength(){
	return peerKeyLengthValue;
}

unsigned char * KeyAgreementDH::peerKey(){
	return peerKeyPtr;
}

MRef<certificate_chain *> KeyAgreementDH::certificateChain(){
	return certChainPtr;
}

MRef<certificate_chain *> KeyAgreementDH::peerCertificateChain(){
	return peerCertChainPtr;
}

void KeyAgreementDH::addPeerCertificate( MRef<certificate *> peerCertPtr ){
	if( this->peerCertChainPtr.isNull() ){
		this->peerCertChainPtr = new certificate_chain();
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
