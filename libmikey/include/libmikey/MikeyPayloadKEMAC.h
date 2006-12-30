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


#ifndef MIKEYPAYLOADKEMAC_H
#define MIKEYPAYLOADKEMAC_H

#include<libmikey/libmikey_config.h>

#include<list>
#include<libmikey/MikeyPayload.h>
#include<libmikey/MikeyPayloadKeyData.h>

#define MIKEYPAYLOAD_KEMAC_PAYLOAD_TYPE 1

#define MIKEY_PAYLOAD_KEMAC_MAC_NULL 0
#define MIKEY_PAYLOAD_KEMAC_MAC_HMAC_SHA1_160 1

#define MIKEY_PAYLOAD_KEMAC_ENCR_NULL       0
#define MIKEY_PAYLOAD_KEMAC_ENCR_AES_CM_128 1
#define MIKEY_PAYLOAD_KEMAC_ENCR_AES_KW_128 2

class MikeyPayloads;

/**
 * @author Erik Eliasson, Johan Bilien
*/
class LIBMIKEY_API MikeyPayloadKEMAC : public MikeyPayload{
	public:
		MikeyPayloadKEMAC( int encrAlg, 
				   int encrDataLength, byte_t * encrData,
				   int macAlg, byte_t * macData );

		MikeyPayloadKEMAC( byte_t *start, int lengthLimit );
		~MikeyPayloadKEMAC();

		virtual int length();

		virtual void writeData( byte_t *start, int expectedLength );
		virtual std::string debugDump();
		
		int encrAlg();
		int encrDataLength();
		byte_t * encrData();

		/**
		 * @return MikeyPayloads object owned by the caller.
		 */
		MikeyPayloads* decodePayloads( int firstPayloadType,
			      byte_t * encrKey, int encrKeyLength,
			      byte_t * iv );

		int macAlg();
		byte_t * macData();

		void setMac( byte_t * data );
	private:
		int encrAlgValue;
		int encrDataLengthValue;
		byte_t * encrDataPtr;

		int macAlgValue;
		byte_t * macDataPtr;
};


#endif
