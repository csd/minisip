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
*/

/*
 * Very simple implementation that I use in a softphone.
 * Implements buffering with play out point, catching up, noice 
 * when silent (tone in this example).
 * TODO: Implement exceptions, make sanity check of values set, 
 * test at other speeds, clean up, ...
 * --Erik Eliasson, eliasson@it.kth.se
 */


/*
 * Sample=
 *
 *
 *
 *
*/


#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include<iostream>
#include<stdio.h>
#include<stdint.h>
#include"SoundIO.h"
#include<assert.h>
#include<signal.h>
#include<sys/time.h>
#include<libmutil/itoa.h>
#include<libmutil/Thread.h>
#include<unistd.h>
#include"../spaudio/SpAudio.h"
#include<samplerate.h>

#define BS 160

bool nprint=false;

float lchvol[POS]={1,0.8,1,0.6,0};
float rchvol[POS]={0,0.6,1,0.8,1};


SoundIO::SoundIO(
                //string device, 
                MRef<SoundDevice *> device,
                int nChannels, 
                int32_t samplingRate, 
                int format): 
		    //nChannels(nChannels),
            	    //useFileInterface(false),
   		    //in_file(string("")),
		    //out_file(string("")),
		    nChannels(nChannels),
		    samplingRate(samplingRate),
		    format(format),
		    recording(false)
		    //openCount(0)
{
        soundDev = device;
	spAudio = new SpAudio(5);

	/* Create the SoundPlayerLoop */
	start_sound_player();
	start_recorder();

}



void SoundIO::closeRecord(){
	if( soundDev ){
		soundDev->lockRead();
		soundDev->closeRecord();
		soundDev->unlockRead();
	}
}

void SoundIO::closePlayback(){
	if( soundDev ){
		soundDev->lockWrite();
		soundDev->closePlayback();
		soundDev->unlockWrite();
	}
}

void SoundIO::sync(){
	if( soundDev ){
		soundDev->sync();
	}
}

void SoundIO::play_testtone( int secs ){
	
	int nSamples = secs * soundDev->getSamplingRate();
	short *data = (short*)malloc( nSamples * soundDev->getSampleSizePlay() );
	for (int32_t i=0; i< nSamples; i++){
		if (i%4==0)data[i]=0;
		if (i%4==1)data[i]=10000;
		if (i%4==2)data[i]=0;
		if (i%4==3)data[i]=-10000;
	}
	byte_t *ptr = (byte_t *)data;
//	int32_t i=0;

	soundDev->write( ptr, nSamples );
}

void SoundIO::openPlayback(){
	if( soundDev ){
		soundDev->lockWrite();
		if( !soundDev->isOpenedPlayback() ){
			soundDev->openPlayback( samplingRate, nChannels, format );
		}
		soundDev->unlockWrite();
	}

}

void SoundIO::openRecord(){
	if( soundDev ){
		soundDev->lockRead();
		if( !soundDev->isOpenedRecord() ){
			soundDev->openRecord( samplingRate, nChannels, format );
		}
		soundDev->unlockRead();
	}

}

void SoundIO::startRecord(){
	recording = true;
	recorderCond.broadcast();
}

void SoundIO::stopRecord(){
	recording = false;
}

void SoundIO::register_recorder_receiver(SoundRecorderCallback *callback, 
                                        int32_t nrsamples, 
                                        bool stereo )
{
	recorder_callbacks.push_back(new RecorderReceiver(callback, stereo));
	recorder_buffer_size = nrsamples;   // FIXME: implement a way to 
                                            // return different amount of data 
                                            // to different recorders - needed 
                                            // for G711+ilbc.
}

void SoundIO::set_recorder_buffer_size(int32_t bs){
	recorder_buffer_size=bs;
}

void SoundIO::fileIOTimeout(int ){
}

