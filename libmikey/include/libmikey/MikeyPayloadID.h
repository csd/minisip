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

#ifndef MIKEYPAYLOADID_H
#define MIKEYPAYLOADID_H

#include<libmikey/MikeyPayload.h>

#define MIKEYPAYLOAD_ID_PAYLOAD_TYPE 6

#define MIKEYPAYLOAD_ID_TYPE_NAI 0
#define MIKEYPAYLOAD_ID_TYPE_URI 1

/**
 * @author Erik Eliasson, Johan Bilien
*/
class MikeyPayloadID : public MikeyPayload{
	public:
		
		MikeyPayloadID( int type, int idLength, byte_t * idData );
		MikeyPayloadID( byte_t * start, int lengthLimit );
		~MikeyPayloadID();

		virtual void writeData(byte_t *start, int expectedLength);
		virtual int length();
		virtual std::string debugDump();

	private:
		int idTypeValue;
		int idLengthValue;
		byte_t * idDataPtr;
		
};


#endif
