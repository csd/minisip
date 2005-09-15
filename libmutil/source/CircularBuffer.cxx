 
 /*
Copyright (C) 2004,2005  Anders Hedstrom

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
00011 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <libmutil/CircularBuffer.h>

#include <config.h>

#include <string.h> //for memcpy

CircularBuffer::CircularBuffer(int size):
		maxSize(size),
		size(0),
		readIdx(0),
		writeIdx(0),
		byteCounter(0) {
		
	buf = NULL;
	buf = new short[size];
}


CircularBuffer::~CircularBuffer()
{
	if( buf != NULL ) {
		delete[] buf;
	}
}


bool CircularBuffer::write(const short *s, int len) {
	
	if( len > getFree() )
	{
		return false; // overflow
	}
	
	byteCounter += (unsigned long)len;
	
	//Check if write block crosses the circular border 
	if ( writeIdx + len > getMaxSize() ) {
		int lenLeft = getMaxSize() - writeIdx; // size left until circular border crossing
		memcpy(buf + writeIdx, s, lenLeft * sizeof(short));
		memcpy(buf, s + lenLeft, (len - lenLeft)* sizeof(short));
		writeIdx = len - lenLeft;
	} else {
		memcpy(buf + writeIdx, s, len * sizeof(short));
		writeIdx += len;
		if (writeIdx >= getMaxSize())
			writeIdx -= getMaxSize();
	}
	size += len;
	return true;
}


bool CircularBuffer::read(short *s, int len)
{
	if (len > getSize() ) {
		return false; // not enough chars
	}
	
	if (readIdx + len > getMaxSize() ) { // block crosses circular border
		int lenLeft = getMaxSize() - readIdx;
		if (s)
		{
			memcpy(s, buf + readIdx, lenLeft * sizeof(short));
			memcpy(s + lenLeft, buf, (len - lenLeft) * sizeof(short));
		}
		readIdx = len - lenLeft;
	} else 	{
		if (s) {
			memcpy(s, buf + readIdx, len * sizeof(short));
		}
		readIdx += len;
		if ( readIdx >= getMaxSize() )
			readIdx -= getMaxSize();
	}
	size -= len;
	
	if ( getSize() == 0 ) {
		readIdx = writeIdx = 0;
	}
	return true;
}


bool CircularBuffer::remove(int len)
{
	return read(NULL, len);
}


