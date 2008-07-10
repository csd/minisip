/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien, Joachim Orrblad
  
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
 *	    Joachim Orrblad <joachim@orrblad.com>
*/


#include<config.h>
#include<libmikey/MikeyPayloadHDR.h>
#include<libmikey/MikeyCsIdMap.h>
#include<libmikey/MikeyException.h>
#include<libmutil/stringutils.h>
#include<assert.h>
#include<string.h>

using namespace std;

MikeyPayloadHDR::MikeyPayloadHDR( int dataType, int v, int prfFunc, uint32_t csbId, 
		int nCs, int mapType, MRef<MikeyCsIdMap *> map ) {

	this->payloadTypeValue = MIKEYPAYLOAD_HDR_PAYLOAD_TYPE;
	this->version = 1;
	this->dataTypeValue = dataType;
	this->vValue = v;
	this->nCsValue = nCs;
	this->prfFunc = prfFunc;
	this->csbIdValue = csbId;
	if( mapType == HDR_CS_ID_MAP_TYPE_SRTP_ID || mapType == HDR_CS_ID_MAP_TYPE_IPSEC4_ID){
		this->csIdMapTypeValue = mapType;
		this->csIdMapPtr = map;
	}
	else{
		throw MikeyExceptionMessageContent(
				"Unknown CS ID map type" );
	}

	if( !csIdMapPtr ){
		throw MikeyExceptionMessageContent(
				"Missing CS ID map" );
	}		
}

MikeyPayloadHDR::MikeyPayloadHDR( byte_t * start, int lengthLimit ):
	MikeyPayload( start ){

	this->payloadTypeValue = MIKEYPAYLOAD_HDR_PAYLOAD_TYPE;
	if( lengthLimit < 10 ){
		throw MikeyExceptionMessageLengthException(
			"Given data is too short to form a HDR Payload" );
		return;
	}

	setNextPayloadType( start[2] );
	this->version = start[0];
	this->dataTypeValue = start[1];
	this->vValue = ( start[3] >> 7 ) & 0x1;
	this->prfFunc = start[3] & 0x7F;
	this->csbIdValue = (int)start[4] << 24 |
		           (int)start[5] << 16 |
		           (int)start[6] <<  8 |
		           (int)start[7];
	this->nCsValue = start[8];
	this->csIdMapTypeValue = start[9];
	if( csIdMapTypeValue == HDR_CS_ID_MAP_TYPE_SRTP_ID || 
		csIdMapTypeValue == HDR_CS_ID_MAP_TYPE_IPSEC4_ID){
		if( lengthLimit < 10 + nCsValue * 9 ){
			throw MikeyExceptionMessageLengthException(
			"Given data is too short to form any HDR Payload" );
			return;
		}
		if( csIdMapTypeValue == HDR_CS_ID_MAP_TYPE_SRTP_ID ){
			this->csIdMapPtr = 
		   		new MikeyCsIdMapSrtp( &start[10], 9 * nCsValue );
			this->endPtr = startPtr + 10 + 9 * this->nCsValue;
		}
		if( csIdMapTypeValue == HDR_CS_ID_MAP_TYPE_IPSEC4_ID ){
			this->csIdMapPtr = 
		   		new MikeyCsIdMapIPSEC4( &start[10], 13 * nCsValue );
			this->endPtr = startPtr + 10 + 13 * this->nCsValue;
		}
		
	}
	else{
		throw MikeyExceptionMessageContent( 
				"Unknown type of CS_ID_map" );
		this->csIdMapPtr = NULL;
	}
}

MikeyPayloadHDR::~MikeyPayloadHDR(){
}
	
int MikeyPayloadHDR::length(){
	return 10 + csIdMapPtr->length();
}

