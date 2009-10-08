/*
 Copyright (C) 2004-2007 the Minisip Team
 
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

/* Copyright (C) 2004-2007
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/

#include<config.h>

#include"G711CODEC.h"

#include"g711/codec_g711.h"

#include<libmutil/massert.h>
#include<iostream>

using namespace std;

static std::list<std::string> pluginList;
static int initialized;

extern "C"
std::list<std::string> *mg711_LTX_listPlugins( MRef<Library *> lib ){
	if( !initialized ){
		pluginList.push_back("getPlugin");
		pluginList.push_back("getPluginG711a");
		initialized = true;
	}

	return &pluginList;
}

extern "C"
MPlugin * mg711_LTX_getPlugin( MRef<Library *> lib ){
	return new G711Codec( lib, G711U );
}

extern "C"
MPlugin * mg711_LTX_getPluginG711a( MRef<Library *> lib ){
	return new G711Codec( lib, G711A );
}

G711Codec::G711Codec( MRef<Library *> lib, G711Version v): AudioCodec( lib ), version( v ){

}

G711Codec::~G711Codec(){

}


G711CodecState::G711CodecState( G711Version v): version( v ){
}

uint32_t G711CodecState::encode(void *in_buf, int32_t in_buf_size, int samplerate, void *out_buf){
	massert(in_buf_size==2*160);
	
	short *in_data = (short*)in_buf;
	unsigned char *out_data = (unsigned char*)out_buf;

	for (int32_t i=0; i< 160; i++){
		if( version == G711A )
			out_data[i]=linear2alaw(in_data[i]);
		else
			out_data[i]=linear2ulaw(in_data[i]);
	}
	
	// pn430 Added to account for change in return value
	return 160;
}

uint32_t G711CodecState::decode(void *in_buf, int32_t in_buf_size, void *out_buf){
//	massert(in_buf_size==getEncodedNrBytes());
	
	unsigned char *in_data = (unsigned char*)in_buf;
	short *out_data = (short*)out_buf;
	
	for (int32_t i=0; i< in_buf_size; i++){
		if( version == G711A )
			out_data[i]=alaw2linear(in_data[i]);
		else
			out_data[i]=ulaw2linear(in_data[i]);
	}

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
	if( version == G711A )
		return "G.711a";
	else
		return "G.711";
}

string G711Codec::getCodecDescription(){
	if( version == G711A )
		return "G.711 8kHz, PCMa";
	else
		return "G.711 8kHz, PCMu";
}

uint8_t G711Codec::getSdpMediaType(){
	if( version == G711A )
		return 8;
	else
		return 0;
}

string G711Codec::getSdpMediaAttributes(){
	if( version == G711A )
		return "PCMA/8000/1";
	else
		return "PCMU/8000/1";
}

MRef<CodecState *> G711Codec::newInstance(){
	MRef<CodecState *> ret = new G711CodecState( version );
	ret->setCodec( this );
	return ret;
}

uint32_t G711Codec::getVersion()const{
	return 0x00000001;
}
