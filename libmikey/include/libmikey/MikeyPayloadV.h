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

#ifndef MIKEYPAYLOADV_H
#define MIKEYPAYLOADV_H

#include<libmikey/MikeyPayload.h>

#define MIKEYPAYLOAD_V_PAYLOAD_TYPE 9

#define MIKEY_PAYLOAD_V_MAC_HMAC_SHA1_160 0
#define MIKEY_PAYLOAD_V_MAC_NULL 1
/**
 * @author Erik Eliasson, Johan Bilien
*/
class MikeyPayloadV : public MikeyPayload{
	public:
		MikeyPayloadV( int mac_alg, byte_t * verData );
		MikeyPayloadV( byte_t * start, int lengthLimit );

		virtual void writeData( byte_t * start, int expectedLength );
		virtual int length();

		int macAlg();
		byte_t * verData();

		void setMac( byte_t * data );

	private:
		int macAlgValue;
		byte_t * verDataPtr;
		
};


#endif
