/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include"../cert.h"

#define CERT_FILE "cert.pem"
#define KEY_FILE "privkey.pem"

int main( void ){

	int sign_length;
	unsigned char * p_sign;
	char p_data[] = "I owe you...";
	int data_length = strlen( p_data );
	int err;
	certificate * certif = new certificate( CERT_FILE, KEY_FILE );

	p_sign = ( unsigned char * )malloc( 4096 );
	sign_length = sizeof( p_sign );
	if( certif->sign_data((unsigned char *)p_data, data_length, p_sign, &sign_length ) )
		return 1;


	if( ( err = certif->verif_sign( p_sign, sign_length, (unsigned char *)p_data, data_length )) < 0 )
		return 1;

	if( err )
		printf( "Signature verified\n" );
	else
		printf( "Signature invalid!\n" );

	delete certif;
	return 0;
}
	
