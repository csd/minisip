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


#ifndef MIKEYPAYLOADPKE_H
#define MIKEYPAYLOADPKE_H

#include<libmikey/libmikey_config.h>

#include<libmikey/MikeyPayload.h>

#define MIKEYPAYLOAD_PKE_PAYLOAD_TYPE 2

/**
 * @author Erik Eliasson, Johan Bilien
*/
class LIBMIKEY_API MikeyPayloadPKE : public MikeyPayload{
	public:
		MikeyPayloadPKE( int c, byte_t * data, int data_length );

		MikeyPayloadPKE( byte_t * start, int lengthLimit );
		~MikeyPayloadPKE();

		virtual int length();

		virtual void writeData( byte_t * start, int expectedLength );
		virtual std::string debugDump();
		int c();
		int dataLength() const;
		const byte_t * data() const;

	private:
		int cValue;
		int dataLengthValue;
		byte_t * dataPtr;
		
};


#endif
