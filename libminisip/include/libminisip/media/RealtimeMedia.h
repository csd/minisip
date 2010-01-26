/*
 Copyright (C) 2004-2007 the Minisip Team
 
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

/* Copyright (C) 2004-2007
 *
 * Authors: Erik Eliasson <ere@kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/



#ifndef REALTIMEMEDIA
#define REALTIMEMEDIA

#include<libminisip/libminisip_config.h>

#include<libminisip/media/Media.h>
#include<libminisip/media/codecs/Codec.h>
#include<libminisip/media/rtp/RtpPacket.h>
#include<libminisip/media/MediaStream.h>

class RealtimeMediaStreamSender;


/**
 * Handles RTP based media (typically audio and video streams).
 * It holds a list of the CODEC relevant for
 * that medium, and other objects used by the medium (SoundIO 
 * for audio, Grabber and Display for video).
 * Media sessions register their MediaStreamSender objects to
 * the Media objects.
 */
class LIBMINISIP_API RealtimeMedia : public Media {
	public:
		~RealtimeMedia();
		/**
		 * Returns a reference to the CODEC given its RTP
		 * payload type. The payload type is the one
		 * used by minisip, not necessarily the one
		 * used by the peers.
		 * @param payloadType the RTP payload type
		 * @returns a reference to the corresponding CODEC, or 
		 * NULL if no such CODEC was found
		 */
		virtual MRef<Codec*> getCodec( uint8_t payloadType );

		/**
		 * Play the given RTP packet on this medium. This includes
		 * decoding if relevant.
		 * @param rtpPacket the RTP packet to play
		 */
		virtual void playData( const MRef<RtpPacket *> & rtpPacket )=0;
		
		/**
		 * Send the data to all the registered RealtimeMediaStreamSender.
		 * If relevant, the data is first encoded using the
		 * RealtimeMediaStream's selected CODEC.
		 * @param data pointer to the data to send
		 * @param length length of the data buffer
		 * @param ts timestamp to use in the RTP header
		 * @param marker whether or not the marker should be set
		 * in the RTP header
		 */
		virtual void sendData( byte_t * data, uint32_t length, int samplerate, uint32_t ts, bool marker/*=false*/ );

		/**
		 * Used by the media sessions to register a RealtimeMediaStreamSender.
		 * When a RealtimeMediaStreamSender is registered to a Media object,
		 * it will be used to send sampled media from the corresponding
		 * medium
		 * @param sender a reference to the RealtimeMediaStreamSender object to
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
		virtual void registerMediaSource( uint32_t ssrc, std::string callId )=0;
		
		/**
		 * Used to unregister a media source when the session ends. 
		 * @param ssrc the SSRC identifier used by the media source to
		 * unregister
		 */
		virtual void unregisterMediaSource( uint32_t ssrc)=0;
		
		/**
		 * deprecated...
		 */
		bool receive;
		bool send;
		
		/**
		 * Used to query available CODECs for that medium. The
		 * CODEC are returned in a list sorted according
		 * to the user's preferences. Used by the media Session
		 * to generate the session description.
		 * @returns a list of reference to Codec objects, sorted
		 * according to user preferences (first is preferred)
		 */
		std::list< MRef<Codec *> >  getAvailableCodecs();

		/**
		 * Used to create a CODEC instance for the given RTP
		 * payload type. The payload type corresponds to the
		 * one used by minisip, not necessarily the one
		 * used by the peer.
		 * @param payloadType the RTP payload type of the CODEC
		 * of which an instance should be created
		 * @returns a reference to the CODEC instance created, or
		 * NULL if no CODEC with this payload type was available
		 */
		MRef<CodecState *> createCodecInstance( uint8_t payloadType );

		//std::list< MRef<RealtimeMediaStreamSender *> > getMediaSenders(){return senders;}

	protected:
		RealtimeMedia( MRef<Codec *> defaultCodec );

		RealtimeMedia( const std::list<MRef<Codec *> > & codecList );

		std::list< MRef<Codec *> > codecList;
		Mutex codecListLock;
		
		std::list< MRef<RealtimeMediaStreamSender *> > senders;
		Mutex sendersLock;
		Mutex sourcesLock;
};


#endif
