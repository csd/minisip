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
 *	    Joachim Orrblad <joachim@orrblad.com>
*/

#ifndef MIKEYPAYLOADGENERALEXTENSIONS_H
#define MIKEYPAYLOADGENERALEXTENSIONS_H

#include<libmikey/MikeyPayload.h>

#define MIKEYPAYLOAD_GENERALEXTENSIONS_PAYLOAD_TYPE 21

#define MIKEY_EXT_TYPE_VENDOR_ID	0 //Vendor specific byte string
#define MIKEY_EXT_TYPE_SDP_ID		1 //List of SDP key mgmt IDs (allocated for use in [KMASDP])
    


class MikeyPayloadGeneralExtensions : public MikeyPayload{
	public:
		//Constructor when receiving Mikey message i.e. contruct MikeyPayloadGeneralExtensions from bytestream.
		MikeyPayloadGeneralExtensions(byte_t *start_of_header, int lengthLimit);
		//Constructor when constructing new MikeyPayloadGeneralExtension message
		MikeyPayloadGeneralExtensions(uint8_t type, uint16_t length, byte_t * data);
		//Destructor
		~MikeyPayloadGeneralExtensions();
		//Generate bytestream of MikeyPayloadGeneralExtension
		virtual void writeData(byte_t *start, int expectedLength);
		//Return the length of the GeneralExtension in bytes
		virtual int length();								
		uint8_t type;
		uint16_t leng;
		byte_t * data;

	private:
		
};


#endif
