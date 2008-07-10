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
#include<libmikey/MikeyPayloadDH.h>
#include<libmikey/MikeyException.h>
#include<libmutil/stringutils.h>

#include<assert.h>
#include<string.h>

using namespace std;

MikeyPayloadDH::MikeyPayloadDH( byte_t * start, int lengthLimit):
		MikeyPayload( start ){
	
	this->payloadTypeValue = MIKEYPAYLOAD_DH_PAYLOAD_TYPE;
	if( lengthLimit < 3 ){
                throw MikeyExceptionMessageLengthException(
                        "Given data is too short to form a DH Payload" );
                return;
        }

	setNextPayloadType( start[0] );
	dhGroup = start[1];
	switch( dhGroup ) {
		case MIKEYPAYLOAD_DH_GROUP5:
			dhKeyLengthValue = 192;
			break;
		case MIKEYPAYLOAD_DH_GROUP1:
			dhKeyLengthValue = 96;
			break;
		case MIKEYPAYLOAD_DH_GROUP2:
			dhKeyLengthValue = 128;
			break;
		default:
			throw MikeyExceptionMessageContent(
					"Unknown DH group" );
			break;
	}

	if( lengthLimit < 3 + dhKeyLengthValue ){
                throw MikeyExceptionMessageLengthException(
                        "Given data is too short to form a DH Payload" );
                return;
        }

	dhKeyPtr = new byte_t[ dhKeyLengthValue ];
	memcpy( dhKeyPtr, &start[2], dhKeyLengthValue );
	
	int kvType = start[2+dhKeyLengthValue] & 0x0F;
	switch( kvType ) {
		case KEYVALIDITY_NULL:
			kvPtr = new KeyValidityNull();
			break;
		case KEYVALIDITY_SPI:
			kvPtr = new KeyValiditySPI(
					&start[ 3 + dhKeyLengthValue ],
					lengthLimit - 3 - dhKeyLengthValue );
			break;
		case KEYVALIDITY_INTERVAL:
			kvPtr = new KeyValidityInterval(
					&start[ 3 + dhKeyLengthValue ],
					lengthLimit - 3 - dhKeyLengthValue );
			break;
		default:
			throw MikeyExceptionMessageContent(
					"Unknown DH key validity " 
					"type");
			break;
	}
	if( lengthLimit < 3 + dhKeyLengthValue + kvPtr->length() ){
                throw MikeyExceptionMessageLengthException(
                        "Given data is too short to form a DH Payload" );
                return;
        }

	
	endPtr = startPtr + 3 + dhKeyLengthValue + kvPtr->length();

	assert( endPtr - startPtr == length() );		

}

MikeyPayloadDH::MikeyPayloadDH( int dhGroup, byte_t *dhKeyPtr,
				MRef<KeyValidity *> kv ){

	this->payloadTypeValue = MIKEYPAYLOAD_DH_PAYLOAD_TYPE;
	this->dhGroup = dhGroup;
	
	switch( dhGroup ) {
		case MIKEYPAYLOAD_DH_GROUP5:
			dhKeyLengthValue = 192;
			break;
		case MIKEYPAYLOAD_DH_GROUP1:
			dhKeyLengthValue = 96;
			break;
		case MIKEYPAYLOAD_DH_GROUP2:
			dhKeyLengthValue = 128;
			break;
		default:
			throw MikeyExceptionMessageContent("Unknown DH group");
			break;
	}

	this->dhKeyPtr = new byte_t[ dhKeyLengthValue ];
	memcpy( this->dhKeyPtr, dhKeyPtr, dhKeyLengthValue );

	this->kvPtr = kv;

}


MikeyPayloadDH::~MikeyPayloadDH(){
	if( dhKeyPtr )
		delete [] dhKeyPtr;
}

void MikeyPayloadDH::writeData( byte_t * start, int expectedLength ){
	assert( expectedLength == length() );
	memset( start, 0, expectedLength );
	start[0] = nextPayloadType();
	start[1] = dhGroup;
	memcpy( &start[2], dhKeyPtr, dhKeyLengthValue );
	start[ 2 + dhKeyLengthValue ] = 0x0F & kvPtr->type();
	kvPtr->writeData( &start[ 3 + dhKeyLengthValue ], kvPtr->length() );
}

int MikeyPayloadDH::length(){
	return 3 + kvPtr->length() + dhKeyLengthValue;

}

string MikeyPayloadDH::debugDump(){
	
	return "MikeyPayloadDH: "
		"nextPayloadType=<" + itoa( nextPayloadTypeValue ) + 
		"> dhGroup=<" + itoa( dhGroup ) + 
		"> dhKeyPtr=<" + binToHex( dhKeyPtr, dhKeyLengthValue ) + 
		"> kvType=<" + itoa( kvPtr->type() ) +
		">" + kvPtr->debugDump();
}

int MikeyPayloadDH::group(){
	return dhGroup;
}

byte_t * MikeyPayloadDH::dhKey(){
	return dhKeyPtr;
}

int MikeyPayloadDH::dhKeyLength(){
	return dhKeyLengthValue;
}

MRef<KeyValidity *> MikeyPayloadDH::kv(){
	return kvPtr;
}

