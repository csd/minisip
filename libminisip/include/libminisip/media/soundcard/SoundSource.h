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

/* Copyright (C) 2004, 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Ignacio Sanchez Pardo <isp@kth.se>
*/


#ifndef SOUND_SOURCE_H
#define SOUND_SOURCE_H

#include<libminisip/libminisip_config.h>

#include<libmutil/Mutex.h>

#include<libminisip/media/soundcard/SoundIOPLCInterface.h>
#include<libminisip/media/soundcard/Resampler.h>

#define LEFT 1
#define RIGHT 5
#define CENTER 3

#include<string>

class CircularBuffer;

/**
Definition of a SoundSource.
A SoundSource object is capable of acting as a buffer for 
audio samples (read from the network) before they are 
"get" by the SoundDevice and played. 
*/
class LIBMINISIP_API SoundSource : public MObject{
	public:
		/**
		 *
		 * @param id ssrc of the system that generated the audio
		 *
		 * @param callId  Minisip can handle multiple simultaneous 
		 * 	ongoing calls. It can be important to know to
		 * 	what call a stream belongs such as if the UA
		 * 	should mix incoming audio from one user (A) when
		 * 	sending to another (B).
		 * 	Example:
		 *        ME is in a conference with A and B, and ME is
		 *        supposed to act as a conference bridge mixing
		 *        audio.
		 *
		 *            a         a+me
		 * 	  A------->(ME)------->B
		 * 	   <-------    <-------
		 *            b+me         b
		 *
		 *      Note that media processing is different when having a 
		 *      full mesh conference (then minsip handles multiple
		 *      calls, and the media system is less complicated).
		 *
		 */
		SoundSource(int id, std::string callId);

		virtual ~SoundSource(){};
		virtual std::string getMemObjectType() const {return "SoundSource";};

		/**
		* @return Identifier of source that generated the audio.
		*/
		int getId();

		/**
		 * @return Identifier of call that the audio in the 
		 *  	buffer belongs to.
		 */
		std::string getCallId(){return callid;}

		/**
		* @return Spatial position of the source.
		*/
		int32_t getPos();


		/**
		* @param position Spatial position of the source.
		*/
		void setPos(int32_t position);

		/**
		Add samples to the SoundSource
		@param samples Buffer with samples that will be enqueued.
		@param nSamples Number of samples (per channel) in buffer.
		@param isStereo Indicates whether the provided samples are in 
			mono or stereo mode (1 or 2 channels).
		*/
		virtual void pushSound(short *samples,
				int32_t nSamples,
				int32_t index,
				int freq,
				bool isStereo /* = false*/)=0;

		/**
		Read Samples from the SoundSource
		@param dest Buffer to which samples will be 
				"dequeued".
		@param dequeue Indicates of the retrieved 
				samples should be removed
				from the queue.
		*/
		virtual void getSound(short *dest,
				bool dequeue=true)=0;

		/**
		Set and get for the silencing of this sources (meaning
		that when read, even if it has data, it will return "silence"
		samples only.
		*/
		bool isSilenced() { return silenced; }
		void setSilenced( bool s ) { silenced = s; }
		
		/**
		Used in spatial audio mixing.
		*/
		int32_t getPointer();
		void setPointer(int32_t wpointer);
		
	
	protected:
		/**
		Holds the id of the source ... for example, the SSRC of the RTP stream 
		that is being processed.
		*/		
		int sourceId;
		
		/**
		Whether this source is silenced, that is, on read of the buffer,
		it will provide silence samples (all zeros)
		*/
		volatile bool silenced;
		
		/**
		Spatial Position.
		In general:
		3 - CENTER
		1 - LEFT
		5 - RIGHT
		2 - FRONT LEFT
		4 - FRONT RIGHT
		*/
		int32_t position;
		
#if 0
		/**
		Sample rate of the audio in this source
		*/
		double sampRate;
#endif
	