void *SoundIO::recorderLoop(void *sc_arg){
	SoundIO *soundcard = (SoundIO *)sc_arg;
	int32_t i;
	short *buffers[2];	// Two sound buffers for "double buffering"
	assert(soundcard!=NULL);
	int32_t nread; /* IN SAMPLES */
	soundcard->recorder_buffer_size = 160;
	
	for (i=0; i<2; i++){
		//buffers[i] = (short *)malloc(soundcard->recorder_buffer_size*sizeof(short)*2);
		buffers[i] = (short *)malloc(2048);
	}
		
	short * tempBuffer=NULL;
	bool tempBufferAllocated = false;

	while( true ){

		if( ! soundcard->recording ){
			if( soundcard->soundDev->isOpenedRecord() ){
				soundcard->closeRecord();
				if( tempBufferAllocated ){
					delete [] tempBuffer;
				}
				tempBuffer = NULL;
			}

			/* sleep until a recorder call back is added */
			soundcard->recorderCond.wait();
			
			if( ! soundcard->soundDev->isOpenedRecord() ){
				soundcard->openRecord();
			}

		}
		
		soundcard->soundDev->lockRead();
                nread = soundcard->soundDev->read( (byte_t *)buffers[i%2], 
                                        soundcard->recorder_buffer_size );
		soundcard->soundDev->unlockRead();

		if( nread < 0 ){
			continue;
		}

		if( soundcard->soundDev->getNChannelsRecord() > 1 ){
			if( !tempBuffer ){
				tempBuffer = new short[soundcard->recorder_buffer_size];
				tempBufferAllocated = true;
			}

			for( int j = 0; j < soundcard->recorder_buffer_size; j++ ){
				tempBuffer[j] = buffers[i%2][j * soundcard->soundDev->getNChannelsRecord() ];
			}
		}
		else{
			tempBuffer = buffers[i%2];
		}
			
		
		if (nread /*!=*/ < soundcard->recorder_buffer_size){
#ifdef DEBUG_OUTPUT
			if (nread>0)
				cerr << "WARNING: dropping "
                                    << nread 
                                    <<" samples in partial buffer"<< endl;
#endif
		}else{
			
			for (list<RecorderReceiver *>::iterator 
                                    cb=soundcard->recorder_callbacks.begin(); 
                                    cb!= soundcard->recorder_callbacks.end(); 
                                    cb++){

				if ((*cb)!=NULL && (*cb)->getCallback()!=NULL){
					(*cb)->getCallback()->
                                                srcb_handleSound(tempBuffer);
				}else{
					cerr << "Ignoring null callback"<< endl;
				}
			}

			i++;
		}
	}
	return NULL;
}

void SoundIO::start_recorder(){
        Thread::createThread(recorderLoop, this);
}



//bool done=false;

