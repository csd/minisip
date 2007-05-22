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
#include<libmcrypto/OakleyDH.h>
#include<libmutil/Exception.h>
#include<openssl/dh.h>
#include"oakley_groups.h"
#include<iostream>

#define opensslDhPtr ((DH*)priv)

using namespace std;

OakleyDH::OakleyDH(): groupValue(-1){
	// Store opensslDhPtr in priv
	priv = DH_new();
	if( opensslDhPtr == NULL )
	{
		throw Exception( "Could not create openssl "
				          "DH parameters." );
	}
}

OakleyDH::OakleyDH( int groupValue_ ){
	// Store opensslDhPtr in priv
	priv = DH_new();
	if( opensslDhPtr == NULL )
	{
		throw Exception( "Could not create openssl "
				          "DH parameters." );
	}

	if( !setGroup( groupValue_ ) ){
		throw Exception( "Could not set the openssl "
				          "DH group." );
	}
}

OakleyDH::~OakleyDH(){
	DH_free( opensslDhPtr );
}

bool OakleyDH::setGroup( int groupValue_ ){
	this->groupValue = groupValue_;
	switch( groupValue ) {
		case DH_GROUP_OAKLEY5:
			BN_hex2bn( &opensslDhPtr->p, OAKLEY5_P );
			BN_hex2bn( &opensslDhPtr->g, OAKLEY5_G );
			break;
		case DH_GROUP_OAKLEY1:
			BN_hex2bn( &opensslDhPtr->p, OAKLEY1_P );
			BN_hex2bn( &opensslDhPtr->g, OAKLEY1_G );
			break;
		case DH_GROUP_OAKLEY2:
			BN_hex2bn( &opensslDhPtr->p, OAKLEY2_P );
			BN_hex2bn( &opensslDhPtr->g, OAKLEY2_G );
			break;
		default:
			return false;
	}
	if( !DH_generate_key( opensslDhPtr ) )
	{
		return false;
	}

	return true;
}
	
uint32_t OakleyDH::publicKeyLength() const{
	return BN_num_bytes( opensslDhPtr->pub_key );
}

uint32_t OakleyDH::getPublicKey(uint8_t *buf, uint32_t buflen) const{
	if( buflen < publicKeyLength() )
		return 0;

	return BN_bn2bin( opensslDhPtr->pub_key, buf );
}

int OakleyDH::computeSecret(const uint8_t *peerKeyPtr, uint32_t peerKeyLengthValue,
			uint8_t *secret, uint32_t secretSize) const{
	if( !peerKeyPtr || !secret || secretSize < secretLength() ){
		return -1;
	}

	BIGNUM * bn_peerKeyPtr =  BN_new();
	BN_bin2bn( peerKeyPtr, peerKeyLengthValue, bn_peerKeyPtr );

	if( DH_compute_key( secret, bn_peerKeyPtr, opensslDhPtr ) < 0 )
	{
		BN_clear_free( bn_peerKeyPtr );
		return -1;
	}

	BN_clear_free( bn_peerKeyPtr );
	return 0;
}

int OakleyDH::group() const{
	return groupValue;

}

uint32_t OakleyDH::secretLength() const{
	return DH_size( opensslDhPtr );
}
