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

#include<libmikey/MikeyPayloadT.h>
#include<libmikey/MikeyException.h>

#include<libmutil/stringutils.h>
#include<libmutil/mtime.h>

#include<assert.h>
#include<string.h>

#ifdef _MSC_VER
#	include<Winsock2.h>
#else
#	include<time.h>
#	include<sys/time.h>
#endif

using namespace std;

MikeyPayloadT::MikeyPayloadT(){

	this->payloadTypeValue = MIKEYPAYLOAD_T_PAYLOAD_TYPE;
	tsTypeValue = T_TYPE_NTP_UTC;

	struct timeval *tv;
	struct timezone *tz;

	tv = new struct timeval;
	tz = new struct timezone;

	gettimeofday( tv, tz );

	uint32_t ts_sec = tv->tv_sec + NTP_EPOCH_OFFSET 
				+ 60 * tz->tz_minuteswest;
	//10^-6 / 2^-32 = 4294.967296
	uint32_t ts_frac = (uint32_t)( tv->tv_usec * 4294.967296 );

	tsValue = ((uint64_t)ts_sec << 32) | ((uint64_t)ts_frac);
	delete tv;
	delete tz;
}


MikeyPayloadT::MikeyPayloadT( int type, uint64_t value ){

	this->payloadTypeValue = MIKEYPAYLOAD_T_PAYLOAD_TYPE;
	
	this->tsTypeValue=type;
	this->tsValue = value;
	
}

MikeyPayloadT::MikeyPayloadT( byte_t * start, int lengthLimit):
MikeyPayload( start ){
	
	int tsLength;
	
	this->payloadTypeValue = MIKEYPAYLOAD_T_PAYLOAD_TYPE;
	if( lengthLimit < 2 ){
                throw MikeyExceptionMessageLengthException(
                        "Given data is too short to form a T Payload" );
                return;
	}
	
	setNextPayloadType( start[0] );
	this->tsTypeValue= start[1];

	switch( tsTypeValue ){
		case 0:
			tsLength = 8;
			break;
		case 1:
			tsLength = 8;
			break;
		case 2:
			tsLength = 4;
			break;
		default:
			throw MikeyExceptionMessageContent(
					"Unknown type of time stamp" );
			break;
	
	}
	if( lengthLimit < 2 + tsLength ){
                throw MikeyExceptionMessageLengthException(
                        "Given data is too short to form a T Payload" );
                return;
	}
	
	switch( tsLength ){
		case 8:
			tsValue =(uint64_t)(start[2]) << 56 |
			    (uint64_t)(start[3]) << 48 |
			    (uint64_t)(start[4]) << 40 |
			    (uint64_t)(start[5]) << 32 |
			    (uint64_t)(start[6]) << 24 |
			    (uint64_t)(start[7]) << 16 |
			    (uint64_t)(start[8]) <<  8 |
			    (uint64_t)(start[9]);
			break;
		case 4:
			tsValue = (uint64_t)(start[2]) << 24 |
			     (uint64_t)(start[3]) << 16 |
			     (uint64_t)(start[4]) <<  8 |
			     (uint64_t)(start[5]);
	}

	endPtr = startPtr + 2 + tsLength;
	
	assert( endPtr - startPtr == length() );
}

MikeyPayloadT::~MikeyPayloadT(){
}

int MikeyPayloadT::length(){
	return 2 + ( ( tsTypeValue == T_TYPE_COUNTER )?4:8 );
}

void MikeyPayloadT::writeData(byte_t *start, int expectedLength){
	int i;
	assert( expectedLength == length() );
	memset( start, 0, expectedLength );
	start[0] = nextPayloadType();
	start[1] = (byte_t) tsTypeValue;
	switch( tsTypeValue ){
		case T_TYPE_NTP_UTC:
		case T_TYPE_NTP:
			for( i = 0; i < 8; i++ ){
				start[2+i] = (byte_t)((tsValue >> ((7-i)*8)) & 0xFF);
			}
			break;
		case T_TYPE_COUNTER:
			for( i = 0; i < 4; i++ ){
				start[2+i] = (byte_t)((tsValue >> ((3-i)*8)) & 0xFF);
			}
	}
}

string MikeyPayloadT::debugDump(){
	return "MikeyPayloadT: next_payload=<" + 
		itoa( nextPayloadType() ) +
		"> tsValue type=<" + itoa( tsTypeValue ) + 
		"> tsValue_value=<" + itoa( tsValue ) + ">";
}

int64_t MikeyPayloadT::offset( int type, uint64_t tsValue ){
	return this->tsValue - tsValue; 
}

bool MikeyPayloadT::checkOffset( uint64_t max ){
	
	struct timeval tv;
	struct timezone tz;
	uint64_t current_time;

	memset( &tv, 0, sizeof( tv ));
	memset( &tz, 0, sizeof( tz ));

	gettimeofday( &tv, &tz );

	//10^-6 / 2^-32 = 4294.967296
	uint32_t ts_frac = (uint32_t)( tv.tv_usec * 4294.967296 );
	uint32_t ts_sec;
	switch( tsTypeValue ){
		case T_TYPE_NTP_UTC:
			ts_sec = tv.tv_sec + NTP_EPOCH_OFFSET 
				+ 60 * tz.tz_minuteswest;
			current_time = ((uint64_t)ts_sec << 32) 
			    	     | ((uint64_t)ts_frac);
		/*	if( current_time > tsValue ){
				return (current_time - tsValue > max );
			}
			else{
				return (tsValue - current_time > max );
			}*/
			return false;
		case T_TYPE_NTP:
			ts_sec = tv.tv_sec + NTP_EPOCH_OFFSET 
				/*+ 60 * tz.tz_minuteswest*/;
			current_time = ((uint64_t)ts_sec << 32) 
			    	     | ((uint64_t)ts_frac);
			/*if( current_time > tsValue ){
				return (current_time - tsValue > max );
			}
			else{
				return (tsValue - current_time > max );
			}*/
			return false;
		case T_TYPE_COUNTER:
			throw MikeyException( 
			"Cannot compute a time offset with a counter ts" );
		default:
			throw MikeyExceptionMessageContent(
				"Unknown type of time stamp in T payload" );
	}
}
	

uint64_t MikeyPayloadT::ts(){
	return tsValue;
}
