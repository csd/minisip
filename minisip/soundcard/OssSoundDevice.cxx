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

#include"OssSoundDevice.h"

#ifndef DISABLE_OSS
#include<config.h>

#include<unistd.h>
#include<errno.h>
#include <sys/time.h>
#include <time.h>

using namespace std;

OssSoundDevice::OssSoundDevice( string device ):SoundDevice( device ){
	
	fdPlayback = -1;
	fdRecord = -1;
}

int OssSoundDevice::openPlayback( int32_t samplingRate, int nChannels, int format ){

	if( isOpenedPlayback() ){
		return 0;
	}

	int mode = O_WRONLY; 	
	/* FIXME */
	this->fragment_setting = 0x00040008;
	
	fdPlayback = ::open( dev.c_str(), mode );
	
	if( fdPlayback == -1 ){
		perror(("open "+dev).c_str());
		exit(-1); //FIX: handle nicer - exception
	}
	
	if( ioctl( fdPlayback, SNDCTL_DSP_SETFRAGMENT, &fragment_setting ) == -1 ){
		perror( "ioctl, SNDCTL_DSP_SETFRAGMENT (set buffer size)" );
	}

/*
	if( channels != this->nChannelsPlay - 1 ){
		cerr << "ERROR: could not set to stereo- running mono"<< endl;
		if( this->nChannelsPlay == 2 ) channels = 0;  
		else channels = 1;
		if( ioctl( fdPlayback, SNDCTL_DSP_STEREO, &channels ) == -1 ){
			perror("ioctl, SNDCTL_DSP_STEREO (tried to fallback)");
		}

	}
*/


	this->format = format;
	int ossFormat = format;
	
	switch( format ){
		case SOUND_S16LE:
			ossFormat = AFMT_S16_LE;
			break;
		case SOUND_S16BE:
			ossFormat = AFMT_S16_BE;
			break;
		case SOUND_U16LE:
			ossFormat = AFMT_U16_LE;
			break;
		case SOUND_U16BE:
			ossFormat = AFMT_U16_BE;
	}
			
	if( ioctl( fdPlayback, SNDCTL_DSP_SETFMT, &ossFormat ) == -1 ){
		perror( "ioctl, SNDCTL_DSP_SETFMT (failed to set format to AFMT_S16_LE)" );
	}

	cerr << "OSSSoundDevice format set to" << ossFormat << endl;
	
	
	int channels = nChannels;
	this->nChannelsPlay = nChannels;
	
	if( ioctl( fdPlayback, SNDCTL_DSP_CHANNELS, &channels ) == -1 ){
		perror("ioctl, SNDCTL_DSP_CHANNELS (tried to set channels number)");
	}
	
	cerr << "OssSoundDevice: number of channels set to "<< channels << endl;

	this->nChannelsPlay = channels;
	

	
	int setSpeed;
	
	/* remove because of the use of spatial audio

#ifdef IPAQ
	// The iPAQ h5550 is known not to support 8kHz, we use 16kHz and
	// resample
	if( samplingRate == 8000 ){
		setSpeed = 16000;
		cerr << "Enabling iPAQ frequency workaround" << endl;
	}
	else
#endif
	*/
	setSpeed = samplingRate;
	
	if( ioctl( fdPlayback, SNDCTL_DSP_SPEED, &setSpeed ) == -1 ){
		perror( "ioctl, SNDCTL_DSP_SPEED (tried to set sample rate to 8000)" );
	}

	this->samplingRate = setSpeed;
	
	cerr << "DSP speed set to "<< this->samplingRate << endl;

	openedPlayback = true;
	return 0;

}

