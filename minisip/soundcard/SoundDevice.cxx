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

#include<config.h>

#include"SoundDevice.h"
#include"FileSoundDevice.h"
#ifndef WIN32
#include"OssSoundDevice.h"
#endif

#ifdef HAVE_LIBASOUND
#include"AlsaSoundDevice.h"
#endif

#ifdef DSOUND
#include"DirectSoundDevice.h"
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
				FILESOUND_TYPE_RAW );
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
	dev = device;
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

#if 0
int SoundDevice::getSampleSizePlay(){
	if( ( format & 0xF0 ) == 0xF0 ){
		return 2 * nChannelsPlay;
	} else {
		return 0;
	}
}

int SoundDevice::getSampleSizeRecord(){
	if( ( format & 0xF0 ) == 0xF0 ){
		return 2 * nChannelsRecord;
	} else {
		return 0;
	}
}
#endif

