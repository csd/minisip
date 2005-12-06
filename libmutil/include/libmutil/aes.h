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


#ifndef AES_H
#define AES_H

#ifdef HAVE_CONFIG_H
#include<config.h>
#endif

#ifdef HAVE_OPENSSL_AES_H
#include <openssl/aes.h>
#else
#include"rijndael-alg-fst.h"
#endif

#include <libmutil/libmutil_config.h>

#include <stdlib.h>

#define AES_BLOCK_SIZE 16

#ifndef HAVE_OPENSSL_AES_H
// Redefinition of this AES type to use Paulo Barreto 
// <paulo.barreto@terra.com.br> implementation instead, 
// to avoid the requirement for libssl 0.9.7
#define NB_ROUND 10 /* 128 bits keys */

struct AES_KEY_s{
	unsigned int key[4*(NB_ROUND+1)];
};

typedef AES_KEY_s AES_KEY;
#endif

class LIBMUTIL_API AES{
	public:
		AES();
		AES( unsigned char * key, int key_length );
		~AES();

		void encrypt( const unsigned char * input, unsigned char * output );
		void get_ctr_cipher_stream( unsigned char * output, unsigned int length,
				unsigned char * iv );
		/* Counter-mode encryption */
		void ctr_encrypt( const unsigned char * input, 
				  unsigned int input_length,
			 unsigned char * output, unsigned char * iv );
		
		/* Counter-mode encryption, in place */
		void ctr_encrypt( unsigned char * data, 
				  unsigned int data_length,
			 	  unsigned char * iv );

	private:
		AES_KEY *key;
};

#endif
