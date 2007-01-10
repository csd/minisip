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


#ifndef MIKEYPAYLOADDH_H
#define MIKEYPAYLOADDH_H

#include<libmikey/libmikey_config.h>

#include<libmikey/MikeyPayload.h>
#include<libmikey/KeyValidity.h>
#include<libmikey/MikeyMessage.h>

#define MIKEYPAYLOAD_DH_PAYLOAD_TYPE 3

#define MIKEYPAYLOAD_DH_GROUP5 0
#define MIKEYPAYLOAD_DH_GROUP1 1
#define MIKEYPAYLOAD_DH_GROUP2 2


/**
 * @author Erik Eliasson, Johan Bilien
*/
class LIBMIKEY_API MikeyPayloadDH : public MikeyPayload{
	public:
		
		MikeyPayloadDH( int dhGroup, byte_t * dhKey, 
				MRef<KeyValidity *> kv );
		MikeyPayloadDH( byte_t * start, int lengthLimit );
		~MikeyPayloadDH();

		virtual void writeData( byte_t * start, int expectedLength );
		virtual int length();
		virtual std::string debugDump();

		int group();
		byte_t * dhKey();
		int dhKeyLength();

		MRef<KeyValidity *> kv();

	private:
		int dhGroup;
		int dhKeyLengthValue;
		byte_t * dhKeyPtr;
		MRef<KeyValidity *> kvPtr;
		
};


#endif
