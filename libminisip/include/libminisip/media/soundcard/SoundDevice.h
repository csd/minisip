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

#ifndef SOUND_DEVICE_H
#define SOUND_DEVICE_H

#include<libminisip/libminisip_config.h>

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
		
The key functions are not implemented, as they are device dependant
(open, close, read, write, ... )

VERY IMPORTANT: 
The device is opened in blocking mode for recording, 
but it is opened in NON-BLOCKING mode for playback (it has to be like
this, otherwise the playback thread cannot catch up with the producers
on the network side).
*/
class LIBMINISIP_API SoundDevice: public MObject{
	public:
		/**
		Creator function. 
		Depending on the string provided as deviceId, it will create
		a FileSoundDevice, an AlsaDevice, OSSDevice, DirectSound, etc.
		How to choose what kind of? 
		if deviceId = file:.....      -> FileSoundDevice
		if deviceId = alsa: ....      -> AlsaSoundDevice
		if deviceId = dsound: ....    -> DirectSoundDevice
		if deviceId = wave: ....    -> WaveSoundDevice
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
		OPENS IN NON-BLOCKING mode.
		*/
		virtual int openPlayback( int32_t samplingRate, int nChannels, int format )=0;
		
		/**
		Close the device for recording only (no more recording possible, till it is opened again)
		*/
		virtual int closeRecord()=0;
		
		/**
		Close the device for playback only (no more playback possible, till it is opened again).
		*/
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
			length of the buffer in bytes, use 
			nSamples * getSampleSize() * getNChannelsXX()
		
		@return It returns the number of samples read (not bytes! samples), or -1 if error
		*/
		virtual int read( byte_t * buffer, uint32_t nSamples );

		/**
		This is the actual call to the device read ... it is done automatically by the "read" function.
		Do not loop or anything. Just attempt to write once and return the result. No more.
		@param The same as "read"
		@return number of samples read, like "read". If error, it returns a negative error code (-EAGAIN,
			-EBADF, etc ...; it is important). If a sync error is detected, return -EPIPE!
		*/
		virtual int readFromDevice( byte_t * buffer, uint32_t nSamples ) = 0;
		
		/**
		If after reading from the device (readFromDevice) we have an error (errcode < 0 ), call this function. 
		Error handling is device dependant, so SoundDevice does not implement it.
		
		@return -1 if something went wrong (read function will exit); 
			0 otherwise (the read will keep trying to read)
		*/
		virtual int readError( int errcode, byte_t * buffer, uint32_t nSamples ) = 0;
		
		/**
		Write to the device.
		The buffer data to write must be already in interleaved format, that is,
		for example for stereo: L-R-L-R - ... 
		@param buffer byte pointer to a memory block where the audio samples
			are stored. 
		@param nSamples number of samples in the buffer. To obtain the actual byte
			length of the buffer in bytes, use
			nSamples * getSampleSize() * getNChannelsXX()
		
		@return It will return the number of samples written (not bytes! samples), or -1 if error
		*/
		virtual int write( byte_t * buffer, uint32_t nSamples );

		/**
		Actual write function to the device. It is used by "write" automatically.
		Do not loop or anything. Just attempt to write once and return the result. No more.
		@param The same as "read"
		@return number of samples written, like "write". If error, it returns a negative error code (-EAGAIN,
			-EBADF, etc ...; it is important). If a sync error is detected, return -EPIPE!
		*/
		virtual int writeToDevice( byte_t * buffer, uint32_t nSamples ) = 0;

		/**
		If after writing to the device we have an error (errcode < 0 ), call this function. 
		Error handling is device dependant, so SoundDevice does not implement it.
		
		@return -1 if something went wrong (write function will exit); 
			0 otherwise (the write will keep trying to write)
		*/
		virtual int writeError( int errcode, byte_t * buffer, uint32_t nSamples ) = 0;
		
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
		
		/**
		Controlls the sleep time for the read/write operations
		This allows to simulate a synchronous source/sink of data, like a 
		soundcard would do. Otherwise, when reading from a file, for example, 
		would produce data continuously.
		@param sleep set the sleep timeout for read/write, in miliseconds. -1 deactivates
			the sleep. 
		*/
		void setSleepTime( int sleep ) { sleepTime = sleep; };
		int getSleepTime( ) { return sleepTime; };

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
		
		/**
		See setSleepTimer function.
		Timeout, in miliseconds.
		Default value is 20 milisecons
		*/
		uint32_t sleepTime;
		
		bool openedRecord;
		bool openedPlayback;
		
	private:
                Mutex mLockRead;
                Mutex mLockWrite;

};

#endif
