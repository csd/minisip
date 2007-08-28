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

#include"DirectSoundDevice.h"

#include<windows.h>

#include<initguid.h>

#include<libmutil/merror.h>

//Output buffer is set to 100ms
#define DS_OUTPUT_BUFFER_SIZE 2*2*48000/10
//Input buffer is set to 40ms
#define DS_INPUT_BUFFER_SIZE 1*2*48000/25

using namespace std;

/**
 * 
 * Internals: 
 *  The DirectSound device maintains two buffers, a output buffer and 
 *  a capture buffer. They are circular buffers with cursors/pointers to
 *  the current play/capture point. The positions can be set using
 *  the SetCurrentPosition method. GetCurrentPosition can be used to
 *  get both the play/capture and write/write cursors.
 *
 */
DirectSoundDevice::DirectSoundDevice( string device ):SoundDevice( device ){
	HRESULT hr;
	LPGUID inputDevice = NULL;
	LPGUID outputDevice = NULL;
	GUID deviceGuid;

	if( device != "0" ){
		unsigned char *stringUuid = (unsigned char*)device.c_str();
		if( UuidFromStringA( stringUuid, &deviceGuid ) == RPC_S_OK ){
			inputDevice = &deviceGuid;
			outputDevice = &deviceGuid;
		}
		else{
			cerr << "DirectSoundDevice: unknown device '" << device << "'" << endl;
		}
	}

	// Capture buffer settings
	WAVEFORMATEX inAudioFormat;
	ZeroMemory( &inAudioFormat, sizeof(WAVEFORMATEX) );
	inAudioFormat.wFormatTag = WAVE_FORMAT_PCM;
	inAudioFormat.nChannels = 1;
	inAudioFormat.nSamplesPerSec = 48000;
	inAudioFormat.nAvgBytesPerSec=48000*2*1;
	inAudioFormat.nBlockAlign=2; 	// number of bytes in smallest "atomic unit" - nChannels*wBitsPerSample/8
	inAudioFormat.wBitsPerSample=16;
	
	// set parameters for what the buffer will look like
	DSCBUFFERDESC dscbd;
	ZeroMemory( &dscbd, sizeof(DSCBUFFERDESC) );
	dscbd.dwSize          = sizeof(DSCBUFFERDESC);
	dscbd.dwFlags         = 0;
	dscbd.dwBufferBytes   = DS_INPUT_BUFFER_SIZE;
	dscbd.lpwfxFormat     = &inAudioFormat;

	// Playout buffer settings
	WAVEFORMATEX outAudioFormat;
	ZeroMemory( &outAudioFormat, sizeof(WAVEFORMATEX) );
	outAudioFormat.wFormatTag = WAVE_FORMAT_PCM;
	outAudioFormat.nChannels = 2;
	outAudioFormat.nSamplesPerSec = 48000;
	outAudioFormat.nAvgBytesPerSec=48000*2*2;
	outAudioFormat.nBlockAlign=4; 	// number of bytes in smallest "atomic unit" - nChannels*wBitsPerSample/8
	outAudioFormat.wBitsPerSample=16;
	
	// set parameters for what the buffer will look like
	DSBUFFERDESC dsbd;
	ZeroMemory( &dsbd, sizeof(DSBUFFERDESC) );
	dsbd.dwSize          = sizeof(DSBUFFERDESC);
	dsbd.dwFlags         = DSBCAPS_GLOBALFOCUS | DSBCAPS_GETCURRENTPOSITION2 ;
	dsbd.dwBufferBytes   = /*g_dwOutputBufferSize*/ DS_OUTPUT_BUFFER_SIZE;
	dsbd.guid3DAlgorithm = GUID_NULL;
	dsbd.lpwfxFormat     = &outAudioFormat;
/*
#if (_WIN32_WINNT >= 0x0500)
#warning "WIN32 0500"
#else
#warning "WIN32 not high enough"
#endif
*/
	//The following requires Windows XP - if not supported, then
	//DirectSoundCreate can be used (windows echo cancelling filter
	//will not be possible).
	if (FAILED(hr=DirectSoundFullDuplexCreate8(inputDevice,
					outputDevice,
					&dscbd,
					&dsbd,
					GetConsoleWindow(),
					DSSCL_PRIORITY,
					&dsDuplexInterfaceHandle,
					&inputBufferHandle,
					&outputBufferHandle,
					NULL ))){
		merror("Can not create DirectSound device (DirectSoundFullDuplexCreate8)");
	}

	setCaptureNotificationPoints(inputBufferHandle);
}


