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


#ifndef MIKEYPAYLOADERR_H
#define MIKEYPAYLOADERR_H

#include<libmikey/libmikey_config.h>

#include<libmikey/MikeyPayload.h>

#define MIKEYPAYLOAD_ERR_PAYLOAD_TYPE 12

#define MIKEY_ERR_TYPE_AUTH_FAILURE  0
#define MIKEY_ERR_TYPE_INVALID_TS    1
#define MIKEY_ERR_TYPE_INVALID_PRF   2
#define MIKEY_ERR_TYPE_INVALID_MAC   3
#define MIKEY_ERR_TYPE_INVALID_EA    4
#define MIKEY_ERR_TYPE_INVALID_HA    5
#define MIKEY_ERR_TYPE_INVALID_DH    6
#define MIKEY_ERR_TYPE_INVALID_ID    7
#define MIKEY_ERR_TYPE_INVALID_CERT  8
#define MIKEY_ERR_TYPE_INVALID_SP    9
#define MIKEY_ERR_TYPE_INVALID_SPPAR 10
#define MIKEY_ERR_TYPE_INVALID_DT    11
#define MIKEY_ERR_TYPE_UNSPEC        12

/**
 * @author Erik Eliasson, Johan Bilien
*/
class LIBMIKEY_API MikeyPayloadERR : public MikeyPayload{
	public:
		
		MikeyPayloadERR( int errType );
		MikeyPayloadERR( byte_t * start, int lengthLimit );
		~MikeyPayloadERR();

		virtual void writeData( byte_t * start, int expectedLength );
		virtual int length();
		virtual std::string debugDump();

		int errorType();

	private:
		int errTypeValue;
		
};


#endif
