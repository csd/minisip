/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef MIKEYPAYLOADSIGN_H
#define MIKEYPAYLOADSIGN_H

#include<libmikey/MikeyPayload.h>

#define MIKEYPAYLOAD_SIGN_PAYLOAD_TYPE 4

#define MIKEYPAYLOAD_SIGN_TYPE_RSA_PKCS 0
#define MIKEYPAYLOAD_SIGN_TYPE_RSA_PSS 1

/**
 * @author Erik Eliasson, Johan Bilien
*/
class MikeyPayloadSIGN : public MikeyPayload{
	public:
	
		MikeyPayloadSIGN( int sigLength, byte_t * data, int type );
		MikeyPayloadSIGN( byte_t * start, int lengthLimit );
		~MikeyPayloadSIGN();

		virtual void writeData( byte_t *start, int expectedLength );
		virtual int length();
		virtual std::string debugDump();

		int sigLength();
		byte_t * sigData();
		int sigType();

		void setSigData( byte_t * data );

	private:
		int sigTypeValue;
		int sigLengthValue;
		byte_t * sigDataPtr;
		
};


#endif
