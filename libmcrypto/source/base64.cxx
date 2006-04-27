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

#include <libmcrypto/base64.h>
#define LINE_LENGTH 60


using namespace std;

#include<iostream>

#ifdef HAVE_MALLOC_H
#include<malloc.h>
#endif

LIBMCRYPTO_API string base64_encode( unsigned char * input, int length )
{
	int counter = 0;
	int i;
	int length_complete_blocks = length - length%3;
	string output = "";
	char alphabet[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	

	for( i = 0; i < length_complete_blocks ; i+=3 )
	{
		counter +=4;
		output += alphabet[ (input[i] >> 2) & 0x3F];
		output += alphabet[ ( (input[i] << 4) | (input[i+1] >> 4) ) & 0x3F];
		output += alphabet[ ( (input[i+1] << 2 ) | (input[i+2] >> 6) ) & 0x3F];
		output += alphabet[ input[i+2] & 0x3F ];
	}

	switch( length % 3) {
		case 0:
			return output;
		case 1:
			output += alphabet[ (input[length -1] >> 2) & 0x3F];
			output += alphabet[ (input[length -1 ] << 4) & 0x3F];
			output += "==";
			return output;
		case 2:
			output += alphabet[ (input[length -2] >> 2) & 0x3F];
			output += alphabet[ ( (input[length - 2] << 4) | 
					      (input[length - 1] >> 4) ) & 0x3F];
			output += alphabet[ (input[length - 1] << 2 ) & 0x3F];
			output += '=';
			return output;
	}
	return "";
}

LIBMCRYPTO_API unsigned char *  base64_decode( string input, int * output_length )
{
	string input_cpy = input;
	size_t pos;
	uint32_t i;
	size_t nb_equals;
	unsigned char * output;
	unsigned char * p_cursor;
#define XX 255
	const unsigned char ialphabet[256] = {
   		XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
		XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
		XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,62, XX,XX,XX,63,
		52,53,54,55, 56,57,58,59, 60,61,XX,XX, XX,XX,XX,XX,
		XX, 0, 1, 2,  3, 4, 5, 6,  7, 8, 9,10, 11,12,13,14,
		15,16,17,18, 19,20,21,22, 23,24,25,XX, XX,XX,XX,XX,
		XX,26,27,28, 29,30,31,32, 33,34,35,36, 37,38,39,40,
		41,42,43,44, 45,46,47,48, 49,50,51,XX, XX,XX,XX,XX,
		XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
		XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
		XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
		XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
		XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
		XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
		XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
		XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	};
#undef XX


	while( ( pos = input_cpy.find( "\n" ) ) != string::npos )
	{
		input_cpy.erase( pos, 1 );
	}
	
	while( ( pos = input_cpy.find( "\r" ) ) != string::npos )
	{
		input_cpy.erase( pos, 1 );
	}
	
	if( input_cpy.length() % 4 )
	{
#ifdef DEBUG_OUTPUT
		cerr << "Invalid base64 input" << endl;
#endif

		return NULL;
	}

	pos = input_cpy.find( "=" );

	nb_equals = (pos == string::npos)?0:input_cpy.length() - pos;

	switch( nb_equals ) {
		case 0:
			*output_length = (int) (3 * input_cpy.length() / 4);
			break;
		case 1:
			*output_length = (int) (3 * ( input_cpy.length() / 4 - 1 ) + 2);
			break;
		case 2:
			*output_length = (int) (3 * ( input_cpy.length() / 4 - 1 ) + 1);
			break;
		default:
#ifdef DEBUG_OUTPUT
			cerr << "Invalid base64 input" << endl;
#endif

			return NULL;
	}

	output = new unsigned char[*output_length * sizeof( unsigned char * )];
	
	if( output == NULL )
	{


		cerr << "Not enough memory to allocate output" << endl;

		return NULL;
	}
	
	p_cursor = output;

	for( i = 0; i < input_cpy.length() - 4; i+=4 )
	{
		if( ialphabet[ (unsigned int)input_cpy[i] ] == 255 || ialphabet[(unsigned int)input_cpy[i+1] ] == 255 ||
				ialphabet[ (unsigned int)(input_cpy[i+2]) ] == 255 )
		{
			cerr << "Invalid base64 input" << endl;

			return NULL;
		}
		//printf( "%i ", ialphabet[ *i ] );
		//printf( "%i ", ialphabet[ *(i+1) ] );
		*(p_cursor++) = ialphabet[ (unsigned int)input_cpy[i] ] << 2 | ialphabet[ (unsigned int)input_cpy[i+1] ] >> 4;
		//printf( "\n%02x\n", *(p_cursor-1) );
		*(p_cursor++) = ialphabet[ (unsigned int)input_cpy[i+1] ] << 4 | ialphabet[ (unsigned int)input_cpy[i+2] ] >> 2;
		*(p_cursor++) = ialphabet[ (unsigned int)input_cpy[i+2] ] << 6 | ialphabet[ (unsigned int)input_cpy[i+3] ];
	}

	switch( nb_equals ) {
		case 0:
			if( ialphabet[ (unsigned int)input_cpy[i] ] == 255 
			     || ialphabet[ (unsigned int)input_cpy[i+1] ] == 255 ||
				ialphabet[ (unsigned int)input_cpy[i+2] ] == 255 )
			{
				cerr << "Invalid base64 input" << endl;

				return NULL;
			}

			*(p_cursor++) = ialphabet[ (unsigned int)input_cpy[i] ] << 2 | ialphabet[ (unsigned int)input_cpy[i+1] ] >> 4;
			*(p_cursor++) = ialphabet[ (unsigned int)input_cpy[i+1] ] << 4 | ialphabet[ (unsigned int)input_cpy[i+2] ] >> 2;
			*p_cursor = ialphabet[ (unsigned int)input_cpy[i+2] ] << 6 | ialphabet[ (unsigned int)input_cpy[i+3] ];
			return output;
		case 1:
			if( ialphabet[ (unsigned int)input_cpy[i] ] == 255 || ialphabet[ (unsigned int)input_cpy[i+1] ] == 255 )
			{
				cerr << "Invalid base64 input" << endl;

				return NULL;
			}

			*(p_cursor++) = ialphabet[ (unsigned int)input_cpy[i] ] << 2 | ialphabet[ (unsigned int)input_cpy[i+1] ] >> 4;
			*p_cursor = ialphabet[ (unsigned int)input_cpy[i+1] ] << 4 | ialphabet[ (unsigned int)input_cpy[i+2] ] >> 2;
			return output;
		case 2:
			if( ialphabet[ (unsigned int)input_cpy[i] ] == 255 )
			{
				cerr << "Invalid base64 input" << endl;

				return NULL;
			}

			*p_cursor = ialphabet[ (unsigned int)input_cpy[i] ] << 2 | ialphabet[ (unsigned int)input_cpy[i+1] ] >> 4;
			return output;
	}

	return NULL;
}

LIBMCRYPTO_API unsigned char * base64_decode( unsigned char * input, int input_length,
		   int * output_length ){
	string s( (char *)input, input_length );

	return base64_decode( s, output_length );
}
