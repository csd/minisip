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

#include<libminisip/media/soundcard/FileSoundSource.h>
#include<fstream>
#include<string.h>

using namespace std;

FileSoundSource::FileSoundSource(string callId, string filename, 
					uint32_t id, 
					uint32_t inputFreq,
					uint32_t inputNChannels,
					uint32_t outputFreq,
					uint32_t outputDurationMs,
					uint32_t outputNChannels,
					bool rep):
					SoundSource( id, callId ), 
					enabled(false),
					repeat(rep),
					index(0) {
	short * input;
	long l,m;
	
	ifstream file (filename.c_str(), ios::in|ios::binary);
	
	l = file.tellg();
	file.seekg (0, ios::end);
	m = file.tellg();
	
	nSamples = (m-l)/(sizeof(short)*inputNChannels);
	cerr << "FileSoundSource: nSample: " << nSamples << endl;

	nOutputFrames = ( outputDurationMs * inputFreq ) / 1000;
	cerr << "FileSoundSource: nOutputFrames: " << nOutputFrames << endl;
	
	input = new short[nSamples*inputNChannels];
	
	file.seekg (0, ios::beg);
	file.read( (char*)input, nSamples*sizeof(short));

	if( inputNChannels > outputNChannels ){
		audio = new short[nSamples*outputNChannels];
		if( inputNChannels % outputNChannels == 0 ){
			/* do some mixing */
			for( uint32_t sample = 0; sample < nSamples; sample++ ){
				for( uint32_t oChannel = 0; oChannel < outputNChannels; oChannel++ ){
					for( uint32_t i = 0; i < inputNChannels/outputNChannels; i++ ){
						audio[sample*outputNChannels+oChannel] += input[sample*inputNChannels + oChannel + i*inputNChannels];
					}
					audio[sample*outputNChannels+oChannel] /= inputNChannels/outputNChannels;
				}
			}
		}
		else{
			/* Just take the first channels */
			for( uint32_t sample = 0; sample < nSamples; sample++ ){
				for( uint32_t oChannel = 0; oChannel < outputNChannels; oChannel++ ){
					audio[sample*outputNChannels+oChannel] = input[sample*inputNChannels+oChannel];
				}
			}
		}
		delete [] input;
	}
	else if( inputNChannels == outputNChannels ){
		audio = input;
	}
	else{
		audio = new short[nSamples*outputNChannels];
		for( uint32_t sample = 0; sample < nSamples; sample++ ){
			for( uint32_t oChannel = 0; oChannel < outputNChannels; oChannel++ ){
				audio[sample*outputNChannels+oChannel] = input[sample*inputNChannels];
			}
		}
		delete [] input;
	}

	nChannels = outputNChannels;

	resampler = ResamplerRegistry::getInstance()->create( inputFreq, outputFreq, outputDurationMs, outputNChannels );

	cerr << "After constructor " << nOutputFrames << endl;
}


FileSoundSource::FileSoundSource(string callid, short *rawaudio, 
					int samples, 
					bool rep):
							SoundSource(0,callid),
							audio(rawaudio),
							nSamples(samples),
							enabled(false),
							repeat(rep),
							index(0)
{

}

FileSoundSource::~FileSoundSource(){
	delete [] audio;
	audio=NULL;
}


void FileSoundSource::enable(){
	enabled=true;
	index=0;
}

void FileSoundSource::disable(){
	enabled=false;
	index=0;
}


void FileSoundSource::pushSound(short *,
				int32_t ,
				int32_t ,
				int ,
				bool )
{
#ifdef DEBUG_OUTPUT
	cerr << "WARNING: FileSoundSource::push_sound: FORBIDDEN"<< endl;
#endif
}


void FileSoundSource::getSound( short *dest, bool dequeue ){
	if( index + nOutputFrames >= nSamples ){
		if (repeat)
			index = 0;
		else
			index = nSamples;
	}

	if( (uint32_t)index == nSamples ){
		memset( dest, '\0', nOutputFrames*nChannels*sizeof(short) );
	}
	else{
		resampler->resample( audio + index, dest );
//                cerr << "audio + index: "  << print_hex( (unsigned char *)(audio + index), nOutputFrames*nChannels) << endl;
//                cerr << "dest: "  << print_hex( (unsigned char *)dest, nOutputFrames*nChannels) << endl;
		
		if (dequeue){
			index += nOutputFrames;
		}
	}
}


