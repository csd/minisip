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

/* Copyright (C) 2004, 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/


#include"Media.h"
#ifdef AEC_SUPPORT
#include "../aec/aec.h"		//hanning
#endif

#include"../soundcard/SoundSource.h"

class AudioMediaSource;
class SilenceSensor;
class Resampler;

/**
 * The AudioMedia class holds the object required to acquire and
 * play out audio data. It is created upon startup, or when the
 * media configuration changes.
 */
class AudioMedia : public Media, public SoundRecorderCallback{

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
		AudioMedia( MRef<SoundIO *> soundIo, std::list<MRef<Codec *> > codecList );
                
		virtual std::string getMemObjectType(){return "AudioMedia";}

                /**
                 * Returns the media type as used in the SDP (audio or video).
                 * @returns the media type as a string
                 */
                virtual std::string getSdpMediaType();

                /**
                 * Play the given RTP packet on this medium. This includes
                 * decoding if relevant.
                 * @param rtpPacket the RTP packet to play
                 */
                virtual void playData( RtpPacket * rtpPacket );

                /**
                 * Used by the media sessions to register a MediaStreamSender.
                 * When a MediaStreamSender is registered to a Media object,
                 * it will be used to send sampled media from the corresponding
                 * medium
                 * @param sender a reference to the MediaStreamSender object to
                 * register
                 */
                virtual void registerMediaSender( MRef<MediaStreamSender *> sender );


                /**
                 * Used by the media sessions to unregister a MediaStreamSender,                 * when a media session ends.
                 * @param sender a reference to the MediaStreamSender object to
                 * unregister
                 */
                virtual void unRegisterMediaSender( MRef<MediaStreamSender *> sender );
                /**
                 * Used to register a new media source. Called upon discovery
                 * of a new SSRC identifier. Each media source may use
                 * a different decoder.
                 * @param ssrc the SSRC identifier used by the new media source
                 */
                virtual void registerMediaSource( uint32_t ssrc );


                /**
                 * Used to unregister a media source when the session ends.
                 * @param ssrc the SSRC identifier used by the media source to
                 * unregister
                 */
                virtual void unRegisterMediaSource( uint32_t ssrc );

		/**
		 * Callback used by the SoundIO when sound samples are
		 * available. The AudioMedia will process them by
		 * encoding them and sending them to all the registered
		 * MediaStreamSender objects.
		 * @param samplearr pointer to the audio samples
		 */
                virtual void srcb_handleSound( void *samplearr );


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
		virtual void sendData( byte_t * data, uint32_t length, uint32_t ts, bool marker );
		#ifdef AEC_SUPPORT 
		virtual void srcb_handleSound( void *samplearr, void *samplearrR);	//hanning
		#endif

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
		virtual string getDebugString();
#endif

	private:
		MRef<AudioMediaSource *> getSource( uint32_t ssrc );

                MRef<Resampler *> resampler;
                SilenceSensor * silenceSensor;
                MRef<SoundIO *> soundIo;                 
		uint32_t seqNo;
                byte_t encoded[1600];                 
		short resampledData[160];
		#ifdef AEC_SUPPORT
		short resampledDataR[160];		//hanning
		static AEC aec;				//hanning
		#endif
		std::list< MRef<AudioCodec *> > codecs;
		std::list< MRef<AudioMediaSource *> > sources;
		
};

class AudioMediaSource : public BasicSoundSource{
	public:
		AudioMediaSource( uint32_t ssrc, MRef<Media *> media );

		void playData( RtpPacket * rtpPacket );
		uint32_t getSsrc();

	private:
		MRef<CodecState *> findCodec( uint8_t payloadType );
		std::list< MRef<CodecState *> > codecs;
		MRef<Media *> media;
		short codecOutput[16384];
		uint32_t ssrc;
};
