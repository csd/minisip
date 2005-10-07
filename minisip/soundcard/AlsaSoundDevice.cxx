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
 *	    Cesc Santasusana <c e s c DOT s a n t a [AT} g m a i l DOT c o m>
*/

#include"AlsaSoundDevice.h"

#include<libmutil/Thread.h>
#include<libmutil/mtime.h>

using namespace std;


AlsaSoundDevice::AlsaSoundDevice( string device ):SoundDevice( device ){

	readHandle = NULL;
	writeHandle = NULL;
}



int AlsaSoundDevice::closePlayback(){
	if( !openedPlayback ){
#ifdef DEBUG_OUTPUT
		cerr << "WARNING: doing close on already "
			"closed sound card (ALSA)"<< endl;
#endif
		return -1;
	}

	if( writeHandle != NULL ){
		snd_pcm_close( writeHandle );
		writeHandle = NULL;
	}

	openedPlayback = false;
	return 1;
}

int AlsaSoundDevice::closeRecord(){
	if( !openedRecord ){
		cerr << "WARNING: doing close on already "
			"closed sound card (ALSA)"<< endl;
		return -1;
	}

	if( readHandle != NULL ){
		snd_pcm_close( readHandle );
		readHandle = NULL;
	}

	openedRecord = false;
	return 1;
}


