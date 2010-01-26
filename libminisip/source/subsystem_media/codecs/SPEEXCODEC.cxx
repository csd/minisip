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

#include"SPEEXCODEC.h"

#include<iostream>

#include<speex/speex.h>

using namespace std;

static std::list<std::string> pluginList;
static int initialized;

extern "C" LIBMINISIP_API
std::list<std::string> *mspeex_LTX_listPlugins( MRef<Library *> lib ){
	if( !initialized ){
		pluginList.push_back("getPlugin");
		initialized = true;
	}

	return &pluginList;
}

extern "C" LIBMINISIP_API
MPlugin * mspeex_LTX_getPlugin( MRef<Library *> lib ){
	return new SpeexCodec( lib );
}


SpeexCodecState::SpeexCodecState(){
	
	speex_bits_init(&bits);  // both for encode and decode
	
	//  Encoder Initialization
	

	const SpeexMode *speex_mode;
	speex_mode = speex_lib_get_mode(SPEEX_MODEID_WB);
	// As a start, just use the narrow-band mode is implemented
	enc_state = speex_encoder_init(speex_mode); 

	massert(enc_state);
	
	// This will hand in the frame size used by the encoder


	int ok;
#if 1
	int sample_rate=16000;
	ok = speex_decoder_ctl(enc_state, SPEEX_SET_SAMPLING_RATE, &sample_rate);
	massert(ok>=0);

	ok = speex_encoder_ctl(enc_state,SPEEX_GET_FRAME_SIZE,&frame_size);
	massert(ok>=0);
	//cerr <<"EEEE: SPEEX encoder frame_size="<<frame_size<<endl;

	int bitrate;
	speex_encoder_ctl(enc_state, SPEEX_GET_BITRATE, &bitrate);
	//cerr <<"EEEE: SPEEX: bitrate="<<bitrate<<endl;

#endif	


	// Decoder Initialization
	
	// As a start, just use the narrow-band mode is implemented
	dec_state = speex_decoder_init(speex_mode); 
	massert(dec_state);

#if 1
	ok = speex_decoder_ctl(dec_state, SPEEX_SET_SAMPLING_RATE, &sample_rate);
	massert(ok>=0);
	
	// This will hand in the frame size used by the decoder  
	ok=speex_decoder_ctl(dec_state, SPEEX_GET_FRAME_SIZE, &frame_size);
	massert(ok>=0);
	//cerr <<"EEEE: SPEEX decoder frame_size="<<frame_size<<endl;
#endif

//	output_frame = new float[320];
	
}

SpeexCodecState::~SpeexCodecState(){

	speex_bits_destroy(&bits);
	speex_encoder_destroy(enc_state);
	speex_decoder_destroy(dec_state);
	//delete [] output_frame;
}


uint32_t SpeexCodecState::encode(void *in_buf, int32_t in_buf_size, int samplerate, void *out_buf){


//	for (int i=0; i< in_buf_size; i++){
//		input_frame[i]= (float) ((short*)in_buf)[i];
//	}
	
	
//	cerr <<"EEEE: Speex: in_buf_size="<<in_buf_size<<endl;
	//  now for every input frame:
	speex_bits_reset(&bits);
//	cerr <<"EEEE: doing speex_encode_int"<<endl;
	speex_encode_int(enc_state, (short*)in_buf, &bits);
//	cerr <<"EEEE: done doing speex_encode_int"<<endl;
	// returns the number of bytes that need to be written
	//int bNum = speex_bits_nbytes(&bits); 
	nbBytes = speex_bits_write(&bits, (char*)out_buf,/* MAX_NB_BYTES*/ 640);
	
	return nbBytes;
}

uint32_t SpeexCodecState::decode(void *in_buf, int32_t in_buf_size, void *out_buf){


	//cerr <<"EEEE: SPEEX decode running;"<<endl;
	input_bytes = (char *) in_buf;  // should in_buf also be changed to short (as in encode function)?  If so, then then you should have a for loop here
	//nbBytes = (int) in_buf_size;


	// for every input frame:
	speex_bits_read_from(&bits, input_bytes, in_buf_size);
	//speex_decode(dec_state, &bits, output_frame);
	speex_decode_int(dec_state, &bits, (short*)out_buf);
	
	return 320;
}

SpeexCodec::SpeexCodec( MRef<Library *> lib ): AudioCodec( lib ){
}

int32_t SpeexCodec::getSamplingSizeMs(){
	return 20;
}

int32_t SpeexCodec::getSamplingFreq(){
	return 16000;
}


/*
int32_t SpeexCodec::getEncodedNrBytes(){
	return 160;
}
*/


int32_t SpeexCodec::getInputNrSamples(){
	return 320;
}

string SpeexCodec::getCodecName(){
	return "speex";
}

string SpeexCodec::getCodecDescription(){
	return "SPEEX 16kHz, Speex";
}

uint8_t SpeexCodec::getSdpMediaType(){
	return 119;  
	// Speex uses Dynamic Payload Type, meaning that there isn't a fixed assigned 
	// payload type number for it.  So, we use an agreed number in minisip 
	// for speex's payload type (114).
}

string SpeexCodec::getSdpMediaAttributes(){
	return "speex/16000/1";
	//     <encoding_name>/<clock_rate>/<number_of_channels(for audio streams)>
	//     Here we use the narrow-band (8000) using only one channel (non-sterio)
}

MRef<CodecState *> SpeexCodec::newInstance(){
	MRef<CodecState *> ret =  new SpeexCodecState();
	ret->setCodec( this );
	return ret;
}

uint32_t SpeexCodec::getVersion()const{
	return 0x00000001;
}

