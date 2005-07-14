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

#include<config.h>
#include<iostream>
#include"SoundSource.h"
#include<libmutil/mtime.h>

#include<iostream>

using namespace std;

SoundSource::SoundSource(int id):sourceId(id){
	leftch = NULL;
	rightch = NULL;
	lookupright = NULL;
	lookupleft = NULL;
}

void SoundSource::setPointer(int32_t wpointer){
	pointer=wpointer;
}

int SoundSource::getId(){
	return sourceId;
}

int32_t SoundSource::getPos(){
	return position;
}

void SoundSource::setPos(int32_t position){
	this->position=position;
}

BasicSoundSource::BasicSoundSource(int32_t id,
                                   SoundIOPLCInterface *plc,
                                   int32_t position,
                                   uint32_t oFreq,
                                   uint32_t oDurationMs,
				   uint32_t oNChannels,
				   int32_t bufferSizeInMonoSamples):
		bufferSizeInMonoSamples(bufferSizeInMonoSamples),
                SoundSource(id),
                plcProvider(plc),
                playoutPtr(0),
                firstFreePtr(0),
                lap_diff(0)
{
        stereoBuffer = new short[bufferSizeInMonoSamples*2];
        firstFreePtr = stereoBuffer;
        playoutPtr = stereoBuffer;
        this->position=position;

	oFrames = ( oDurationMs * oFreq ) / 1000;
	iFrames = ( oDurationMs * 8000 ) / 1000;
	this->oNChannels = oNChannels;
	
	resampler = Resampler::create( 8000, oFreq, oDurationMs, oNChannels );

	temp = new short[iFrames * oNChannels];

        /* spatial audio initialization */
        leftch = new short[1028];
        rightch = new short[1028];
        lookupleft = new short[65536];
        lookupright = new short[65536];
        pointer = 0;
        j=0;
        k=0;

}

BasicSoundSource::~BasicSoundSource(){
	delete [] temp;
        delete [] stereoBuffer;
        delete [] leftch;
        delete [] rightch;
        delete [] lookupleft;
        delete [] lookupright;
}

// Two cases - might have to "wrap around"
//            v-firstFree
// 1:  ...xxxx..........
// 2:  ............xxxx.
bool nprint=false;
int npush=1;
void BasicSoundSource::pushSound(short * samples,
                                int32_t nMonoSamples,
                                int32_t index,
                                bool isStereo)
{
//	cerr << "Calling pushSound for source " << getId() << endl;
        index++; //dummy op
        npush++;
        if (nprint)
                nprint=false,cerr << "npush="<< npush<< endl;;

        short *endOfBufferPtr = stereoBuffer + bufferSizeInMonoSamples*2;


        if (firstFreePtr+nMonoSamples*2 >= endOfBufferPtr){
                if (lap_diff==0 &&
                            stereoBuffer+
                            (((firstFreePtr-stereoBuffer)+
                            nMonoSamples*2)%(bufferSizeInMonoSamples*2))
                            > playoutPtr  ){
                        cerr << "Buffer overflow - dropping packet"<<endl;
                        return;
                }
                lap_diff=1;
        }

        if (isStereo){
                for (int32_t i=0; i< nMonoSamples*2; i++)
                        stereoBuffer[((firstFreePtr-stereoBuffer)+i)%
                                (bufferSizeInMonoSamples*2)] = samples[i];
        }else{
                for (int32_t i=0; i< nMonoSamples; i++){
                        stereoBuffer[((( (firstFreePtr-stereoBuffer)/2)+i)*2)%
                                    (bufferSizeInMonoSamples*2)] = samples[i];
                        stereoBuffer[((( (firstFreePtr-stereoBuffer)/2)+i)*2)%
                                    (bufferSizeInMonoSamples*2)+1] = samples[i];                }
        }
        firstFreePtr = stereoBuffer + ((firstFreePtr-stereoBuffer)+
                        nMonoSamples*2)%(bufferSizeInMonoSamples*2);

}


int nget=1;
void BasicSoundSource::getSound(short *dest,
                bool dequeue)
{
//	cerr << "Calling getSound for source " << getId() << endl;
        nget++;
        if (nget%1000==0)
                nprint=true,cerr << "nget="<< nget<< endl;
        short *endOfBufferPtr = stereoBuffer + bufferSizeInMonoSamples*2;
#ifdef DEBUG_OUTPUT
        static int counter = 0;
        static bool do_print=false;
        if (counter%1000==0)
                do_print=true;

        if (do_print && !lap_diff){
                do_print=false;
        }
        counter++;
#endif
        if ((!lap_diff && (firstFreePtr-playoutPtr< iFrames*oNChannels)) ||
                        (lap_diff && (firstFreePtr-stereoBuffer+
                        endOfBufferPtr-playoutPtr<iFrames*oNChannels))){

                /* Underflow */
#ifdef DEBUG_OUTPUT
                cerr << "u"<< flush;
#endif
                if (plcProvider){
                        cerr << "PLC!"<< endl;
                        short *b = plcProvider->get_plc_sound(oFrames);
                        memcpy(dest, b, oFrames);
                }else{

                        for (int32_t i=0; i < oFrames * oNChannels; i++){
                                dest[i]=0;
                        }
                }
                return;
        }

//        if (stereo){
                for (int32_t i=0; i<iFrames*oNChannels; i++){
                        temp[i] = stereoBuffer[ ((playoutPtr-stereoBuffer)+i)%
                                        (bufferSizeInMonoSamples*2) ];
                }
#if 0
        }else{
                for (int32_t i=0; i<nMono; i++){
                        temp[i]=stereoBuffer[((playoutPtr-stereoBuffer)+i*2)%
                                                (bufferSizeInMonoSamples*2) ]/2;
			temp[i]+=stereoBuffer[((playoutPtr-stereoBuffer)+i*2+1)%                                                (bufferSizeInMonoSamples*2) ]/2;
		}
        }
#endif
        if (playoutPtr+oFrames*oNChannels>=endOfBufferPtr)
                lap_diff=0;

        if (dequeue){
                playoutPtr = stereoBuffer + ((playoutPtr-stereoBuffer)+
			     oNChannels*iFrames ) % (bufferSizeInMonoSamples*2);
        }

	resampler->resample( temp, dest );
}

