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
#include<libmikey/MikeyPayloadERR.h>
#include<libmikey/MikeyException.h>

#include<assert.h>

MikeyPayloadERR::MikeyPayloadERR( int errType ){
	this->payloadTypeValue = MIKEYPAYLOAD_ERR_PAYLOAD_TYPE;
	this->errTypeValue = errType;
}

MikeyPayloadERR::MikeyPayloadERR( byte_t * start, int lengthLimit ):
	MikeyPayload(start){
	
	this->payloadTypeValue = MIKEYPAYLOAD_ERR_PAYLOAD_TYPE;
	if( lengthLimit < 4 ){
		throw new MikeyExceptionMessageLengthException(
                        "Given data is too short to form a ERR Payload" );
	}

	setNextPayloadType( start[0] );
	this->errTypeValue = start[1];
	endPtr = startPtr + 4;

	assert( endPtr - startPtr == length() );

}

MikeyPayloadERR::~MikeyPayloadERR(){
}

int MikeyPayloadERR::length(){

	return 4;
}

void MikeyPayloadERR::writeData( byte_t * start, int expectedLength ){
	assert( expectedLength == length() );
	start[0] = nextPayloadType();
	start[1] = errTypeValue & 0xFF;
	memset( &start[2], 0, 2 );

}

string MikeyPayloadERR::debugDump(){
	return "MikeyPayloadERR: nextPayloadType=<" +
		itoa( nextPayloadTypeValue ) + "> err_type=<" +
		itoa( errTypeValue ) + ">";
}

int MikeyPayloadERR::errorType(){
	return errTypeValue;
}