		/**
		Spatial Audio related
		FIXME: is it needed? can someone add some doc comments on it?
		*/
		short *leftch; //spaudio
		/**
		Spatial Audio related
		FIXME: is it needed? can someone add some doc comments on it?
		*/
		short *rightch; //spaudio
		/**
		Spatial Audio related
		FIXME: is it needed? can someone add some doc comments on it?
		*/
		int32_t pointer; //spaudio
		/**
		Spatial Audio related
		FIXME: is it needed? can someone add some doc comments on it?
		*/
		int32_t j; //spaudio
		/**
		Spatial Audio related
		FIXME: is it needed? can someone add some doc comments on it?
		*/
		int32_t k; //spaudio
		
		friend class SpAudio;

		std::string callid;
};

/**
Simple implementation of an audio sound source. 
It has a circular buffer object, where we keep the audio samples.
The mutation from mono input to stereo output, and from
input frequency to output frequency is done in push/getSound().
*/
class LIBMINISIP_API BasicSoundSource: public SoundSource{
        public:
		/**
		* Implementation of very simple queueing algorithm.
		* @param id            Identifier of sound source that
		*                      is generating audio for
		*                      stream.
		* @param callId	       see class SoundSource for details.
		* @param pcl           Packet loss concealment provider. 
		*                      The codec that is used
		*                      to decode the audio data can 
		*                      provide a PLC mechanism.
		* @param position      Position for spatial audio
		* @param oFreq         Output frquency
		* @param oDurationMs   Output duration (in ms)
		* @param oNChannels    Output number of channels
		* @param buffersize    Number of samples in buffer (per channel)
		*/
		BasicSoundSource(int32_t id,
			std::string callId,
			SoundIOPLCInterface *plc,
			int32_t position,
			uint32_t oFreq,
			uint32_t oDurationMs,
			uint32_t oNChannels);

		virtual ~BasicSoundSource();

		/**
		Add audio samples to the buffer.
		At the same time, the iNChannels and oNChannels
		conversion is applied (usually, turn a mono
		stream into a stereo stream).
		*/
		void pushSound(short *samples,
				int32_t nSamples,
				int32_t index,
				int samplerate,
				bool isStereo = false);


		/**
		Read (and deque) audio samples from the buffer.
		It reads from stereoBuffer (so the nChannels
		is already with the right number). It performs
		the resampling of the frames, to adapt it to 
		the output sample freq.
		*/
		virtual void getSound(short *dest,
				bool dequeue=true);
        
	private:
		SoundIOPLCInterface *plcProvider;
		

                /**
		Output buffer.
		Its size is oFrames * oNChannels samples.
		Whatever is stored here already has been:
		- converted to the output format (at least the 
		oNumberOfChannels, done in pushSound() ).
		- The sampling freq is the inputFreq (8000Hz).
		
		FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME
		With this implementation, we keep losing audio ... the whole 
		audio system thing ... not this particular soundsource or the
		circular buffer.
		
		For every 1000 round of getSound(), pushSound() only does 999, 
		causing a delay for the audio on the headphones from which we never
		recover. 
		
		HACK Set a small buffer size ... kind of limiting the max delay.
			This limits the delay, but we loose audio frames all along ...
			For now, we do 20ms * 5 = 100ms
			We can set this even smaller ... but then we may have problems
			if rtp packets come in burst  ... 
		*/
		CircularBuffer * cbuff;

		short plcCache[2048];

		int oFreq;

		/**
		Auxiliary buffer .. used both in pushSound() and getSound().
		We need the mutex
		*/
		short *temp;
		
		/**
		We lock access to the CircularBuffer object, as well as to the 
		"temp" buffer
		*/		
		Mutex bufferLock;
		
		/**
		Output frame size ... that is, how many samples
		are we to return per call to getSound()
		*/
		uint32_t oFrames;
		
		/**
		Input frame size, that is, how many samples
		do we get each time we are pushSound()'d
		It is calculated with 8000Hz sampling rate, thus
		the only variable is oDurationMs (usually 20ms).
		*/
		uint32_t iFrames;
		
		/**
		Number of channels the output has.
		*/
		uint32_t oNChannels;
		
		/**
		Used to resample the samples stored in the
		circular buffer to be used at the soundcard
		(done in getSound() ).
		*/
		MRef<Resampler *> resampler;

};

#endif
