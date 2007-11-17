#ifndef WAVE_SOUND_DEVICE_MSIP_H
#define WAVE_SOUND_DEVICE_MSIP_H
 
#include<libminisip/libminisip_config.h>

#include <windows.h>
#include <commctrl.h>
#include <iostream>
#include <mmsystem.h>
#include<libminisip/media/soundcard/SoundDevice.h>
#include<libminisip/media/soundcard/SoundDevice.h>

class LIBMINISIP_API WaveSoundDevice:public SoundDevice{

	public:
		virtual ~WaveSoundDevice( );
		WaveSoundDevice( std::string device );
		
		virtual int readFromDevice( byte_t * buffer, uint32_t nSamples );
		virtual int writeToDevice( byte_t * buffer, uint32_t nSamples );
				
		//virtual int readError( int errcode, byte_t * buffer, uint32_t nSamples );
		//virtual int writeError( int errcode, byte_t * buffer, uint32_t nSamples );
		
		virtual int openPlayback( int32_t samplingRate, int nChannels, int format );
		virtual int openRecord( int32_t samplingRate, int nChannels, int format );
		
		virtual int closePlayback();
		virtual int closeRecord();

		virtual int readError( int errcode, byte_t * buffer, uint32_t nSamples );
		virtual int writeError( int errcode, byte_t * buffer, uint32_t nSamples );

		void playBack();
		
		virtual void sync();

		virtual std::string getMemObjectType() const { return "OssSoundDevice";};

//		HWND getWindow(){return hWnd;};

		//already provided by the parrent class > soundDevice
		int getSampleSize(){return 2;}
		int getNChannelsPlay(){ return 1; };
		int getNChannelsRecord(){ return 1; };
		

	private:

	
	/* WAVEFORMATEX structure for reading in the WAVE fmt chunk */
	//bool openedPlayBack;
	//bool openedRecord;
//window
//	HWND hWnd;
};


void playback();
void freeBlocks(WAVEHDR* blockArray);
void CALLBACK waveOutProc(HWAVEOUT hWaveOut,
							UINT uMsg,
							DWORD dwInstance, 
							DWORD dwParam1,
							DWORD dwParam2 );


/*
long QueueWaveData(WAVEHDR * waveHeader);
int stopRecord();
int setOutputFormat();
int prepareOutputBuffers();
DWORD WINAPI waveInProc(LPVOID arg);
void CALLBACK WaveOutProc(HWAVEOUT waveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
*/
#endif
