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

#include <assert.h>
#include<config.h>
#include<libmikey/MikeyPayloadGeneralExtension.h>

//Constructor when receiving Mikey message i.e. contruct MikeyPayloadGeneralExtensions from bytestream.
MikeyPayloadGeneralExtensions::MikeyPayloadGeneralExtensions(byte_t *start, int lengthLimit):MikeyPayload(start){
	this->payloadTypeValue = MIKEYPAYLOAD_GENERALEXTENSIONS_PAYLOAD_TYPE;
	this->nextPayloadTypeValue = start[0];
	this->type = start[1];
	this->leng = (uint16_t)start[2] << 8 | (uint16_t)start[3];
	this->data  = (byte_t*) calloc (lengthLimit - 4 ,sizeof(byte_t));
	for(int i=4; i< lengthLimit; i++)
			this->data[i] = start[i];
}
//Constructor when constructing new MikeyPayloadGeneralExtension message
MikeyPayloadGeneralExtensions::MikeyPayloadGeneralExtensions(uint8_t type, uint16_t length, byte_t * data){
	this->payloadTypeValue = MIKEYPAYLOAD_GENERALEXTENSIONS_PAYLOAD_TYPE;
	this->type = type;
	this->leng = length;
	this->data  = (byte_t*) calloc (length ,sizeof(byte_t));
	for(int i=4; i< length; i++)
			this->data[i] = data[i];
}
//Destructor
MikeyPayloadGeneralExtensions::~MikeyPayloadGeneralExtensions(){
	free(data);
}
//Return the length of the GeneralExtension in bytes
int MikeyPayloadGeneralExtensions::length(){
	return this->leng + 4;
}
//Generate bytestream of MikeyPayloadGeneralExtension
void MikeyPayloadGeneralExtensions::writeData(byte_t *start, int expectedLength){
	assert( expectedLength == this->length() );
	start[0] = this->nextPayloadTypeValue;
	start[1] = this->type;
	start[2] = (byte_t) ((this->leng & 0xFF00) >> 8);
	start[3] = (byte_t) (this->leng & 0xFF);
	for(int i= 4; i< expectedLength; i++)
		start[i] = data[i-4];
}