void MikeyPayloadHDR::writeData( byte_t * start, int expectedLength ){
	assert( expectedLength == length() );
	memset( start, 0, expectedLength );
	start[0] = (byte_t) version;
	start[1] = (byte_t) dataTypeValue;
	start[2] = nextPayloadType();
	start[3] = ( (byte_t)vValue & 0x1) << 7 
		 | ( (byte_t)prfFunc & 0x7F );
	start[4] = (byte_t) ((csbIdValue & 0xFF000000) >> 24);
	start[5] = (byte_t) ((csbIdValue & 0xFF0000) >> 16);
	start[6] = (byte_t) ((csbIdValue & 0xFF00) >> 8);
	start[7] = (byte_t) (csbIdValue & 0xFF);
	start[8] = (byte_t) nCsValue;
	start[9] = (byte_t) csIdMapTypeValue;
	csIdMapPtr->writeData( &start[10], csIdMapPtr->length() );

}

string MikeyPayloadHDR::debugDump(){
	string ret= "MikeyPayloadHDR: version=<"+itoa(version)+"> datatype=";
	switch( dataTypeValue ){
		case  HDR_DATA_TYPE_PSK_INIT: 
			ret=ret+"<Pre-shared>";
			break;
		case HDR_DATA_TYPE_PSK_RESP:
			ret=ret+"<PS ver msg>";
			break;
		case HDR_DATA_TYPE_PK_INIT:
			ret=ret+"<Public key>";
			break;
		case HDR_DATA_TYPE_PK_RESP:
			ret=ret+"<PK ver msg>";
			break;
		case  HDR_DATA_TYPE_DH_INIT:
			ret=ret+"<D-H init>";
			break;
		case HDR_DATA_TYPE_DH_RESP:
			ret=ret+"<D-H resp>";
			break;
		case HDR_DATA_TYPE_DHHMAC_INIT:
			ret=ret+"<DHMAC init>";
			break;
		case HDR_DATA_TYPE_DHHMAC_RESP:
			ret=ret+"<DHMAC resp>";
			break;
		case HDR_DATA_TYPE_RSA_R_INIT:
			ret=ret+"<RSA-R I_MSG>";
			break;
		case HDR_DATA_TYPE_RSA_R_RESP:
			ret=ret+"<RSA-R R_MSG>";
			break;
		case HDR_DATA_TYPE_ERROR:
			ret=ret+"<Error>";
			break;
	}

	ret += " next_payload=" + itoa( nextPayloadType() );
	ret += " V=" + itoa( vValue );
	ret += " PRF_func=";
	switch( prfFunc ){
		case HDR_PRF_MIKEY_1:
			ret += "<MIKEY-1>";
			break;
		case  HDR_PRF_MIKEY_256:
			ret += "<MIKEY-256>";
			break;
		case HDR_PRF_MIKEY_384:
			ret += "<MIKEY-384>";
			break;
		case  HDR_PRF_MIKEY_512:
			ret += "<MIKEY-512>";
			break;
	}
	
	ret += " CSB_id=<" + itoa( csbIdValue ) + ">";
	ret += " #CS=<" + itoa( nCsValue );
	ret += " CS ID map type=";
	if ( csIdMapTypeValue == HDR_CS_ID_MAP_TYPE_SRTP_ID )
		ret += "<SRTP-ID>";
	if ( csIdMapTypeValue == HDR_CS_ID_MAP_TYPE_IPSEC4_ID )
		ret += "<IPSEC4-ID>";
	else
		ret += "<unknown (" + itoa( csIdMapTypeValue ) + ")>";

	if( csIdMapPtr ){
		ret += "\n\n";
		ret += csIdMapPtr->debugDump();
		ret += "\n\n";
	}
	
	return ret;
}

int MikeyPayloadHDR::dataType() const{
	return dataTypeValue;
}

int MikeyPayloadHDR::v() const{
	return vValue;
}

uint8_t MikeyPayloadHDR::nCs() const{
	return nCsValue;
}

unsigned int MikeyPayloadHDR::csbId() const{
	return csbIdValue;
}

int MikeyPayloadHDR::csIdMapType() const{
	return csIdMapTypeValue;
}

MRef<MikeyCsIdMap *> MikeyPayloadHDR::csIdMap(){
	return csIdMapPtr;
}
