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

#ifndef MIKEYPAYLOADCERT_H
#define MIKEYPAYLOADCERT_H

#include<libmikey/MikeyPayload.h>
//#include"../util/cert.h"

#define MIKEYPAYLOAD_CERT_PAYLOAD_TYPE 7

#define MIKEYPAYLOAD_CERT_TYPE_X509V3 0
#define MIKEYPAYLOAD_CERT_TYPE_X509V3URL 1
#define MIKEYPAYLOAD_CERT_TYPE_X509V3SIGN 2
#define MIKEYPAYLOAD_CERT_TYPE_X509V3ENCR 3

/**
 * @author Erik Eliasson, Johan Bilien
*/

class certificate;
class certificate_db;

class MikeyPayloadCERT : public MikeyPayload{
	public:
	
		MikeyPayloadCERT( int type, MRef<certificate *> cert );
		MikeyPayloadCERT( int type, int length, byte_t *data );
		MikeyPayloadCERT( byte_t * start, int lengthLimit );
		~MikeyPayloadCERT();

		virtual void writeData( byte_t * start, int expectedLength );
		virtual int length();
		virtual std::string debugDump();

		byte_t * certData();
		int certLength();

	private:
		int type;
		int certLengthValue;
		byte_t * certDataPtr;
		
};


#endif
