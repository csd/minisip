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
                 * @param dest          Buffer to which samples will be "dequeued".
                 * @param nMono         Number of samples per channel to retrieve.
                 * @param stereo        Indicates if one or two channels will be retrieved
                 * @param dequeue       Indicates of the retrieved samples should be removed
                 *                      from the queue.
                 */
                virtual void getSound(short *dest,
                                      int32_t nMono,
                                      bool stereo,
                                      bool dequeue=true)=0;

                virtual std::string getMemObjectType(){return "SoundSource";};

                short* getLeftBuf();

                short* getRightBuf();

                short* getLookupLeft();

                short* getLookupRight();

                int32_t getPointer();

                void setPointer(int32_t wpointer);

		void resample( short * input, short * output );
        private:
                int sourceId;

        protected:
		MRef<Resampler *> resampler;
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



class BasicSoundSource: public SoundSource{
        public:
                /**
                 * Implementation of very simple queueing algorithm.
                 * @param id            Identifier of sound source that is generating audio for
                 *                      stream.
                 * @param pcl           Packet loss concealment provider. The codec that is used
                 *                      to decode the audio data can provide a PLC mechanism.
                 * @param buffersize    Number of samples in buffer (per channel)
                 */
  BasicSoundSource(int32_t id,
                   SoundIOPLCInterface *plc,
                   int32_t position,
                   int32_t nSources,
                   double sRate,
                   int32_t frameSize,
                   int32_t buffernmonosamples=16000);

                virtual ~BasicSoundSource();

                void pushSound(short *samples,
                                int32_t nSamples,
                                int32_t index,
                                bool isStereo=false);



                virtual void getSound(short *dest,
                                      int32_t nMono,
                                      bool stereo,
                                      bool dequeue=true);
        private:
                SoundIOPLCInterface *plcProvider;
                short *stereoBuffer;
                int32_t bufferSizeInMonoSamples;
                short *playoutPtr;
                short *firstFreePtr;
                int32_t lap_diff; //roll over counter

};

#endif
