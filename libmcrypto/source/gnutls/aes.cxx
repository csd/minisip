/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
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
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/


#include<config.h>

#include <libmcrypto/aes.h>
#include <libmutil/Exception.h>
#include <gcrypt.h>
#include <string>
#include <sstream>

#define AES_BLOCK_SIZE 16

using namespace std;

AES::AES( unsigned char * key, int key_length ){
	gcry_error_t err;
	int algo;

	switch( key_length ){
		case 16:
			algo = GCRY_CIPHER_AES;
			break;
		case 24:
			algo = GCRY_CIPHER_AES192;
			break;
		case 32:
			algo = GCRY_CIPHER_AES256;
			break;
		default: {
			ostringstream msg;
			msg << "Unsupported AES key length: " << key_length;
			throw Exception( msg.str().c_str() );
		}
	}

	err = gcry_cipher_open( (gcry_cipher_hd_t*)&m_key, algo,
				GCRY_CIPHER_MODE_ECB, 0 );
	set_encrypt_key( key, key_length * 8 );
}

AES::~AES(){
	if( m_key )
		gcry_cipher_close( (gcry_cipher_hd_t)m_key );
}

void AES::set_encrypt_key( unsigned char *key, int key_nb_bits ){
	gcry_error_t err;

	err = gcry_cipher_setkey( (gcry_cipher_hd_t)m_key,
				  key, key_nb_bits / 8 );
}

void AES::encrypt( const unsigned char * input, unsigned char * output ){
	gcry_error_t err;

	err = gcry_cipher_encrypt( (gcry_cipher_hd_t)m_key, output,
				   AES_BLOCK_SIZE, input, AES_BLOCK_SIZE );
}
