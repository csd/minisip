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
 * 	Spatial Audio Mixer implementation
*/


#ifndef AUDIO_MIXER_SPATIAL_H
#define AUDIO_MIXER_SPATIAL_H

#include<libminisip/libminisip_config.h>

#include<libminisip/media/soundcard/AudioMixer.h>

class SoundSource;
class SpAudio;

/**
A Spatial Audio mixer.

This mixer processes the audio of each source previous to mixing to
achieve a spatial audio feeling, that is, the sources appear to be 
in different positions space-wise.

The only pitfall is that it uses quite some memory, for now.
*/
class LIBMINISIP_API AudioMixerSpatial: public AudioMixer {

	public:
		AudioMixerSpatial(MRef<SpAudio *> spatial);
		virtual ~AudioMixerSpatial();
		
		virtual std::string getMemObjectType() const {return "AudioMixerSpatial";};
		
		/**
		Given the list of sources, mix their audio and return
		the mixed audio as the return value.
		The returned short * buffer is not to be deleted!
		Before using this function, a call to init() must be made!!!
		*/
		virtual short * mix(std::list<MRef<SoundSource *> > sources);
		
		/**
		Position the sources as we want.
		Each source has an index number. This index defines a position in the 
		space infront of you, going from 1 (your left side) to 
		SPATIAL_POS (your right side). Sources with index between 1 and SPATIAL_POS
		are placed equally spaced over the semi-circle in front of you.
		Example
		* SPATIL_POS = 5; number of source = 3 -> 1=LEFT, 3=CENTER, 5=RIGHT
		*/
		virtual bool setSourcesPosition( std::list<MRef<SoundSource *> > &sources,
						bool addingSource = true);
		
	protected:
		/**
		Home-made algorithm to sort the sources in the list according to its
		position index.
		*/
		bool sortSoundSourceList( std::list<MRef<SoundSource *> > &list );
	
	private:
		/**
		The SpAudio object, which performs the audio processing. 
		NOTE: this object is huge (memory-wise) ... it uses around 1.5 megas
		*/
		MRef< SpAudio *> spAudio;
		
		AudioMixerSpatial(); //don't use this one

};

#endif
