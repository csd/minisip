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
#include<libmikey/MikeyPayloadKeyData.h>
#include<libmikey/MikeyException.h>
#include<libmutil/stringutils.h>
#include<assert.h>
#include<string.h>

using namespace std;

MikeyPayloadKeyData::MikeyPayloadKeyData( int type, byte_t * keyDataPtr, 
		int keyDataLengthValue, MRef<KeyValidity *> kvPtr ){

	this->payloadTypeValue = MIKEYPAYLOAD_KEYDATA_PAYLOAD_TYPE;
	if((type == KEYDATA_TYPE_TGK_SALT) || (type == KEYDATA_TYPE_TEK_SALT))
		throw MikeyException( 
			"This type of KeyData Payload requires a salt" );
	this->typeValue = type;
	this->keyDataPtr = new byte_t[keyDataLengthValue];
	this->keyDataLengthValue = keyDataLengthValue;
	memcpy( this->keyDataPtr, keyDataPtr, keyDataLengthValue );
	this->kvPtr = kvPtr;
	this->saltDataLengthValue = 0;
	this->saltDataPtr = NULL;
}

MikeyPayloadKeyData::MikeyPayloadKeyData(int type, byte_t * keyDataPtr, 
		int keyDataLengthValue, byte_t * saltDataPtr, 
		int saltDataLengthValue, MRef<KeyValidity *> kvPtr ){

	this->payloadTypeValue = MIKEYPAYLOAD_KEYDATA_PAYLOAD_TYPE;
	this->typeValue = type;
	this->keyDataPtr = new byte_t[keyDataLengthValue];
	this->keyDataLengthValue = keyDataLengthValue;
	memcpy( this->keyDataPtr, keyDataPtr, keyDataLengthValue );
	this->saltDataPtr = new byte_t[saltDataLengthValue];
	this->saltDataLengthValue = saltDataLengthValue;
	memcpy( this->saltDataPtr, saltDataPtr, saltDataLengthValue );
	this->kvPtr = kvPtr;
}

MikeyPayloadKeyData::MikeyPayloadKeyData(byte_t *start, int lengthLimit):MikeyPayload(start){
	int lengthWoKvPtr;
	this->payloadTypeValue = MIKEYPAYLOAD_KEYDATA_PAYLOAD_TYPE;
	if( lengthLimit < 4 )
		throw MikeyExceptionMessageLengthException(
                        "Given data is too short to form a KeyData Payload" );
		
	setNextPayloadType( start[0] );
	typeValue = ( start[1] >> 4 ) & 0x0F;
	int kvPtrType = ( start[1]      ) & 0x0F;
	keyDataLengthValue = ( (int)start[2] ) << 8 |
		             ( (int)start[3] );
	if( (typeValue == KEYDATA_TYPE_TGK_SALT) || 
			(typeValue == KEYDATA_TYPE_TEK_SALT)){
		
		if( lengthLimit < 6 + keyDataLengthValue )
			throw MikeyExceptionMessageLengthException(
			  "Given data is too short to form a KeyData Payload" );
		keyDataPtr = new byte_t[ keyDataLengthValue ];
		memcpy( keyDataPtr, &start[4], keyDataLengthValue );
		saltDataLengthValue = 
			( (int)start[2+keyDataLengthValue] ) << 8 |
		        ( (int)start[3+keyDataLengthValue] );
		if( lengthLimit < 6 + keyDataLengthValue + saltDataLengthValue )
			throw MikeyExceptionMessageLengthException(
			  "Given data is too short to form a KeyData Payload" );
		saltDataPtr = new byte_t[ saltDataLengthValue ];
		memcpy( saltDataPtr, &start[4+keyDataLengthValue],
				saltDataLengthValue );
		lengthWoKvPtr = keyDataLengthValue + saltDataLengthValue + 6;
	}
	else{
		if( lengthLimit < 4 + keyDataLengthValue )
			throw MikeyExceptionMessageLengthException(
			  "Given data is too short to form a KeyData Payload" );
		keyDataPtr = new byte_t[ keyDataLengthValue ];
		memcpy( keyDataPtr, &start[4], keyDataLengthValue );
		saltDataLengthValue = 0;
		saltDataPtr = NULL;
		lengthWoKvPtr = keyDataLengthValue + 4;
	}

	switch( kvPtrType ){
		case KEYVALIDITY_NULL:
			kvPtr = new KeyValidityNull();
                        break;
                case KEYVALIDITY_SPI:
                        kvPtr = new KeyValiditySPI(
                          	&start[lengthWoKvPtr], 
			  	lengthLimit - lengthWoKvPtr );
                        break;
                case KEYVALIDITY_INTERVAL:
			
                        kvPtr = new KeyValidityInterval(
                          	&start[lengthWoKvPtr],
			  	lengthLimit - lengthWoKvPtr ); 
                        break;
                default:
                        throw MikeyExceptionMessageContent(
					"Unknown key validity" 
                                        "type");
                        break;
        }

	endPtr = startPtr + length();
}

