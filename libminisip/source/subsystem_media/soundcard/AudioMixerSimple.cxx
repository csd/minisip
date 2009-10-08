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

/* Copyright (C) 2004-5
 *
 * Authors: Cesc Santasusana <cesc dot santa at gmail dot com>
*/

/* Name
 * 	AudioMixerSimple.h
 * Author
 * 	Cesc Santasusana <cesc dot santa at gmail dot com>
 * Purpose
 * 	Simple Audio Mixer implementation (mix all together and normalize)
*/

#include<config.h>

#include<libminisip/media/soundcard/AudioMixerSimple.h>
#include<libminisip/media/soundcard/SoundSource.h>

#include<string.h>
#include<stdlib.h> //abs()

using namespace std;

AudioMixerSimple::AudioMixerSimple() {
}

AudioMixerSimple::~AudioMixerSimple() {
}


bool AudioMixerSimple::init( uint32_t numChannels_ ) {
	
	AudioMixer::init( numChannels_ );
	
	normalizeFactor = 32;
	
	return true;
}

short * AudioMixerSimple::mix (list<MRef<SoundSource *> > sources) {
	
	uint32_t size = frameSize * numChannels;

	memset( mixBuffer, '\0', size * sizeof( int32_t ) );
	
	for (list<MRef<SoundSource *> >::iterator 
			i = sources.begin(); 
			i != sources.end(); i++){

		(*i)->getSound( inputBuffer );

		for (uint32_t j=0; j<size; j++){
#ifdef IPAQ
			/* iPAQ hack, to reduce the volume of the
				* output */
			mixBuffer[j]+=(inputBuffer[j]/32);
#else
			mixBuffer[j]+=inputBuffer[j];
#endif
		}
	}
	//mix buffer is 32 bit to prevent saturation ... 
	// normalize, if needed, to prevent it
	normalize( size );
	return outputBuffer;
}


bool AudioMixerSimple::setSourcesPosition( list<MRef<SoundSource *> > &sources, bool addingSource) {
	return true;
}

bool AudioMixerSimple::normalize( int32_t length) {
	bool ret = false;
	if( numChannels < 1 ) 
		ret = false;
	else if (numChannels == 1 ) {
		ret = normalizeMono( length );
	} else if( numChannels == 2 ) {
		ret = normalizeStereo( length );
	} else {
		ret = normalizeMulti( length );
	}
	return ret;
}

bool AudioMixerSimple::normalizeMono( int32_t length ) {
	//we need running pointers ...
	short * outbuff = outputBuffer;
	int32_t * inbuff = mixBuffer;
	
	//indicates the end ...
	short * end = outbuff + length;
	
	if( normalizeFactor < 64 )
		normalizeFactor++;
		
	while( outbuff != end ) {
		int32_t sample = (*inbuff * normalizeFactor) >> 6;
		if( abs(sample) > NORMALIZE_MAX_RANGE) {
			normalizeFactor = abs( (NORMALIZE_MAX_RANGE<<6) / (*inbuff) );
			if( sample < 0 )
				sample = -NORMALIZE_MAX_RANGE;
			else 
				sample = NORMALIZE_MAX_RANGE;
		}
		*(outbuff++) = short(sample);
		inbuff++;
	}
	return true;
}	

bool AudioMixerSimple::normalizeStereo( int32_t length ) {
	//we need running pointers ...
	short * outbuff = outputBuffer;
	int32_t * inbuff = mixBuffer;
	int32_t * sample = new int32_t[2];

	
	//indicates the end ...
	short * end = outbuff + length;
	
	if( normalizeFactor < 64 )
		normalizeFactor++;
		
	while( outbuff != end ) {
		sample[0] = (*inbuff * normalizeFactor) >> 6;
		inbuff++;
		sample[1] = (*inbuff * normalizeFactor) >> 6;

		if( abs(sample[0]) > NORMALIZE_MAX_RANGE) {
			normalizeFactor = abs( (NORMALIZE_MAX_RANGE<<6) / (*inbuff) );
			if( sample[0] < 0 )
				sample[0] = -NORMALIZE_MAX_RANGE;
			else 
				sample[0] = NORMALIZE_MAX_RANGE;
			if( sample[1]< 0 )
				sample[1] = -NORMALIZE_MAX_RANGE;
			else 
				sample[1] = NORMALIZE_MAX_RANGE;
			#ifdef DEBUG_OUTPUT
			merr << "n";
			#endif
		}
		
		*(outbuff++) = short(sample[0]);
		*(outbuff++) = short(sample[1]);
		
		inbuff++;
	}
	if( sample )
		delete [] sample;
	return true;
}	

bool AudioMixerSimple::normalizeMulti( int32_t length ) {
	//we need running pointers ...
	short * outbuff = outputBuffer;
	int32_t * inbuff = mixBuffer;
	uint32_t i;
	int32_t * sample = new int32_t[numChannels];
	int32_t originalSample;
	
	//indicates the end ...
	short * end = outbuff + length;
	
	if( normalizeFactor < 64 )
		normalizeFactor++;
		
	while( outbuff != end ) {
		originalSample = *inbuff; //keep it, we may need it to normalize
		
		for( i = 0; i<numChannels; i++ ) {
			sample[i] = (*inbuff * normalizeFactor) >> 6;
			inbuff++;
		}
		
		if( abs(sample[0]) > NORMALIZE_MAX_RANGE) {
			normalizeFactor = abs( (NORMALIZE_MAX_RANGE<<6) / originalSample );			
			//after updating the norm factor ... 
			//update all the samples from the channels
			for( i = 0; i<numChannels; i++ ) {
				if( sample[i]< 0 )
					sample[i] = -NORMALIZE_MAX_RANGE;
				else 
					sample[i] = NORMALIZE_MAX_RANGE;
			}
		}
		
		for( i = 0; i<numChannels; i++ ) {
			*outbuff = short(sample[i]);
			outbuff++;
		}
	}
	if( !sample )
		delete [] sample;
	return true;
}	