void SoundIO::registerSource(int sourceId, SoundIOPLCInterface *plc){	
	queueLock.lock();
	for (list<MRef<SoundSource *> >::iterator i=sources.begin(),int32_t j=1; 
                        i!= sources.end(); 
                        i++,j++){
		if (sourceId==(*i)->getId()){
			queueLock.unlock();
			sourceListCond.broadcast();
			return;
		}
		(*i)->setPos(spAudio->assignPos(j,sources.size()+1));
        }
	sources.push_front(new BasicSoundSource(sourceId,plc,spAudio->assignPos(sources.size()+1,sources.size()+1));
	queueLock.unlock();
	sourceListCond.broadcast();
}

void SoundIO::registerSource( MRef<SoundSource *> source ){
        queueLock.lock();
	for (list<MRef<SoundSource *> >::iterator i=sources.begin(),int32_t j=1; 
                        i!= sources.end(); 
                        i++,j++){
		if (source->getId()==(*i)->getId()){
			queueLock.unlock();
			sourceListCond.broadcast();
			return;
		}
		(*i)->setPos(spAudio->assignPos(j,sources.size()+1));
        }
	sources.push_front(source);
	queueLock.unlock();
	sourceListCond.broadcast();
}


void SoundIO::unRegisterSource(int sourceId){
	queueLock.lock();
	for (list<MRef<SoundSource *> >::iterator i = sources.begin(); 
                        i!=sources.end(); 
                        i++){
		if ((*i)->getId()==sourceId){
			sources.erase(i);
			queueLock.unlock();
			return;
		}
        }
	queueLock.unlock();
}

void SoundIO::pushSound(int sourceId,
	       	short *buf, 
		int32_t nMonoSamples, 
		int index,
		bool isStereo){

	if (sourceId==-1){
		return;
	}
        
	queueLock.lock();

	for (list<MRef<SoundSource *> >::iterator i=sources.begin(); i!= sources.end(); i++){
		if (sourceId==(*i)->getId()){
			(*i)->pushSound(buf, nMonoSamples, index, isStereo);
			queueLock.unlock();	
			return;
		}
        }
	queueLock.unlock();	
}

void SoundIO::send_to_card(short *buf, int32_t n_samples){
	byte_t *ptr = (byte_t *)buf;
	int32_t nWritten;

	soundDev->lockWrite();
	nWritten = soundDev->write( ptr, n_samples );
	soundDev->unlockWrite();
}


MRef<SoundSource *> SoundIO::getSoundSource(int32_t id){
	for (list<MRef<SoundSource *> >::iterator i = sources.begin(); 
                        i!= sources.end(); i++){
		if ((*i)->getId()==id)
			return *i;
        }

	return NULL;	
}

void *SoundIO::playerLoop(void *arg){
	SoundIO *active_soundcard = (SoundIO *)arg;

	short *buf = NULL;
	short *tmpbuf = NULL;
	short *resbuf = NULL;
	short *outbuf = NULL;
	uint32_t nChannels = 0;
	
	while( true ){

                active_soundcard->queueLock.lock();
		if( active_soundcard->sources.size() == 0 ){
			active_soundcard->queueLock.unlock();

			if( active_soundcard->soundDev->isOpenedPlayback() ){
				active_soundcard->closePlayback();
			}
			
			/* Wait for someone to add a source */
			active_soundcard->sourceListCond.wait();

			active_soundcard->openPlayback();
			nChannels = active_soundcard->soundDev->getNChannelsPlay();
			active_soundcard->queueLock.lock();
		}

                
		if( !buf ){
			buf = new short[BS*nChannels];
		}

		if( !tmpbuf ){
			tmpbuf = new short[BS*nChannels];
		}

		if( !resbuf ){
		  resbuf = new short[1764];
		}
		
		if( !outbuf ){
		  outbuf = new short[1764];
		}
		
		memset( buf, '\0', BS*nChannels*2 );
                
		int j=0;
		for (list<MRef<SoundSource *> >::iterator 
				i = active_soundcard->sources.begin(); 
				i != active_soundcard->sources.end(); i++,j++){
			if ((*i)->getId()!=-1 && (*i)->getId()!=-2){
				(*i)->getSound(tmpbuf, BS, nChannels - 1);
				
				/* spatial audio */
				spAudio->resample(tmpbuf,resbuf,320,1764,(*i)->getSrcData(),(*i)->getSrcState());
				(*i)->setPointer(spAudio->spatialize(resbuf,(*i)->getLeftBuf(),(*i)->getRightBuf(),
								     (*i)->getLookupLeft(),(i*)->getLookupRight(),
								     (*i)->getPos(),(*i)->getPointer(),outbuf));

				for (uint32_t j=0; j<1764; j++)
					buf[j]+=outbuf[j];
			}
		}
		active_soundcard->queueLock.unlock();

		if( active_soundcard->soundDev->isOpenedPlayback() ){
			active_soundcard->send_to_card(buf, BS);
		}
		
	}
	return NULL;
}

void SoundIO::start_sound_player(){
        Thread::createThread(playerLoop, this);
}

SoundSource::SoundSource(int id):sourceId(id){
}

int error;

BasicSoundSource::BasicSoundSource(int32_t id, 
				   SoundIOPLCInterface *plc,
				   int32_t position, 
				   int32_t bufferNMonoSamples):
                SoundSource(id), 
		plcProvider(plc),
		bufferSizeInMonoSamples(bufferNMonoSamples),
		playoutPtr(0), 
		firstFreePtr(0),
		lap_diff(0)
{
	stereoBuffer = new short[bufferNMonoSamples*2];
	firstFreePtr = stereoBuffer;
	playoutPtr = stereoBuffer;
	sourcePos=position;

	/* spatial audio initialization */
	src_data.input_frames = 160;
	src_data.output_frames = 882;
	src_data.src_ratio = 5.5125;

	src_state=src_new(0,2,&error);

	leftChannelBuffer = new short[950];
	rightChannelBuffer = new short[950];
	lookupleft = new short[65536];
	lookupright = new short[65536];
	initLookup();
	
}

BasicSoundSource::~BasicSoundSource(){
	delete [] stereoBuffer;
	delete [] leftChannelBuffer;
	delete [] rightChannelBuffer;
	delete [] lookupleft;
	delete [] lookupright;
}

void BasicSoundSource::initLookup(){
   for(int32_t i=0;i<65536;i++){
     lookupleft[i]=(short)((float)(i-32768)*lchvol[sourcePos]/sources.size());
     lookupright[i]=(short)((float)(i-32768)*rchvol[sourcePos]/sources.size());
   } 
}

 short* BasicSoundSource::getLeftBuf(){
   return leftChannelBuffer;
 }

 short* BasicSoundSource::getRightBuf(){
   return rightChannelBuffer;
 }

 short* BasicSoundSource::getLookupLeft(){
   return lookupleft;
 }

 short* BasicSoundSource::getLookupRight(){
   return lookupright;
 }

 int32_t BasicSoundSource::getPointer(){
   return pointer;
 }

 void BasicSoundSource::setPointer(int32_t wpointer){
   pointer=wpointer;
 }

int SoundSource::getId(){
	return sourceId;
}

int32_t SoundSource::getPos(){
	return sourcePos;
}

void SoundSource::setPos(int32_t position){
  sourcePos=position;
}

// Two cases - might have to "wrap around"
//            v-firstFree
// 1:  ...xxxx..........
// 2:  ............xxxx.

int npush=1;
void BasicSoundSource::pushSound(short * samples, 
                                int32_t nMonoSamples, 
                                int32_t index, 
                                bool isStereo)
{
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
                                    (bufferSizeInMonoSamples*2)+1] = samples[i];
		}
	}
	firstFreePtr = stereoBuffer + ((firstFreePtr-stereoBuffer)+
                        nMonoSamples*2)%(bufferSizeInMonoSamples*2);
}


