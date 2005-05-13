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

#include<config.h>

#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include<iostream>
#include<stdio.h>

#include"SoundIO.h"
#include<assert.h>
#include<signal.h>
#include<libmutil/itoa.h>
#include<libmutil/Thread.h>
#include"../spaudio/SpAudio.h"
#include<libmutil/mtime.h>
#include<libmutil/print_hex.h>

#ifdef _MSC_VER

#else
#include<sys/time.h>
#include<unistd.h>
#endif


#define BS 160


/* lookup tables without gain control */
float lchvol[POS]={1,0.8,1,0.6,0};
float rchvol[POS]={0,0.6,1,0.8,1};

/* lookup tables for gain control
float lchvol[POS][POS]={{1,0.8,1,0.6,0},{1,0,0,0,0},{0.5,0,0.5,0,0},{0.5,0.5,0,0.3,0},{0.5,0.5,0.5,0.3,0}};
float rchvol[POS][POS]={{0,0.6,1,0.8,1},{0,0,0,0,1},{0,0,0.5,0,0.5},{0,0.3,0,0.5,0.5},{0,0.3,0.5,0.5,0.5}};
*/

SpAudio SoundIO::spAudio(5);

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
//	spAudio = new SpAudio(5);

	/* Create the SoundPlayerLoop */
	start_sound_player();
	start_recorder();
	initLookup();

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
        recorderCondLock.lock();
	recorderCond.broadcast();
        recorderCondLock.unlock();
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
	int32_t nread=0; /* IN SAMPLES */
	//FIXME
	soundcard->recorder_buffer_size = SOUND_CARD_FREQ*20/1000;
	
	for (i=0; i<2; i++){
		//buffers[i] = (short *)malloc(soundcard->recorder_buffer_size*sizeof(short)*2);
		buffers[i] = (short *)malloc(4096);
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
                        soundcard->recorderCondLock.lock();
			soundcard->recorderCond.wait( &(soundcard->recorderCondLock) );
                        soundcard->recorderCondLock.unlock();
			
			if( ! soundcard->soundDev->isOpenedRecord() ){
				soundcard->openRecord();
			}

		}
		
		soundcard->soundDev->lockRead();
		if( soundcard->soundDev->isOpenedRecord() ){
				nread = soundcard->soundDev->read( (byte_t *)buffers[i%2], soundcard->recorder_buffer_size );
		}
				
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

#if 0
void SoundIO::registerSource(int sourceId, SoundIOPLCInterface *plc){	
	cerr << "Calling register source on ID " << sourceId << endl;
	int32_t j=1;
	int32_t nextSize=sources.size()+1;
        queueLock.lock();
	for (list<MRef<SoundSource *> >::iterator i=sources.begin(); 
                        i!= sources.end(); 
                        i++,j++){
		if (sourceId==(*i)->getId()){
			queueLock.unlock();
			sourceListCond.broadcast();
			return;
		}
		(*i)->setPos(spAudio.assignPos(j,nextSize));
//		(*i)->initLookup(nextSize);
	}
	sources.push_front(
		new BasicSoundSource( sourceId,plc,
			spAudio.assignPos(nextSize,nextSize), 
			/* Output parameters */
			SOUND_CARD_FREQ, 20, 2 ));
	queueLock.unlock();
	sourceListCond.broadcast();
}
#endif

void SoundIO::registerSource( MRef<SoundSource *> source ){
	cerr << "Calling register source on created source " << source->getId() << endl;
       int32_t j=1;
       int32_t nextSize=sources.size()+1; 
       queueLock.lock();
       for (list<MRef<SoundSource *> >::iterator i=sources.begin(); 
                        i!= sources.end(); 
                        i++){
		if (source->getId()==(*i)->getId()){
			queueLock.unlock();

                        sourceListCondLock.lock();
			sourceListCond.broadcast();
                        sourceListCondLock.unlock();
			return;
		}
		(*i)->setPos(spAudio.assignPos(j,nextSize));
//		(*i)->initLookup(nextSize);
		j++;
	}
        source->setPos( spAudio.assignPos(j,nextSize) );
	sources.push_front(source);
	queueLock.unlock();

        sourceListCondLock.lock();
	sourceListCond.broadcast();
        sourceListCondLock.unlock();
}


