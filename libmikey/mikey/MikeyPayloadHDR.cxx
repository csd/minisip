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
#include<assert.h>


MikeyPayloadHDR::MikeyPayloadHDR( int dataType, int v, int prfFunc, int csbId, 
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
		throw new MikeyExceptionMessageContent(
				"Unknown CS ID map type" );
	}
	
}

MikeyPayloadHDR::MikeyPayloadHDR( byte_t * start, int lengthLimit ):
	MikeyPayload( start ){

	this->payloadTypeValue = MIKEYPAYLOAD_HDR_PAYLOAD_TYPE;
	if( lengthLimit < 10 ){
		throw new MikeyExceptionMessageLengthException(
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
			throw new MikeyExceptionMessageLengthException(
			"Given data is too short to form a HDR Payload" );
			return;
		}
		if( csIdMapTypeValue == HDR_CS_ID_MAP_TYPE_SRTP_ID )
			this->csIdMapPtr = 
		   		new MikeyCsIdMapSrtp( &start[10], 9 * nCsValue );
		if( csIdMapTypeValue == HDR_CS_ID_MAP_TYPE_IPSEC4_ID )
			this->csIdMapPtr = 
		   		new MikeyCsIdMapIPSEC4( &start[10], 9 * nCsValue );
		this->endPtr = startPtr + 10 + 9 * this->nCsValue;
	}
	else{
		throw new MikeyExceptionMessageContent( 
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
	memset( start, expectedLength, 0 );
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
	
	return ret;
}

int MikeyPayloadHDR::dataType(){
	return dataTypeValue;
}

int MikeyPayloadHDR::v(){
	return vValue;
}

uint8_t MikeyPayloadHDR::nCs(){
	return nCsValue;
}

unsigned int MikeyPayloadHDR::csbId(){
	return csbIdValue;
}

int MikeyPayloadHDR::csIdMapType(){
	return csIdMapTypeValue;
}

MRef<MikeyCsIdMap *> MikeyPayloadHDR::csIdMap(){
	return csIdMapPtr;
}
MikeySrtpCs::MikeySrtpCs( uint8_t policyNo, uint32_t ssrc, uint32_t roc ):
	policyNo( policyNo ), ssrc( ssrc ), roc( roc ){};
//added 041201 JOOR
MikeyIPSEC4Cs::MikeyIPSEC4Cs( uint8_t policyNo, uint32_t spi, uint32_t spiaddr ):
	policyNo( policyNo ), spi( spi ), spiaddr( spiaddr ){};

MikeyCsIdMapSrtp::MikeyCsIdMapSrtp(){
	cs = list<MikeySrtpCs *>::list();
}
//added 041201 JOOR
MikeyCsIdMapIPSEC4::MikeyCsIdMapIPSEC4(){
	cs = list<MikeyIPSEC4Cs *>::list();
}

MikeyCsIdMapSrtp::MikeyCsIdMapSrtp( byte_t * data, int length ){
	if( length % 9 ){
		throw new MikeyException( 
				"Invalid length of SRTP_ID map info" );
	}

	uint8_t nCs = length / 9;
	uint8_t i;
	uint32_t ssrc, roc;
	byte_t policyNo;

	for( i = 0; i < nCs; i++ ){
		policyNo = data[ i*9 ];
		ssrc = (uint32_t)data[ i*9 + 1 ] << 24 |
		       (uint32_t)data[ i*9 + 2 ] << 16 |
		       (uint32_t)data[ i*9 + 3 ] <<  8 |
		       (uint32_t)data[ i*9 + 4 ];
		roc  = (uint32_t)data[ i*9 + 5 ] << 24 |
		       (uint32_t)data[ i*9 + 6 ] << 16 |
		       (uint32_t)data[ i*9 + 7 ] <<  8 |
		       (uint32_t)data[ i*9 + 8 ];
		addStream( ssrc, roc, policyNo );
	}
}
//added 041201 JOOR
MikeyCsIdMapIPSEC4::MikeyCsIdMapIPSEC4( byte_t * data, int length ){
	if( length % 9 ){
		throw new MikeyException( 
				"Invalid length of IPSEC4_ID map info" );
	}

	uint8_t nCs = length / 9;
	uint8_t i;
	uint32_t spi, spiaddr;
	byte_t policyNo;

	for( i = 0; i < nCs; i++ ){
		policyNo = data[ i*9 ];
		spi = (uint32_t)data[ i*9 + 1 ] << 24 |
		      (uint32_t)data[ i*9 + 2 ] << 16 |
		      (uint32_t)data[ i*9 + 3 ] <<  8 |
		      (uint32_t)data[ i*9 + 4 ];
		spiaddr  = (uint32_t)data[ i*9 + 5 ] << 24 |
		      (uint32_t)data[ i*9 + 6 ] << 16 |
		      (uint32_t)data[ i*9 + 7 ] <<  8 |
		      (uint32_t)data[ i*9 + 8 ];
		addSA( spi, spiaddr, policyNo );
	}
}

MikeyCsIdMapSrtp::~MikeyCsIdMapSrtp(){
	list<MikeySrtpCs *>::iterator i;

	for( i = cs.begin(); i!= cs.end() ; i++ )
		delete *i;
}
//added 041201 JOOR
MikeyCsIdMapIPSEC4::~MikeyCsIdMapIPSEC4(){
	list<MikeyIPSEC4Cs *>::iterator i;

	for( i = cs.begin(); i!= cs.end() ; i++ )
		delete *i;
}

int MikeyCsIdMapSrtp::length(){
	return 9 * cs.size();
}
//added 041201 JOOR
int MikeyCsIdMapIPSEC4::length(){
	return 9 * cs.size();
}

void MikeyCsIdMapSrtp::writeData( byte_t * start, int expectedLength ){
	if( expectedLength < length() ){
		throw new MikeyExceptionMessageLengthException(
				"CsSrtpId is too long" );
	}

	int j = 0,k;
	list<MikeySrtpCs *>::iterator i;

	for( i = cs.begin(); i != cs.end(); i++ ){
		start[ 9*j ] = (*i)->policyNo & 0xFF;
		for( k = 0; k < 4; k++ ){
			start[9*j+1+k] = ((*i)->ssrc >> 8*(3-k)) & 0xFF;
		}
		for( k = 0; k < 4; k++ ){
			start[9*j+5+k] = ((*i)->roc >> 8*(3-k)) & 0xFF;
		}
		j++;
	}
}
//added 041202 JOOR
void MikeyCsIdMapIPSEC4::writeData( byte_t * start, int expectedLength ){
	if( expectedLength < length() ){
		throw new MikeyExceptionMessageLengthException(
				"CsIPSEC4Id is too long" );
	}

	int j = 0,k;
	list<MikeyIPSEC4Cs *>::iterator i;

	for( i = cs.begin(); i != cs.end(); i++ ){
		start[ 9*j ] = (*i)->policyNo & 0xFF;
		for( k = 0; k < 4; k++ ){
			start[9*j+1+k] = ((*i)->spi >> 8*(3-k)) & 0xFF;
		}
		for( k = 0; k < 4; k++ ){
			start[9*j+5+k] = ((*i)->spiaddr >> 8*(3-k)) & 0xFF;
		}
		j++;
	}
}

byte_t MikeyCsIdMapSrtp::findCsId( uint32_t ssrc ){
	list<MikeySrtpCs *>::iterator i;
	uint8_t j = 1;

	for( i = cs.begin(); i != cs.end()  ; i++,j++ ){
		if( (*i)->ssrc == ssrc ){
			return j;
		}
	}
	return 0;
}
//added 041201 JOOR
byte_t MikeyCsIdMapIPSEC4::findCsId( uint32_t spi, uint32_t spiaddr ){
	list<MikeyIPSEC4Cs *>::iterator i;
	uint8_t j = 1;

	for( i = cs.begin(); i != cs.end()  ; i++,j++ ){
		if( (*i)->spi == spi && (*i)->spiaddr == spiaddr ){
			return j;
		}
	}
	return 0;
}
//added 041214 JOOR
byte_t MikeyCsIdMapSrtp::findpolicyNo( uint32_t ssrc ){
	list<MikeySrtpCs *>::iterator i;
	for( i = cs.begin(); i != cs.end()  ; i++ ){
		if( (*i)->ssrc == ssrc ){
			return (*i)->policyNo;
		}
	}
	return 0;
}

//added 041201 JOOR
byte_t MikeyCsIdMapIPSEC4::findpolicyNo( uint32_t spi, uint32_t spiaddr ){
	list<MikeyIPSEC4Cs *>::iterator i;
	for( i = cs.begin(); i != cs.end()  ; i++ ){
		if( (*i)->spi == spi && (*i)->spiaddr == spiaddr ){
			return (*i)->policyNo;
		}
	}
	return 0;
}



uint32_t MikeyCsIdMapSrtp::findRoc( uint32_t ssrc ){
	list<MikeySrtpCs *>::iterator i;

	for( i = cs.begin(); i != cs.end()  ; i++ ){
		if( (*i)->ssrc == ssrc ){
			return (*i)->roc;
		}
	}
	return 0;
}

void MikeyCsIdMapSrtp::addStream( uint32_t ssrc, uint32_t roc, byte_t policyNo, byte_t csId ){
	if( csId == 0 ){
		cs.push_back( new MikeySrtpCs( policyNo, ssrc, roc ) );
		return;
	}

	list<MikeySrtpCs *>::iterator i;
	uint8_t j = 1;
	for( i = cs.begin(); i != cs.end() ; i++,j++ ){
		if( j == csId ){
			(*i)->ssrc = ssrc;
			(*i)->policyNo = policyNo;
			(*i)->roc = roc;
		}
	}

	return;
}
//added 041201 JOOR
void MikeyCsIdMapIPSEC4::addSA( uint32_t spi, uint32_t spiaddr, byte_t policyNo, byte_t csId){
	if( csId == 0 ){
		cs.push_back( new MikeyIPSEC4Cs( policyNo, spi, spiaddr ) );
		return;
	}

	list<MikeyIPSEC4Cs *>::iterator i;
	uint8_t j = 1;
	for( i = cs.begin(); i != cs.end() ; i++,j++ ){
		if( j == csId ){
			(*i)->spi = spi;
			(*i)->policyNo = policyNo;
			(*i)->spiaddr = spiaddr;
		}
	}

	return;
}


