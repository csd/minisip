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

class AudioMediaSource;
class SilenceSensor;


class AudioMedia : public Media, public SoundRecorderCallback{

        public:
		// pn430 Next line changed for multicodec
		//AudioMedia( MRef<SoundIO *> soundIo, MRef<Codec *> codec );
		AudioMedia( MRef<SoundIO *> soundIo, std::list<MRef<Codec *> > codecList );
                
		virtual std::string getMemObjectType(){return "AudioMedia";}
                virtual std::string getSdpMediaType();

                virtual void playData( RtpPacket * rtpPacket );

                virtual void registerMediaSender( MRef<MediaStreamSender *> sender );
                virtual void unRegisterMediaSender( MRef<MediaStreamSender *> sender );
                virtual void registerMediaSource( uint32_t ssrc );
                virtual void unRegisterMediaSource( uint32_t ssrc );

                virtual void srcb_handleSound( void *samplearr );
		virtual void sendData( byte_t * data, uint32_t length, uint32_t ts, bool marker );
		#ifdef AEC_SUPPORT 
		virtual void srcb_handleSound( void *samplearr, void *samplearrR);	//hanning
		#endif
                void startRinging( std::string ringtoneFile );
                void stopRinging();         

		MRef<CodecState *> createCodecInstance( uint8_t payloadType );

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
