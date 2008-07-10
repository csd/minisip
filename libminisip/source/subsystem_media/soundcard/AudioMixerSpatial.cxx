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
 * 	AudioMixer.h
 * Author
 * 	Cesc Santasusana <cesc dot santa at gmail dot com>
 * Purpose
 * 	Base class for audio mixing implementations.
*/

#include<config.h>

#include<libminisip/media/soundcard/AudioMixerSpatial.h>
#include<libminisip/media/soundcard/SoundSource.h>
#include<libminisip/media/spaudio/SpAudio.h>

	// cesc ... remove
#include<libmutil/stringutils.h>

#ifdef _WIN32_WCE
#	include"../include/minisip_wce_extra_includes.h"
#endif

#include<string.h>

using namespace std;

AudioMixerSpatial::AudioMixerSpatial(MRef<SpAudio *> spatial) {
	this->spAudio = spatial;
}

AudioMixerSpatial::~AudioMixerSpatial() {
}

short * AudioMixerSpatial::mix (list<MRef<SoundSource *> > sources) {
	
	uint32_t size = frameSize * numChannels;
	int32_t pointer;
	
	memset( mixBuffer, '\0', size * sizeof( int32_t ) );
	
	for (list<MRef<SoundSource *> >::iterator 
			i = sources.begin(); 
			i != sources.end(); i++){

		(*i)->getSound( inputBuffer );
	
		/* spatial audio */
		pointer = this->spAudio->spatialize(inputBuffer, 
						(*i),
						outputBuffer); //use this buffer as temporary output
						
		(*i)->setPointer(pointer);

		for (uint32_t j=0; j<size; j++){
#ifdef IPAQ
			/* iPAQ hack, to reduce the volume of the
				* output */
			mixBuffer[j]+=(outputBuffer[j]/32);
#else
			mixBuffer[j]+=outputBuffer[j];
#endif
		}
	}
	//mix buffer is 32 bit to prevent saturation ... 
	// some kind of normalization/scaling should be performed here
	//TODO: for now, simply copy the mix to the output buffer
	for( uint32_t k = 0; k < size; k++ )
		outputBuffer[k] = (short)mixBuffer[k];
		
	return outputBuffer;
}

bool AudioMixerSpatial::setSourcesPosition( 
			list<MRef<SoundSource *> > &sources, 
			bool addingSource) {
	list< MRef<SoundSource *> >::iterator it;
	int sourceIdx;
	
	if( addingSource ) {
		int size = (int)sources.size();
		int newPosition=1;
		//if we have 5 sources, optimize the result with this 
		//previous knowledge we have
		if( SPATIAL_POS == 5 ) {
			switch( size ) {
				case 1: 
				case 3: 
				case 5: newPosition = 3; break;
				case 2: newPosition = 5; break;
				case 4: newPosition = 4; break;

			}
		} else {
			if( SPATIAL_POS % 2 ) { //if odd number of positions
				newPosition = (SPATIAL_POS/2) + 1; //tend to send it up high
			} else {
				newPosition = (SPATIAL_POS/2);
			}
		}
		it = sources.end();
		it --;
		(*it)->setPos( newPosition );
// 		cerr << "CESC: MixSpatial:adding: newPosition = " << itoa(newPosition) << endl;
		sortSoundSourceList( sources );
		for( it = sources.begin(), sourceIdx = 1; it != sources.end(); it++, sourceIdx++ ) {
			int pos = spAudio->assignPos(sourceIdx, (int)sources.size() );
#ifdef DEBUG_OUTPUT
			cerr << "AudioMixerSpatial::setSourcesPosition: adding: set source id = " << 
					itoa((*it)->getId() ) << endl <<
					"            to position = " << itoa( pos ) << endl;
#endif
			(*it)->setPos( pos );
		}
	} else { //we have just removed a source ...
		//sources are still sorted correctly ... simply reassing the positions ... 
		for( it = sources.begin(), sourceIdx = 1; it != sources.end(); it++, sourceIdx++ ) {
			int pos = spAudio->assignPos(sourceIdx, (int)sources.size() );
#ifdef DEBUG_OUTPUT
			cerr << "AudioMixerSpatial::setSourcesPosition:removing: set source id = " << 
					itoa((*it)->getId() ) << endl <<
					"            to position = " << itoa( pos ) << endl;
#endif
			(*it)->setPos( pos );
		}
	}
	return true;
}

bool AudioMixerSpatial::sortSoundSourceList( list<MRef<SoundSource *> > &srcList) {
	//very rudimentary ... but much easier than using the sort from STL ..
	int max;
	MRef<SoundSource *> ref;
	
	list<MRef<SoundSource *> > tmpList;
	list<MRef<SoundSource *> >::iterator srcIt, destIt;

	while( srcList.size() > 0 ) {
		max = -1;
		for( srcIt = srcList.begin(); srcIt!=srcList.end(); srcIt++ ) {
			if( (*srcIt)->getPos() >= max ) {
				ref = (*srcIt);
				max = ref->getPos();
			}
		}
		tmpList.push_front( ref );
		srcList.remove( ref );
	}
	srcList= tmpList;
	return true;
}