MikeyPayloadKeyData::~MikeyPayloadKeyData(){
	if( keyDataPtr != NULL )
		delete [] keyDataPtr;
	if( saltDataPtr != NULL )
		delete [] saltDataPtr;
}

int MikeyPayloadKeyData::length(){
	return keyDataLengthValue + saltDataLengthValue + kvPtr->length() +
	  (((   typeValue == KEYDATA_TYPE_TGK_SALT) 
	    || (typeValue == KEYDATA_TYPE_TEK_SALT))?
	  6:4 );
}

void MikeyPayloadKeyData::writeData( byte_t * start, int expectedLength ){
	assert( expectedLength == length() );
	start[0] = nextPayloadType();
	start[1] = ( ( typeValue & 0x0F  ) << 4 ) | ( kvPtr->type() & 0x0F );
	start[2] = ( keyDataLengthValue >> 8 ) & 0xFF;
	start[3] = ( keyDataLengthValue      ) & 0xFF;
	memcpy( &start[4], keyDataPtr, keyDataLengthValue );
	if( ( typeValue == KEYDATA_TYPE_TGK_SALT )
	 || ( typeValue == KEYDATA_TYPE_TEK_SALT ) ){
		start[4 + keyDataLengthValue] = 
			( saltDataLengthValue >> 8 ) & 0xFF;
		start[5 + keyDataLengthValue] = 
			( saltDataLengthValue      ) & 0xFF;
		memcpy( &start[ 6 + keyDataLengthValue ], saltDataPtr, 
				saltDataLengthValue);
		kvPtr->writeData( &start[ 6 + keyDataLengthValue + 
				saltDataLengthValue], kvPtr->length() );
	}
	else{
		kvPtr->writeData( &start[ 4 + keyDataLengthValue ], 
				kvPtr->length() );
	}
}

string MikeyPayloadKeyData::debugDump(){
	return "MikeyPayloadKeyData:" 
		" nextPayloadType=<" + itoa( nextPayloadType() ) +
		"> type=<" + itoa( typeValue ) + 
		"> keyDataPtr=<" + binToHex( keyDataPtr, keyDataLengthValue ) +
		"> saltDataPtr=<" + 
			binToHex( saltDataPtr, saltDataLengthValue ) +
		"> kvPtr_type=<" + itoa( kvPtr->type() ) +
		"> kvPtr_data=<" + kvPtr->debugDump() + ">";
}

int MikeyPayloadKeyData::type(){
	return typeValue;
}

MRef<KeyValidity *> MikeyPayloadKeyData::kv(){
	return kvPtr;
}

byte_t * MikeyPayloadKeyData::keyData(){
	return keyDataPtr;
}

int MikeyPayloadKeyData::keyDataLength(){
	return keyDataLengthValue;
}

byte_t * MikeyPayloadKeyData::saltData(){
	return saltDataPtr;
}

int MikeyPayloadKeyData::saltDataLength(){
	return saltDataLengthValue;
}