int nget=1;
void BasicSoundSource::getSound(short *dest, 
                int32_t nMono, 
                bool stereo, 
                bool dequeue)
{
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
	if ((!lap_diff && (firstFreePtr-playoutPtr< nMono*2)) || 
                        (lap_diff && (firstFreePtr-stereoBuffer+
                        endOfBufferPtr-playoutPtr<nMono*2))){
                
                /* Underflow */
#ifdef DEBUG_OUTPUT
		cerr << "u"<< flush;
#endif
		if (plcProvider){
			cerr << "PLC!"<< endl;
			short *b = plcProvider->get_plc_sound(nMono);
			memcpy(dest, b, nMono);
		}else{

			for (int32_t i=0; i < nMono * (stereo?2:1); i++){
				dest[i]=0;
			}
		}
		return;
	}

	if (stereo){
		for (int32_t i=0; i<nMono*2; i++){
			dest[i] = stereoBuffer[ ((playoutPtr-stereoBuffer)+i)%
                                        (bufferSizeInMonoSamples*2) ];
		}
	}else{
		for (int32_t i=0; i<nMono; i++){
			dest[i]=stereoBuffer[((playoutPtr-stereoBuffer)+i*2)%
                                                (bufferSizeInMonoSamples*2) ]/2;
			dest[i]+=stereoBuffer[((playoutPtr-stereoBuffer)+i*2+1)%
                                                (bufferSizeInMonoSamples*2) ]/2;
		}
	}	
	if (playoutPtr+nMono*2>=endOfBufferPtr)
		lap_diff=0;
	
	if (dequeue){
		playoutPtr = stereoBuffer + ((playoutPtr-stereoBuffer)+nMono*2)%
                                                (bufferSizeInMonoSamples*2);
	}
}

RecorderReceiver::RecorderReceiver(SoundRecorderCallback *cb, 
                                    bool stereo) : 
                                        callback(cb), 
                                        stereo(stereo)
{

}

bool RecorderReceiver::getStereo(){
	return stereo;
}

SoundRecorderCallback *RecorderReceiver::getCallback(){
	return callback;
}


