/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien, Joachim Orrblad
  
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


#include<stdint.h>

#include<libmutil/cert.h>


//typedef byte_t uint8_t;
typedef uint8_t byte_t;

/* HMAC-SHA1 MAC computation */
void hmac_sha1( unsigned char * key, unsigned int key_length,
		unsigned char * data, unsigned int data_length,
		unsigned char * mac, unsigned int * mac_length );

/* base64 encoding and decoding functions */
std::string base64_encode( unsigned char *, int );
unsigned char * base64_decode( std::string, int * );
unsigned char * base64_decode( unsigned char *, int, int * );

/* prints out a buffer in hexa */
std::string print_hex( unsigned char * data, int length );

/* Integer to string */
std::string itoa(int i);
