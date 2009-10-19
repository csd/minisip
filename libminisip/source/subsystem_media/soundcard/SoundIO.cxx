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

/*
 * Very simple implementation that I use in a softphone.
 * Implements buffering with play out point, catching up, noice 
 * when silent (tone in this example).
 * TODO: Implement exceptions, make sanity check of values set, 
 * test at other speeds, clean up, ...
 * --Erik Eliasson, eliasson@it.kth.se
 */

#include<config.h>

#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<libminisip/media/soundcard/SoundIO.h>
#include<libminisip/media/soundcard/SoundSource.h>

#include<libmutil/massert.h>
#include<signal.h>
#include<libmutil/Thread.h>

#include<libmutil/mtime.h>

#include<libminisip/media/soundcard/AudioMixerSpatial.h>
#include<libminisip/media/soundcard/AudioMixerSimple.h>
#include<libminisip/media/spaudio/SpAudio.h>

#ifdef AEC_SUPPORT
#	include<libminisip/media/aec/aec.h>
#endif

#ifdef _MSC_VER

#else
#	include<sys/time.h>
#	include<unistd.h>
#endif

#include<libminisip/media/soundcard/SoundDevice.h>

#define BS 160

using namespace std;

// //This object is created always, for now, even we do not 
// //use spatial audio ...
// SpAudio SoundIO::spAudio(5);

SoundIO::SoundIO(
		MRef<SoundDevice *> inputDevice,
		MRef<SoundDevice *> outputDevice,
		string mixerType,
		int nChannels_, 
		int32_t samplingRate_, 
		int format_): 
			nChannels(nChannels_),
			samplingRate(samplingRate_),
			format(format_),
			recording(false)
{
	soundDevIn = inputDevice;
	soundDevOut = outputDevice;

	setMixer( mixerType );
	/* Create the SoundPlayerLoop */
	start_sound_player();
	start_recorder();
}

SoundIO::~SoundIO(){
	while( recorder_callbacks.size() ){
		delete *( recorder_callbacks.begin() );
		recorder_callbacks.pop_back();
	}

}



void SoundIO::closeRecord(){
	if( soundDevIn ){
		soundDevIn->lockRead();
		soundDevIn->closeRecord();
		soundDevIn->unlockRead();
	}
}

void SoundIO::closePlayback(){
	if( soundDevOut ){
		soundDevOut->lockWrite();
		soundDevOut->closePlayback();
		soundDevOut->unlockWrite();
	}
}

void SoundIO::sync(){
	if( soundDevOut ){
		soundDevOut->sync();
	}
	if( soundDevIn ){
		soundDevIn->sync();
	}
}

void SoundIO::play_testtone( int secs ){
	
	int nSamples = secs * soundDevOut->getSamplingRate();
	short *data = (short*)malloc( nSamples * soundDevOut->getSampleSize() * soundDevOut->getNChannelsPlay() );
	for (int32_t i=0; i< nSamples; i++){
		if (i%4==0)data[i]=0;
		if (i%4==1)data[i]=10000;
		if (i%4==2)data[i]=0;
		if (i%4==3)data[i]=-10000;
	}
// 	byte_t *ptr = (byte_t *)data;
//	int32_t i=0;

	send_to_card( data, nSamples );
}

void SoundIO::openPlayback(){
	if( soundDevOut ){
		soundDevOut->lockWrite();
		if( !soundDevOut->isOpenedPlayback() ){
			soundDevOut->openPlayback( samplingRate, nChannels, format );
		}
		soundDevOut->unlockWrite();
	}

}

