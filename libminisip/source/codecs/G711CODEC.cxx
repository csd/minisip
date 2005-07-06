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


/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<config.h>

#include"G711CODEC.h"
#include"g711/codec_g711.h"
#include<assert.h>
#include<iostream>

G711CODEC::G711CODEC(){

}

G711CODEC::~G711CODEC(){

}

// pn430 return type changed from void to uint32_t
uint32_t G711CODEC::encode(void *in_buf, int32_t in_buf_size, void *out_buf){
	assert(in_buf_size==2*getInputNrSamples());
	
	short *in_data = (short*)in_buf;
	unsigned char *out_data = (unsigned char*)out_buf;

	for (int32_t i=0; i< getInputNrSamples(); i++)
		out_data[i]=linear2ulaw(in_data[i]);
	
	// pn430 Added to account for change in return value
	return getEncodedNrBytes();
}

uint32_t G711CODEC::decode(void *in_buf, int32_t in_buf_size, void *out_buf){
//	assert(in_buf_size==getEncodedNrBytes());
	
	unsigned char *in_data = (unsigned char*)in_buf;
	short *out_data = (short*)out_buf;
	
	for (int32_t i=0; i< in_buf_size; i++)
		out_data[i]=ulaw2linear(in_data[i]);

	return in_buf_size;
}

void G711CODEC::decode(void *out_buf){
	//FIX: implement packet loss concealment
//	cerr << "PLC"<< endl;
	for (int32_t i=0; i< getInputNrSamples(); i++)
		((short*)out_buf)[i]=rand()%25;
}

int32_t G711CODEC::getSamplingSizeMs(){
	return 20;
}

int32_t G711CODEC::getSamplingFreq(){
	return 8000;
}

int32_t G711CODEC::getEncodedNrBytes(){
	return 160;
}

int32_t G711CODEC::getInputNrSamples(){
	return 160;
}

string G711CODEC::getCodecName(){
	return "G.711";
}

string G711CODEC::getCodecDescription(){
	return "G.711 8kHz, PCMu";
}

int32_t G711CODEC::getSdpMediaType(){
	return 0;		
}

string G711CODEC::getSdpMediaAttributes(){
	return "PCMU/8000/1";
}
