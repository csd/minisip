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
 *	    Cesc Santasusana <c e s c DOT s a n t a [AT} g m a i l DOT c o m>
*/

#include<config.h>

#include"OssSoundDevice.h"

#ifndef DISABLE_OSS
#include<config.h>

#include<libmutil/merror.h>

#include<string.h>
#include<unistd.h>
#include<errno.h>
// #include <sys/time.h>
// #include <time.h>

#define PLAYOUT_FRAGMENT_SETTINGS 0x0002000C
#define RECORD_FRAGMENT_SETTINGS  0x0014000C

#define OPEN_OSS_IN_NON_BLOCKING_MODE true

using namespace std;

OssSoundDevice::OssSoundDevice( string device ):SoundDevice( device ){
	
	fdPlayback = -1;
	fdRecord = -1;
}

int OssSoundDevice::openPlayback( int32_t samplingRate, int nChannels, int format ){
	this->nChannelsPlay = nChannels;

	if( isOpenedPlayback() ){
		return 0;
	}
	
	int mode = O_WRONLY; 	
	/* FIXME */
	this->fragment_setting = PLAYOUT_FRAGMENT_SETTINGS;
	
	fdPlayback = ::open( dev.c_str(), mode | O_NONBLOCK );
	
	if( fdPlayback == -1 ){
		merr << "Could not open the sound device " << dev << 
			" for playback: "
			<< strerror( errno ) << endl;
		return -1;
	}

	bool openNonBlocking = OPEN_OSS_IN_NON_BLOCKING_MODE;
	
	if( openNonBlocking ) {
		sleepTime = 20; //min time between calls ... simulated
#ifdef DEBUG_OUTPUT
		cerr << "OSS: opening playback in non-blocking mode" << endl;
#endif
	} else {
		int flags = fcntl( fdPlayback, F_GETFL );
		sleepTime = 0;
		// Remove O_NONBLOCK
		flags &= ~O_NONBLOCK;
		fcntl( fdPlayback, F_SETFL, flags );
#ifdef DEBUG_OUTPUT
		cerr << "OSS: opening playback in blocking mode" << endl;
#endif
	}
	
	if( ioctl( fdPlayback, SNDCTL_DSP_SETFRAGMENT, &fragment_setting ) == -1 ){
		#ifdef DEBUG_OUTPUT	
		merror( "ioctl, SNDCTL_DSP_SETFRAGMENT (set buffer size)" );
		#endif
	}

/*
	if( channels != this->nChannelsPlay - 1 ){
		cerr << "ERROR: could not set to stereo- running mono"<< endl;
		if( this->nChannelsPlay == 2 ) channels = 0;  
		else channels = 1;
		if( ioctl( fdPlayback, SNDCTL_DSP_STEREO, &channels ) == -1 ){
			merror("ioctl, SNDCTL_DSP_STEREO (tried to fallback)");
		}

	}
*/


	setFormat( format );
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
		#ifdef DEBUG_OUTPUT	
		merror( "ioctl, SNDCTL_DSP_SETFMT (failed to set format to AFMT_S16_LE)" );
		#endif
	}

#ifdef DEBUG_OUTPUT
	cerr << "OSSSoundDevice format set to" << ossFormat << endl;
#endif
	
	int channels = nChannels;
	this->nChannelsPlay = nChannels;
	
	if( ioctl( fdPlayback, SNDCTL_DSP_CHANNELS, &channels ) == -1 ){
		#ifdef DEBUG_OUTPUT	
		merror("ioctl, SNDCTL_DSP_CHANNELS (tried to set channels number)");
		#endif
	}
	
#ifdef DEBUG_OUTPUT
	cerr << "OssSoundDevice: number of channels set to "<< channels << endl;
#endif

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
		#ifdef DEBUG_OUTPUT	
		merror( "ioctl, SNDCTL_DSP_SPEED (tried to set sample rate to 8000)" );
		#endif
	}

	this->samplingRate = setSpeed;
	
#ifdef DEBUG_OUTPUT
	cerr << "OSSDevice: DSP speed set to "<< this->samplingRate << endl;
#endif

	openedPlayback = true;
	return 0;

}

int OssSoundDevice::openRecord( int32_t samplingRate, int nChannels, int format ){

	this->nChannelsRecord = nChannels;
	if( isOpenedRecord() ){
		return 0;
	}
	
	int mode = O_RDONLY; /*duplex ? O_RDWR : O_WRONLY;*/
	/* FIXME */
	this->fragment_setting = RECORD_FRAGMENT_SETTINGS;
	
	fdRecord = ::open( dev.c_str(), mode | O_NONBLOCK );
	
	if( fdRecord == -1 ){
		#ifdef DEBUG_OUTPUT
		merr << "Could not open the sound device " << dev << 
			" for recording: "
			<< strerror( errno ) << endl;
		#endif
		return -1;
	}

	// Remove O_NONBLOCK
	int flags = fcntl( fdRecord, F_GETFL );
	flags &= ~O_NONBLOCK;
	fcntl( fdRecord, F_SETFL, flags );
	
	if( ioctl( fdRecord, SNDCTL_DSP_SETFRAGMENT, &fragment_setting ) == -1 ){
		#ifdef DEBUG_OUTPUT
		merror( "ioctl, SNDCTL_DSP_SETFRAGMENT (set buffer size)" );
		#endif
	}
	
	//int channels = 1;
	int channels = nChannels;
	//this->nChannels = nChannels;
	
	if( ioctl( fdRecord, SNDCTL_DSP_CHANNELS, &channels ) == -1 ){
		#ifdef DEBUG_OUTPUT
		merror("ioctl, SNDCTL_DSP_CHANNELS");
		#endif
	}
	
	#ifdef DEBUG_OUTPUT
	cerr << "OssSoundDevice: number of channels set to "<< channels << endl;
	#endif

	this->nChannelsRecord = channels;

	setFormat( format );
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
		merror( "ioctl, SNDCTL_DSP_SETFMT (failed to set format to AFMT_S16_LE)" );
	}

	int setSpeed;

