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


#include"AudioMixerSimple.h"
#include"SoundSource.h"

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

