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
#include<libmikey/MikeyPayloadID.h>
#include<libmikey/MikeyException.h>
#include<libmutil/stringutils.h>
#include<assert.h>
#include<string.h>

using namespace std;

MikeyPayloadID::MikeyPayloadID( int type, int length, byte_t * data ){
	this->payloadTypeValue = MIKEYPAYLOAD_ID_PAYLOAD_TYPE;
	this->idTypeValue = type;
	this->idLengthValue = length;
	this->idDataPtr = new byte_t[ length ];
	memcpy( this->idDataPtr, data, length );

}

MikeyPayloadID::MikeyPayloadID( byte_t * start, int lengthLimit ):
MikeyPayload(start){
	
	if( lengthLimit < 4 ){
		throw MikeyExceptionMessageLengthException(
                        "Given data is too short to form a ID Payload" );
                return;
        }
	this->payloadTypeValue = MIKEYPAYLOAD_ID_PAYLOAD_TYPE;
	setNextPayloadType( start[0] );
	idTypeValue = start[1];
	idLengthValue = (int)( start[ 2 ] ) << 8 | start[ 3 ];
	if( lengthLimit < 4 + idLengthValue ){
		throw MikeyExceptionMessageLengthException(
                        "Given data is too short to form a ID Payload" );
                return;
        }

	idDataPtr = new byte_t[ idLengthValue ];
	memcpy( idDataPtr, &start[4], idLengthValue );
	endPtr = startPtr + 4 + idLengthValue;

	assert( endPtr - startPtr == length() );

}

MikeyPayloadID::~MikeyPayloadID(){
	if( idDataPtr )
		delete [] idDataPtr;
	idDataPtr = NULL;
}

void MikeyPayloadID::writeData( byte_t * start, int expectedLength ){

	assert( expectedLength == length() );
	memset( start, 0, expectedLength );
	start[0] = nextPayloadType();
	start[1] = idTypeValue;
	start[2] = (byte_t) ((idLengthValue & 0xFF00) >> 8);
	start[3] = (byte_t) (idLengthValue & 0xFF);
	memcpy( &start[4], idDataPtr, idLengthValue );

}

int MikeyPayloadID::length(){

	return 4 + idLengthValue;
}

string MikeyPayloadID::debugDump(){

	return "MikeyPayloadID: nextPayloadType=<" + itoa( nextPayloadType() ) +
		"> type=<" + itoa( idTypeValue ) +
		"> length=<" + itoa( idLengthValue ) +
		"> data=<" + binToHex( idDataPtr, idLengthValue ) + ">";
}

int MikeyPayloadID::idType() const{
	return idTypeValue;
}

int MikeyPayloadID::idLength() const{
	return idLengthValue;
}

const byte_t * MikeyPayloadID::idData() const{
	return idDataPtr;
}
