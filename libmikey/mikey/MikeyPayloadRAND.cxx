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
#include<libmikey/MikeyPayloadRAND.h>
#include<libmikey/MikeyException.h>
#include<libmutil/stringutils.h>
#include<assert.h>
#include<stdlib.h>
#include<time.h>
#include<sys/types.h>
#include<string.h>
#include<libmcrypto/rand.h>

#ifndef _MSC_VER
#include<unistd.h>
#endif

using namespace std;

MikeyPayloadRAND::MikeyPayloadRAND( int randlen, byte_t * randDataPtr ){
	this->payloadTypeValue = MIKEYPAYLOAD_RAND_PAYLOAD_TYPE;
	this->randLengthValue = randlen;
	this->randDataPtr = new byte_t[ randlen ];
	memcpy( this->randDataPtr, randDataPtr, randlen );
}

MikeyPayloadRAND::MikeyPayloadRAND( byte_t *start, int lengthLimit ):
MikeyPayload( start ){

	this->payloadTypeValue = MIKEYPAYLOAD_RAND_PAYLOAD_TYPE;
	if( lengthLimit < 2 ){
                throw MikeyExceptionMessageLengthException(
                        "Given data is too short to form a RAND Payload" );
                return;
        }
	
	setNextPayloadType( start[0] );
	randLengthValue = start[1];
	if( lengthLimit < 2 + randLengthValue ){
                throw MikeyExceptionMessageLengthException(
                        "Given data is too short to form a RAND Payload" );
                return;
        }
	randDataPtr = new byte_t[ randLengthValue ];
	memcpy( randDataPtr, &start[2],randLengthValue );
	endPtr = startPtr + 2 + randLengthValue;
	
	assert( endPtr - startPtr == length() );

}

MikeyPayloadRAND::MikeyPayloadRAND():MikeyPayload(){
	this->payloadTypeValue = MIKEYPAYLOAD_RAND_PAYLOAD_TYPE;
	randLengthValue = 16;
	
	randDataPtr = new byte_t[ randLengthValue ];
	Rand::randomize( randDataPtr, randLengthValue );

}

MikeyPayloadRAND::MikeyPayloadRAND(MRef<SipSim *> sim):MikeyPayload(){
	this->payloadTypeValue = MIKEYPAYLOAD_RAND_PAYLOAD_TYPE;
	randLengthValue = 16;
	randDataPtr = new byte_t[ randLengthValue ];
	Rand::randomize(randDataPtr, randLengthValue, sim);
}

MikeyPayloadRAND::~MikeyPayloadRAND(){
	if( randDataPtr ){
		delete [] randDataPtr;
	}
	randDataPtr=NULL;
}

int MikeyPayloadRAND::length(){
	
	return 2+randLengthValue;
}

void MikeyPayloadRAND::writeData(byte_t *start, int expectedLength){
	assert( expectedLength == length() );
	start[0] = nextPayloadType();
	start[1] = randLengthValue;
	memcpy( &start[2], randDataPtr, randLengthValue );

}

string MikeyPayloadRAND::debugDump(){

	return "MikeyPayloadRAND: nextPayloadType=<" + 
		itoa( nextPayloadType() ) +
		"> randLengthValue=<" + itoa( randLengthValue ) + 
		"> randDataPtr=<" + binToHex( randDataPtr, randLengthValue ) + 
		">";
}

int MikeyPayloadRAND::randLength(){
	return randLengthValue;
}

byte_t * MikeyPayloadRAND::randData(){
	return randDataPtr;
}
