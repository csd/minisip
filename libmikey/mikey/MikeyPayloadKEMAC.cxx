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
#include<libmikey/MikeyPayloadKEMAC.h>
#include<libmikey/MikeyException.h>
#include<assert.h>
#include<string.h>
#include<libmcrypto/aes.h>
#include<libmutil/stringutils.h>

using namespace std;

MikeyPayloadKEMAC::MikeyPayloadKEMAC( int encrAlgValue, 
		int encrDataLengthValue, byte_t * encrDataPtr, 
		int macAlgValue, byte_t * macDataPtr ){
	this->payloadTypeValue = MIKEYPAYLOAD_KEMAC_PAYLOAD_TYPE;
	this->encrAlgValue = encrAlgValue;
	this->encrDataLengthValue = encrDataLengthValue;
	this->encrDataPtr = new byte_t[ encrDataLengthValue ];
	memcpy( this->encrDataPtr, encrDataPtr, encrDataLengthValue );
	this->macAlgValue = macAlgValue;
	switch( macAlgValue ){
		case MIKEY_PAYLOAD_KEMAC_MAC_HMAC_SHA1_160:
			this->macDataPtr = new byte_t[ 20 ];
			memcpy( this->macDataPtr, macDataPtr, 20 );
			break;
		case MIKEY_PAYLOAD_KEMAC_MAC_NULL:
			this->macDataPtr = NULL;
			break;
		default:
			throw MikeyExceptionMessageContent(
					"Unknown MAC algorithm in KEYMAC Payload (1)" );
	}
}

MikeyPayloadKEMAC::MikeyPayloadKEMAC(byte_t *start, int lengthLimit):MikeyPayload(start){
	if(  lengthLimit < 5 ){
		throw MikeyExceptionMessageLengthException(
                        "Given data is too short to form a KEMAC Payload" );
                return;
        }
	this->payloadTypeValue = MIKEYPAYLOAD_KEMAC_PAYLOAD_TYPE;
	setNextPayloadType(start[0]);
	encrAlgValue = start[1];
	encrDataLengthValue = ( (int)start[2] << 8) |
		             (int)start[3];
	if( lengthLimit < 5 + encrDataLengthValue ){	
		throw MikeyExceptionMessageLengthException(
                        "Given data is too short to form a KEMAC Payload" );
                return;
        }
	macAlgValue = (int)start[4 + encrDataLengthValue];
	switch( macAlgValue ){
		case MIKEY_PAYLOAD_KEMAC_MAC_HMAC_SHA1_160:
			if( lengthLimit < 25 + encrDataLengthValue ){	
				throw MikeyExceptionMessageLengthException(
                        	"Given data is too short to form" 
				"a KEMAC Payload" );
              			return;
        		}
			this->macDataPtr = new byte_t[ 20 ];
			memcpy( this->macDataPtr, &
				start[5 + encrDataLengthValue],
				20 );
			endPtr = startPtr + 25 + encrDataLengthValue;
			break;
		case MIKEY_PAYLOAD_KEMAC_MAC_NULL:
			this->macDataPtr = NULL;
			endPtr = startPtr + 5 + encrDataLengthValue;
			break;
		default:
			throw MikeyExceptionMessageContent(
				"Unknown MAC algorithm in KEYMAC Payload (2)" );
			return;
	}
	
	encrDataPtr = new byte_t[ encrDataLengthValue ];
	memcpy( encrDataPtr, &start[4], encrDataLengthValue );

	assert( endPtr - startPtr == length() );

}

MikeyPayloadKEMAC::~MikeyPayloadKEMAC(){
	if( encrDataPtr != NULL )
		delete [] encrDataPtr;
	if( macDataPtr != NULL )
		delete [] macDataPtr;
}

int MikeyPayloadKEMAC::length(){
	return  5 + encrDataLengthValue + 
		( (macAlgValue == MIKEY_PAYLOAD_KEMAC_MAC_HMAC_SHA1_160)?20:0 );
}

void MikeyPayloadKEMAC::writeData(byte_t *start, int expectedLength){
	assert( expectedLength == length() );
	start[0] = (byte_t)nextPayloadType();
	start[1] = (byte_t)( encrAlgValue & 0xFF );
	start[2] = (byte_t)( (encrDataLengthValue >> 8) & 0xFF );
	start[3] = (byte_t)( (encrDataLengthValue) & 0xFF );
	memcpy( &start[4], encrDataPtr, encrDataLengthValue );
	start[4 + encrDataLengthValue] = (byte_t)( macAlgValue&0xFF );
	if( macAlgValue == MIKEY_PAYLOAD_KEMAC_MAC_HMAC_SHA1_160 )
		memcpy( &start[5 + encrDataLengthValue], macDataPtr, 20 );
}

int MikeyPayloadKEMAC::encrAlg(){
	return encrAlgValue;
}

int MikeyPayloadKEMAC::encrDataLength(){
	return encrDataLengthValue;
}

byte_t * MikeyPayloadKEMAC::encrData(){
	return encrDataPtr;
}

int MikeyPayloadKEMAC::macAlg(){
	return macAlgValue;
}

byte_t * MikeyPayloadKEMAC::macData(){
	return macDataPtr;
}

MikeyPayloads* MikeyPayloadKEMAC::decodePayloads( int firstPayloadType,
		byte_t * encrKey, int encrKeyLength,
		byte_t * iv){
	
	byte_t * decrData = new byte_t[ encrDataLengthValue ];
	AES * aes;

	switch( encrAlgValue ){
		case MIKEY_PAYLOAD_KEMAC_ENCR_AES_CM_128:
			aes = new AES( encrKey, encrKeyLength );
			aes->ctr_encrypt( encrDataPtr, encrDataLengthValue, 
					decrData, iv );
			delete aes;
			break;
		case MIKEY_PAYLOAD_KEMAC_ENCR_NULL:
			memcpy( decrData, encrDataPtr, encrDataLengthValue );
			break;
		case MIKEY_PAYLOAD_KEMAC_ENCR_AES_KW_128:
			//TODO
		default:
			delete [] decrData;
			throw MikeyException( 
					"Unknown encryption algorithm" );
			break;
	}

	MikeyPayloads *output =
		new MikeyPayloads( firstPayloadType, decrData,
				   encrDataLengthValue );
	// decrData is owned and deleted by MikeyPayloads
	return output;
}

string MikeyPayloadKEMAC::debugDump(){
	return "MikeyPayloadKEMAC: encrAlgValue=<" + itoa( encrAlgValue ) +
		"> encrDataLengthValue=<" + itoa( encrDataLengthValue ) +
		"> encrDataPtr=<" + 
		binToHex( encrDataPtr, encrDataLengthValue ) +
		"> macAlgValue=<" + itoa( macAlgValue ) +
		"> macDataPtr=<" + binToHex( macDataPtr, 
		((macAlgValue==MIKEY_PAYLOAD_KEMAC_MAC_HMAC_SHA1_160)?20:0)
		) + ">";
}

void MikeyPayloadKEMAC::setMac( byte_t * data ){
	if( macDataPtr != NULL )
		delete [] macDataPtr;

	switch( macAlgValue ){
		case MIKEY_PAYLOAD_KEMAC_MAC_HMAC_SHA1_160:
			macDataPtr = new byte_t[ 20 ];
			memcpy( macDataPtr, data, 20 );
			break;
		case MIKEY_PAYLOAD_KEMAC_MAC_NULL:
			macDataPtr = NULL;
			break;
		default:
			throw MikeyException( 
				"Unknown MAC algorithm (PayloadKEMAC::setMac)" );
	}
}
