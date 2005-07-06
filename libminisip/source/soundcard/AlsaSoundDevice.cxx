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

#include<libminisip/AlsaSoundDevice.h>

using namespace std;


AlsaSoundDevice::AlsaSoundDevice( string device ):SoundDevice( device ){

	readHandle = NULL;
	writeHandle = NULL;
}



int AlsaSoundDevice::closePlayback(){
	if( !openedPlayback ){
#ifdef DEBUG_OUTPUT
		cerr << "WARNING: doing close on already "
			"closed sound card"<< endl;
#endif
		return -1;
	}

	if( writeHandle != NULL ){
		snd_pcm_close( writeHandle );
		writeHandle = NULL;
	}

	openedPlayback = false;

}

int AlsaSoundDevice::closeRecord(){
	if( !openedRecord ){
		cerr << "WARNING: doing close on already "
			"closed sound card"<< endl;
		return -1;
	}

	if( readHandle != NULL ){
		snd_pcm_close( readHandle );
		readHandle = NULL;
	}

	openedRecord = false;

}


int AlsaSoundDevice::openPlayback( int samplingRate, int nChannels, int format ){

	snd_pcm_hw_params_t *hwparams;
	snd_pcm_sw_params_t *swparams;
	snd_pcm_hw_params_alloca(&hwparams);
	snd_pcm_sw_params_alloca(&swparams);
	
	// Play ...
	
	if (snd_pcm_open(&writeHandle, dev.c_str(), SND_PCM_STREAM_PLAYBACK,  0/*SND_PCM_NONBLOCK*/)<0){
		cerr << "Could not open Alsa sound card" << endl;
		exit(-1);
	}
	
	if (snd_pcm_hw_params_any(writeHandle, hwparams) < 0) {
		cerr << "Could not get Alsa sound card parameters" << endl;
		exit(-1);
	}

	if (snd_pcm_hw_params_set_access(writeHandle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
		cerr << "Could not set Alsa mode" << endl;
		exit(-1);
	}
	
	if (snd_pcm_hw_params_set_channels(writeHandle, hwparams, nChannels)<0){
		cerr << "Cound not configure ALSA for playout on "<<  nChannels<< endl;
		exit(-1);
	}

	this->nChannelsPlay = nChannels;

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
			cerr << "Unhandled sound format" << endl;
			exit( -1 );
	}

	this->format = format;



	if (snd_pcm_hw_params_set_format(writeHandle, hwparams, 
				alsaFormat) < 0) {
		cerr << "Could not set Alsa format" << endl;
		exit(-1);
	}
/*
	if (snd_pcm_hw_params_set_period_size(readHandle, hwparams, 
				PERIOD_SIZE, 0) < 0) {
		cerr << "Could not set Alsa period size" << endl;
		exit(-1);
	}
*/	
	uint32_t min_buffer_size;
	int32_t dir;

	if (snd_pcm_hw_params_get_buffer_time_min(hwparams,
				&min_buffer_size, &dir) < 0){
		cerr << "Could not get Alsa min buffer time" << endl;
	}

	/* Some soundcards seems to be a bit optimistic */

	if (min_buffer_size < MIN_HW_PO_BUFFER){
		min_buffer_size = MIN_HW_PO_BUFFER;
	}

#ifdef DEBUG_OUTPUT
	cerr << "Hardware playout buffer size: " << min_buffer_size << endl;
#endif

	if (snd_pcm_hw_params_set_buffer_time(writeHandle, hwparams, 
				min_buffer_size, dir) < 0) {
		cerr << "Could not set Alsa buffer time" << endl;
		exit(-1);
	}
	
	unsigned int wantedSamplingRate = (unsigned int)samplingRate; 

	if( snd_pcm_hw_params_set_rate_near(writeHandle, hwparams, (unsigned int *)&samplingRate, NULL) < 0){
		cerr << "Could not set Alsa rate" << endl;
		exit(-1);
	}

	if( samplingRate != wantedSamplingRate  ){
		cerr << "Could not set chosen rate of " << wantedSamplingRate << ", set to "<< samplingRate <<endl;
	}

	this->samplingRate = samplingRate;
/*
	if (snd_pcm_hw_params_set_period_time(readHandle, hwparams, 200000, 0 
				) < 0) {
		cerr << "Could not set Alsa period_time" << endl;
		exit(-1);
	}
*/
	

	if (snd_pcm_hw_params(writeHandle, hwparams) < 0) {
		cerr << "Could not apply parameters to Alsa sound card for playout" << endl;
		exit(-1);
	}

	if (snd_pcm_sw_params_current(writeHandle, swparams) < 0) {
		cerr << "Could not get Alsa software parameters" << endl;
		exit(-1);
	}


	if (snd_pcm_sw_params_set_start_threshold(writeHandle, swparams, 32)){
		cerr << "Could not set Alsa start threshold" << endl;
		exit(-1);
	}
	
	/* Disable the XRUN detection */
	if (snd_pcm_sw_params_set_stop_threshold(writeHandle, swparams, 0x7FFFFFFF)){
		cerr << "Could not set Alsa stop threshold" << endl;
		exit(-1);
	}
	
	if (snd_pcm_sw_params_set_avail_min(writeHandle, swparams,16)){
		cerr << "Could not set Alsa avail_min" << endl;
		exit(-1);
	}
	
	if (snd_pcm_sw_params(writeHandle, swparams) < 0) {
		cerr << "Could not apply sw parameters to Alsa sound card" << endl;
		exit(-1);
	}
	
	openedPlayback = true;
}
	
