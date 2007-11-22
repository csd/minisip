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

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *	    Cesc Santasusana <c e s c DOT s a n t a [AT} g m a i l DOT c o m>
*/

#include<config.h>

#include<libminisip/media/soundcard/SoundDevice.h>
#include<libminisip/media/soundcard/SoundDriverRegistry.h>

#ifdef WAVE_SOUND
#	include"WaveSoundDevice.h"
#endif

#include<stdio.h>

#include<libmutil/Thread.h>
#include<libmutil/mtime.h>

using namespace std;

MRef<SoundDevice *> SoundDevice::create( string devideId ){
	if( devideId == "" ){
		return NULL;
	}

	MRef<SoundDevice*> device = SoundDriverRegistry::getInstance()->createDevice( devideId );
	if( device ){
		return device;
	}

#ifdef WAVE_SOUND
#define SOUND_DEVICE_IMPLEMENTED
	if( devideId.substr( 0, 5 ) == "wave:" ){
		return new WaveSoundDevice( devideId.substr( 5, string::npos ) );
	}
		
#endif
	return NULL;
}

SoundDevice::SoundDevice( string device ):openedRecord(false),openedPlayback(false){
	dev = device;
	setSleepTime( 20 );
}

SoundDevice::~SoundDevice()
{

}

void SoundDevice::lockRead(){
	mLockRead.lock();
}


void SoundDevice::lockWrite(){
	mLockWrite.lock();
}

void SoundDevice::unlockRead(){
	mLockRead.unlock();
}

void SoundDevice::unlockWrite(){
	mLockWrite.unlock();
}

void SoundDevice::setFormat( int format_ ) {
	switch( format_ ) {
		case SOUND_S16LE: 
		case SOUND_S16BE: 
		case SOUND_U16LE: 
		case SOUND_U16BE: 
			sampleSize = 2;
			this->format = format_;
			break;
		case SOUND_S8LE: 
		case SOUND_U8LE: 
			sampleSize = 1;
			this->format = format_;
			break;
		case SOUND_S32LE: 
		case SOUND_U32LE: 
			sampleSize = 4;
			this->format = format_;
			break;
		default: 
			cerr << "SoundDevice::setFormat - format not understood!" << endl;
			break;
	}
}

int SoundDevice::read( byte_t * buffer, uint32_t nSamples ){

	int nSamplesRead = 0;
	int totalSamplesRead = 0;
	
	if( !openedRecord ) {
		return -1;
	}
	
	byte_t * byteBuffer = buffer;

	while( (uint32_t)totalSamplesRead < nSamples ){
		nSamplesRead = readFromDevice( byteBuffer, 
						nSamples - totalSamplesRead );
		if( nSamplesRead >= 0 ) {
// 			fprintf( stderr, "nSamplesRead %d\n", nSamplesWritten );
			byteBuffer += nSamplesRead * getSampleSize() * getNChannelsPlay();
			totalSamplesRead += nSamplesRead;
		} else {
			nSamplesRead = readError( nSamplesRead,
						byteBuffer, 
						nSamples - totalSamplesRead );
			if( nSamplesRead < 0 ) { return -1; }
			else { continue; }
		}
	}
	return totalSamplesRead;
}


int SoundDevice::write( byte_t * buffer, uint32_t nSamples ){

	byte_t * byteBuffer = buffer;

	if( !openedPlayback ) {
		return -1;
	}
	
	int nSamplesWritten = 0;
	int totalSamplesWritten = 0;

	//timed access ..
	uint64_t currentTime;
	static uint64_t lastTimeWrite = 0;
	
	if( sleepTime > 0 ) {
		currentTime = mtime();
		if( lastTimeWrite == 0 ) {
			lastTimeWrite = currentTime - sleepTime; //init last time we wrote ... 
			
	#ifdef DEBUG_OUTPUT
			printf( "nsamples = %d\n\n", nSamples );
	#endif
	
		} else if( (currentTime - lastTimeWrite) > sleepTime*10 ) {
	
	#ifdef DEBUG_OUTPUT
			printf( "SoundDevice: resetting lastTimeWrite! +++++++++++++++++++++++++++++ \n\n");
	#endif
			
			lastTimeWrite = currentTime - sleepTime;
		}
		
		int64_t sleep = sleepTime - (currentTime-lastTimeWrite);
	// 	printf( "\n\nsleep = %d\n", sleep );
		while ( sleep > 0 ){
			Thread::msleep( (int32_t)sleep );
			currentTime = mtime();
			sleep = sleepTime - (currentTime-lastTimeWrite);
		}
		lastTimeWrite += sleepTime;
	}
	
	while( (uint32_t)totalSamplesWritten < nSamples ){
		nSamplesWritten = writeToDevice( byteBuffer, 
						nSamples - totalSamplesWritten );

		if( nSamplesWritten >= 0 ) {
// 			fprintf( stderr, "nSamplesWritten %d\n", nSamplesWritten );
			byteBuffer += nSamplesWritten * getSampleSize() * getNChannelsPlay();
			totalSamplesWritten += nSamplesWritten;
		} else {
			nSamplesWritten = writeError( nSamplesWritten,
						byteBuffer, 
						nSamples - totalSamplesWritten );
			if( nSamplesWritten < 0 ) { return -1; }
			else { continue; }
		}
	}
	return totalSamplesWritten;
}

