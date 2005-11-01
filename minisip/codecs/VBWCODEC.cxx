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

#include<config.h>

#include"VBWCODEC.h"
#include<assert.h>
#include<iostream>

VBWCodec::VBWCodec(){

}

VBWCodec::~VBWCodec(){

}

VBWCodecState::VBWCodecState(){
	resampler = Resampler::create( 48000, 8000, 20, 1 /*Nb channels */);
}

uint32_t VBWCodecState::encode(void *in_buf, int32_t in_buf_size, void *out_buf){
	assert(in_buf_size==2*960);
	
	short *in_data = (short*)in_buf;
	unsigned char *out_data = (unsigned char*)out_buf;

	for (int32_t i=0; i< in_buf_size; i++)
		out_data[i]=in_data[i];
	
	// pn430 Added to account for change in return value
	return in_buf_size;
}

uint32_t VBWCodecState::decode(void *in_buf, int32_t in_buf_size, void *out_buf){
//	assert(in_buf_size==getEncodedNrBytes());
	
	unsigned char *in_data = (unsigned char*)in_buf;
	short *out_data = (short*)out_buf;
	
	for (int32_t i=0; i< in_buf_size; i++)
		out_data[i]=in_data[i];

	return in_buf_size;
}

int32_t VBWCodec::getSamplingSizeMs(){
	return 20;
}

int32_t VBWCodec::getSamplingFreq(){
	return 48000;
}

/*
int32_t VBWCodec::getEncodedNrBytes(){
	return -1;
}
*/

int32_t VBWCodec::getInputNrSamples(){
	return 960;
}

string VBWCodec::getCodecName(){
	return "VBW";
}

string VBWCodec::getCodecDescription(){
	return "VBW 0-192kbps raw";
}

uint8_t VBWCodec::getSdpMediaType(){
	return 0;		
}

string VBWCodec::getSdpMediaAttributes(){
	return "VBW";
}

MRef<CodecState *> VBWCodec::newInstance(){
	MRef<CodecState *> ret = new VBWCodecState();
	ret->setCodec( this );
	return ret;
}
