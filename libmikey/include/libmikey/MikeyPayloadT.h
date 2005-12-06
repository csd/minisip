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


#ifndef MIKEYPAYLOADT_H
#define MIKEYPAYLOADT_H

#include<libmikey/libmikey_config.h>

#include<libmikey/MikeyPayload.h>

#define MIKEYPAYLOAD_T_PAYLOAD_TYPE 5

#define T_TYPE_NTP_UTC 0
#define T_TYPE_NTP     1
#define T_TYPE_COUNTER 2

#define NTP_EPOCH_OFFSET 2208988800UL
/**
 * @author Erik Eliasson, Johan Bilien
*/
class LIBMIKEY_API MikeyPayloadT : public MikeyPayload{
	public:
		MikeyPayloadT(); // Will compute the timestamp	
		MikeyPayloadT( int type, uint64_t value );
		MikeyPayloadT( byte_t * start, int lengthLimit );
		~MikeyPayloadT();

		virtual int length();

		virtual void writeData( byte_t *start, int expectedLength );
		virtual std::string debugDump();

		int64_t offset( int type, uint64_t ts );
		bool checkOffset( uint64_t max );

		uint64_t ts();

	private:
		int tsTypeValue;
		uint64_t tsValue;
};


#endif
