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
#include<libmikey/KeyValidity.h>
#include<libmikey/MikeyException.h>
#include<libmikey/MikeyDefs.h>
#include<assert.h>
#include<string.h>
#include<libmutil/stringutils.h>

using namespace std;

KeyValidity::KeyValidity(){ 
	typeValue = KEYVALIDITY_NULL;

}

KeyValidity::KeyValidity( const KeyValidity& ){
	typeValue = KEYVALIDITY_NULL;

}

KeyValidity::~KeyValidity(){};

int KeyValidity::type(){
	return typeValue;
}

int KeyValidity::length(){
	return 0;
}

void KeyValidity::writeData( byte_t * start, int expectedLength){

}

string KeyValidity::debugDump(){
	return "KeyValidityNull";
}

void KeyValidity::operator =( const KeyValidity& ){
	typeValue = KEYVALIDITY_NULL;
}

KeyValiditySPI::KeyValiditySPI():spiLength(0),spiPtr(NULL){
	typeValue = KEYVALIDITY_SPI;

}

KeyValiditySPI::KeyValiditySPI( const KeyValiditySPI& source ){
	typeValue = KEYVALIDITY_SPI;
	spiLength = source.spiLength;
	spiPtr = new byte_t[ spiLength ];
	memcpy( this->spiPtr, source.spiPtr, spiLength );
}

KeyValiditySPI::KeyValiditySPI( byte_t * rawData, int lengthLimit ){

	if( lengthLimit < 1 ){
		throw MikeyExceptionMessageLengthException(
			"Given data is too short to form a KeyValiditySPI" );
	}

	spiLength = rawData[0];
	
	if( lengthLimit < 1 + spiLength ){
		throw MikeyExceptionMessageLengthException(
			"Given data is too short to form a KeyValiditySPI" );
	}
	
	spiPtr = new byte_t[ spiLength ];
	memcpy( spiPtr, &rawData[1], spiLength );
}
	

KeyValiditySPI::KeyValiditySPI( int length, byte_t * spi ){
	this->spiPtr = new byte_t[ length ];
	memcpy( this->spiPtr, spi, length );
	this->spiLength = length;
}

int KeyValiditySPI::length(){
	return spiLength + 1; //data + length;
}

void KeyValiditySPI::writeData( byte_t * start, int expectedLength ){
	assert( expectedLength == length() );
	start[0] = spiLength;
	memcpy( &start[1], spiPtr, spiLength );
}

string KeyValiditySPI::debugDump(){
	return (const char *)("KeyValiditySPI: spi=<") + 
		binToHex( spiPtr, spiLength );
}

KeyValiditySPI::~KeyValiditySPI(){
	if( spiPtr )
		delete [] spiPtr;
	return;
}

void KeyValiditySPI::operator =( const KeyValiditySPI& source ){
	if( spiPtr ){
		delete [] spiPtr;
	}

	spiLength = source.spiLength;
	spiPtr = new byte_t[ spiLength ];
	memcpy( spiPtr, source.spiPtr, spiLength );
}

KeyValidityInterval::KeyValidityInterval():vfLength(0),vf(NULL),vtLength(0),
	vt(NULL){
	typeValue = KEYVALIDITY_INTERVAL;
}

KeyValidityInterval::KeyValidityInterval( const KeyValidityInterval& source ){
	typeValue = KEYVALIDITY_INTERVAL;
	vfLength = source.vfLength;
	vf = new byte_t[ vfLength ];
	memcpy( vf, source.vf, vfLength );
	vtLength = source.vtLength;
	vt = new byte_t[ vtLength ];
	memcpy( vt, source.vt, vtLength );
}


KeyValidityInterval::KeyValidityInterval(byte_t * raw_data, int length_limit){
	if( length_limit < 2 )
		throw MikeyExceptionMessageLengthException(
			"Given data is too short to form a KeyValidityInterval" );
	vfLength = raw_data[0];
	if( length_limit < 2 + vfLength )
		throw MikeyExceptionMessageLengthException(
			"Given data is too short to form a KeyValidityInterval" );
	vf = new byte_t[ vfLength ];
	memcpy( vf, &raw_data[1], vfLength );
	vtLength = raw_data[vfLength + 1];
	if( length_limit < 2 + vfLength + vtLength )
		throw MikeyExceptionMessageLengthException(
			"Given data is too short to form a KeyValidityInterval" );
	vt = new byte_t[ vtLength ];
	memcpy( vt, &raw_data[vfLength + 2], vfLength );
}

KeyValidityInterval::KeyValidityInterval(int vfLength, byte_t * vf,
				         int vtLength, byte_t * vt)
{
	this->vf = new byte_t[ vfLength ];
	memcpy( this->vf, vf, vfLength );
	this->vfLength = vfLength;
	this->vt = new byte_t[ vtLength ];
	memcpy( this->vt, vt, vtLength );
	this->vtLength = vtLength;
}

int KeyValidityInterval::length(){
	return vtLength + vfLength + 3;

}

void KeyValidityInterval::writeData(byte_t * start, int expectedLength){
	assert( expectedLength == length() );
	start[0] = vfLength;
	memcpy( &start[1], vf, vfLength );
	start[ 1+vfLength ] = vtLength;
	memcpy( &start[2+vfLength], vt, vtLength );
}

void KeyValidityInterval::operator =( const KeyValidityInterval& source ){
	typeValue = KEYVALIDITY_INTERVAL;
	if( vf ){
		delete [] vf;
	}
	if( vt ){
		delete [] vt;
	}

	vfLength = source.vfLength;
	vf = new byte_t[ vfLength ];
	memcpy( vf, source.vf, vfLength );
	vtLength = source.vtLength;
	vt = new byte_t[ vtLength ];
	memcpy( vt, source.vt, vtLength );
}

KeyValidityInterval::~KeyValidityInterval(){
	if( vf )
		delete [] vf;
	if( vt )
		delete [] vt;
}

string KeyValidityInterval::debugDump(){
	return "KeyValidityInterval: vf=<"+binToHex( vf, vfLength )+
		"> vt=<"+binToHex( vt, vtLength );
}
