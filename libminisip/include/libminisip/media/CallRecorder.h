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

/* Copyright (C) 2004, 2005, 2006
 *
 * Authors: Cesc Santasusana (cesc DOT santa A{T gmail D.OT com )
*/


#ifndef CALL_RECORDER_H
#define CALL_RECORDER_H

#define CALLREC_BUFF_SIZE 16000

#include<libminisip/media/MediaStream.h>
#include<libminisip/media/soundcard/SoundRecorderCallback.h>
#include<libminisip/media/soundcard/FileSoundDevice.h>

template <class T> class MRef;
class RealtimeMediaStreamReceiver;
class AudioMedia;
class RtpReceiver;
class IpProvider;
class RtpPacket;
class SRtpPacket;
class Mutex;
class CircularBuffer;

/**
 The call recorder is to be used to record all audio in & out related
  to a particular call.
  It inherits from SoundRecorderCallback (to be registered as a recorder receiver
    in SoundIO) and from RealtimeMediaStreamReceiver (to be registered as a stream receiver
    to the associated RtpReceiver).
  It implements a producer/consumer model, where we have two producers (one for the 
  	SoundIO::playerLoop() and another for the RtpReceiver::run(). The nice thing
	is that we do not need an extra thread for the consumer: everytime one of the
	producers writes to its buffer, it executes a consume (flush() ) function. 
	This is mutex protected as well as it keeps control of the last time it was 
	executed, allowing one execution every 20 ms (time enough to have received
	on bunch of samples from each producer).
*/
class CallRecorder: 
		public RealtimeMediaStreamReceiver,
		public SoundRecorderCallback  { 
	public:
		/**
		 * Upgrade constructor ... build an override receiver from
		 *   a normal receiver
		 * @param ssrc ssrc of the call, so we can register to the AudioMedia and
		 *		receive the rtp packets related to it. Actually, it is 
		 *		the Session::callid.
		 * @param media reference to the Media object (actually, an AudioMedia)
		 */
		CallRecorder(  MRef<AudioMedia *> aMedia, 
				MRef<RtpReceiver *> rtpReceiver,
				MRef<IpProvider *> ipProvider );
		virtual ~CallRecorder( );

		/**
		 * Removes reference loops (needs to be called to let the 
		 * garbage collector remove this object)
		 */
		void free();

		virtual std::string getMemObjectType() const {return "CallRecorder";}


		/**
		* Inherited from SoundRecorderCallback
		* Function that will be called when sound data is available from
		* the soundcard.
		* @param samplearr Array of raw sound data. Typically 160 
		* samples, 16 bit, but it can be specified in SoundCard.
		* @param length length of samplearr
		* @see SoundCard
		*/
		virtual void srcb_handleSound(void *samplearr, int length);
		#ifdef AEC_SUPPORT
		virtual void srcb_handleSound(void *samplearr, void *samplearrR);
		#endif
		
		/**
		Inherited from RealtimeMediaStreamReceiver
		*/
		virtual void handleRtpPacket( MRef<SRtpPacket *> packet, MRef<IPAddress *> from );
// 		virtual void handleRtpPacket( MRef<SRtpPacket *> packet );

		/**
		Get the filename of the file where audio is stored
		*/
		std::string getFilename() { return filename; };
		
		/**
		Sets the name to be used for the file (use it before opening it!)
		@param name a user supplied string, to ease identification of the file
		@param ssrc ssrc of the call
		*/
		void setFilename( std::string name, int ssrc );
		
#if 0
		/**
		Set the ssrc (int) of the AudioMediaSource
		@param ssrc a std::string identifying this source.
		*/
		void setSsrc( std::string ssrc );
#endif
		
		/**
		set/get functions to check whether the callRecorder is 
		enabled or not. We can enable/disable the network side and 
		the microphone side. 
		*/
		bool isEnabled() { return enabledMic && enabledNtwk; };
		void setEnabledMic( bool en ) { 
			enabledMic = en; 
			#ifdef DEBUG_OUTPUT
			std::cerr << getDebugString() << "[1]"  << std::endl;
			#endif
		};
		void setEnabledNetwork( bool en ) { 
			enabledNtwk = en; 
			#ifdef DEBUG_OUTPUT
			std::cerr << getDebugString() << "[2]"  << std::endl;
			#endif
		};
		
		void setAllowStart( bool allow ) { 
			allowStart = allow; 
			#ifdef DEBUG_OUTPUT
			std::cerr << getDebugString() << "[3]" << std::endl;
			#endif
			if( getFilename() != "" ) {
				std::cerr << "CallRecorder: Stopped recording to file <" 
					<< getFilename() << ">" << std::endl;
			}
		};
		
			
		/**
		Used by the microphone producer thread (SoundIO::playerLoop)
		to add samples to the mic buffer.
		@param data a short * buffer
		@param nSamples number of samples in the buffer (short size)
		*/
		void addMicData( void * data, int nSamples );
		
		/**
		Used by the network producer thread (RtpReceiver::run)
		to add samples to the ntwk buffer.
		@param data a short * buffer
		@param nSamples number of samples in the buffer (short size)
		*/
		void addNtwkData( void * data, int nSamples );
		
		/**
		Consumer function, called by either of the two producer functions.
		It only allows execution every Xms. When this time has elapsed, 
		it reads a bunch of samples from each buffer and "mixes" them 
		accordingly, and then sends them to write()
		*/
		void flush( );
	
		/**
		Write audio samples to the file, via the FileDevice
		@param data Audio samples to be written to the file
		@param length length of the data, in samples
		*/
		int write( void * data, int length );
	
#ifdef DEBUG_OUTPUT
		std::string getDebugString();
#endif	
		
	protected:
	
		
		/**
		Name of the file to which we record to.
		*/
		std::string filename;
		
		/**
		Whether it is active or not. If enabled = false, nothing
		will be written to the file.
		*/
		bool enabledMic;
		bool enabledNtwk;
		
		/**
		Allow start acts like a global enable/disable ... 
		*/
		bool allowStart;
		
		/**
		Physical access to a file, where we can write.
		*/
		MRef<FileSoundDevice *> fileDev;
		
		Mutex fileDevMutex;
		
		/**
		Via this object we can access all the other audio related
		objects ... SoundIO, AudioSoundSource, Codecs, ... 
		*/
		MRef<AudioMedia *> audioMedia;
		
		/**
		Temp buffer used to hold the output of the codec, when 
		decoding the samples received from the network.
		*/
		short codecOutput[320]; //we take max 160 samples ...
		
		/**
		Temp buffer used to receive the resampled samples from the
		soundcard (we get at 48000Hz, we need 8000Hz, thus downsample).
		*/
		short resampledData[160];
		
		/**
		Buffer where the microphone producer dumps the audio samples
		collected.
		*/
		CircularBuffer * micData;
		Mutex micMutex;
		
		/**
		Buffer where the network producer dumps the audio samples
		collected.
		*/
		CircularBuffer * ntwkData;
		Mutex ntwkMutex;
			
		/**
		Temp Buffer used in the flush function ... we use it
		to read the samples from the circular buffers previous to mixing,
		as well as after "normalizing", to send to the write() function
		*/
		short tempFlush[160*2];
		
		/**
		Temp buffer, where we can mix several streams (it has a bigger 
		byte size, 32 bits, thus not easy to overflow).
		*/
		int mixedData[160*2];
		
		/**
		Used in the flush() function to keep track of the last time 
		we sent to write some audio samples. Only allow execution 
		every Xms ...
		*/
		uint64_t flushLastTime;
};

#endif
