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
*/

#ifndef SOUND_DEVICE_H
#define SOUND_DEVICE_H

#include<config.h>

#ifdef _MSC_VER
#ifndef byte_t
typedef unsigned char  byte_t;
#endif
#ifndef int32_t
typedef __int32  int32_t;
#endif
#ifndef uint32_t
typedef unsigned int  uint32_t;
#endif

#else
#include<stdint.h>
#endif

#include<libmutil/Mutex.h>
#include<libmutil/MemObject.h>

#include<iostream>

/**
Define sound types, specially useful for the soundcard access
SOUND_XNNYE
 - X  is S or U, meaning signed or unsigned samples
 - NN is the size of the samples, in bits (8, 16, 32, .. )
 - Y is the endiannes (L little, B big).

The defined values have hardware meaning when opening a real soundcard.
(for now, only the 16 bit ones ... the rest are random values.
*/
#define SOUND_S16LE 	0xF0
#define SOUND_S16BE 	0xF1
#define SOUND_U16LE	0xF2
#define SOUND_U16BE	0xF3
#define SOUND_S8LE	0x01
#define SOUND_U8LE	0x02
#define SOUND_S32LE	0x03
#define SOUND_U32LE	0x04

/**
Provide an API to heterogeneous sound devices (source and sink).
SoundIO class uses this class.
Playback refers to the action of sending audio samples to the 
	device in order for it to "play" them.
		Ex - write to soundcard, thus playing on the speakers
Record refers to the action of reading from the device in order
	to process and send them to the network.
		Ex - read from soundcard/mic, to capture the sound
*/
class SoundDevice: public MObject{
	public:
		/**
		Creator function. 
		Depending on the string provided as deviceId, it will create
		a FileSoundDevice, an AlsaDevice, OSSDevice, DirectSound, etc.
		How to choose what kind of? 
		if deviceId = file:.....      -> FileSoundDevice
		if deviceId = alsa: ....      -> AlsaSoundDevice
		if deviceId = dsound: ....    -> DirectSoundDevice
		anything else (i.e /dev/dsp)  -> OssSoundDevice
		See each .h for extra information
		@param deviceId string identifying the device. 
		*/
		static MRef<SoundDevice *> create( std::string deviceId );

		virtual ~SoundDevice();

		/**
		Open the "record" side of the SoundDevice, that is, the one 
		in charge of reading from the device.
		*/
		virtual int openRecord( int32_t samplingRate, int nChannels, int format )=0;
		
		/**
		Open the "playback" side of the SoundDevice, that is, the one 
		in charge of writing to the device.
		*/
		virtual int openPlayback( int32_t samplingRate, int nChannels, int format )=0;
		
		virtual int closeRecord()=0;
		virtual int closePlayback()=0;
		
		bool isOpenedPlayback(){return openedPlayback;};
		bool isOpenedRecord(){return openedRecord;};

		/**
		Read from the device.
		The buffer read will contain the data of all channels interleaved, for 
		example, stereo: L-R-L-R ...
		@param buffer byte pointer to a memory block where the audio samples
			are to be stored. 
		@param nSamples number of samples in the buffer. To obtain the actual byte
			length of the buffer, use nSamples * getSampleSize() * getNChannelsXX()
		*/
		virtual int read( byte_t * buffer, uint32_t nSamples )=0;
		
		/**
		Write to the device.
		The buffer data to write must be already in interleaved format, that is,
		for example for stereo: L-R-L-R - ... 
		@param buffer byte pointer to a memory block where the audio samples
			are stored. 
		@param nSamples number of samples in the buffer. To obtain the actual byte
			length of the buffer, use nSamples * getSampleSize() * getNChannelsXX()
		*/
		virtual int write( byte_t * buffer, uint32_t nSamples )=0;

		/**
		Wait till the devices buffers are empty.
		*/
		virtual void sync()=0;

		/**
		Locking functions
		*/
		void lockRead();
		void unlockRead();
		void lockWrite();
		void unlockWrite();
		
		/**
		String identifying the device
		*/
		std::string dev;

		int getSamplingRate(){ return samplingRate; };
		int getNChannelsPlay(){ return nChannelsPlay; };
		int getNChannelsRecord(){ return nChannelsRecord; };
		
		/**
		Use setFormat() to set the format of the audio samples for this
		device. It will set the format variable, as well as the sampleSize
		variable according to the format selected.
		*/
		virtual void setFormat( int format_ );
		
		/**
		Return the number of bytes per sample.
		*/
		int getSampleSize() { return sampleSize; }

	protected:
		SoundDevice( std::string fileName );

		/**
		Sampling rate, in samples per second (Hz)
		*/
		int samplingRate;
		
		/**
		Number of channels, for playback or for recording.
		*/
		int nChannelsPlay;
		int nChannelsRecord;
		
		/**
		Format of the audio samples read/written by this device.
		It has a per device meaning, thus the value of this param
		is dependant on the device. 
		Use the getSampleSizeXXX() functions to obtain the size 
		of the samples (in bytes).
		Format can provide more info, like signed/unsigned samples,
		big endian or little endian ...
		*/
		int format;
		
		/**
		Bytes per audio sample.
		*/
		int sampleSize;
		
		bool openedRecord;
		bool openedPlayback;
		
	private:
                Mutex mLockRead;
                Mutex mLockWrite;

};

#endif
