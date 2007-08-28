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


#ifndef AUDIO_MIXER_H
#define AUDIO_MIXER_H

#include<libminisip/libminisip_config.h>

#include<libmutil/MemObject.h>

// PCM16 range: [-32767:32768]
#define NORMALIZE_MAX_RANGE 32737

#include<string>
#include<list>

#include<libminisip/media/soundcard/SoundSource.h>

/**
Class AudioMixer (abstract).

This is the base class for all specialized audio mixers.
To create your own, derive from this base classe and implement
the mix() function.

The selected mixer is set within the SoundIO class, using the 
SoundIO::setMixer(string) function.
*/
class LIBMINISIP_API AudioMixer: public MObject{
	public:
		AudioMixer();
		virtual ~AudioMixer();
		
		virtual std::string getMemObjectType() const {return "AudioMixer";};
		
		/**
		Given the list of sources, mix their audio and return
		the mixed audio as the return value.
		The returned short * buffer is not to be deleted!
		Before using this function, a call to init() must be made!!!
		*/
		virtual short * mix(std::list<MRef<SoundSource *> > sources) = 0;
		
		/**
		Initialize the buffers and stuff, as well as receive any needed
		parameter.
		Rewrite it if you need it to do something special.
		*/
		virtual bool init( uint32_t numChannels_ );

		/**
		For some mixers (like spatial audio), we may want to position the
		sources ... write your own algorithm.
		This function should be called by SoundIO everytime a source is 
		added or removed to the list.
		Sources would better be added to the back of the list, and this
		function called once the operations on the list have finished
		(add or remove).
		
		To easy the tas, addingSource bool param is to be set to true if
		  we have added a source, false if we have removed a source.
		*/
		virtual bool setSourcesPosition( std::list<MRef<SoundSource *> > &sources,
						bool addingSource = true) = 0;
		
		/**
		Return the number of channels the sources to be mixed use. 
		All sources must use the same number of channels (1 mono, 2 stereo,
		3 ... ).
		*/
		uint32_t getNumChannels() {return numChannels;}
		
		/**
		Set the frameSize ... see explanation below.
		*/
		uint32_t getFrameSize() {return frameSize;}
		
	protected:
	
		/**
		Number of channels of the audio device ... 1 for mono, 2 for stereo, ...
		 */
		uint32_t numChannels;
		
		/**
		frame is the size, in samples, that we want to process.
		Given the sampling rate, we select how many of those samples
		we want to consider a frame. For now, we always take 20ms frames
		 = SOUND_CARD_FREQ * 20 / 1000
		 Frame size is per channel ... thus, the total size of the data to process
		    is frameSize * numChannels
		*/
		uint32_t frameSize;
		
		/**
		This is where the output-mixed audio is stored, and returned
		by the mix function.
		*/
		short * outputBuffer;
		
		/**
		Buffer to hold the audio samples from a single source, still
		to be mixed
		*/
		short * inputBuffer;
		
		/**
		Intermediate buffer, used to mix the audio.
		It is bigger (32 bits), so we don't get into saturation problems.
		*/
		int32_t * mixBuffer;
		
	private:

};

#endif
