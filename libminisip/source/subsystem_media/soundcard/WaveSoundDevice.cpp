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
#include<config.h>

#include"WaveSoundDevice.h"

#include <libmutil/thread.h>

#define		NUMOUTPUTBUFFER	2

using namespace std;

/* Handle to the WAVE In Device */
HWAVEIN				waveInDevice;
/* Handle to the WAVE Out Device */
HWAVEOUT            waveOutDevice;
/* We use two WAVEHDR's for recording (ie, double-buffering) in this example */
WAVEHDR				WaveInHeader[2];
/* We use two WAVEHDR's for playing (ie, double-buffering) in this example */
WAVEHDR             waveOutHeader[NUMOUTPUTBUFFER];



DWORD WaveDataPlayed;
//input wave format
WAVEFORMATEX					waveInputFormat;
WAVEFORMATEX					waveOutputFormat;


//boolean to initialize the recording
bool startedRecording=false;
bool startedPlaying=false;

HANDLE inputHandle;
HANDLE outputHandle;

WaveSoundDevice::WaveSoundDevice(std::string device):SoundDevice(device){

	MMRESULT err = MMSYSERR_NOERROR;
	//setting input format & buffer
	printf("creating Wavesoundapi, setting input format\n");
	ZeroMemory( &waveInputFormat, sizeof(WAVEFORMATEX) );
	waveInputFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveInputFormat.nChannels = 1;
	waveInputFormat.nSamplesPerSec = 48000;
	waveInputFormat.wBitsPerSample = 16;
	waveInputFormat.nBlockAlign = waveInputFormat.nChannels * (waveInputFormat.wBitsPerSample/8);
	waveInputFormat.nAvgBytesPerSec = waveInputFormat.nSamplesPerSec * waveInputFormat.nBlockAlign;
	waveInputFormat.cbSize = 0;
 
	
	inputHandle = CreateEvent(NULL, TRUE, FALSE, NULL);

	ResetEvent(inputHandle);

	err = waveInOpen(&waveInDevice, 0, &waveInputFormat, (DWORD_PTR)inputHandle, 0, CALLBACK_EVENT);
	printf ("\n opening input device \n ");
	if(err!=MMSYSERR_NOERROR){
		printf("!!!!could not create wave INPUT device!!!\n");
		exit(1);	
	}

	printf("after opening device in constructor\n");

	printf("creating input buffer!\n");
	WaveInHeader[1].dwBufferLength = WaveInHeader[0].dwBufferLength = waveInputFormat.nAvgBytesPerSec/50;
	if (!(WaveInHeader[0].lpData = (char *)VirtualAlloc(0, (WaveInHeader[0].dwBufferLength * 2), MEM_COMMIT, PAGE_READWRITE))){
		printf("ERROR: Can't allocate memory for WAVE buffer!\n");
	}
	else{
		/* Fill in WAVEHDR fields for buffer starting address. We've already filled in the size fields above */
		WaveInHeader[1].lpData = WaveInHeader[0].lpData + WaveInHeader[0].dwBufferLength;
	}
	
	if (err=(waveInPrepareHeader(waveInDevice, &WaveInHeader[0], sizeof(WAVEHDR)))){
		printf("Error preparing WAVEHDR 1! %08X\n", err);
		//exit(1);
	}
	else{

		if ((waveInPrepareHeader(waveInDevice, &WaveInHeader[1], sizeof(WAVEHDR)))!= MMSYSERR_NOERROR){
			printf("Error preparing WAVEHDR 2! \n");
			//exit(1);
		}

	}

	

//	outputHandle = CreateEvent(NULL, TRUE, FALSE, NULL);


	//setting output format & buffer
	ZeroMemory( &waveOutputFormat, sizeof(WAVEFORMATEX) );
	waveOutputFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveOutputFormat.nChannels = 1;
	waveOutputFormat.nSamplesPerSec = 48000;
	waveOutputFormat.nAvgBytesPerSec=48000*1*2;
	waveOutputFormat.nBlockAlign=4; 	// number of bytes in smallest "atomic unit" - nChannels*wBitsPerSample/8
	waveOutputFormat.wBitsPerSample=16;

	outputHandle = CreateEvent(NULL, TRUE, FALSE, NULL);

	ResetEvent(outputHandle);

	//Create and handle of WaveOut
	err=waveOutOpen(&waveOutDevice, WAVE_MAPPER, &waveOutputFormat, (DWORD_PTR)outputHandle, 0,  CALLBACK_EVENT);
	if(err!=MMSYSERR_NOERROR){
		printf("!!!!could not create wave OUTPUT device!!!\n");
		exit(1);
	}

	printf("creating output buffer!\n");
	//waveOutHeader[1].dwBufferLength = waveOutHeader[0].dwBufferLength = waveOutputFormat.nAvgBytesPerSec/50;
	for(int i1=0;i1<2;i1++) {
		waveOutHeader[i1].dwBufferLength=waveOutputFormat.nAvgBytesPerSec/50;
	}

	if (!(waveOutHeader[0].lpData = (char *)VirtualAlloc(0, (waveOutHeader[0].dwBufferLength * NUMOUTPUTBUFFER), MEM_COMMIT, PAGE_READWRITE))){
		printf("ERROR: Can't allocate memory for WAVE buffer!\n");
	}
	
	else{
		/* Fill in WAVEHDR fields for buffer starting address. We've already filled in the size fields above */
		for(int i2=0;i2<NUMOUTPUTBUFFER;i2++){
			waveOutHeader[i2].lpData = waveOutHeader[0].lpData + waveOutHeader[0].dwBufferLength*i2;
		}
		//waveOutHeader[1].lpData = waveOutHeader[0].lpData + waveOutHeader[0].dwBufferLength;
	}

	for(int i3=0;i3<NUMOUTPUTBUFFER;i3++){
	
		if ((err = waveOutPrepareHeader(waveOutDevice, &waveOutHeader[i3], sizeof(WAVEHDR)))){
			printf("Error preparing WAVEHDR %d! -- %08X\n",i3,err);
		}
	}

}


