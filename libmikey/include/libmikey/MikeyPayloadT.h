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

#ifndef MIKEYPAYLOADT_H
#define MIKEYPAYLOADT_H

#include<libmikey/MikeyPayload.h>

#define MIKEYPAYLOAD_T_PAYLOAD_TYPE 5

#define T_TYPE_NTP_UTC 0
#define T_TYPE_NTP     1
#define T_TYPE_COUNTER 2

#define NTP_EPOCH_OFFSET 2208988800UL
/**
 * @author Erik Eliasson, Johan Bilien
*/
class MikeyPayloadT : public MikeyPayload{
	public:
		MikeyPayloadT(); // Will compute the timestamp	
		MikeyPayloadT( int type, unsigned long long value );
		MikeyPayloadT( byte_t * start, int lengthLimit );
		~MikeyPayloadT();

		virtual int length();

		virtual void writeData( byte_t *start, int expectedLength );
		virtual std::string debugDump();

		long long offset( int type, unsigned long long ts );
		bool checkOffset( unsigned long long max );

		unsigned long long ts();

	private:
		int tsTypeValue;
		unsigned long long tsValue;
};


#endif
