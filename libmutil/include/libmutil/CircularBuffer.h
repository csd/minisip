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
The original code of the Circular buffer was by Anders Hedstrom. 
It has been modified and simplified. 
Variable names where changed to make it more clear as to what was its functionality.
New functions providing different status over the buffer where added, some others 
were removed from the original code.
Also, the original code was for a circular buffer of chars ... this is for shorts :D
But as there are not many different ways to 
implement a circular buffer, the core lines for read and write stay practically the
same (i mean, even writting from scratch, it would look pretty much the same).
*/


#ifndef _CIRCULARBUFFER_H
#define _CIRCULARBUFFER_H

#include <libmutil/libmutil_config.h>

/**
A circular buffer for shorts
*/
class LIBMUTIL_API CircularBuffer {
	public:
		CircularBuffer(int size);
		CircularBuffer(const CircularBuffer &);
		virtual ~CircularBuffer();
	
		/**
		Write len elements from buffer s into the circular
		buffer.
		If there is not enough space, nothing will be written.
		@param s data buffer
		@param len length of the data buffer
		@param forcedWrite force writting, even if this means overwritting 
			part of the contents. Use with caution.
		@return true if written correctly, else false (indicates
			either an error or that there was not enough space).
			If forcedWrite is true and we managed to write (the buff max capacity
			is big enough), we return true.
		*/
		virtual bool write( const short *s, int len, bool forcedWrite = false);
		
		/**
		Read len elements from circular buffer to s buffer.
		If more than available elements is requested, nothing is read.
		@param s data buffer
		@param len length of the data buffer
		@return true if read correctly, else false (indicates
			either an error or that there were not enough elements).
		*/
		virtual bool read( short *s, int len);
		
		/**
		Simulate a read of len elements, but do not care what is read.
		If more than available elements is requested, nothing is removed
		@param len length of the data buffer
		@return true if read correctly, else false (indicates
			either an error or that there were not enough elements).
		*/
		bool remove(int len);
	
		/**
		Empty the buffer
		*/
		void clear();
		
		/**
		Return the currently used slots.
		For the total size of the buffer, use 
		getMaxSize()
		*/
		int getSize() { return size; };
		
		/**
		Return the max size of the buffer, that is, the one
		requested when the buffer was created.
		*/
		int getMaxSize() { return maxSize; };
		
		/**
		Returns how many available slots are there in 
		the buffer, that is, return how many slots can 
		be written into the buffer without a problem.
		*/
		int getFree() { return (getMaxSize() - getSize() ); };
	
		/**
		Return the total number of bytes that have been 
		written into the buffer during all its exhistence
		*/
		unsigned long getByteCounter() { return byteCounter; };
		
	private:
		CircularBuffer& operator=(const CircularBuffer& ) { return *this; }
		
		/**
		Return the read start point in the buffer
		*/
		char * getReadStart();
		
		/**
		The actual buffer structure
		*/
		short *buf;
		
		/**
		Max number of elements this buffer can have
		*/
		int maxSize;
		
		/**
		Current number of used elements from the buffer
		*/
		int size;
		
		/**
		Read Pointer index
		*/
		int readIdx;
		
		/**
		Write pointer index
		*/
		int writeIdx;
		
		/**
		Counter of how many bytes have been written here
		*/
		unsigned long byteCounter;
};

#endif // _CIRCULARBUFFER_H