WaveSoundDevice::~WaveSoundDevice(){ 
/*
* DO sth here...
*/

}

int WaveSoundDevice::openRecord(int32_t samplingRate, int nChannels, int format){
	
	MMRESULT err;
	
	printf("\nOPEN Record!!\n");

	this->nChannelsRecord = 1;
	this->samplingRate = 48000;
	this->openedRecord = true;
	err=waveInStart(waveInDevice);
//	waveInAddBuffer(waveInDevice, &WaveInHeader[0], sizeof(WAVEHDR));
//	waveInAddBuffer(waveInDevice, &WaveInHeader[1], sizeof(WAVEHDR));
	if(err!=MMSYSERR_NOERROR){
		printf("!!!!waveInStart Error!!!\n");
		return -1;
	}
	return 0;

}

int WaveSoundDevice::closeRecord(){
	MMRESULT err;
	err=waveInStop(waveInDevice);
	printf("waveInstop called!!!!\n");
	if(err!=MMSYSERR_NOERROR){
		printf("!!!!waveInStop Error!!!\n");
		return -1;
	}
	return 0;
}

int WaveSoundDevice::openPlayback(int32_t samplingRate, int nChannels, int format){
	this->nChannelsPlay = 1;
	this->samplingRate = 8000;
	openedPlayback = true;
	                                                                                                                     
	return 0;
}

int WaveSoundDevice::closePlayback(){
	MMRESULT err;
	err=waveOutReset(waveOutDevice);
	printf("!!!! in closePlayBack!!!\n");
	if(err!=MMSYSERR_NOERROR){
		printf("!!!!waveOutReset Error!!!\n");
		return -1;
	}
	return 0;

}

