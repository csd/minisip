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

#include"ILBCCODEC.h"
#include<assert.h>

ILBCCODEC::ILBCCODEC(){
	initEncode(&enc_inst); 
	initDecode(&dec_inst, 1);
	
}

void ILBCCODEC::encode(void *in_buf, int32_t in_buf_size, void *out_buf){
	float block[240];
	
	for (int32_t i=0; i<getInputNrSamples(); i++)
		block[i]=(float)(((short*)in_buf)[i]);

	iLBC_encode((unsigned char *)out_buf, block, &enc_inst);
}

void ILBCCODEC::decode(void *in_buf, int32_t in_buf_size, void *out_buf){
	float decblock[240], dtmp;
	int32_t k;

	iLBC_decode(decblock, (unsigned char*)in_buf, &dec_inst, 1);
	
	for(k=0;k<BLOCKL;k++){  
		dtmp=decblock[k]; 
		if (dtmp<MIN_SAMPLE) 
			dtmp=MIN_SAMPLE; 
		else if (dtmp>MAX_SAMPLE) 
			dtmp=MAX_SAMPLE; 
		((short*)out_buf)[k] = (short) dtmp; 
	} 
}

void ILBCCODEC::decode(void *out_buf){
	float decblock[240], dtmp;
	int32_t k;
	unsigned char dummyencoded[50];
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


int32_t ILBCCODEC::getSamplingSizeMs(){
	assert(BLOCKL==240);
	return 30;
}

int32_t ILBCCODEC::getEncodedNrBytes(){
	assert(BLOCKL==240);
	assert(50==NO_OF_BYTES);
	return 50;
}


int32_t ILBCCODEC::getInputNrSamples(){
	return 240;
}

string ILBCCODEC::getCodecName(){
	return "iLBC";
}

string ILBCCODEC::getCodecDescription(){
	return "iLBC - Internet Low Bit rate Codec, 13.33kb/s, 30ms blocks";

}

int32_t ILBCCODEC::getSamplingFreq(){
	return 8000;
}


int32_t ILBCCODEC::getSdpMediaType(){
	return 97;		
}

string ILBCCODEC::getSdpMediaAttributes(){
	return "iLBC";
}
