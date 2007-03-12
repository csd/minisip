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

#include"AlsaSoundDevice.h"

#include<libmutil/Thread.h>
#include<libmutil/mtime.h>

/**
Note (Cesc)
I have tried and getting alsa to work in non-blocking for a wide range of soundcards
is difficult, very. 
When it seems to work, you may start getting errors, or clicks on the audio ...
Only turn non-blocking to true (and play with the settings (buffer size - see the header, 
and number of periods and periodsize) if sound using alsa is bad. Godd luck!
*/
#define OPEN_ALSA_IN_NON_BLOCKING_MODE false;

using namespace std;


AlsaSoundDevice::AlsaSoundDevice( string device ):SoundDevice( device ){

	readHandle = NULL;
	writeHandle = NULL;
	periodSize = 0;
	numPeriods = 0;

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

int AlsaSoundDevice::calculateAlsaParams( unsigned long &periodSizeMin, 
					unsigned long &periodSizeMax, 
					uint32_t &periodsMin, 
					uint32_t &periodsMax,
					unsigned long &bufferMax ) {
	unsigned long buffer;
	
	//if it has already been opened ... return old values ...
	if( this->periodSize != 0 && this->numPeriods != 0 ) {
		periodSizeMin = this->periodSize;
		periodsMin = this->numPeriods;
		return 0;
	}
	
	unsigned long bufferMin = (unsigned long) periodsMin * periodSizeMin;
	if( periodsMin == 1 ) periodsMin = 2; //we want at least two periods ... 

	//set the goal for the buffer size between the limits ...
	buffer = (MIN_HW_PO_BUFFER/1000) * (this->samplingRate/1000); //size in alsa Frames
	if( buffer < bufferMin ) buffer = bufferMin;
	else if( buffer > bufferMax ) buffer = bufferMax;

#ifdef DEBUG_OUTPUT	
	printf( "Alsa Calc Values: sampling = %d, period size [%d,%d],\n"
	        "                  num periods = [%d, %d], buffer size = [%d, %d]\n", (int)this->samplingRate/1000, 
							(int)periodSizeMin, 
							(int)periodSizeMax, 
							(int)periodsMin, 
							(int)periodsMax, 
							(int)bufferMin, 
							(int)bufferMax );
	printf( "Alsa Calc: buffer we want is = %d [alsaframes]\n", (int)buffer );
#endif
	//Now ... iteratively calculate the best possible values ... we 
	//prefer having biffer period size and the smaller possible period size ..
	
	//the first time we obtain a buffer size >= our goal, we break;
	bool found = false;
	unsigned long tmp;
	uint32_t per;
	unsigned long siz; 
	for(  per = periodsMin; per <= periodsMax; per++ ) {
		for(  siz = periodSizeMin; siz <= periodSizeMax; siz+=4 ) {
			tmp = per * siz;
			if( tmp >= buffer ) {
				if(  tmp > bufferMax ) { tmp = bufferMax; }
				found = true;
				buffer = tmp;
				break;
			}
		}//inner loop
		if( found ) break;
	} // outer loop

#ifdef DEBUG_OUTPUT
	cerr << "Alsa - CalculatedParams: periodSize = " << siz << "; numPeriods = " << per << "; bufferSize = " << buffer << flush << endl;
#endif
	if( !found ) {
		return -1;
	}
	this->periodSize = siz ;
	this->numPeriods = per;
	bufferMin = buffer;
	return 0;
}

int AlsaSoundDevice::openPlayback( int samplingRate, int nChannels, int format ){

	snd_pcm_hw_params_t *hwparams;
	snd_pcm_sw_params_t *swparams;
	snd_pcm_hw_params_alloca(&hwparams);
	snd_pcm_sw_params_alloca(&swparams);
	
	lockOpen.lock();
	// Play ...

	int openMode = 0;
	bool openNonBlocking = OPEN_ALSA_IN_NON_BLOCKING_MODE;
	
	if( openNonBlocking ) {
		sleepTime = 20; //min time between calls ... simulated
		openMode = SND_PCM_NONBLOCK;
#ifdef DEBUG_OUTPUT
		cerr << "ALSA: opening playback in non-blocking mode" << endl;
#endif
	} else {
		openMode = 0;
		sleepTime = 0;
#ifdef DEBUG_OUTPUT
		cerr << "ALSA: opening playback in non-blocking mode" << endl;
#endif
	}
	
	if (snd_pcm_open(&writeHandle, dev.c_str(), SND_PCM_STREAM_PLAYBACK,  openMode ) < 0 ) {
		cerr << "Could not open ALSA sound card (playback)" << endl;
		exit(-1);
	}
	
	if (snd_pcm_hw_params_any(writeHandle, hwparams) < 0) {
		cerr << "Could not get ALSA sound card parameters (playback)" << endl;
		exit(-1);
	}

	if (snd_pcm_hw_params_set_access(writeHandle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
		cerr << "Could not set ALSA mode (playback)" << endl;
		exit(-1);
	}
	
	if (snd_pcm_hw_params_set_channels(writeHandle, hwparams, nChannels)<0){
		cerr << "Cound not configure ALSA (playback) for playout on "<<  nChannels<< endl;
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
			cerr << "Unhandled sound format (ALSA) (playback)" << endl;
			exit( -1 );
	}

	if (snd_pcm_hw_params_set_format(writeHandle, hwparams, 
				alsaFormat) < 0) {
		cerr << "Could not set ALSA format (playback)" << endl;
		exit(-1);
	}
	
	unsigned int wantedSamplingRate = (unsigned int)samplingRate; 

	if( snd_pcm_hw_params_set_rate_near(writeHandle, hwparams, (unsigned int *)&samplingRate, NULL) < 0){
		cerr << "Could not set ALSA rate (playback)" << endl;
		exit(-1);
	}

	if( (unsigned int)samplingRate != wantedSamplingRate  ){
#ifdef DEBUG_OUTPUT
		cerr << "ALSA (playback): Could not set chosen rate of " << wantedSamplingRate << ", set to "<< samplingRate <<endl;
#endif
	}
	this->samplingRate = samplingRate;
	
	unsigned long periodSizeMin = 960; //desired size of one period ... in number of frames
	unsigned long periodSizeMax; //size of one period ... in number of frames
	uint32_t numPeriodsMin;  //desired buffer length in number of periods
	uint32_t numPeriodsMax;  //desired buffer length in number of periods
	unsigned long max_buffer_size; //max buffer size allowd
	
	uint32_t startThreshold = 16;  //number of frames in the buffer so the hw will start processing
	uint32_t minAvailable = 32;   //number of available frames so we can read/write
	
	int32_t dir;

	//fetch the params ... exit if not able ...	
	if( snd_pcm_hw_params_get_period_size_min (hwparams, &periodSizeMin, &dir) < 0 ) {
		cerr << "Playback: Could not get ALSA period min size" << endl;
		exit( -1 );
	}
	if( snd_pcm_hw_params_get_period_size_max (hwparams, &periodSizeMax, &dir) < 0 ) {
		cerr << "Playback: Could not get ALSA period max size" << endl;
		exit( -1 );
	}
	if( snd_pcm_hw_params_get_periods_min (hwparams, &numPeriodsMin, &dir) < 0 ) {
		cerr << "Playback: Could not get ALSA periods min " << endl;
		exit( -1 );
	}	
	if( snd_pcm_hw_params_get_periods_max (hwparams, &numPeriodsMax, &dir) < 0 ) {
		cerr << "Playback: Could not get ALSA periods max " << endl;
		exit( -1 );
	}	
	if( snd_pcm_hw_params_get_buffer_size_max (hwparams, &max_buffer_size) < 0 ) {
		cerr << "Playback: Could not get ALSA max buffer size " << endl;
		exit( -1 );
	}	

	if( calculateAlsaParams( periodSizeMin, 
				periodSizeMax, 
				numPeriodsMin, 
				numPeriodsMax,
				max_buffer_size)  < 0 ) {
		cerr << "Playback: Could Not calculate Alsa Params" << endl;
		exit( -1 );
	}

#ifdef DEBUG_OUTPUT
	printf( "ALSA playback: setting values numperiods %d, period Size %d\n", (int)this->numPeriods, (int)this->periodSize );
#endif
	//Set the calculated params ... they should not give an eror ... still, check ...	
	if( snd_pcm_hw_params_set_periods (writeHandle, hwparams, this->numPeriods, 0) < 0 ) {
		cerr << "Could not set ALSA (playback) periods" << endl;
		exit( -1 );
	}
	if( snd_pcm_hw_params_set_period_size_near (writeHandle, hwparams, &this->periodSize, 0) < 0 ) {
		cerr << "Could not set ALSA (playback) period size" << endl;
		exit( -1 );
	} else {
		#ifdef DEBUG_OUTPUT
		cerr << "Record: alsa period size set to " << this->periodSize << endl;
		#endif
	}
	if (snd_pcm_hw_params(writeHandle, hwparams) < 0) {
		cerr << "Could not apply parameters to ALSA (playback) sound card for playout" << endl;
		exit(-1);
	}

	
	if (snd_pcm_sw_params_current(writeHandle, swparams) < 0) {
		cerr << "Could not get ALSA software parameters (playback)" << endl;
		exit(-1);
	}

	if (snd_pcm_sw_params_set_start_threshold(writeHandle, swparams, startThreshold)){
		#ifdef DEBUG_OUTPUT	
		cerr << "Could not set ALSA start threshold (playback)" << endl;
		#endif
// 		exit(-1);
	}
	
	/* Disable the XRUN detection */
	if (snd_pcm_sw_params_set_stop_threshold(writeHandle, swparams, 0x7FFFFFFF)){
		cerr << "Could not set ALSA stop threshold (playback)" << endl;
		exit(-1);
	}
	
	if (snd_pcm_sw_params_set_avail_min(writeHandle, swparams, minAvailable)){
		#ifdef DEBUG_OUTPUT	
		cerr << "Could not set ALSA avail_min (playback)" << endl;
		#endif
// 		exit(-1);
	}
	
	if (snd_pcm_sw_params(writeHandle, swparams) < 0) {
		cerr << "Could not apply sw parameters to ALSA sound card (playback)" << endl;
		exit(-1);
	}
	
// 	snd_pcm_prepare( writeHandle );
#ifdef DEBUG_OUTPUT
	cerr << "ALSA OPENED playback!!" << endl << flush;
#endif
	openedPlayback = true;
	lockOpen.unlock();
	return 1;
}
	
int AlsaSoundDevice::openRecord( int samplingRate, int nChannels, int format ){

	snd_pcm_hw_params_t *hwparams2;
	snd_pcm_sw_params_t *swparams2;

	lockOpen.lock();
	
	snd_pcm_hw_params_alloca(&hwparams2);
	snd_pcm_sw_params_alloca(&swparams2);
	

	if (snd_pcm_open(&readHandle, dev.c_str(), SND_PCM_STREAM_CAPTURE, 0)<0){
		cerr << "Could not open ALSA sound card for recording" << endl;
		exit(-1);
	}
	
	if (snd_pcm_hw_params_any(readHandle, hwparams2) < 0) {
		cerr << "Could not get ALSA sound card parameters (record) " << endl;
		exit(-1);
	}

	if (snd_pcm_hw_params_set_access(readHandle, hwparams2, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
		cerr << "Could not set ALSA mode (record) " << endl;
		exit(-1);
	}
	
	if (snd_pcm_hw_params_set_channels(readHandle, hwparams2, nChannels)<0){
		//if desired num of channels fails, try setting to 1 channel
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
			cerr << "ALSA: Unhandled sound format (record) " << endl;
			exit( -1 );
	}

	if (snd_pcm_hw_params_set_format(readHandle, hwparams2, 
				alsaFormat) < 0) {
		cerr << "Could not set ALSA format (record) " << endl;
		exit(-1);
	}

/*	if (snd_pcm_hw_params_set_buffer_time(readHandle, hwparams2, 
				40000, 0) < 0) {
		cerr << "Could not set ALSA buffer time" << endl;
		exit(-1);
	}
	*/
	unsigned int wantedSamplingRate = (unsigned int)samplingRate; 
	
	if( snd_pcm_hw_params_set_rate_near(readHandle, hwparams2, (unsigned int *)&samplingRate, NULL) < 0){
		cerr << "Could not set ALSA rate (record) " << endl;
		exit(-1);
	}

	if( (unsigned int)samplingRate != wantedSamplingRate ){
		#ifdef DEBUG_OUTPUT		
		cerr << "Could not set chosen (record) rate of " << wantedSamplingRate << ", set to "<< samplingRate <<endl;
		#endif
	}
	this->samplingRate = samplingRate;

	unsigned long periodSizeMin = 960; //desired size of one period ... in number of frames
	unsigned long periodSizeMax; //size of one period ... in number of frames
	uint32_t numPeriodsMin;  //desired buffer length in number of periods
	uint32_t numPeriodsMax;  //desired buffer length in number of periods
	unsigned long max_buffer_size; //max buffer size allowd
	
// 	uint32_t startThreshold = 16;  //number of frames in the buffer so the hw will start processing
// 	uint32_t minAvailable = 32;   //number of available frames so we can read/write
	
	int32_t dir;

	//fetch the params ... exit if not able ...	
	if( snd_pcm_hw_params_get_period_size_min (hwparams2, &periodSizeMin, &dir) < 0 ) {
		cerr << "Record: Could not get ALSA period min size" << endl;
		exit( -1 );
	}
	if( snd_pcm_hw_params_get_period_size_max (hwparams2, &periodSizeMax, &dir) < 0 ) {
		cerr << "Record: Could not get ALSA period max size" << endl;
		exit( -1 );
	}
	if( snd_pcm_hw_params_get_periods_min (hwparams2, &numPeriodsMin, &dir) < 0 ) {
		cerr << "Record: Could not get ALSA periods min " << endl;
		exit( -1 );
	}	
	if( snd_pcm_hw_params_get_periods_max (hwparams2, &numPeriodsMax, &dir) < 0 ) {
		cerr << "Record: Could not get ALSA periods max " << endl;
		exit( -1 );
	}	
	if( snd_pcm_hw_params_get_buffer_size_max (hwparams2, &max_buffer_size) < 0 ) {
		cerr << "Record: Could not get ALSA max buffer size " << endl;
		exit( -1 );
	}	

	if( calculateAlsaParams( periodSizeMin, 
				periodSizeMax, 
				numPeriodsMin, 
				numPeriodsMax,
				max_buffer_size)  < 0 ) {
		cerr << "Record: Could Not calculate Alsa Params" << endl;
		exit( -1 );
	}

#ifdef DEBUG_OUTPUT
	printf( "ALSA Record: setting values %d, %d\n", (int)this->numPeriods, (int)this->periodSize );
#endif
	//Set the calculated params ... they should not give an eror ... still, check ...	
	if( snd_pcm_hw_params_set_periods (readHandle, hwparams2, this->numPeriods, 0) < 0 ) {
		cerr << "Record Could not set ALSA periods" << endl;
		exit( -1 );
	}
	if( snd_pcm_hw_params_set_period_size_near (readHandle, hwparams2, &this->periodSize, 0) < 0 ) {
		cerr << "Record Could not set ALSA period size" << endl;
		exit( -1 );
	} 
#ifdef DEBUG_OUTPUT	
	else {
		cerr << "Record: alsa period size set to " << this->periodSize << endl;
		
	}
#endif
	if (snd_pcm_hw_params(readHandle, hwparams2) < 0) {
		cerr << "Record Could not apply parameters to ALSA sound card for playout" << endl;
		exit(-1);
	}

	if (snd_pcm_sw_params_current(readHandle, swparams2) < 0) {
		cerr << "Record Could not get ALSA software parameters" << endl;
		exit(-1);
	}


	/* Disable the XRUN detection */
	if (snd_pcm_sw_params_set_stop_threshold(readHandle, swparams2, 0x7FFFFFFF)){
		cerr << "Record Could not set ALSA stop threshold" << endl;
		exit(-1);
	}
	
	if (snd_pcm_sw_params(readHandle, swparams2) < 0) {
		cerr << "Record Could not apply sw parameters to ALSA sound card" << endl;
		exit(-1);
	}

	
	//snd_pcm_prepare(writeHandle);
	snd_pcm_prepare(readHandle);
	//snd_pcm_start(readHandle);
	
#ifdef DEBUG_OUTPUT
	cerr << "ALSA OPENED record!!" << endl << flush;
#endif
	openedRecord = true;
	lockOpen.unlock();
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
