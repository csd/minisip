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

/* Copyright (C) 2004, 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Ignacio Sanchez Pardo <isp@kth.se>
*/


#ifndef SOUND_SOURCE_H
#define SOUND_SOURCE_H

#include"SoundIOPLCInterface.h"
#include"resampler/Resampler.h"

#define LEFT 1
#define RIGHT 5
#define CENTER 3

class SoundSource : public MObject{
        public:
                SoundSource(int id);
                virtual ~SoundSource(){};

                /**
                 * @return              Identifier of source that generated the audio.
                 */
                int getId();

                /**
                 * @return              Spatial position of the source.
                 */
                int32_t getPos();


                /**
                 * @set                 Spatial position of the source.
                 */
                void setPos(int32_t position);


                /**
                 * @param samples       Buffer with samples that will be enqueued.
                 * @param nSamples      Number of samples (per channel) in buffer.
                 * @isStereo            Indicates if one or two channels are used.
                 */
                virtual void pushSound(short *samples,
                                int32_t nSamples,
                                int32_t index,
                                bool isStereo=false)=0;

                /**
                 * @param dest          Buffer to which samples will be 
                 *                      "dequeued".
                 * @param dequeue       Indicates of the retrieved 
                 *                      samples should be removed
                 *                      from the queue.
                 */
                virtual void getSound(short *dest,
                                      bool dequeue=true)=0;

                virtual std::string getMemObjectType(){return "SoundSource";};

                int32_t getPointer();

                void setPointer(int32_t wpointer);
		
		bool isSilenced() { return silenced; }
		void setSilenced( bool s ) { silenced = s; }
		
        private:
                int sourceId;

        protected:
		/**
		Whether this source is silenced, that is, on read of the buffer,
		it will provide silence samples (all zeros)
		*/
		volatile bool silenced;
		
                int32_t position;
                double sampRate;
               short *leftch; //spaudio
               short *rightch; //spaudio
//                short *lookupleft; //spaudio
//                 short *lookupright; //spaudio
                int32_t pointer; //spaudio
//                 int32_t numSources; //spaudio
                int32_t j; //spaudio
                int32_t k; //spaudio

                friend class SpAudio;

};



class BasicSoundSource: public SoundSource{
        public:
                /**
                 * Implementation of very simple queueing algorithm.
                 * @param id            Identifier of sound source that
                 *                      is generating audio for
                 *                      stream.
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
			SoundIOPLCInterface *plc,
			int32_t position,
			uint32_t oFreq,
			uint32_t oDurationMs,
			uint32_t oNChannels,
			int32_t buffersize=16000);

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
                                bool isStereo=false);


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
		*/
		short *stereoBuffer;
                
		int32_t bufferSizeInMonoSamples;
                short *playoutPtr;
                short *firstFreePtr;
                short *temp;
                int32_t lap_diff; //roll over counter
		
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
		
		MRef<Resampler *> resampler;

};

#endif
