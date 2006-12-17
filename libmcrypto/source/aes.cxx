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


#include<config.h>

#ifdef HAVE_OPENSSL_AES_H
#include <openssl/aes.h>
#else
# define AES_BLOCK_SIZE 16
typedef struct AES_KEY_s AES_KEY;
#include <libmcrypto/rijndael-alg-fst.h>
#endif

#include <libmcrypto/aes.h>

//debug:
#ifdef DEBUG_OUTPUT
#include<iostream>
using namespace std;
#endif

#include<string.h>

#ifndef HAVE_GNUTLS
#ifndef HAVE_OPENSSL_AES_H

// Redefinition of this AES type to use Paulo Barreto 
// <paulo.barreto@terra.com.br> implementation instead, 
// to avoid the requirement for libssl 0.9.7
#define NB_ROUND 10 /* 128 bits keys */

struct AES_KEY_s{
	unsigned int key[4*(NB_ROUND+1)];
};



// openssl AES functions using Paulo Barreto <paulo.barreto@terra.com.br>
// implementation, to avoid the requirement for version 0.9.7 of libssl
void AES::set_encrypt_key( unsigned char * key, int key_nb_bits ){
  rijndaelKeySetupEnc( ((AES_KEY*)m_key)->key, key, key_nb_bits );
}

void AES::encrypt( const unsigned char * input, unsigned char * output ){
  rijndaelEncrypt( ((AES_KEY*)m_key)->key, NB_ROUND, input, output );
}

#endif	// HAVE_OPENSSL_AES_H

AES::AES():m_key(NULL){
}

AES::AES( unsigned char * key, int key_length ){
	m_key = (AES_KEY *) malloc( sizeof( AES_KEY ) );
	memset( m_key, '\0', sizeof( AES_KEY ) );;
	set_encrypt_key( key, key_length*8 );
}

AES::~AES(){
	if( m_key )
		free( m_key );
}

#endif	// HAVE_GNUTLS

void AES::get_ctr_cipher_stream( unsigned char * output, unsigned int length,
				unsigned char * iv ){
	unsigned long ctr; // Should be 128 bits, but we
	                     // assume that we 32 bits is enough ...
	unsigned char * aes_input, * temp;
	uint32_t input;
	
	aes_input = (unsigned char *)malloc( AES_BLOCK_SIZE );
	temp = (unsigned char *)malloc( AES_BLOCK_SIZE );

//	memcpy( aes_input, iv, 12 );
//	iv += 12;
        memcpy( aes_input, iv, 14 );
        iv += 14;

	for( ctr = 0; ctr < length/AES_BLOCK_SIZE; ctr++ ){
//		input = ctr + U32_AT(iv);
                input = ctr;
		//compute the cipher stream
//		aes_input[12] = (byte_t)((input & 0xFF000000) >> 24);
//		aes_input[13] = (byte_t)((input & 0x00FF0000) >> 16);
		aes_input[14] = (byte_t)((input & 0x0000FF00) >>  8);
		aes_input[15] = (byte_t)((input & 0x000000FF));

		encrypt( aes_input, &output[ctr*AES_BLOCK_SIZE] );
	}

	// Treat the last bytes:
//	input = ctr + U32_AT(iv);
        input = ctr;
//      aes_input[12] = (byte_t)((input & 0xFF000000) >> 24);
//	aes_input[13] = (byte_t)((input & 0x00FF0000) >> 16);
	aes_input[14] = (byte_t)((input & 0x0000FF00) >>  8);
	aes_input[15] = (byte_t)((input & 0x000000FF));
	
	encrypt( aes_input, temp );
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
	delete[] cipher_stream;
}

void AES::f8_encrypt(unsigned char *data, unsigned int data_length,
		     unsigned char *iv, unsigned char *origKey, unsigned int keyLen,
		     unsigned char *salt, unsigned int saltLen ){
    
    f8_encrypt(data, data_length, data, iv, origKey, keyLen, salt, saltLen);
}

