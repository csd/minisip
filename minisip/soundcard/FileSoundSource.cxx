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

FileSoundSource::FileSoundSource(string filename, uint32_t id, bool rep):
                SoundSource(id), 
                enabled(false),
                repeat(rep),
                index(0)
{
        long l,m;
        ifstream file (filename.c_str(), ios::in|ios::binary);
        l = file.tellg();
        file.seekg (0, ios::end);
        m = file.tellg();
        nSamples = (m-l)/2;
        audio = new short[nSamples];
        file.seekg (0, ios::beg);
        file.read( (char*)audio, nSamples*2);
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
        delete audio;
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


void FileSoundSource::getSound(short *dest,
        int32_t nMono,
        bool stereo,
        bool dequeue)
{
    if (index+nMono>=nSamples){
	    if (repeat)
		index = 0;
	    else
        	index=nSamples;
    }
    cerr << index << endl;

    int mul = stereo ? 2 : 1 ;

    if (index==nSamples){
        for (int i=0; i< nMono*mul; i++){
            dest[i]=0;
        }
    }else{
        for (int i=0; i<nMono; i++){
            if (stereo){
		    //cerr << "stereo" << endl;
                dest[i*2] = dest[i*2+1] = audio[i+index];
            }else{
                dest[i] = audio[i+index];
            }
        }
        if (dequeue){
            index+=nMono;
        }
    }
}


