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


#ifndef AUDIO_MIXER_SIMPLE_H
#define AUDIO_MIXER_SIMPLE_H

#include<libminisip/libminisip_config.h>

#include<libminisip/media/soundcard/AudioMixer.h>

class SoundSource;

/**
A simple audio mixer with dynamic normalization to prevent saturation.

This mixer is slightly more than simple. 
- It mixes all the audio from the sources into a single stream. 
- Process the audio stream (all channels are considered equal) with 
   the normalize() function to prevent audio saturation.

The sources are all mixed together without any spatial audio processing, 
thus they all sound as if they were in front of us (in stereo).
*/
class LIBMINISIP_API AudioMixerSimple: public AudioMixer {

	public:
		AudioMixerSimple();
		virtual ~AudioMixerSimple();
		
		virtual std::string getMemObjectType() const {return "AudioMixerSimple";};
		
		/**
		Given the list of sources, mix their audio and return
		the mixed audio as the return value.
		The returned short * buffer is not to be deleted!
		Before using this function, a call to init() must be made!!!
		
		This mixer calls the normalize function, to prevent audio saturation.
		*/
		virtual short * mix(std::list<MRef<SoundSource *> > sources);
	
		/**
		Overload the init() function ... we need to initialize the normalize
		factor
		*/
		virtual bool init( uint32_t numChannels_ );
			
		/**
		Position the sources as we want ... in this simple mixer, do nothing
		*/
		virtual bool setSourcesPosition( std::list<MRef<SoundSource *> > &sources,
						bool addingSource = true);
		
	protected:
		
		/*Normalization functions ... see below*/
		
		/**
		Scale mixed audio, to prevent saturation.
		It is very important to call this function every time we perfrom 
		a mix, as it keeps some state related variables.
		
		This function processes the contents of mixBuffer and the results 
		are left in outputBuffer.
		Length input parameter is the length of these buffers.
		
		Each position in the buffers is a sample. 
		Buffers contain numChannels * frameSize samples (frameSize samples per channel)
		The structure of data in the buffers is interleaved ... that is, if we
		have 3 sources, A, B and C, samples are stored like:
		A - B - C - A - B - C - A - .... 
		For  stereo sound ... L - R - L - R - ...
		*/
		virtual inline bool normalize( int32_t length);
		
		/**
		Easier version, for a mono source. Each sample is scaled with 
		normalizeFactor/64 (normalizeF = 64 if no saturation is happening).
		If the scaled sample is saturated (exceeds the 16-bit range), then
		we recalculate the normalizeF and set the sample value to a MAX level.
		*/
		virtual inline bool normalizeMono( int32_t length);
		
		/**
		We say stereo, but in this mixer we know that it means two identical
		mono channels ... so, we only update one normalize factor (identical
		channels, thus not independent).
		*/
		virtual inline bool normalizeStereo( int32_t length);
		
		/**
		This is like the stereo case, but for more than 2 channels.
		Again, we consider all channels to be equal, thus no independent
		normalization takes place. 
		*/
		virtual inline bool normalizeMulti( int32_t length);
	
	private:
		/**
		Used in the normalize function.
		We only keep one factor, as we consider (we know) that 
		all channels are going to be equal ... 
		If this was not the case, inherit from this class and 
		modify the normalization functions ... turn normalizeFactor
		into a vector, and update them separately.
		
		Normalizing N independent channels is easy, almost as easy
		as easy as N identical channels.
		The problem comes when you try to normalize coupled channels,
		like the spatial audio thing: it uses stereo, thus two channels,
		each of them carries a different sound stream, which is coupled
		to the other one ... 
		*/
		int32_t normalizeFactor;

	
};
	

#endif

