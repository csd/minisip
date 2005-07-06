/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  
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
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


/* Copyright (C) 2004, 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Ignacio Sanchez Pardo <isp@kth.se>
*/


#ifndef SOUND_SOURCE_H
#define SOUND_SOURCE_H

#ifdef _MSC_VER
#ifdef LIBMINISIP_EXPORTS
#define LIBMINISIP_API __declspec(dllexport)
#else
#define LIBMINISIP_API __declspec(dllimport)
#endif
#else
#define LIBMINISIP_API
#endif


#include<libminisip/SoundIOPLCInterface.h>
#include<libminisip/Resampler.h>


class LIBMINISIP_API SoundSource : public MObject{
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

                short* getLeftBuf();

                short* getRightBuf();

                short* getLookupLeft();

                short* getLookupRight();

                int32_t getPointer();

                void setPointer(int32_t wpointer);
        private:
                int sourceId;

        protected:
                int32_t position;
                double sampRate;
                short *leftch;
                short *rightch;
                short *lookupleft;
                short *lookupright;
                int32_t pointer;
                int32_t numSources;
                int32_t j;
                int32_t k;

                friend class SpAudio;

};



class LIBMINISIP_API BasicSoundSource: public SoundSource{
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

                void pushSound(short *samples,
                                int32_t nSamples,
                                int32_t index,
                                bool isStereo=false);



                virtual void getSound(short *dest,
                                      bool dequeue=true);
        private:
                SoundIOPLCInterface *plcProvider;
                short *stereoBuffer;
                int32_t bufferSizeInMonoSamples;
                short *playoutPtr;
                short *firstFreePtr;
                short *temp;
                int32_t lap_diff; //roll over counter
		uint32_t oFrames;
		uint32_t iFrames;
		uint32_t oNChannels;
		MRef<Resampler *> resampler;

};

#endif
