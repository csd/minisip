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

/* Copyright (C) 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<config.h>

#include"GsmCodec.h"

#include<gsm.h>

#define GSM_EXEPECTED_INPUT 160
#define GSM_FRAME_SIZE 33


static std::list<std::string> pluginList;
static int initialized;

extern "C" LIBMINISIP_API
std::list<std::string> *mgsm_LTX_listPlugins( MRef<Library *> lib ){
	if( !initialized ){
		pluginList.push_back("getPlugin");
		initialized = true;
	}

	return &pluginList;
}

extern "C" LIBMINISIP_API
MPlugin * mgsm_LTX_getPlugin( MRef<Library *> lib ){
	return new GsmCodec( lib );
}

GsmCodec::GsmCodec( MRef<Library *> lib ): AudioCodec( lib ){
}

std::string GsmCodec::getCodecName(){
	return "GSM";
}

std::string GsmCodec::getCodecDescription(){
	return "GSM CODEC (13.2kb/s)";
}

uint8_t GsmCodec::getSdpMediaType(){
	return 3;
}

std::string GsmCodec::getSdpMediaAttributes(){
	return "GSM/8000";
}

/*
int32_t GsmCodec::getEncodedNrBytes(){
	return GSM_FRAME_SIZE;
}
*/

int32_t GsmCodec::getInputNrSamples(){
	return GSM_EXEPECTED_INPUT;
}

int32_t GsmCodec::getSamplingFreq(){
	return 8000;
}

int32_t GsmCodec::getSamplingSizeMs(){
	return 20;
}

GsmCodecState::GsmCodecState(){
	gsmState = gsm_create();
}

GsmCodecState::~GsmCodecState(){
	gsm_destroy( gsmState );
}

uint32_t GsmCodecState::encode( void *inBuf, int32_t inSize, int samplerate, void *outBuf ){
	if( inSize != GSM_EXEPECTED_INPUT * sizeof( short ) ){
		return 0;
	}

	gsm_encode( gsmState, (gsm_signal *)inBuf, (gsm_byte *)outBuf );

	return GSM_FRAME_SIZE;
}

uint32_t GsmCodecState::decode( void *inBuf, int32_t inSize, void *outBuf ){
	if( inSize != GSM_FRAME_SIZE ){
		return 0;
	}

	if( gsm_decode( gsmState, (gsm_byte *)inBuf, (gsm_signal *)outBuf ) < 0 ){
		return 0;
	}

	return GSM_EXEPECTED_INPUT;
}

MRef<CodecState *> GsmCodec::newInstance(){
	MRef<CodecState *> ret =  new GsmCodecState();
	ret->setCodec( this );
	return ret;
}

uint32_t GsmCodec::getVersion()const{
	return 0x00000001;
}
