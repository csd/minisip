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

#include"ILBCCODEC.h"

#include<libmutil/massert.h>

using namespace std;

static std::list<std::string> pluginList;
static int initialized;

extern "C" MILBC_API
std::list<std::string> *milbc_LTX_listPlugins( MRef<Library *> lib ){
	if( !initialized ){
		pluginList.push_back("getPlugin");
		initialized = true;
	}

	return &pluginList;
}

extern "C" MILBC_API
MPlugin * milbc_LTX_getPlugin( MRef<Library *> lib ){
	return new ILBCCodec( lib );
}


ILBCCodecState::ILBCCodecState(){
	initEncode(&enc_inst); 
	initDecode(&dec_inst, 1);
	
}

uint32_t ILBCCodecState::encode(void *in_buf, int32_t in_buf_size, void *out_buf){
	float block[160];
	//int s = in_buf_size;
	//s;//dummy op 
	
	for (int32_t i=0; i<160; i++)
		block[i]=(float)(((short*)in_buf)[i]);

	iLBC_encode((unsigned char *)out_buf, block, &enc_inst);
        return NO_OF_BYTES;
}

uint32_t ILBCCodecState::decode(void *in_buf, int32_t in_buf_size, void *out_buf){
	float decblock[BLOCKL], dtmp;
	int32_t k;
	//int s = in_buf_size;
	//s;//dummy op 

	iLBC_decode(decblock, (unsigned char*)in_buf, &dec_inst, 1);
	
	for(k=0;k<BLOCKL;k++){  
		dtmp=decblock[k]; 
		if (dtmp<MIN_SAMPLE) 
			dtmp=MIN_SAMPLE; 
		else if (dtmp>MAX_SAMPLE) 
			dtmp=MAX_SAMPLE; 
		((short*)out_buf)[k] = (short) dtmp; 
	} 

	return BLOCKL;
}


ILBCCodec::ILBCCodec( MRef<Library *> lib ): AudioCodec( lib ){
}

#if 0
void ILBCCodec::decode(void *out_buf){
	float decblock[160], dtmp;
	int32_t k;
	unsigned char dummyencoded[38];
	for (k=0; k<50; k++)
		dummyencoded[k]=0;

	iLBC_decode(decblock, (unsigned char*)dummyencoded, &dec_inst, 0);
	
	for(k=0;k<BLOCKL;k++){  
		dtmp=decblock[k]; 
		if (dtmp<MIN_SAMPLE) 
			dtmp=MIN_SAMPLE; 
		else if (dtmp>MAX_SAMPLE) 
			dtmp=MAX_SAMPLE; 
		((short*)out_buf)[k] = (short) dtmp; 
	} 
}
#endif


int32_t ILBCCodec::getSamplingSizeMs(){
	massert(BLOCKL==160);
	return 20;
}

/*
int32_t ILBCCodec::getEncodedNrBytes(){
	massert(BLOCKL==160);
	massert(38==NO_OF_BYTES);
	return 38;
}
*/

int32_t ILBCCodec::getInputNrSamples(){
	return 160;
}

string ILBCCodec::getCodecName(){
	return "iLBC";
}

string ILBCCodec::getCodecDescription(){
	return "iLBC - Internet Low Bit rate Codec, 13.33kb/s, 30ms blocks";

}

int32_t ILBCCodec::getSamplingFreq(){
	return 8000;
}


uint8_t ILBCCodec::getSdpMediaType(){
	return 97;		
}

string ILBCCodec::getSdpMediaAttributes(){
	return "iLBC/8000";
}

MRef<CodecState *> ILBCCodec::newInstance(){
	MRef<CodecState *> ret = new ILBCCodecState();
	ret->setCodec( this );
	return ret;
}

uint32_t ILBCCodec::getVersion()const{
	return 0x00000001;
}