HRESULT DirectSoundDevice::setCaptureNotificationPoints(LPDIRECTSOUNDCAPTUREBUFFER8 pDSCB)
{

	LPDIRECTSOUNDNOTIFY8 pDSNotify;
	WAVEFORMATEX         wfx;  
	DSBPOSITIONNOTIFY  rgdsbpn[cEvents];
	HRESULT    hr;

	if (NULL == pDSCB) return E_INVALIDARG;
	if (FAILED(hr = pDSCB->QueryInterface(IID_IDirectSoundNotify, (LPVOID*)&pDSNotify)))
	{
		return hr;
	}
	if (FAILED(hr = pDSCB->GetFormat(&wfx, sizeof(WAVEFORMATEX), NULL)))
	{
		return hr;
	}

	// Create events.
	for (int i = 0; i < cEvents; ++i)
	{
		inputSoundEvent[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (NULL == inputSoundEvent[i])
		{
			hr = GetLastError();
			return hr;
		}
	}

	// Describe notifications. 
	rgdsbpn[0].dwOffset = (DS_INPUT_BUFFER_SIZE/2) -1;
	rgdsbpn[0].hEventNotify = inputSoundEvent[0];

	rgdsbpn[1].dwOffset = DS_INPUT_BUFFER_SIZE - 1;
	rgdsbpn[1].hEventNotify = inputSoundEvent[1];

	// Create notifications.

	//cerr << "setCaptureNotificationPoints: cEvents="<< cEvents<<endl;
	if (FAILED(hr = pDSNotify->SetNotificationPositions(cEvents, rgdsbpn))){
		cerr << "DirectSoundDevice::setCaptureNotificationPoints: ERROR: failed to set notification points"<<endl;
		cerr << "Error code: "<< hex << hr << dec << endl;
	}
	
	pDSNotify->Release();	 //TODO: This line should not be commented
				 //out?!
	
	return hr;
}





DirectSoundDevice::~DirectSoundDevice(){
	//TODO: FIXME: Release DirectSound resources 
}

int DirectSoundDevice::openRecord( int samplingRate, int nChannels, int format ){
	this->nChannelsRecord = 1;
	this->samplingRate = 48000;
	this->openedRecord = true;

	if (FAILED(inputBufferHandle->Start(DSCBSTART_LOOPING))){
		merror("Could not start recording on direct sound input buffer");		
	}

	return 0;
}

int DirectSoundDevice::openPlayback( int samplingRate, 
				int nChannels, 
				int format ){
	HRESULT hr;

	this->nChannelsPlay = nChannels;
	this->nChannelsPlay = 2;
	this->samplingRate = 48000;
	openedPlayback = true;

	if (FAILED(hr = outputBufferHandle->Play(0,
					/*0xFFFFFFFF*/ 0,	//Priority: TODO: check if we can set it to 0xFFFFFFFF (source must be created with DSBCAPS_LOCDEFER)
					DSBPLAY_LOOPING
					))){
		cerr << "ERROR: DirectSoundDevice: Play failed."<<endl;
		cerr << "Error code: "<< hex << hr << endl;
	}
	
	return 0;
}

int DirectSoundDevice::closeRecord(){
	openedRecord= false;
	if (FAILED(inputBufferHandle->Stop())){
		merror("Could not stop recording on direct sound input buffer");		
	}
	return 0;
}

int DirectSoundDevice::closePlayback(){
	openedPlayback = false;
	if (FAILED(outputBufferHandle->Stop())){
		merror("Could not stop recording on direct sound input buffer");		
	}
	return 0;
}

int DirectSoundDevice::readFromDevice( byte_t * buffer, uint32_t nSamples ){
	static int wait_i=0;

	//Wait until the record cursor reaches the end of a 20ms block
	if (WaitForSingleObject( inputSoundEvent[wait_i], INFINITE)==WAIT_FAILED){
		merror("DirectSoundDevice::readFromDevice (waiting for capture data): WaitForSingleObject");
	}
	//The condition must be manually reset.
	ResetEvent(inputSoundEvent[wait_i]);

	VOID * lockedBuffer;
	DWORD lockedBufferSize;
	int readBlockSize = (48000*1*2)/50; // 20 ms, mono, 16 bit samples
	assert( (int)nSamples == (readBlockSize/2) );
	inputBufferHandle->Lock(wait_i*readBlockSize, readBlockSize, &lockedBuffer, &lockedBufferSize, NULL,NULL,0);
	short *sp = (short*) lockedBuffer;
	for (int n=0; n<readBlockSize/2; n++){
		((short*)buffer)[n] = sp[n];
	}
	inputBufferHandle->Unlock(lockedBuffer, lockedBufferSize, NULL,0);

	
	wait_i = (wait_i+1) % cEvents;
	
	return nSamples;
}

int DirectSoundDevice::writeToDevice( byte_t * buffer, uint32_t nSamples ){
	static int wait_i=1;
	HRESULT hr;

	VOID * lockedBuffer;
	DWORD lockedBufferSize;
	int writeBlockSize = (48000*2*2)/50;
	assert( (int)(nSamples * 2 * getNChannelsPlay()) == writeBlockSize);
	
	if (FAILED(hr=outputBufferHandle->Lock(wait_i*writeBlockSize, writeBlockSize, &lockedBuffer, &lockedBufferSize, NULL,NULL,0))){
		cerr <<"DirectSoundDevice: Lock failed on buffer"<< endl;
	}
	short *sp = (short*) lockedBuffer;
	for (int n=0; n<(int)nSamples * (int)getNChannelsPlay(); n++){
		sp[n]=((short*)buffer)[n];
	}
	if (FAILED(hr=outputBufferHandle->Unlock(lockedBuffer, lockedBufferSize, NULL,0))){
		cerr <<"DirectSoundDevice: Unlock failed on buffer"<< endl;
	}

	
	wait_i = (wait_i+1) % 5; //FIXME: Hardcoded to be 100ms - 5  20ms blocks
	
	return nSamples;
}
		
int DirectSoundDevice::readError( int errcode, byte_t * buffer, uint32_t nSamples ) {
	return -1;
}

int DirectSoundDevice::writeError( int errcode, byte_t * buffer, uint32_t nSamples ) {
	return -1;
}
	
void DirectSoundDevice::sync(){
}
