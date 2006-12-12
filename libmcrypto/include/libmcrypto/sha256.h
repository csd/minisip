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
 * Authors: Werner Dittmann <Werner.Dittmann@t-online.de>
 */

#ifndef _SHA256_H
#define _SHA256_H

#include <libmcrypto/config.h>


/**
 * Compute SHA256 digest.
 *
 * This functions takes one data chunk and computes its SHA256 digest. It uses
 * the openSSL SHA256 implementation as SHA256 engine.
 *
 * @param data
 *    Points to the data chunk.
 * @param data_length
 *    Length of the data in bytes
 * @param digest
 *    Points to a buffer that receives the computed digest. This
 *    buffer must have a size of at least 32 bytes (SHA256_DIGEST_LENGTH).
 */
LIBMCRYPTO_API void sha256(unsigned char *data, 
			   unsigned int data_length,
			   unsigned char *digest);

/**
 * Compute SHA256 digest over several data cunks.
 *
 * This functions takes several data chunk and computes the SHA256 digest. It
 * uses the openSSL SHA256 implementation as SHA256 engine.
 *
 * @param data
 *    Points to an array of pointers that point to the data chunks. A NULL
 *    pointer in an array element terminates the data chunks.
 * @param data_length
 *    Points to an array of integers that hold the length of each data chunk.
 * @param digest
 *    Points to a buffer that receives the computed digest. This
 *    buffer must have a size of at least 32 bytes (SHA256_DIGEST_LENGTH).
 */
LIBMCRYPTO_API void sha256(unsigned char *data[], 
			   unsigned int data_length[],
			   unsigned char *digest);
#endif

