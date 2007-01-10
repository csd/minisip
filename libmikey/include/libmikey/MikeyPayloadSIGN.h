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


#ifndef MIKEYPAYLOADSIGN_H
#define MIKEYPAYLOADSIGN_H

#include<libmikey/libmikey_config.h>

#include<libmikey/MikeyPayload.h>

#define MIKEYPAYLOAD_SIGN_PAYLOAD_TYPE 4

#define MIKEYPAYLOAD_SIGN_TYPE_RSA_PKCS 0
#define MIKEYPAYLOAD_SIGN_TYPE_RSA_PSS 1

/**
 * @author Erik Eliasson, Johan Bilien
*/
class LIBMIKEY_API MikeyPayloadSIGN : public MikeyPayload{
	public:
	
		MikeyPayloadSIGN( int sigLength, int type );
		MikeyPayloadSIGN( byte_t * start, int lengthLimit );
		~MikeyPayloadSIGN();

		virtual void writeData( byte_t *start, int expectedLength );
		virtual int length();
		virtual std::string debugDump();

		int sigLength();
		byte_t * sigData();
		int sigType();

		void setSigData( byte_t * data, int sigLength);

	private:
		int sigTypeValue;
		int sigLengthValue;
		byte_t * sigDataPtr;
		
};


#endif
