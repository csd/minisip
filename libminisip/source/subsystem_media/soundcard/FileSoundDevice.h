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

#ifndef FILESOUNDDEVICE_H
#define FILESOUNDDEVICE_H

#include<libminisip/libminisip_config.h>

#ifdef _MSC_VER
#	include<io.h>
#	undef open
#	undef close
#	undef read
#	undef write
#else
#	include<sys/time.h>
#	include<unistd.h>
#endif

#include<libminisip/media/soundcard/SoundDevice.h>

#include<stdio.h>
#include <fcntl.h>
#include<iostream>


#define FILESOUND_TYPE_RAW 0
#define FILESOUND_TYPE_WAV 1
#define FILESOUND_TYPE_MP3 2

/**
SoundDevice which reads from/to files, simulating a synchronous
access to the soundcard.
The deviceId needs to be such:
file:RECORDFILE,PLAYBACKFILE
where
- RECORDFILE is a readable file, from where we extract audio samples
	and send them to the callee. If the file was a soundcard,
	this would be the microphone.
	Example: /tmp/minisip.audio.mic
- PLAYBACKFILE is a writable file, where we dump all the received 
	audio.
	Example: /tmp/minisip.audio.dump
	
When opened from the SoundIO class, it will be opened using the 
same parameters as any other soundcard, this means
	samplingRate = 48000
	numberChannels = 2
	format = 2 bytes per sample, unsigned
NOTE: i know it needs/generates huge files ... but hey, they are 
	in a very good audio quality ;)
	
TODO: implement other filetypes, not just RAW. Interesting would be
	at least WAV (like RAW but with header) and MP3 (to save space).

NOTE: The output/input of this device for now is RAW. This means that 
	to play it or record it you most probably need to first create 
	a WAV or MP3 file and then convert it to RAW (use the linux
	tool SOX, for example).
	sox somemp3.mp3 -r 48000 -c 2 -s -w minisip.play.sw
*/
class LIBMINISIP_API FileSoundDevice: public SoundDevice{
	public:
		FileSoundDevice(std::string in_file="", 
				std::string out_file="", 
				int32_t filetype=FILESOUND_TYPE_RAW );
		
		/**
		We need to add a blocking behavior to the reading of the file, 
		plus looping.
		Thus, we some specific code, but still call the SoundDevice::read function
		*/
		virtual int read( byte_t * buffer, uint32_t nSamples );
		
		/**
		Read from the "record" file, that is, this file is read
		and the audio samples are to be processed (i.e, to send to
		the network).
		*/
		virtual int readFromDevice( byte_t * buffer, uint32_t nSamples );
		
		/**
		We need to add a blocking behavior to the writing of the file
		(only if SoundDevice::sleepTime == 0 --> device playback open in blocking mode).
		Thus, we some specific code, but still call the SoundDevice::write function
		*/
		virtual int write( byte_t * buffer, uint32_t nSamples );
		
		/**
		Write to the "playback" file, that is, the file where the
		"received" audio is stored for later playback.
		*/
		virtual int writeToDevice( byte_t * buffer, uint32_t nSamples );

		virtual int readError( int errcode, byte_t * buffer, uint32_t nSamples );
		virtual int writeError( int errcode, byte_t * buffer, uint32_t nSamples );
		
		virtual int openPlayback( int32_t samplingRate, int nChannels, int format=SOUND_S16LE );
		virtual int openRecord( int32_t samplingRate, int nChannels, int format=SOUND_S16LE );
		
		virtual int closePlayback();
		virtual int closeRecord();
		
		/**
		Set/get functions for the looping of the record file (the one played to the
		other peer). 
		If set to true, the file is looped for as long as the call is active.
		*/
		bool getLoopRecord() { return loopRecord; };
		void setLoopRecord( bool loop ) { loopRecord = loop; };
		
		virtual void sync();

		virtual std::string getMemObjectType() const { return "FileSoundDevice";};

	protected:
	
		void setAudioParams( int nChannels_, int filetype_ );
	
		/**
		File type of this device. For now, we can only open the same type 
		for reading and writing ... 
		//FIXME the above ...
		*/
		int fileType;
		
		/**
		Filenames
		 - inFile is the "record" file, to read from
		 - outFile is the "playback" file, to write to
		*/
                std::string inFilename;
                std::string outFilename;
		
		/**
		File descriptors
		*/
		int in_fd;
                int out_fd;
		
		/**
		During a single execution of minisip, the sound device is started
		and stopped several times. 
		If it is the first time we open for playback (dump audio to a file),
		we create the file with filesize = 0. If it is not the first time
		we open the sound device in the current run of minisip, we do not
		create it, we open it in APPEND mode.
		*/
		bool isFirstTimeOpenWrite;
		
		/**
		Whether to loop or not the record file, that is, if the other peer
		will receive the same message over and over.
		*/
		bool loopRecord;
                
		/**
		Print the "errno" variables ... for debug
		*/
		void printError( std::string func );
		
		
		/**
		Do not reuse the sleepTime variable in SoundDevice ... for now 
		(as long as the audio playback is still blocking).
		*/
		uint32_t fileSoundBlockSleep;
		
		/**
		Implements a sleep function, used to synchronize the writing of
		audio samples. It sleeps for sleepTime miliseconds.
		*/		
		void writeSleep( );
		
		/**
		State variable, used in writeSleep()
		*/
		uint64_t lastTimeWrite;

		/**
		Implements a sleep function, used to synchronize the reading of
		audio samples. It sleeps for sleepTime miliseconds.
		*/		
		void readSleep( );
		
		/**
		State variable, used in readSleep()
		*/
		uint64_t lastTimeRead;

		int getFileSize( int fd );
};


#endif	
