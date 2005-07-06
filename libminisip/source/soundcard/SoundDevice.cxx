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


/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<config.h>

#include<libminisip/SoundDevice.h>
#include<libminisip/FileSoundDevice.h>
#ifndef WIN32
#include<libminisip/OssSoundDevice.h>
#endif

#ifdef HAVE_LIBASOUND
#include<libminisip/AlsaSoundDevice.h>
#endif

#ifdef DSOUND
#include<libminisip/DirectSoundDevice.h>
#endif

#include<stdio.h>

using namespace std;

MRef<SoundDevice *> SoundDevice::create( string devideId ){
	if( devideId == "" ){
		return NULL;
	}

	if( devideId.substr( 0, 5 ) == "file:" ){
		size_t comaPos = devideId.find( "," );
		if( comaPos == string::npos ){
			merr << "Invalid file sound device specified in"
				"configuration file."
				"Sound will be disabled." << end;
			return NULL;
		}
		return new FileSoundDevice( 
				devideId.substr( 5, comaPos-5 ),
				devideId.substr( comaPos + 1, string::npos ),
				2, 8000 );
	}

#ifdef HAVE_LIBASOUND
	if( devideId.substr( 0, 5 ) == "alsa:" ){
		return new AlsaSoundDevice( devideId.substr( 5, string::npos ) );
	}
#endif
	
#ifdef DSOUND
	if( devideId.substr( 0, 7 ) == "dsound:" ){
		return new DirectSoundDevice( devideId.substr( 7, string::npos ) );
	}
#endif

#ifndef WIN32
	return new OssSoundDevice( devideId );
#else
	return NULL;
#endif
}

SoundDevice::SoundDevice( string device ):openedRecord(false),openedPlayback(false){

	
//	pthread_mutex_init( &mLockRead, NULL );
//	pthread_mutex_init( &mLockWrite, NULL );

	dev = device;

	
}

SoundDevice::~SoundDevice()
{

}

void SoundDevice::lockRead(){
	//pthread_mutex_lock( &mLockRead );
        mLockRead.lock();
}


void SoundDevice::lockWrite(){
	//pthread_mutex_lock( &mLockWrite );
        mLockWrite.lock();
}

void SoundDevice::unlockRead(){
	//pthread_mutex_unlock( &mLockRead );
        mLockRead.unlock();
}

void SoundDevice::unlockWrite(){
	//pthread_mutex_unlock( &mLockWrite );
        mLockWrite.unlock();
}

int SoundDevice::getSampleSizePlay(){
//	fprintf( stderr, "format: %x\n",format );
//	fprintf( stderr, "nChannels: %d\n", nChannels );
	if( ( format & 0xF0 ) == 0xF0 ){
		return 2 * nChannelsPlay;
	}

	else{
		return 0;
	}
}

int SoundDevice::getSampleSizeRecord(){
//	fprintf( stderr, "format: %x\n",format );
//	fprintf( stderr, "nChannels: %d\n", nChannels );
	if( ( format & 0xF0 ) == 0xF0 ){
		return 2 * nChannelsRecord;
	}

	else{
		return 0;
	}
}
