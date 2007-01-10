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


#ifndef MIKEYPAYLOADKEYDATA_H
#define MIKEYPAYLOADKEYDATA_H

#include<libmikey/libmikey_config.h>

#include<libmikey/MikeyPayload.h>
#include<libmikey/KeyValidity.h>

#define MIKEYPAYLOAD_KEYDATA_PAYLOAD_TYPE 20

#define KEYDATA_TYPE_TGK      0
#define KEYDATA_TYPE_TGK_SALT 1
#define KEYDATA_TYPE_TEK      2
#define KEYDATA_TYPE_TEK_SALT 3
/**
 * @author Erik Eliasson, Johan Bilien
*/
class LIBMIKEY_API MikeyPayloadKeyData : public MikeyPayload{
	public:
		MikeyPayloadKeyData( int type, byte_t * keyData, 
				     int keyDataLength, 
				     MRef<KeyValidity *> kv );
		MikeyPayloadKeyData( int type, byte_t * keyData, 
				     int keyDataLength, byte_t * saltData, 
				     int saltDataLength, 
				     MRef<KeyValidity *> kv );
		MikeyPayloadKeyData( byte_t * start, int lengthLimit );
		~MikeyPayloadKeyData();

		virtual void writeData( byte_t * start, int expectedLength );
		virtual int length();
		virtual std::string debugDump();

		int type();
		MRef<KeyValidity *> kv();
		
		byte_t * keyData();
		int keyDataLength();
		
		byte_t * saltData();
		int saltDataLength();

	private:
		int typeValue;

		byte_t * keyDataPtr;
		int keyDataLengthValue;

		byte_t * saltDataPtr;
		int saltDataLengthValue;

		MRef<KeyValidity *> kvPtr;
		
};


#endif
