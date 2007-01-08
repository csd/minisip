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


#ifndef MIKEYPAYLOAD_H
#define MIKEYPAYLOAD_H

#include<libmikey/libmikey_config.h>

#include<libmutil/mtypes.h>
#include<libmutil/MemObject.h>

#include<string>

#define MIKEYPAYLOAD_LAST_PAYLOAD 0

class LIBMIKEY_API MikeyPayload: public MObject{
	public:
		static const int LastPayload;

		MikeyPayload();
		
		MikeyPayload( byte_t *start_of_message );
		virtual ~MikeyPayload();

		/**
		 * @returns The type of this payload.
		 */
		int payloadType();

		/**
		 *
		 * @returns The type of the payload that is starting 
		 * 	at get_end().
		 */
		int nextPayloadType();
		
		void setNextPayloadType(int t);

		/**
		 *
		 * @returns Pointer to the first memory location after 
		 * 	this payload
		 */
		byte_t * end();
		

		virtual int length()=0;

		virtual void writeData(byte_t *start, int expectedLength)=0;
		
		virtual std::string debugDump(){return "not_defined";}
		
	protected:

		

		bool rawPacketValid;
		
		byte_t *startPtr;	//Points to the first memory position
					//within this payload.

		
		byte_t * endPtr; 	//Points to the first memory position
					//after this payload
		
		int nextPayloadTypeValue;
		int payloadTypeValue;
		
	private:
};


#endif