int AlsaSoundDevice::openPlayback( int samplingRate, int nChannels, int format ){

	snd_pcm_hw_params_t *hwparams;
	snd_pcm_sw_params_t *swparams;
	snd_pcm_hw_params_alloca(&hwparams);
	snd_pcm_sw_params_alloca(&swparams);
	
	// Play ...
	
 	if (snd_pcm_open(&writeHandle, dev.c_str(), SND_PCM_STREAM_PLAYBACK,  SND_PCM_NONBLOCK)<0){
//	if (snd_pcm_open(&writeHandle, dev.c_str(), SND_PCM_STREAM_PLAYBACK,  0/*SND_PCM_NONBLOCK*/)<0){
		cerr << "Could not open ALSA sound card" << endl;
		exit(-1);
	}
	
	if (snd_pcm_hw_params_any(writeHandle, hwparams) < 0) {
		cerr << "Could not get ALSA sound card parameters" << endl;
		exit(-1);
	}

	if (snd_pcm_hw_params_set_access(writeHandle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
		cerr << "Could not set ALSA mode" << endl;
		exit(-1);
	}
	
	if (snd_pcm_hw_params_set_channels(writeHandle, hwparams, nChannels)<0){
		cerr << "Cound not configure ALSA for playout on "<<  nChannels<< endl;
		exit(-1);
	}

	this->nChannelsPlay = nChannels;

	_snd_pcm_format alsaFormat;
	setFormat( format );
	
	switch( format ){
		case SOUND_S16LE:
			alsaFormat = SND_PCM_FORMAT_S16_LE;
			break;
		case SOUND_S16BE:
			alsaFormat = SND_PCM_FORMAT_S16_BE;
			break;
		case SOUND_U16LE:
			alsaFormat = SND_PCM_FORMAT_U16_LE;
			break;
		case SOUND_U16BE:
			alsaFormat = SND_PCM_FORMAT_U16_BE;
			break;
		default:
			cerr << "Unhandled sound format (ALSA)" << endl;
			exit( -1 );
	}

	if (snd_pcm_hw_params_set_format(writeHandle, hwparams, 
				alsaFormat) < 0) {
		cerr << "Could not set ALSA format" << endl;
		exit(-1);
	}
	
	unsigned int wantedSamplingRate = (unsigned int)samplingRate; 

	if( snd_pcm_hw_params_set_rate_near(writeHandle, hwparams, (unsigned int *)&samplingRate, NULL) < 0){
		cerr << "Could not set ALSA rate" << endl;
		exit(-1);
	}

	if( (unsigned int)samplingRate != wantedSamplingRate  ){
#ifdef DEBUG_OUTPUT
		cerr << "ALSA: Could not set chosen rate of " << wantedSamplingRate << ", set to "<< samplingRate <<endl;
#endif
	}
	this->samplingRate = samplingRate;
	
	unsigned long periodSize = 0;
	int32_t dirPeriod;
	if( snd_pcm_hw_params_get_period_size_min (hwparams, &periodSize, &dirPeriod) < 0 ) {
		cerr << "Could not get ALSA period size" << endl;
	} else {
		cerr << "AlsaPlayback min period size is (alsaFrames) =  " << periodSize << endl;
	}
	printf( "alsa playback dir = %d\n", dirPeriod );
	uint32_t numPeriods;
	numPeriods =  snd_pcm_hw_params_set_periods (writeHandle, hwparams, 1, 0);
	if( numPeriods < 0 ) {
		cerr << "Could not get ALSA periods" << endl;
	} else {
		cerr << "AlsaPlayback periods set to (numPeriodsPerBuffer) =  1" << numPeriods << endl;
	}
	
	periodSize = snd_pcm_hw_params_set_period_size(writeHandle, hwparams, 960 *2, 0);
	if( periodSize < 0) {
		cerr << "Could not set ALSA period size" << endl;
	} else {
		cerr << "AlsaPlayback period size is (alsaFrames) = 64 " << periodSize << endl;
	}

	uint32_t min_buffer_size;
	int32_t dir;

	if (snd_pcm_hw_params_get_buffer_time_min(hwparams,
				&min_buffer_size, &dir) < 0){
#ifdef DEBUG_OUTPUT
		cerr << "Could not get ALSA min buffer time" << endl;
#endif
	}

	/* Some soundcards seems to be a bit optimistic */

#ifdef DEBUG_OUTPUT
	cerr << "ALSA Hardware playout buffer size: (microsecs) " << min_buffer_size << endl;
#endif
	if (min_buffer_size < MIN_HW_PO_BUFFER){
		min_buffer_size = MIN_HW_PO_BUFFER;
	}

#ifdef DEBUG_OUTPUT
	cerr << "ALSA Hardware playout buffer size: (microsecs) " << min_buffer_size << endl;
#endif

	if (snd_pcm_hw_params_set_buffer_time(writeHandle, hwparams, 
				min_buffer_size, dir) < 0) {
		cerr << "Could not set ALSA buffer time" << endl;
// 		exit(-1);
	}
	
/*
	if (snd_pcm_hw_params_set_period_time(readHandle, hwparams, 200000, 0 
				) < 0) {
		cerr << "Could not set ALSA period_time" << endl;
		exit(-1);
	}
*/
	

	if (snd_pcm_hw_params(writeHandle, hwparams) < 0) {
		cerr << "Could not apply parameters to ALSA sound card for playout" << endl;
		exit(-1);
	}

	if (snd_pcm_sw_params_current(writeHandle, swparams) < 0) {
		cerr << "Could not get ALSA software parameters" << endl;
		exit(-1);
	}


	if (snd_pcm_sw_params_set_start_threshold(writeHandle, swparams, 16)){
		cerr << "Could not set ALSA start threshold" << endl;
// 		exit(-1);
	}
	
	/* Disable the XRUN detection */
/*	if (snd_pcm_sw_params_set_stop_threshold(writeHandle, swparams, 0x7FFFFFFF)){
		cerr << "Could not set ALSA stop threshold" << endl;
		exit(-1);
	}*/
	
	if (snd_pcm_sw_params_set_avail_min(writeHandle, swparams, 32)){
		cerr << "Could not set ALSA avail_min" << endl;
// 		exit(-1);
	}
	
	if (snd_pcm_sw_params(writeHandle, swparams) < 0) {
		cerr << "Could not apply sw parameters to ALSA sound card" << endl;
		exit(-1);
	}
	
	snd_pcm_prepare( writeHandle );
	
	openedPlayback = true;
	return 1;
}
	
int AlsaSoundDevice::openRecord( int samplingRate, int nChannels, int format ){
	snd_pcm_hw_params_t *hwparams2;
	snd_pcm_sw_params_t *swparams2;

	snd_pcm_hw_params_alloca(&hwparams2);
	snd_pcm_sw_params_alloca(&swparams2);
	

	if (snd_pcm_open(&readHandle, dev.c_str(), SND_PCM_STREAM_CAPTURE, 0)<0){
		cerr << "Could not open ALSA sound card for recording" << endl;
		exit(-1);
	}
	
	if (snd_pcm_hw_params_any(readHandle, hwparams2) < 0) {
		cerr << "Could not get ALSA sound card parameters" << endl;
		exit(-1);
	}

	if (snd_pcm_hw_params_set_access(readHandle, hwparams2, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
		cerr << "Could not set ALSA mode" << endl;
		exit(-1);
	}
	
	if (snd_pcm_hw_params_set_channels(readHandle, hwparams2, nChannels)<0){
		cerr << "Cound not configure ALSA for recording on "<<  nChannels<< "channels." << endl;
		if( nChannels != 1 ){
			if (snd_pcm_hw_params_set_channels(
					readHandle, hwparams2, 1)<0){
			// try to fall back on 1 channel, which should work
			// in most cases
			cerr << "Cound not configure ALSA for "
				"recording on 1 channel." << endl;
			cerr << "Minisip will now exit." << endl;
			exit(-1);
			}
			else{
				this->nChannelsRecord = 1;
			}
		}
		else{
			cerr << "Cound not configure ALSA "
				"for recording on 1 channel." << endl;
			cerr << "Minisip will now exit." << endl;
			exit(-1);
		}
	}
	else{
		this->nChannelsRecord = nChannels;
	}
	
	setFormat( format );
	_snd_pcm_format alsaFormat;
	
	switch( format ){
		case SOUND_S16LE:
			alsaFormat = SND_PCM_FORMAT_S16_LE;
			break;
		case SOUND_S16BE:
			alsaFormat = SND_PCM_FORMAT_S16_BE;
			break;
		case SOUND_U16LE:
			alsaFormat = SND_PCM_FORMAT_U16_LE;
			break;
		case SOUND_U16BE:
			alsaFormat = SND_PCM_FORMAT_U16_BE;
			break;
		default:
			cerr << "ALSA: Unhandled sound format" << endl;
			exit( -1 );
	}

	if (snd_pcm_hw_params_set_format(readHandle, hwparams2, 
				alsaFormat) < 0) {
		cerr << "Could not set ALSA format" << endl;
		exit(-1);
	}

	if (snd_pcm_hw_params_set_buffer_time(readHandle, hwparams2, 
				40000, 0) < 0) {
		cerr << "Could not set ALSA buffer time" << endl;
		exit(-1);
	}
	
	unsigned int wantedSamplingRate = (unsigned int)samplingRate; 
	
	if( snd_pcm_hw_params_set_rate_near(readHandle, hwparams2, (unsigned int *)&samplingRate, NULL) < 0){
		cerr << "Could not set ALSA rate" << endl;
		exit(-1);
	}

	if( (unsigned int)samplingRate != wantedSamplingRate ){
#ifdef DEBUG_OUTPUT		
		cerr << "Could not set chosen rate of " << wantedSamplingRate << ", set to "<< samplingRate <<endl;
#endif
	}

/*	if (snd_pcm_hw_params_set_period_time(readHandle, hwparams, 200, 0 
				) < 0) {
		cerr << "Could not set ALSA period_time" << endl;
		exit(-1);
	}

*/	

	if (snd_pcm_hw_params(readHandle, hwparams2) < 0) {
		cerr << "Could not apply parameters to ALSA sound card for recording" << endl;
		exit(-1);
	}

	if (snd_pcm_sw_params_current(readHandle, swparams2) < 0) {
		cerr << "Could not get ALSA software parameters" << endl;
		exit(-1);
	}


	/* Disable the XRUN detection */
	if (snd_pcm_sw_params_set_stop_threshold(readHandle, swparams2, 0x7FFFFFFF)){
		cerr << "Could not set ALSA stop threshold" << endl;
		exit(-1);
	}
	
	if (snd_pcm_sw_params(readHandle, swparams2) < 0) {
		cerr << "Could not apply sw parameters to ALSA sound card" << endl;
		exit(-1);
	}

	
	//snd_pcm_prepare(writeHandle);
	snd_pcm_prepare(readHandle);
	//snd_pcm_start(readHandle);
	
	openedRecord = true;

	return 0;
}

//Note: in alsa jargon, nSamples would be named nFrames. An AlsaFrame is made of 
//   N samples per each channel, and each sample is X bytes long.
int AlsaSoundDevice::readFromDevice( byte_t * buffer, uint32_t nSamples ){

	int nSamplesRead = 0;

	if( readHandle == NULL ){
		return -EBADF;
	}
	
	//it reads nSamples and returns the number of samples read ... 
	// or directly the negative error code if an error
	nSamplesRead = snd_pcm_readi( readHandle, 
					buffer, 
					nSamples );

	return nSamplesRead;
}

int AlsaSoundDevice::readError( int errcode, byte_t * buffer, uint32_t nSamples ) { 
	string msg = "";
	bool mustReturn = true;
	switch( errcode ){
		case -EAGAIN:
		case -EINTR:
			msg = "REAGAIN";
			mustReturn = false;
			break;
		case -EPIPE:
			msg = "REPIPE";
			if( snd_pcm_prepare( readHandle ) == -1 ) { mustReturn = true;}
			else { mustReturn = false; }
			break;
		default:
			msg = "RERROR";
			break;
	}
#ifdef DEBUG_OUTPUT
	fprintf( stderr, msg.c_str() );
#endif
	if( mustReturn ) { return -1; }
	else { return 0; }
}

int AlsaSoundDevice::writeToDevice( byte_t * buffer, uint32_t nSamples ){

	int nSamplesWritten = 0;

	if( writeHandle == NULL ){
		return -EBADF;
	}

	nSamplesWritten = snd_pcm_writei( writeHandle, 
					buffer, 
					nSamples );

	return nSamplesWritten; //the return is already the samples written or a negative value
}

int AlsaSoundDevice::writeError( int errcode, byte_t * buffer, uint32_t nSamples ) { 
	string msg = "";
	bool mustReturn = true;
	switch( errcode ){
		case -EAGAIN:
		case -EINTR:
			msg = "WEAGAIN";
			mustReturn = false;
			break;
		case -EPIPE:
			msg = "WEPIPE";
			if( snd_pcm_prepare( writeHandle ) == -1 ) { mustReturn = true;}
			else { mustReturn = false; }
			break;
		default:
			msg = "WERROR";
			break;
	}
#ifdef DEBUG_OUTPUT
	fprintf( stderr, msg.c_str() );
#endif
	if( mustReturn ) { return -1; }
	else { return 0; }
}

void AlsaSoundDevice::sync(){
	if( snd_pcm_drain( writeHandle ) < 0 ){
#ifdef DEBUG_OUTPUT
		cerr << "ALSA: Error on pcm_drain" << endl;
#endif
		//exit(-1);
	}
}
