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


#include <libmutil/aes.h>
#include<config.h>

//debug:
#ifdef DEBUG_OUTPUT
#include<iostream>
#include<libmutil/print_hex.h>
using namespace std;
#endif

#include<string.h>



#ifndef HAVE_OPENSSL_AES_H


// openssl AES functions using Paulo Barreto <paulo.barreto@terra.com.br>
// implementation, to avoid the requirement for version 0.9.7 of libssl
void AES_set_encrypt_key( unsigned char * key, int key_nb_bits, AES_KEY * aes_key ){
	rijndaelKeySetupEnc( aes_key->key, key, key_nb_bits );
}

void AES_encrypt( const unsigned char * input, unsigned char * output, AES_KEY * aes_key ){
	rijndaelEncrypt( aes_key->key, NB_ROUND, input, output );
}
#endif

AES::AES():key(NULL){
}

AES::AES( unsigned char * key, int key_length ){
	this->key = (AES_KEY *) malloc( sizeof( AES_KEY ) );
	memset( this->key, '\0', sizeof( AES_KEY ) );;
	AES_set_encrypt_key( key, key_length*8, this->key );
}

AES::~AES(){
	if( key )
		free( key );
}

void AES::encrypt( const unsigned char * input, unsigned char * output ){
	AES_encrypt( input, output, key );
}

void AES::get_ctr_cipher_stream( unsigned char * output, unsigned int length,
				unsigned char * iv ){
	unsigned long ctr; // Should be 128 bits, but we
	                     // assume that we 32 bits is enough ...
	unsigned char * aes_input, * temp;
	uint32_t input;
	
	aes_input = (unsigned char *)malloc( AES_BLOCK_SIZE );
	temp = (unsigned char *)malloc( AES_BLOCK_SIZE );

	memcpy( aes_input, iv, 12 );

	for( ctr = 0; ctr < length/AES_BLOCK_SIZE; ctr++ ){
		input = ctr + U32_AT(iv);
		//compute the cipher stream
		aes_input[12] = (byte_t)((input & 0xFF000000) >> 24);
		aes_input[13] = (byte_t)((input & 0x00FF0000) >> 16);
		aes_input[14] = (byte_t)((input & 0x0000FF00) >>  8);
		aes_input[15] = (byte_t)((input & 0x000000FF));

		AES_encrypt( aes_input, &output[ctr*AES_BLOCK_SIZE], key );
	}

	// Treat the last bytes:
	input = ctr + U32_AT(iv);
	aes_input[12] = (byte_t)((input & 0xFF000000) >> 24);
	aes_input[13] = (byte_t)((input & 0x00FF0000) >> 16);
	aes_input[14] = (byte_t)((input & 0x0000FF00) >>  8);
	aes_input[15] = (byte_t)((input & 0x000000FF));
	
	AES_encrypt( aes_input, temp, key );
	memcpy( &output[ctr*AES_BLOCK_SIZE], temp, length % AES_BLOCK_SIZE );

	free( temp );
	free( aes_input );
}
	


void AES::ctr_encrypt( const unsigned char * input, unsigned int input_length,
		 unsigned char * output, unsigned char * iv ){
//	unsigned char cipher_stream[input_length];
	unsigned char *cipher_stream = new unsigned char[input_length];
	

	get_ctr_cipher_stream( cipher_stream, input_length, iv );

	for( unsigned int i = 0; i < input_length; i++ ){
		output[i] = cipher_stream[i] ^ input[i];
	}
	delete []cipher_stream;
}

void AES::ctr_encrypt( unsigned char * data, unsigned int data_length,
		       unsigned char * iv ){
	//unsigned char cipher_stream[data_length];
	unsigned char *cipher_stream = new unsigned char[data_length];

	get_ctr_cipher_stream( cipher_stream, data_length, iv );

	for( unsigned int i = 0; i < data_length; i++ ){
		data[i] ^= cipher_stream[i];
	}
	delete cipher_stream;
}