int WaveSoundDevice::readFromDevice(byte_t *buffer, uint32_t nSamples){	
	static int buf_i=0;

	if (!startedRecording){
		printf("starting record by adding the first buffer\n");
		waveInAddBuffer(waveInDevice, &WaveInHeader[0], sizeof(WAVEHDR));
		waveInAddBuffer(waveInDevice, &WaveInHeader[1], sizeof(WAVEHDR));

		startedRecording=true;
		ResetEvent(inputHandle);
	}


	//cerr<< "Waiting for data"<<endl;
	if (WaitForSingleObject( inputHandle, INFINITE)==WAIT_FAILED){
		printf("error receiving event\n");
	}
	ResetEvent(inputHandle);
/*
	for (int i=0; i<WaveInHeader[buf_i].dwBufferLength; i++){
		buffer[i]= ((byte_t*)WaveInHeader[buf_i].lpData)[i];
	}
*/
	//memcpy(buffer, WaveInHeader[buf_i].lpData, WaveInHeader[buf_i].dwBufferLength);
	
	WaveInHeader[buf_i].lpData=(char*)buffer;

	MMRESULT err = waveInAddBuffer(waveInDevice, &WaveInHeader[buf_i], sizeof(WAVEHDR));
	if (err != MMSYSERR_NOERROR){
		cerr << "Error: "<<endl;
		printf("readFromDevice: could not do waveInAddBuffer\n");
	}
	
	cerr << "R"<<flush;

	buf_i++;
	buf_i=buf_i%2;
	return nSamples;
}



int WaveSoundDevice::writeToDevice(byte_t *buffer, uint32_t nSamples){
	static int buf_i=0;
	static int count=0;
	MMRESULT err;

//	for (unsigned i=0; i<waveOutHeader[buf_i].dwBufferLength/2; i++){
//		((short*)waveOutHeader[buf_i].lpData)[i] =  i%2==1  ?  5000 : -5000;
//	}	
	
	if(!startedPlaying){
	//	timeBeginPeriod(2);
		memcpy(waveOutHeader[NUMOUTPUTBUFFER-1].lpData,buffer, waveOutHeader[NUMOUTPUTBUFFER-1].dwBufferLength);
		/*for(int i=0;i<NUMOUTPUTBUFFER;i++){	
			err=waveOutWrite(waveOutDevice, &waveOutHeader[i], sizeof(WAVEHDR));
			if(err!=MMSYSERR_NOERROR){
				cerr << "Error code: "<<err <<endl;
				printf("writeToDevice: !!!!waveOutWrite Error!!!\n");
			}       
		}
		*/
		err=waveOutWrite(waveOutDevice, &waveOutHeader[0], sizeof(WAVEHDR));
		err=waveOutWrite(waveOutDevice, &waveOutHeader[1], sizeof(WAVEHDR));
		if(err!=MMSYSERR_NOERROR){
			cerr << "Error code: "<<err <<endl;
			printf("writeToDevice: !!!!waveOutWrite Error!!!\n");
		}       
		startedPlaying=true;
		ResetEvent(outputHandle);
	}
	

	if (WaitForSingleObject( outputHandle, INFINITE)==WAIT_FAILED){
		printf("error receiving event\n");
	}
	ResetEvent(outputHandle);


	memcpy(waveOutHeader[buf_i].lpData,buffer, waveOutHeader[buf_i].dwBufferLength);
	

	err=waveOutWrite(waveOutDevice, &waveOutHeader[buf_i], sizeof(WAVEHDR));
	if(err!=MMSYSERR_NOERROR){
		cerr << "Error code: "<<err <<endl;
		printf("writeToDevice: !!!!waveOutWrite Error!!!\n");
	}                      

	buf_i++;
	buf_i=buf_i%NUMOUTPUTBUFFER;
	return nSamples;
}



int WaveSoundDevice::readError( int errcode, byte_t * buffer, uint32_t nSamples ) {
	return -1;
}

int WaveSoundDevice::writeError( int errcode, byte_t * buffer, uint32_t nSamples ) {
	return -1;
}
	
void WaveSoundDevice::sync(){
}

	
