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

#include <libmcrypto/config.h>

#include <stdlib.h>

typedef struct _f8_ctx {
    unsigned char *S;
    unsigned char *ivAccent;
    uint32_t J;
} F8_CIPHER_CTX;



class LIBMCRYPTO_API AES{
	public:
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

		/* f8-mode encryption, in place */
		void f8_encrypt( unsigned char * data, 
				 unsigned int data_length,
				 unsigned char * iv,
				 unsigned char *key,
				 unsigned int keyLen,
				 unsigned char *salt, 
				 unsigned int saltLen);

		/* f8-mode encryption */
		void f8_encrypt(unsigned char *in, 
				unsigned int in_length, 
				unsigned char *out,
				unsigned char *iv, 
				unsigned char *key,
				unsigned int keyLen,
				unsigned char *salt, 
				unsigned int saltLen);


	private:
		void set_encrypt_key( unsigned char *key, int key_nb_bits );

		int processBlock(F8_CIPHER_CTX *f8ctx, 
			     unsigned char *in, 
			     int length, 
			     unsigned char *out);
		void *m_key;

		AES();
};

#endif