int OssSoundDevice::openRecord( int32_t samplingRate, int nChannels, int format ){

	if( isOpenedRecord() ){
		return 0;
	}

	int mode = O_RDONLY; /*duplex ? O_RDWR : O_WRONLY;*/
	/* FIXME */
	this->fragment_setting = 0x00140008;
	
	fdRecord = ::open( dev.c_str(), mode );
	
	if( fdRecord == -1 ){
		perror(("open "+dev).c_str());
		exit(-1); //FIX: handle nicer - exception
	}
	
	if( ioctl( fdRecord, SNDCTL_DSP_SETFRAGMENT, &fragment_setting ) == -1 ){
		perror( "ioctl, SNDCTL_DSP_SETFRAGMENT (set buffer size)" );
	}
	
	//int channels = 1;
	int channels = nChannels;
	//this->nChannels = nChannels;
	
	if( ioctl( fdRecord, SNDCTL_DSP_CHANNELS, &channels ) == -1 ){
		perror("ioctl, SNDCTL_DSP_CHANNELS");
	}
	
	cerr << "OssSoundDevice: number of channels set to "<< channels << endl;

	this->nChannelsRecord = channels;



	this->format = format;
	int ossFormat = format;
	
	switch( format ){
		case SOUND_S16LE:
			ossFormat = AFMT_S16_LE;
			break;
		case SOUND_S16BE:
			ossFormat = AFMT_S16_BE;
			break;
		case SOUND_U16LE:
			ossFormat = AFMT_U16_LE;
			break;
		case SOUND_U16BE:
			ossFormat = AFMT_U16_BE;
	}
			
	if( ioctl( fdRecord, SNDCTL_DSP_SETFMT, &ossFormat ) == -1 ){
		perror( "ioctl, SNDCTL_DSP_SETFMT (failed to set format to AFMT_S16_LE)" );
	}

	int setSpeed;

	/*
#ifdef IPAQ
	// The iPAQ h5550 is known not to support 8kHz, we use 16kHz and
	// resample
	if( samplingRate == 8000 ){
		setSpeed = 16000;
		cerr << "Enabling iPAQ frequency workaround" << endl;
	}
	else
#endif
	*/
	setSpeed = samplingRate;
	
	if( ioctl( fdRecord, SNDCTL_DSP_SPEED, &setSpeed ) == -1 ){
		perror( "ioctl, SNDCTL_DSP_SPEED (tried to set sample rate to 8000)" );
	}

	this->samplingRate = setSpeed;
	
	cerr << "DSP speed set to "<< this->samplingRate << endl;

	openedRecord = true;
	return 0;

}

int OssSoundDevice::closePlayback(){
	if( !openedPlayback || fdPlayback == -1 ){
		cerr << "WARNING: doing close on already "
			"closed sound card"<< endl;
		return -1;
	}

	::close( fdPlayback );
	fdPlayback = -1;
	openedPlayback = false;
	return 0;
}

int OssSoundDevice::closeRecord(){
	cerr << "Closing sound card for recording" << endl;
	if( !openedRecord || fdRecord == -1 ){
		cerr << "WARNING: doing close on already "
			"closed sound card"<< endl;
		return -1;
	}

	::close( fdRecord );
	fdRecord = -1;
	openedRecord = false;
	return 0;
}

int OssSoundDevice::read( byte_t * buffer, uint32_t nSamples ){

	int nReadBytes = 0;
	int totalBytesRead = 0;
//	struct timeval tv, tv2;
//	struct timezone tz;

        //FIXME: Hack to make sampleSize correct - I don't understand the
        //getSampleSize method... -Erik
        sampleSize=2;               
        
        
	int nBytesToRead = nSamples * sampleSize * nChannelsRecord;
	
	if( fdRecord == -1 ){
		return -1;
	}

	while( totalBytesRead < nBytesToRead ){

		nReadBytes = ::read( fdRecord, buffer, nBytesToRead - totalBytesRead );

		if( nReadBytes < 0 ){
			if( ioctl( fdRecord, SNDCTL_DSP_SYNC ) == -1 ){
				perror( "ioctl sync error on soundcard" );
			}
			nReadBytes = ::read( fdRecord, buffer, nBytesToRead - totalBytesRead );

			if( nReadBytes < 0 ){
				perror( "read" );
				return -1;
			}
		}

		totalBytesRead += nReadBytes;
	}
	return totalBytesRead;
}

int OssSoundDevice::write( byte_t * buffer, uint32_t nSamples ){

	int nWrittenBytes = 0;
	int totalBytesWritten = 0;
	int nBytesToWrite = nSamples * getSampleSizePlay();

	if( fdPlayback == -1 ){
		return -1;
	}

	while( totalBytesWritten < nBytesToWrite ){
		nWrittenBytes = ::write( fdPlayback, buffer, nBytesToWrite - totalBytesWritten );

		if( nWrittenBytes < 0 ){
			/* FIXME */
			cerr << "Error while writing to soundcard" << endl;
			return -1;
		}

		totalBytesWritten += nWrittenBytes;
	}

	return totalBytesWritten;
}
	
void OssSoundDevice::sync(){
	bool interrupted = false;
	do{
		interrupted = false;
		if( ioctl( fdPlayback, SNDCTL_DSP_SYNC ) == -1 ){
			perror( "ioctl sync error on soundcard" );
			interrupted=true;
		}
	}while( interrupted && errno==EINTR );
}


#endif //DISABLE_OSS