int AlsaSoundDevice::openRecord( int samplingRate, int nChannels, int format ){
	snd_pcm_hw_params_t *hwparams2;
	snd_pcm_sw_params_t *swparams2;

	snd_pcm_hw_params_alloca(&hwparams2);
	snd_pcm_sw_params_alloca(&swparams2);
	

	if (snd_pcm_open(&readHandle, dev.c_str(), SND_PCM_STREAM_CAPTURE, 0)<0){
		cerr << "Could not open Alsa sound card for recording" << endl;
		exit(-1);
	}
	
	if (snd_pcm_hw_params_any(readHandle, hwparams2) < 0) {
		cerr << "Could not get Alsa sound card parameters" << endl;
		exit(-1);
	}

	if (snd_pcm_hw_params_set_access(readHandle, hwparams2, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
		cerr << "Could not set Alsa mode" << endl;
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
			cerr << "Unhandled sound format" << endl;
			exit( -1 );
	}

	if (snd_pcm_hw_params_set_format(readHandle, hwparams2, 
				alsaFormat) < 0) {
		cerr << "Could not set Alsa format" << endl;
		exit(-1);
	}

	if (snd_pcm_hw_params_set_buffer_time(readHandle, hwparams2, 
				40000, 0) < 0) {
		cerr << "Could not set Alsa buffer time" << endl;
		exit(-1);
	}
	
	unsigned int wantedSamplingRate = (unsigned int)samplingRate; 
	
	if( snd_pcm_hw_params_set_rate_near(readHandle, hwparams2, (unsigned int *)&samplingRate, NULL) < 0){
		cerr << "Could not set Alsa rate" << endl;
		exit(-1);
	}

	if( samplingRate != wantedSamplingRate ){
		cerr << "Could not set chosen rate of " << wantedSamplingRate << ", set to "<< samplingRate <<endl;
	}

/*	if (snd_pcm_hw_params_set_period_time(readHandle, hwparams, 200, 0 
				) < 0) {
		cerr << "Could not set Alsa period_time" << endl;
		exit(-1);
	}

*/	

	if (snd_pcm_hw_params(readHandle, hwparams2) < 0) {
		cerr << "Could not apply parameters to Alsa sound card for recording" << endl;
		exit(-1);
	}

	if (snd_pcm_sw_params_current(readHandle, swparams2) < 0) {
		cerr << "Could not get Alsa software parameters" << endl;
		exit(-1);
	}


	/* Disable the XRUN detection */
	if (snd_pcm_sw_params_set_stop_threshold(readHandle, swparams2, 0x7FFFFFFF)){
		cerr << "Could not set Alsa stop threshold" << endl;
		exit(-1);
	}
	
	if (snd_pcm_sw_params(readHandle, swparams2) < 0) {
		cerr << "Could not apply sw parameters to Alsa sound card" << endl;
		exit(-1);
	}

	
	//snd_pcm_prepare(writeHandle);
	//snd_pcm_prepare(readHandle);
	//snd_pcm_start(readHandle);
	
	openedRecord = true;

	return 0;

}

int AlsaSoundDevice::read( byte_t * buffer, uint32_t nSamples ){

	
	int nSamplesRead = 0;
	int totalSamplesRead = 0;

	if( readHandle == NULL ){
		return -1;
	}
	
	while( totalSamplesRead != nSamples ){
		nSamplesRead = snd_pcm_readi( readHandle, buffer, nSamples - totalSamplesRead );

		if( nSamplesRead < 0 ){
			cerr << "An error occured when reading from sound card" << endl;
			switch( nSamplesRead ){
				case -EAGAIN:
					break;
				case -EPIPE:
					snd_pcm_prepare( readHandle );
					break;
				case -ESTRPIPE:
					return -1;
			}
			continue;
		}

		else{
			totalSamplesRead += nSamplesRead;
		}
	}

	return totalSamplesRead;

}

int AlsaSoundDevice::write( byte_t * buffer, uint32_t nSamples ){

	int nSamplesWritten = 0;
	int totalSamplesWritten = 0;

	if( writeHandle == NULL ){
		return -1;
	}

	while( totalSamplesWritten < nSamples ){
		nSamplesWritten = snd_pcm_writei( writeHandle, buffer, nSamples - totalSamplesWritten );

//		fprintf( stderr, "nSamplesWritten %d\n", nSamplesWritten );

		if( nSamplesWritten < 0 ){
			switch( nSamplesWritten ){
				case -EAGAIN:
					fprintf( stderr,"EAGAIN" );
					break;
				case -EPIPE:
					fprintf( stderr,"EPIPE" );
					snd_pcm_prepare( writeHandle );
					break;
				case -ESTRPIPE:
					fprintf( stderr,"ESTRIPE" );
					return -1;
				case -EBADFD:
					fprintf( stderr,"EBADFD" );
					return -1;
			}
		}

		else{
			totalSamplesWritten += nSamplesWritten;
		}

	}

	return totalSamplesWritten;
}

void AlsaSoundDevice::sync(){
	if( snd_pcm_drain( writeHandle ) < 0 ){
		cerr << "Alsa: Error on pcm_drain" << endl;
		//exit(-1);
	}
}
