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


#ifndef KEYVALIDITY_H
#define KEYVALIDITY_H

#include<libmikey/libmikey_config.h>

#define KEYVALIDITY_NULL     0
#define KEYVALIDITY_SPI      1
#define KEYVALIDITY_INTERVAL 2

#include<libmikey/MikeyDefs.h>
#include<libmutil/MemObject.h>

#define KeyValidityNull KeyValidity

class LIBMIKEY_API KeyValidity : public MObject{
	public:
		KeyValidity();
		KeyValidity( const KeyValidity& );
		~KeyValidity();
		
		void operator =( const KeyValidity& );
		virtual int length();
		int type();
		virtual void writeData( byte_t * start, int expectedLength );
		virtual std::string debugDump();
		virtual std::string getMemObjectType() const { return "KeyValidity"; };
	protected:
		int typeValue;
		
};

class LIBMIKEY_API KeyValiditySPI : public KeyValidity{
	public:
		KeyValiditySPI();
		KeyValiditySPI( const KeyValiditySPI& );
		KeyValiditySPI( byte_t * rawData, int lengthLimit );
		KeyValiditySPI( int length, byte_t * spi );
		virtual ~KeyValiditySPI();
		
		void operator =( const KeyValiditySPI& );
		virtual int length();
		virtual void writeData( byte_t * start, int expectedLength );
		virtual std::string debugDump();
	private:
		int spiLength;
		byte_t *spiPtr;
};

class LIBMIKEY_API KeyValidityInterval : public KeyValidity{
	public:
		KeyValidityInterval();
		KeyValidityInterval( const KeyValidityInterval& );
		KeyValidityInterval( byte_t * rawData, int lengthLimit );
		KeyValidityInterval( int vfLength, byte_t * vf,
				     int vtLength, byte_t * vt );
		virtual ~KeyValidityInterval();
		
		void operator =( const KeyValidityInterval& );
		virtual int length();
		virtual void writeData(byte_t * start, int expectedLength);
		virtual std::string debugDump();
	private:
		int vfLength;
		byte_t * vf;
		int vtLength;
		byte_t * vt;
};

#endif
