/*
  Copyright (C) 2006 Werner Dittmann
  
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
 * 	    Pan Xuan <xuan@kth.se>
 */

#ifndef _SHA1_H
#define _SHA1_H

#include <libmcrypto/config.h>


/**
 * Compute SHA1 digest.
 *
 * This functions takes one data chunk and computes its SHA1 digest. It uses
 * the openSSL SHA1 implementation.
 *
 * @param data
 *    Points to the data chunk.
 * @param data_length
 *    Length of the data in bytes
 * @param digest
 *    Points to a buffer that receives the computed digest. This
 *    buffer must have a size of at least 32 bytes (SHA256_DIGEST_LENGTH).
 */
LIBMCRYPTO_API void sha1(unsigned char * data, 
			   unsigned int data_length,
			   unsigned char * digest);

#endif

