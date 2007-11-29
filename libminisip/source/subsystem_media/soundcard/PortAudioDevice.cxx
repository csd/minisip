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

#include<config.h>

#include"PortAudioDevice.h"

#include<portaudio.h>
#include<math.h>
#include<cstdlib>
#include<iostream>

using namespace std;

const int BUFFER_WIDTH = sizeof(short);

//#define PA_DEBUG

static PaSampleFormat toPaSampleFormat( int format )
{
	switch( format ){
		case SOUND_S16LE:
			return paInt16;
		case SOUND_S16BE:
			return 0;
		case SOUND_U16LE:
			return 0;
		case SOUND_U16BE:
			return 0;
		case SOUND_S8LE:
			return paInt8;
		case SOUND_U8LE:
			return paUInt8;
		case SOUND_S32LE:
			return paInt32;
		case SOUND_U32LE:
			return 0;
		default:
			return 0;
	}
}

//PortAudioDevice::PortAudioDevice( string device ): SoundDevice( device )
PortAudioDevice::PortAudioDevice( PaDeviceIndex device ): SoundDevice("!PORTAUDIO!"), inId( device ), outId( device )
{
	initialized = false;

	outStream = NULL;
	inStream = NULL;

	outRing = NULL;
	inRing = NULL;

	outBytesPerSample = 1;
	inBytesPerSample = 1;

	// non initialized SoundDevice members!
	nChannelsPlay = 1;
	nChannelsRecord = 1;

// 	outId = atoi(device.c_str());
// 	inId = outId;

	latency = 0.020;

	PaError res = Pa_Initialize();

	if( res == paNoError ){
		initialized = true;
	}
	else
		merr << "PortAudio failed to initialize: " << Pa_GetErrorText( res ) << endl;
}

PortAudioDevice::~PortAudioDevice()
{
	if( initialized ){
		Pa_Terminate();
		initialized = false;
	}
}

int PortAudioDevice::readFromDevice( byte_t * buffer, uint32_t nSamples )
{
	long nBytes = nSamples * inBytesPerSample * nChannelsRecord;

#ifdef PA_DEBUG
	cerr << "readFromDevice" << endl;
#endif
	inMutex.lock();
	while( inRing ){
		long available = inRing->getSize() * BUFFER_WIDTH;

		if( available < nBytes ){
#ifdef PA_DEBUG
			char buf[128];

			snprintf(buf, sizeof(buf), "readFromDevice wait %ld < %ld", available, nBytes);
			cerr << buf << endl;
#endif
			inCond.wait( inMutex );
#ifdef PA_DEBUG
			cerr << "readFromDevice wake up" << endl;
#endif
			continue;
		}

		bool res = inRing->read((short *)buffer, nBytes / BUFFER_WIDTH );
		inMutex.unlock();
		return res ? nBytes / inBytesPerSample / nChannelsRecord : 0;
	}
	inMutex.unlock();

	return -1;
}

int PortAudioDevice::writeToDevice( byte_t * buffer, uint32_t nSamples )
{
	long nBytes = nSamples * outBytesPerSample * nChannelsPlay;

#ifdef PA_DEBUG
	cerr << "writeToDevice" << endl;
#endif
	outMutex.lock();
	while( outRing ){
		long available = outRing->getFree() * BUFFER_WIDTH;

		if( available < nBytes ){
#ifdef PA_DEBUG
			char buf[128];

			snprintf(buf, sizeof(buf), "writeToDevice wait %ld < %ld", available, nBytes);
			cerr << buf << endl;
#endif
			outCond.wait( outMutex );
#ifdef PA_DEBUG
			cerr << "writeToDevice wake up" << endl;
#endif
			continue;
		}

		bool res = outRing->write( (const short *)buffer, nBytes / BUFFER_WIDTH );
		outMutex.unlock();
		return res ? nBytes / outBytesPerSample / nChannelsPlay : 0;
	}
	outMutex.unlock();

	return -1;
}
				
int PortAudioDevice::readError( int errcode, byte_t * buffer, uint32_t nSamples )
{
	// TODO
	cerr << "PortAudioDevice::readError unimplemented" << endl;
	return -1;
}

int PortAudioDevice::writeError( int errcode, byte_t * buffer, uint32_t nSamples )
{
	// TODO
	cerr << "PortAudioDevice::writeError unimplemented" << endl;
	return -1;
}
		
