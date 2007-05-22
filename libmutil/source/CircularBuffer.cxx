/*
  Copyright (C) 2004,2005  Cesc Santasuana

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

/**
See Note in the header file about the source.
*/

#include <config.h>
#include <libmutil/CircularBuffer.h>

// #include<stdio.h>

#include <string.h> //for memcpy

CircularBuffer::CircularBuffer(int size_):
		maxSize(size_),
		size(0),
		readIdx(0),
		writeIdx(0),
		byteCounter(0) {
		
	buf = NULL;
	buf = new short[size_];
}

CircularBuffer::CircularBuffer(const CircularBuffer &b):
		maxSize(b.maxSize),
		size(b.size),
		readIdx(b.readIdx),
		writeIdx(b.writeIdx),
		byteCounter(b.byteCounter) {
		
	buf = new short[size];
	memcpy(buf, b.buf, size*sizeof(short));
}


CircularBuffer::~CircularBuffer()
{
	if( buf != NULL ) {
		delete[] buf;
	}
}

bool CircularBuffer::write(const short *s, int len, bool forcedWrite ) {
	
	if( len > getFree() )
	{
		if( !forcedWrite ) {
			return false; // overflow
		} else {
			if( len > getMaxSize() ) {
				return false; //force write but not enough capacity
			} else { //remove enough stuff to fit this forces write
				int removeNum;
				removeNum = len - getFree(); 
// 				printf("CBRM\n");
				if( ! remove( removeNum )  ) {
					return false; //strange error ...
				}
			}
		}
	}
	
	//from here, just write ... 
	//if the write is forced, we already made space for it to fit ...
	
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

void CircularBuffer::clear() {
	read( NULL, getSize() ); //remove all current elements ... = clear
}
