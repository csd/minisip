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
                                                                                         
/* Copyright (C) 2004
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include"FileSoundSource.h"
#include<fstream>
#include<libmutil/print_hex.h>

using namespace std;

FileSoundSource::FileSoundSource(string filename, uint32_t id, 
                uint32_t inputFreq,
                uint32_t inputNChannels,
                uint32_t outputFreq,
                uint32_t outputDurationMs,
                uint32_t outputNChannels,
		bool rep):
                SoundSource(id), 
                enabled(false),
                repeat(rep),
                index(0)
{
        short * input;
        long l,m;
        
        ifstream file (filename.c_str(), ios::in|ios::binary);
        
        l = file.tellg();
        file.seekg (0, ios::end);
        m = file.tellg();
        
        nSamples = (m-l)/(sizeof(short)*inputNChannels);
        cerr << "nSample: " << nSamples << endl;

        nOutputFrames = ( outputDurationMs * outputFreq ) / 1000;
        cerr << "nOutputFrames: " << nOutputFrames << endl;
        
        audio = new short[nSamples*outputNChannels];
        input = new short[nSamples*inputNChannels];
        
        file.seekg (0, ios::beg);
        file.read( (char*)input, nSamples*sizeof(short));

        if( inputNChannels > outputNChannels ){
                if( inputNChannels % outputNChannels == 0 ){
                        /* do some mixing */
                        for( uint32_t sample = 0; sample < nSamples; sample++ ){
                                for( uint32_t oChannel = 0; oChannel < outputNChannels; oChannel++ ){
                                        for( uint32_t i = 0; i < inputNChannels/outputNChannels; i++ ){
                                                audio[sample*outputNChannels+oChannel] += input[sample*inputNChannels + oChannel + i*inputNChannels];
                                        }
                                        audio[sample*outputNChannels+oChannel] /= inputNChannels/outputNChannels;
                                }
                        }
                }
                else{
                        /* Just take the first channels */
                        for( uint32_t sample = 0; sample < nSamples; nSamples++ ){
                                for( uint32_t oChannel = 0; oChannel < outputNChannels; oChannel++ ){
                                        audio[sample*outputNChannels+oChannel] = input[sample*inputNChannels+oChannel];
                                }
                        }
                }
        }
        else{
                for( uint32_t sample = 0; sample < nSamples; nSamples++ ){
                        for( uint32_t oChannel = 0; oChannel < outputNChannels; oChannel++ ){
                                audio[sample*outputNChannels+oChannel] = input[sample*inputNChannels];
                        }
                }
        }

        nChannels = outputNChannels;

        resampler = Resampler::create( inputFreq, outputFreq, outputDurationMs, outputNChannels );

        delete [] input;
}

                
                                        



FileSoundSource::FileSoundSource(short *rawaudio, int samples, bool rep): 
                SoundSource(0),
                audio(rawaudio),
                nSamples(samples),
                enabled(false),
                repeat(rep),
                index(0)
{

}

FileSoundSource::~FileSoundSource(){
        delete [] audio;
        audio=NULL;
}


void FileSoundSource::enable(){
        enabled=true;
        index=0;
}

void FileSoundSource::disable(){
        enabled=false;
        index=0;
}


void FileSoundSource::pushSound(short *samples,
                                int32_t nSamples,
                                int32_t index,
                                bool isStereo)
{
#ifdef DEBUG_OUTPUT
    cerr << "WARNING: FileSoundSource::push_sound: FORBIDDEN"<< endl;
#endif
}


void FileSoundSource::getSound( short *dest, bool dequeue ){
        if( index + nOutputFrames >= nSamples ){
                if (repeat)
                        index = 0;
                else
                        index = nSamples;
        }

        if( index == nSamples ){
                memset( dest, '\0', nOutputFrames*nChannels*sizeof(short) );
        }
        else{
                resampler->resample( audio + index, dest );
                cerr << "audio + index: "  << print_hex( (unsigned char *)(audio + index), nOutputFrames*nChannels) << endl;
                cerr << "dest: "  << print_hex( (unsigned char *)dest, nOutputFrames*nChannels) << endl;
                
                if (dequeue){
                        index += nOutputFrames;
                }
        }
}


