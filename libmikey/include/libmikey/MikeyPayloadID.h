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


#ifndef MIKEYPAYLOADID_H
#define MIKEYPAYLOADID_H

#include<libmikey/libmikey_config.h>

#include<libmikey/MikeyPayload.h>

#define MIKEYPAYLOAD_ID_PAYLOAD_TYPE 6

#define MIKEYPAYLOAD_ID_TYPE_NAI 0
#define MIKEYPAYLOAD_ID_TYPE_URI 1

/**
 * @author Erik Eliasson, Johan Bilien
*/
class LIBMIKEY_API MikeyPayloadID : public MikeyPayload{
	public:
		
		MikeyPayloadID( int type, int idLength, byte_t * idData );
		MikeyPayloadID( byte_t * start, int lengthLimit );
		~MikeyPayloadID();

		virtual void writeData(byte_t *start, int expectedLength);
		virtual int length();
		virtual std::string debugDump();

		int idType() const;
		int idLength() const;
		const byte_t * idData() const;

	private:
		int idTypeValue;
		int idLengthValue;
		byte_t * idDataPtr;
		
};


#endif
