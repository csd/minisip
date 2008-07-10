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
#include<libmikey/MikeyDefs.h>
#include<libmikey/MikeyPayloadCERT.h>
#include<libmikey/MikeyException.h>
#include<libmutil/stringutils.h>
#include<assert.h>
#include<string.h>

using namespace std;

MikeyPayloadCERT::MikeyPayloadCERT( int type, int length, byte_t * data ){
	this->payloadTypeValue = MIKEYPAYLOAD_CERT_PAYLOAD_TYPE;
	this->type = type;
	this->certLengthValue = length;
	this->certDataPtr = new byte_t[ length ];
	memcpy( this->certDataPtr, data, length );

}

MikeyPayloadCERT::MikeyPayloadCERT( int type, MRef<Certificate *> cert ){
	this->payloadTypeValue = MIKEYPAYLOAD_CERT_PAYLOAD_TYPE;
	this->type = type;
	this->certLengthValue = cert->getDerLength();
	this->certDataPtr = new byte_t[ certLengthValue ];
	unsigned int size = certLengthValue;
	cert->getDer( this->certDataPtr, &size );
}

MikeyPayloadCERT::MikeyPayloadCERT( byte_t *start, int lengthLimit ):
		MikeyPayload(start){
	
	this->payloadTypeValue = MIKEYPAYLOAD_CERT_PAYLOAD_TYPE;
	if( lengthLimit < 4 ){
		throw MikeyExceptionMessageLengthException(
			"Given data is too short to form a CERT Payload" );
		return;
	}
                
	setNextPayloadType( start[0] );
	type = start[1];
	certLengthValue = (int)(start[2]) <<8 | start[3];
	if( lengthLimit < 4 + certLengthValue ){
		throw MikeyExceptionMessageLengthException(
			"Given data is too short to form a CERT Payload" );
		return;
	}
                
	certDataPtr = new byte_t[ certLengthValue ];
	memcpy( certDataPtr, &start[4], certLengthValue );
	endPtr = startPtr + 4 + certLengthValue;

	assert( endPtr - startPtr == length() );

}

MikeyPayloadCERT::~MikeyPayloadCERT(){
	if( certDataPtr )
		delete [] certDataPtr;
	certDataPtr = NULL;
}

void MikeyPayloadCERT::writeData( byte_t * start, int expectedLength ){

	assert( expectedLength == length() );
	memset( start, 0, expectedLength );
	start[0] = nextPayloadType();
	start[1] = type;
	start[2] = (byte_t) ((certLengthValue & 0xFF00) >> 8);
	start[3] = (byte_t) (certLengthValue & 0xFF);
	memcpy( &start[4], certDataPtr, certLengthValue );

}

int MikeyPayloadCERT::length(){

	return 4 + certLengthValue;
}

string MikeyPayloadCERT::debugDump(){

	return "MikeyPayloadCERT: nextPayloadType=<"
		+itoa( nextPayloadType() ) +
		"> type=<"+itoa(type) +
		"> length=<" + itoa( certLengthValue )+
		"> data=<" + binToHex( certDataPtr, certLengthValue )+ ">";
}

byte_t * MikeyPayloadCERT::certData(){
	return certDataPtr;
}

int MikeyPayloadCERT::certLength(){
	return certLengthValue;
}
