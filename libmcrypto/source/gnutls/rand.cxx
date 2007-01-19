/*
  Copyright (C) 2006 Mikael Magnusson
  
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
 * Authors: Mikael Magnusson <mikma@users.sourceforge.net>
*/


#include<config.h>

#include <libmcrypto/rand.h>
#include <gcrypt.h>

bool Rand::randomize(void *buffer, size_t length)
{
	gcry_randomize(buffer, length, GCRY_STRONG_RANDOM);
	return true;
}

bool Rand::randomize(void *buffer, size_t length, MRef<SipSim *> sim)
{
	unsigned char * tempBufferPtr = new unsigned char[16];
        int index = 0;
	int left = length;
	while(left > 16){
		if(sim->getRandomValue(tempBufferPtr, 16)){
			memcpy(&((unsigned char *)buffer)[index], tempBufferPtr, 16);
			index += 16;
			left -= 16;
		}
		else{
			delete [] tempBufferPtr;
			return false;
	        }
	}
	if(sim->getRandomValue(tempBufferPtr, 16)){
	        memcpy(&((unsigned char *)buffer)[index], tempBufferPtr, left);
		delete [] tempBufferPtr;
		return true;
	}
	else{
		delete [] tempBufferPtr;
		return false;
	}
}
