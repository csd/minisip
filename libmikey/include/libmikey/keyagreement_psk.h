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

#ifndef KEYAGREEMENT_PSK_H
#define KEYAGREEMENT_PSK_H

#include<openssl/dh.h>
#include<openssl/rand.h>
#include<libmikey/keyagreement.h>
#include<libmikey/oakley_groups.h>

#define DH_GROUP_OAKLEY5 0
#define DH_GROUP_OAKLEY1 1
#define DH_GROUP_OAKLEY2 2


using namespace std;


class KeyAgreementPSK : public KeyAgreement{
	public:
		KeyAgreementPSK( byte_t * psk, int pskLength );
		~KeyAgreementPSK();

		void generateTgk( unsigned int tgkLength = 192 );

		void genTranspEncrKey( byte_t * encrKey, int encrKeyLength );

		void genTranspSaltKey( byte_t * encrKey, int encrKeyLength );
		
		void genTranspAuthKey( byte_t * encrKey, int encrKeyLength );

		unsigned long long tSent();
		void setTSent( unsigned long long tSent );

	private:
		byte_t * pskPtr;
		int pskLengthValue;

		unsigned long long tSentValue;
};

#endif
