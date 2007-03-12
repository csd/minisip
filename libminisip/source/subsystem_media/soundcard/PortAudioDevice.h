/*
 Copyright (C) 2005-2006  Mikael Magnusson <mikma@users.sourceforge.net>
 
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

#ifndef PORTAUDIODEVICE_H
#define PORTAUDIODEVICE_H

#include<libminisip/libminisip_config.h>

#include<portaudio.h>
#include<libmutil/CircularBuffer.h>
#include<libmutil/CondVar.h>
#include<libminisip/media/soundcard/SoundDevice.h>

typedef void PortAudioStream;

class PortAudioDevice: public SoundDevice{
	public:
// 		PortAudioDevice( std::string device );
		PortAudioDevice( PaDeviceIndex device );
		virtual ~PortAudioDevice();
		
		virtual int readFromDevice( byte_t * buffer, uint32_t nSamples );
		virtual int writeToDevice( byte_t * buffer, uint32_t nSamples );
				
		virtual int readError( int errcode, byte_t * buffer, uint32_t nSamples );
		virtual int writeError( int errcode, byte_t * buffer, uint32_t nSamples );
		
		virtual int openPlayback( int32_t samplingRate, int nChannels, int format );
		virtual int openRecord( int32_t samplingRate, int nChannels, int format );
		
		virtual int closePlayback();
		virtual int closeRecord();

		virtual void sync();

		virtual std::string getMemObjectType() const { return "PortaudioDevice";};

	protected:
		virtual int callback( const void *inputBuffer,
				      void *outputBuffer,
				      unsigned long frameCount,
				      const PaStreamCallbackTimeInfo *timeinfo,
				      PaStreamCallbackFlags statusFlags );

		static int paCallback( const void *inputBuffer,
				       void *outputBuffer,
				       unsigned long frameCount,
				       const PaStreamCallbackTimeInfo *timeinfo,
				       PaStreamCallbackFlags statusFlags,
				       void *userData );

	private:
		bool initialized;
		PaTime latency;

		PaDeviceIndex inId;
		PaDeviceIndex outId;

		PaStream *outStream;
		PaStream *inStream;

		CircularBuffer *outRing;
		CircularBuffer *inRing;

		int outBytesPerSample;
		int inBytesPerSample;

		CondVar outCond;
		CondVar inCond;

		Mutex inMutex;
		Mutex outMutex;
};


#endif