void SoundIO::unRegisterSource(int sourceId){
	int32_t j =1;
	queueLock.lock();
	list<MRef<SoundSource *> >::iterator i;
	for (i = sources.begin(); 
                        i!=sources.end(); 
                        i++){
		if ((*i)->getId()==sourceId){
			sources.erase(i);
			break;
		}
			
        }
	int32_t nextSize=sources.size();
	for (i = sources.begin(); 
                        i!=sources.end(); 
                        i++, j++){
		(*i)->setPos(spAudio.assignPos(j,nextSize));
//		(*i)->initLookup(nextSize);
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
	if( soundDev->isOpenedPlayback() ){
		nWritten = soundDev->write( ptr, n_samples );
	}
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
	uint32_t counter = 0;
	
	while( true ){

                active_soundcard->queueLock.lock();
		if( active_soundcard->sources.size() == 0 ){
			active_soundcard->queueLock.unlock();

			if( active_soundcard->soundDev->isOpenedPlayback() ){
				active_soundcard->closePlayback();
			}
			
			/* Wait for someone to add a source */
                        active_soundcard->sourceListCondLock.lock();
			active_soundcard->sourceListCond.wait( &active_soundcard->sourceListCondLock );
                        active_soundcard->sourceListCondLock.unlock();

			active_soundcard->openPlayback();
			nChannels = active_soundcard->soundDev->getNChannelsPlay();
			active_soundcard->queueLock.lock();
		}
		int nFrames = (SOUND_CARD_FREQ * 20) / 1000;

                
		if( !buf ){
			buf = new short[nChannels * nFrames];
		}

		if( !resbuf ){
			resbuf = new short[nChannels * nFrames];
		}
		
		if( !outbuf ){
			outbuf = new short[nChannels * nFrames];
		}
		
		memset( buf, '\0', nChannels * nFrames * sizeof( short ) );
                
		for (list<MRef<SoundSource *> >::iterator 
				i = active_soundcard->sources.begin(); 
				i != active_soundcard->sources.end(); i++){

			(*i)->getSound( resbuf );

			/* spatial audio */
			(*i)->setPointer(spAudio.spatialize(resbuf, (*i),outbuf));

			for (uint32_t j=0; j<nFrames*nChannels; j++){
#ifdef IPAQ
				/* iPAQ hack, to reduce the volume of the
				 * output */
				buf[j]+=(outbuf[j]/32);
#else
				buf[j]+=outbuf[j];
#endif
			}
		}
		active_soundcard->queueLock.unlock();
		if( !(counter++ % 100) ){
			fprintf(stderr ,  ".\n" );
		}

		if( active_soundcard->soundDev->isOpenedPlayback() ){
			active_soundcard->send_to_card(buf, nFrames);
		}
		
	}
	return NULL;
}

void SoundIO::start_sound_player(){
        Thread::createThread(playerLoop, this);
}

short int lookupleftGlobal[65536][POS]; 
short int lookuprightGlobal[65536][POS]; 

void SoundIO::initLookup(){
	for(int32_t j=0; j < POS; j++ ){
		for(int32_t i=0;i<65536;i++){
			/* without gain control */
			lookupleftGlobal[i][j]=(short)((float)(i-32768)*lchvol[j]);
			lookuprightGlobal[i][j]=(short)((float)(i-32768)*rchvol[j]);

			/* with gain control 
			   lookupleft[i]=(short)((float)(i-32768)*lchvol[nSources-1][position-1]);
			   lookupright[i]=(short)((float)(i-32768)*rchvol[nSources-1][position-1]);
			   */

		} 
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