int PortAudioDevice::openPlayback( int32_t samplingRate, int nChannels, int format )
{
	PaError err;
	PaSampleFormat sampleFormat = toPaSampleFormat( format );
	PaStreamParameters outParams;

	memset(&outParams, 0, sizeof(outParams));

	outParams.device = outId;
	outParams.channelCount = nChannels;
	outParams.sampleFormat = sampleFormat;
	outParams.suggestedLatency = latency;

	err = Pa_OpenStream( &outStream,
			     NULL,
			     &outParams,
			     samplingRate,
			     (unsigned long)(latency * samplingRate),
			     0,
			     paCallback, this );

	if( err != paNoError ){
		cerr << "openPlayback " << Pa_GetErrorText( err ) << endl;
		return -1;
	}

	nChannelsPlay = nChannels;
	outBytesPerSample = Pa_GetSampleSize( sampleFormat );

	long num = 2 << int(ceil(log(samplingRate * latency * 2 * nChannels * sizeof(short))/log(2)));

#ifdef DEBUG_OUTPUT
	cerr << "RingBuffer playback size: " << num << endl;
#endif
	
	outRing = new CircularBuffer(num / BUFFER_WIDTH);

	if( Pa_StartStream( outStream ) != paNoError ){
		cerr << "Pa_StartStream failed" << endl;
		return -1;
	}

	openedPlayback = true;
	return 0;
}

int PortAudioDevice::openRecord( int32_t samplingRate, int nChannels, int format )
{
	PaError err;
	PaSampleFormat sampleFormat = toPaSampleFormat( format );
	PaStreamParameters inParams;

	memset(&inParams, 0, sizeof(inParams));

	inParams.device = inId;
	inParams.channelCount = nChannels;
	inParams.sampleFormat = sampleFormat;
	inParams.suggestedLatency = latency;


	err = Pa_OpenStream( &inStream,
			     &inParams,
			     NULL,
			     samplingRate,
			     (unsigned long)(latency * samplingRate),
			     0,
			     paCallback, this );

	if( err != paNoError ){
		cerr << "openRecord " << Pa_GetErrorText( err ) << endl;
		return -1;
	}

	nChannelsRecord = nChannels;
	inBytesPerSample = Pa_GetSampleSize( sampleFormat );

	long num = 2 << int(ceil(log(samplingRate * latency * 2 * nChannels * sizeof(short)) / log(2)));

#ifdef DEBUG_OUTPUT
	cerr << "RingBuffer record size: " << num << endl;
#endif

	inRing = new CircularBuffer(num / BUFFER_WIDTH);

	if( Pa_StartStream( inStream ) != paNoError ){
		cerr << "Pa_StartStream failed" << endl;
		return -1;
	}

	openedRecord = true;
	return 0;
}
		
int PortAudioDevice::closePlayback()
{
	if( outStream ){
		Pa_StopStream( outStream );
		Pa_CloseStream( outStream );
		outStream = NULL;

		outMutex.lock();

		if( outRing ){
			delete outRing;
			outRing = NULL;
		}
		
		outMutex.unlock();

		openedPlayback = false;

		return 0;
	}

	return -1;
}

int PortAudioDevice::closeRecord()
{
	if( inStream ){
		Pa_StopStream( inStream );
		Pa_CloseStream( inStream );
		inStream = NULL;

		inMutex.lock();

		if( inRing ){
			delete inRing;
			inRing = NULL;
		}
		
		inMutex.unlock();

		openedRecord = false;

		return 0;
	}

	return -1;
}

void PortAudioDevice::sync()
{
	// TODO
	cerr << "PortAudioDevice::sync unimplemented" << endl;
}

int PortAudioDevice::paCallback( const void *inputBuffer,
				 void *outputBuffer,
				 unsigned long frameCount,
				 const PaStreamCallbackTimeInfo *timeinfo,
				 PaStreamCallbackFlags statusFlags,
				 void *userData )
{
	PortAudioDevice *device = (PortAudioDevice*)userData;
	return device->callback( inputBuffer, outputBuffer,
				 frameCount, timeinfo, statusFlags );
}

int PortAudioDevice::callback( const void *inputBuffer,
			       void *outputBuffer,
			       unsigned long frameCount,
			       const PaStreamCallbackTimeInfo *timeinfo,
			       PaStreamCallbackFlags statusFlags )
{
	if( inputBuffer && inRing ){
		inMutex.lock();

		//		cerr << frameCount << ',';

		long nBytes = frameCount * inBytesPerSample * nChannelsRecord;
		long available = inRing->getFree() * BUFFER_WIDTH;

		if( available > nBytes ){
			available = nBytes;
		} else if( available < nBytes ){
			cerr << 'V';
		}

		inRing->write( (short *)inputBuffer, available / BUFFER_WIDTH );
		inCond.broadcast();
#ifdef PA_DEBUG
		cerr << '<';
#endif
		inMutex.unlock();
	}

	if( outputBuffer ) {

		long nBytes = frameCount * outBytesPerSample * nChannelsPlay;
		if ( outRing ){
			outMutex.lock();
			long available = outRing->getSize() * BUFFER_WIDTH;

			if( available > nBytes ){
				available = nBytes;
			} else if( available < nBytes ){
				cerr << '^';
			}

			outRing->read( (short *)outputBuffer, available / BUFFER_WIDTH );
			outCond.broadcast();
			
			if( available < nBytes ){
				memset( (byte_t*)outputBuffer + available, 0, nBytes - available );
			}
#ifdef PA_DEBUG
			cerr << '>';
#endif
			outMutex.unlock();
		}
		else {
			memset( outputBuffer, 0, nBytes );
		}
	}

	return 0;
}