void SoundIO::openRecord(){
	if( soundDevIn ){
		soundDevIn->lockRead();
		if( !soundDevIn->isOpenedRecord() ){
			soundDevIn->openRecord( samplingRate, nChannels, format );
		}
		soundDevIn->unlockRead();
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

void SoundIO::unregisterRecorderReceiver( SoundRecorderCallback *callback ) {
	list<RecorderReceiver *>::iterator iter;
	for( iter = recorder_callbacks.begin();
		iter != recorder_callbacks.end();
		iter++ ) {
		if( (*iter)->getCallback() == callback ) {
			recorder_callbacks.erase( iter );
			return;
		}
	}
}

void SoundIO::set_recorder_buffer_size(int32_t bs){
	recorder_buffer_size=bs;
}

void SoundIO::fileIOTimeout(int ){
}

void *SoundIO::recorderLoop(void *sc_arg){
#ifdef DEBUG_OUTPUT
	setThreadName("SoundIO::recorderLoop");
#endif
	SoundIO *soundcard = (SoundIO *)sc_arg;
	int32_t i;
	short *buffers[2];	// Two sound buffers for "double buffering"
	massert(soundcard!=NULL);
	int32_t nread=0; /* IN SAMPLES */
	//FIXME
	soundcard->recorder_buffer_size = SOUND_CARD_FREQ*20/1000;
// 	printf( "SoundIO::recLoop - recorder_buff_size = %d\n", soundcard->recorder_buffer_size );
	
	for (i=0; i<2; i++){
		//buffers[i] = (short *)malloc(soundcard->recorder_buffer_size*sizeof(short)*2);
		buffers[i] = (short *)malloc(4096);
		memset(buffers[i],0,4096);
	}
		
	short * tempBuffer=NULL;
	#ifdef AEC_SUPPORT
	short * tempBufferR=NULL;		//hanning
	#endif
	bool tempBufferAllocated = false;

	while( true ){

		if( ! soundcard->recording ){
			if( soundcard->soundDevIn->isOpenedRecord() ){
				soundcard->closeRecord();
				if( tempBufferAllocated ){
					delete [] tempBuffer;
					#ifdef AEC_SUPPORT
					delete [] tempBufferR;		//hanning
					#endif
				}
				tempBuffer = NULL;
			}

			/* sleep until a recorder call back is added */
			soundcard->recorderCond.wait();
			
			if( ! soundcard->soundDevIn->isOpenedRecord() ){
				soundcard->openRecord();
// 				printf( "SoundIO::recLoop: openrecord channels = %d\n", soundcard->soundDevIn->getNChannelsRecord() );
			}

		}
		
		soundcard->soundDevIn->lockRead();
		if( soundcard->soundDevIn->isOpenedRecord() ){
				//soundcard->recorder_buffer_size is, for now, fixed to 960
				//		(SNDCARD_FREQ * 20 / 1000 )
				nread = soundcard->soundDevIn->read( 
						(byte_t *)buffers[i%2], 
						soundcard->recorder_buffer_size );
				//cerr <<"EEEE: SoundIO: read n="<<nread<<endl;
				//cerr <<"EEEE: SoundIO: data is"<<binToHex((unsigned char*)buffers[i%2], soundcard->recorder_buffer_size*2)<<endl;
		}
				
		soundcard->soundDevIn->unlockRead();

		if( nread < 0 ){
			continue;
		}

		if( soundcard->soundDevIn->getNChannelsRecord() > 1 ){
			if( !tempBuffer ){
				tempBuffer = new short[soundcard->recorder_buffer_size];
				#ifdef AEC_SUPPORT
				tempBufferR = new short[soundcard->recorder_buffer_size];	//hanning
				#endif
				tempBufferAllocated = true;
			}

			for( int j = 0; j < soundcard->recorder_buffer_size; j++ ){
				tempBuffer[j] = buffers[i%2][j * soundcard->soundDevIn->getNChannelsRecord() ];
				#ifdef AEC_SUPPORT
				tempBufferR[j] = buffers[i%2][j * soundcard->soundDevIn->getNChannelsRecord() + 1];	//hanning
				#endif
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
			//AudioMedia iimplements the callback ...
			for (list<RecorderReceiver *>::iterator 
                                    cb=soundcard->recorder_callbacks.begin(); 
                                    cb!= soundcard->recorder_callbacks.end(); 
                                    cb++){

				if ((*cb)!=NULL && (*cb)->getCallback()!=NULL){
					#ifdef AEC_SUPPORT
					(*cb)->getCallback()->srcb_handleSound(
							tempBuffer, 
							soundcard->recorder_buffer_size,
							tempBufferR); //hanning
					#else
					//cerr <<"EEEE: SoundIO: data is"<<binToHex((unsigned char*)tempBuffer, soundcard->recorder_buffer_size*2)<<endl;
					(*cb)->getCallback()->srcb_handleSound(
							tempBuffer, 
							soundcard->recorder_buffer_size,
							soundcard->samplingRate
							);
					#endif
					
					
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


void SoundIO::registerSource( MRef<SoundSource *> source ){
#ifdef DEBUG_OUTPUT
	cerr << "SoundIO::registerSource - Calling register source on created source " << source->getId() << endl;
#endif
//	int32_t j=1;
//	int32_t nextSize=sources.size()+1; 
	queueLock.lock();
	for (list<MRef<SoundSource *> >::iterator i=sources.begin(); 
				i!= sources.end(); 
				i++){
		if (source->getId()==(*i)->getId()){
			sourceListCond.broadcast();
			queueLock.unlock();
			return;
		}
//		(*i)->setPos(spAudio.assignPos(j,nextSize));
//		(*i)->initLookup(nextSize);
//		j++;
	}
//         source->setPos( spAudio.assignPos(j,nextSize) );
	//sources.push_front(source);
	sources.push_back(source);
	mixer->setSourcesPosition( sources, true ); //added sources

	sourceListCond.broadcast();
	queueLock.unlock();
}


void SoundIO::unregisterSource(int sourceId){
	
#ifdef DEBUG_OUTPUT
	cerr << "SoundIO::unregisterSource - Calling unregister source on source " << sourceId << endl;
#endif
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
// 	int32_t nextSize=sources.size();
// 	int32_t j =1;
// 	for (i = sources.begin(); 
// 			i!=sources.end(); 
// 			i++/*, j++*/){
//		(*i)->setPos(spAudio.assignPos(j,nextSize));
//		(*i)->initLookup(nextSize);
//	}
	mixer->setSourcesPosition( sources, false );//removed sources
	queueLock.unlock();
}

void SoundIO::send_to_card(short *buf, int32_t n_samples){
	byte_t *ptr = (byte_t *)buf;
	int32_t nWritten;

	soundDevOut->lockWrite();
	if( soundDevOut->isOpenedPlayback() ){
		nWritten = soundDevOut->write( ptr, n_samples );
	}
	soundDevOut->unlockWrite();
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
#ifdef DEBUG_OUTPUT
	setThreadName("SoundIO::playerLoop");
#endif
	SoundIO *soundcard = (SoundIO *)arg;

	short *outbuf = NULL;
	uint32_t nChannels = 0;

	if( soundcard->getMixer().isNull() ) {
		cerr << "Error: Sound I/O ... mixer is null ... stopping the thread!!!" << endl;
		return NULL;
	}
	
#ifdef DEBUG_OUTPUT
	//uint32_t counter = 0;
#endif
	while( true ){

                soundcard->queueLock.lock();
		if( soundcard->sources.size() == 0 ){
			if( soundcard->soundDevOut->isOpenedPlayback() ){
				soundcard->closePlayback();
			}
			
			/* Wait for someone to add a source */
			soundcard->sourceListCond.wait( soundcard->queueLock );

			soundcard->openPlayback();
			nChannels = soundcard->soundDevOut->getNChannelsPlay();
			soundcard->mixer->init(nChannels);
		}

		outbuf = soundcard->mixer->mix( soundcard->sources );
 		
		soundcard->queueLock.unlock();

		if( soundcard->soundDevOut->isOpenedPlayback() ){
			soundcard->send_to_card(outbuf, soundcard->mixer->getFrameSize());
		}
		
	}
	return NULL;
}

void SoundIO::start_sound_player(){
        Thread::createThread(playerLoop, this);
}

MRef<AudioMixer *> SoundIO::getMixer() { 
	return mixer;
}

bool SoundIO::setMixer( string type ) {
	bool ret = true;
	if( type == "simple" ) {
#ifdef DEBUG_OUTPUT
		cout << "Sound I/O: using Simple Mixer" << endl;
#endif
		mixer = new AudioMixerSimple();
	} else if( type == "spatial" ) {
#ifdef DEBUG_OUTPUT
		cout << "Sound I/O: using Spatial Audio Mixer" << endl;
#endif
	 	MRef<SpAudio *> spatial = new SpAudio( 5 );
	 	spatial->init();
		mixer = new AudioMixerSpatial(spatial);
	} else {
		cerr << "ERROR: SoundIO could not create requested mixer! (type _" << type << "_ not understood)" << endl;
		cerr << "ERROR:      Creating Spatial Audio mixer instead." << endl;
		setMixer( "spatial" );
		ret = false;
	}
	return ret;
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


