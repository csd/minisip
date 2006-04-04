/*
 Copyright (C) 2004-2006 the Minisip Team
 
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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<config.h>

#include<libminisip/codecs/G711CODEC.h>

#include<libminisip/codecs/g711/codec_g711.h>

#include<libmutil/massert.h>
#include<iostream>

using namespace std;

G711Codec::G711Codec(){

}

G711Codec::~G711Codec(){

}

uint32_t G711CodecState::encode(void *in_buf, int32_t in_buf_size, void *out_buf){
	massert(in_buf_size==2*160);
	
	short *in_data = (short*)in_buf;
	unsigned char *out_data = (unsigned char*)out_buf;

	for (int32_t i=0; i< 160; i++)
		out_data[i]=linear2ulaw(in_data[i]);
	
	// pn430 Added to account for change in return value
	return 160;
}

uint32_t G711CodecState::decode(void *in_buf, int32_t in_buf_size, void *out_buf){
//	massert(in_buf_size==getEncodedNrBytes());
	
	unsigned char *in_data = (unsigned char*)in_buf;
	short *out_data = (short*)out_buf;
	
	for (int32_t i=0; i< in_buf_size; i++)
		out_data[i]=ulaw2linear(in_data[i]);

	return in_buf_size;
}

int32_t G711Codec::getSamplingSizeMs(){
	return 20;
}

int32_t G711Codec::getSamplingFreq(){
	return 8000;
}

/*
int32_t G711Codec::getEncodedNrBytes(){
	return 160;
}
*/

int32_t G711Codec::getInputNrSamples(){
	return 160;
}

string G711Codec::getCodecName(){
	return "G.711";
}

string G711Codec::getCodecDescription(){
	return "G.711 8kHz, PCMu";
}

uint8_t G711Codec::getSdpMediaType(){
	return 0;		
}

string G711Codec::getSdpMediaAttributes(){
	return "PCMU/8000/1";
}

MRef<CodecState *> G711Codec::newInstance(){
	MRef<CodecState *> ret = new G711CodecState();
	ret->setCodec( this );
	return ret;
}