void AES::f8_encrypt(unsigned char *in, unsigned int in_length, unsigned char *out,
		     unsigned char *iv, unsigned char *origKey, unsigned int keyLen,
		     unsigned char *salt, unsigned int saltLen ){


    unsigned char *saltMask;
    unsigned char *maskedKey;
    unsigned char *cp_in, *cp_in1, *cp_out;
    unsigned int i;
    int offset = 0;

    F8_CIPHER_CTX f8ctx;
   
    /*
     * Get memory for the derived IV (IV')
     */
    f8ctx.ivAccent = (unsigned char *)malloc(AES_BLOCK_SIZE);
    
    /*
     * Get memory for the special key. This is the key to compute the
     * derived IV (IV').
     */
    saltMask = (unsigned char *)malloc(keyLen);
    maskedKey = (unsigned char *)malloc(keyLen);

    /*
     * First copy the salt into the mask field, then fill with 0x55 to
     * get a full key.
     */
    memcpy(saltMask, salt, saltLen);
    memset(saltMask+saltLen, 0x55, keyLen-saltLen);
    
    /*
     * XOR the original key with the above created mask to
     * get the special key.
     */
    cp_out = maskedKey;
    cp_in = origKey;
    cp_in1 = saltMask;    
    for (i = 0; i < keyLen; i++) {
        *cp_out++ = *cp_in++ ^ *cp_in1++;
    }    
    /*
     * Prepare the a new AES cipher with the special key to compute IV'
     */
    AES *aes = new AES(maskedKey, keyLen);
    
    /*
     * Use the masked key to encrypt the original IV to produce IV'. 
     * 
     * After computing the IV' we don't need this cipher context anymore, free it.
     */
    aes->encrypt(iv, f8ctx.ivAccent);
    delete aes;

    memset(maskedKey, 0, keyLen);
    free(saltMask);
    free(maskedKey);                   // both values are no longer needed

    f8ctx.J = 0;                       // initialize the counter
    f8ctx.S = (unsigned char *)malloc(AES_BLOCK_SIZE);  // get the key stream buffer

    memset(f8ctx.S, 0, AES_BLOCK_SIZE); // initial value for key stream
 
    while (in_length >= AES_BLOCK_SIZE) {
        processBlock(&f8ctx, in+offset, AES_BLOCK_SIZE, out+offset);
        in_length -= AES_BLOCK_SIZE;
        offset += AES_BLOCK_SIZE;
    }
    if (in_length > 0) {
        processBlock(&f8ctx, in+offset, in_length, out+offset);
    }
    memset(f8ctx.ivAccent, 0, AES_BLOCK_SIZE);
    memset(f8ctx.S, 0, AES_BLOCK_SIZE);
    free(f8ctx.ivAccent);
    free(f8ctx.S);
}

int AES::processBlock(F8_CIPHER_CTX *f8ctx, unsigned char *in, int length, unsigned char *out) {

    int i;
    unsigned char *cp_in, *cp_in1, *cp_out;
    uint32_t *ui32p;

    /*
     * XOR the previous key stream with IV'
     * ( S(-1) xor IV' )
     */
    cp_in = f8ctx->ivAccent;
    cp_out = f8ctx->S;
    for (i = 0; i < AES_BLOCK_SIZE; i++) {
        *cp_out++ ^= *cp_in++;
    }
    /*
     * Now XOR (S(n-1) xor IV') with the current counter, then increment the counter
     */
    ui32p = (uint32_t *)f8ctx->S;
    ui32p[3] ^= hton32(f8ctx->J);
    f8ctx->J++;
    /*
     * Now compute the new key stream using AES encrypt
     */
    encrypt(f8ctx->S, f8ctx->S);
    /*
     * as the last step XOR the plain text with the key stream to produce 
     * the ciphertext.
     */
    cp_out = out;
    cp_in = in;
    cp_in1 = f8ctx->S;
    for (i = 0; i < length; i++) {
        *cp_out++ = *cp_in++ ^ *cp_in1++;
    }
    return length;
}