#ifdef IPAQ
	// The iPAQ h5550 is known not to support 8kHz, we use 16kHz and
	// resample
	if( samplingRate == 8000 ){
		setSpeed = 16000;
		cerr << "Enabling iPAQ frequency workaround" << endl;
	}
	else
#endif
	setSpeed = samplingRate;
	
	if( ioctl( fdRecord, SNDCTL_DSP_SPEED, &setSpeed ) == -1 ){
		#ifdef DEBUG_OUTPUT
		merror( "ioctl, SNDCTL_DSP_SPEED (tried to set sample rate to 8000)" );
		#endif
	}

	this->samplingRate = setSpeed;
	
#ifdef DEBUG_OUTPUT
	cerr << "DSP speed set to "<< this->samplingRate << endl;
#endif

	openedRecord = true;
	return 0;

}

int OssSoundDevice::closePlayback(){
	if( !openedPlayback || fdPlayback == -1 ){
		#ifdef DEBUG_OUTPUT
		cerr << "WARNING: doing close on already "
			"closed sound card"<< endl;
		#endif
		return -1;
	}

	::close( fdPlayback );
	fdPlayback = -1;
	openedPlayback = false;
	return 0;
}

int OssSoundDevice::closeRecord(){
#ifdef DEBUG_OUTPUT
	cerr << "OSS: Closing sound card for recording" << endl;
#endif
	if( !openedRecord || fdRecord == -1 ){
		#ifdef DEBUG_OUTPUT
		cerr << "WARNING: doing close on already "
			"closed sound card"<< endl;
		#endif
		return -1;
	}

	::close( fdRecord );
	fdRecord = -1;
	openedRecord = false;
	return 0;
}

int OssSoundDevice::readFromDevice( byte_t * buffer, uint32_t nSamples ){

	int nReadBytes = 0;
	int nBytesToRead = nSamples * getSampleSize() * getNChannelsRecord();
	int totalSamplesRead = 0;
	
	if( fdRecord == -1 ){
		return -1;
	}

	nReadBytes = ::read( fdRecord, 
				buffer, 
				nBytesToRead );

	if( nReadBytes >= 0 ){
		totalSamplesRead = nReadBytes / ( getSampleSize() * getNChannelsRecord() );
	} else {
		totalSamplesRead = -errno;
	}
	return totalSamplesRead;
}

int OssSoundDevice::writeToDevice( byte_t * buffer, uint32_t nSamples ){

	int nWrittenBytes = 0;
	int nBytesToWrite = nSamples * getSampleSize() * getNChannelsPlay();
	int totalSamplesWritten = 0;

	if( fdPlayback == -1 ){
		return -1;
	}

	nWrittenBytes = ::write( fdPlayback, 
				buffer, 
				nBytesToWrite );
	
	if( nWrittenBytes >= 0 ) {
		//convert back to samples ... 
		totalSamplesWritten = nWrittenBytes / ( getSampleSize() * getNChannelsPlay() );
	} else {
		totalSamplesWritten = -errno;
	}
	return totalSamplesWritten;
}

int OssSoundDevice::readError( int errcode, byte_t * buffer, uint32_t nSamples ) {
	bool mustReturn = true;
	switch( errcode ) {
		case -EAGAIN:
		case -EINTR:
			mustReturn = false;
			break;
		default:
			mustReturn = true;
			break;
	}
	if( mustReturn ) { return -1; }
	else { return 0; } 
}

int OssSoundDevice::writeError( int errcode, byte_t * buffer, uint32_t nSamples ) {
	bool mustReturn = true;
	switch( errcode ) {
		case -EAGAIN:
		case -EINTR:
			mustReturn = false;
			break;
		default:
			mustReturn = true;
			break;
	}
	if( mustReturn ) { return -1; }
	else { return 0; } 
}

void OssSoundDevice::sync(){
	bool interrupted = false;
	do{
		interrupted = false;
		if( ioctl( fdPlayback, SNDCTL_DSP_SYNC ) == -1 ){
			#ifdef DEBUG_OUTPUT
			merror( "ioctl sync error on soundcard" );
			#endif
			interrupted=true;
		}
	}while( interrupted && errno==EINTR );
}


#endif //DISABLE_OSS
