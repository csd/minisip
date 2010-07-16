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
*/

#ifndef AUDIO_MEDIA_AND_AUDIOSOURCE_H
#define AUDIO_MEDIA_AND_AUDIOSOURCE_H

#include<libminisip/libminisip_config.h>

#include<libminisip/media/RealtimeMedia.h>
#include<libminisip/media/soundcard/SoundIO.h>

#ifdef AEC_SUPPORT
#include<libminisip/media/aec/aec.h>		//hanning
#endif

#include<string>

class AudioMediaSource;
class SilenceSensor;
class Resampler;

#define AUDIOMEDIA_CODEC_MAXLEN 16384

/**
 * The AudioMedia class holds the object required to acquire and
 * play out audio data. It is created upon startup, or when the
 * media configuration changes.
 */
class LIBMINISIP_API AudioMedia : public RealtimeMedia, public SoundRecorderCallback{

	public:
		/**
		* Constructor, called by the MediaHandler upon
		* initialization, at startup or when the media
		* configuration changes.
		* @param soundIo a reference to the SoundIO object to 
		* use for interaction with the soundcard
		* @param codecList a list of references to Codec
		* objects, representing the CODEC chosen by the user
		* and sorted according to her preference
		*/
		AudioMedia( MRef<SoundIO *> soundIo, const std::list<MRef<Codec *> > & codecList );
		
		virtual std::string getMemObjectType() const {return "AudioMedia";}

		/**
		* Returns the media type as used in the SDP (audio or video).
		* @returns the media type as a string
		*/
		virtual std::string getSdpMediaType();
		
		uint8_t  getCodecgetSdpMediaType(){};

///////
		  MRef<CodecState *> getCodecInstance (){};

		/**
		* Play the given RTP packet on this medium. This includes
		* decoding if relevant.
		* @param rtpPacket the RTP packet to play
		*/
		virtual void playData( const MRef<RtpPacket *> & rtpPacket );

		/**
		* Used by the media sessions to register a MediaStreamSender.
		* When a MediaStreamSender is registered to a Media object,
		* it will be used to send sampled media from the corresponding
		* medium
		* @param sender a reference to the MediaStreamSender object to
		* register
		*/
		virtual void registerRealtimeMediaSender( MRef<RealtimeMediaStreamSender *> sender );


		/**
		* Used by the media sessions to unregister a MediaStreamSender,
		* when a media session ends.
		* @param sender a reference to the MediaStreamSender object to
		* unregister
		*/
		virtual void unregisterRealtimeMediaSender( MRef<RealtimeMediaStreamSender *> sender );
		/**
		* Used to register a new media source. Called upon discovery
		* of a new SSRC identifier. Each media source may use
		* a different decoder.
		* @param ssrc the SSRC identifier used by the new media source
		*/
		virtual void registerMediaSource( uint32_t ssrc, std::string callId );


		/**
		* Used to unregister a media source when the session ends.
		* @param ssrc the SSRC identifier used by the media source to
		* unregister
		*/
		virtual void unregisterMediaSource( uint32_t ssrc );

		/**
		* Callback used by the SoundIO when sound samples are
		* available. The AudioMedia will process them by
		* encoding them and sending them to all the registered
		* MediaStreamSender objects.
		* @param samplearr pointer to the audio samples
		* @param length length of samplearr
		*/
		virtual void srcb_handleSound( void *samplearr, int length, int samplerate );
		#ifdef AEC_SUPPORT 
		virtual void srcb_handleSound( void *samplearr, int length, void *samplearrR);	//hanning
		#endif


		/**
		* Send the data to all the registered MediaStreamSender.
		* If relevant, the data is first encoded using the
		* MediaStream's selected CODEC.
		* @param data pointer to the data to send
		* @param length length of the data buffer
		* @param ts timestamp to use in the RTP header
		* @param marker whether or not the marker should be set
		* in the RTP header
		*/
		virtual void sendData( byte_t * data, uint32_t nsamples, int samplerate, uint32_t ts, bool marker );

		/**
		* Used to start the playout of a ringtone, contained
		* in the given filename. The file should contain raw
		* audio PCM.
		* @param ringtoneFile the file from which the ringtone
		* data is read
		*/
		void startRinging( std::string ringtoneFile );

		/**
		* Used to stop the playout of any ringtone.
		*/
		void stopRinging();

#ifdef DEBUG_OUTPUT
		virtual std::string getDebugString();
#endif
		
		MRef<AudioMediaSource *> getSource( uint32_t ssrc );
		
		MRef<Resampler *> getResampler() { return resampler; };
		
		MRef<SoundIO *> getSoundIO() { return soundIo; };

	protected:
		MRef<Resampler *> resampler;
		SilenceSensor * silenceSensor;
		MRef<SoundIO *> soundIo;                 
		uint32_t seqNo;
		byte_t encoded[1600];                 
		short resampledData[1600];
		#ifdef AEC_SUPPORT
		short resampledDataR[160];		//hanning
		static AEC aec;				//hanning
		#endif
		std::list< MRef<AudioCodec *> > codecs;
		std::list< MRef<AudioMediaSource *> > sources;
};

class LIBMINISIP_API AudioMediaSource : public BasicSoundSource{
	public:
		AudioMediaSource( uint32_t ssrc, std::string callId, MRef<Media *> media );

		void playData( const MRef<RtpPacket *> & rtpPacket );
		uint32_t getSsrc();
		
		MRef<Media *> getMedia() { return media; };

		MRef<CodecState *> findCodec( uint8_t payloadType );
		
		short * getCodecOutputBuffer() { return codecOutput; }

	protected:
		std::list< MRef<CodecState *> > codecs;
		MRef<Media *> media;
		short codecOutput[AUDIOMEDIA_CODEC_MAXLEN];
		uint32_t ssrc;

};


#endif //#ifndef AUDIO_MEDIA_AND_AUDIOSOURCE_H


