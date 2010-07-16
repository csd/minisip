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
*/

#ifndef VIDEO_MEDIA_H
#define VIDEO_MEDIA_H

#include<libminisip/libminisip_config.h>

#include<libminisip/media/RealtimeMedia.h>
#include<libminisip/media/video/ImageHandler.h>
#include<libminisip/media/video/display/VideoDisplay.h>
#include<libminisip/media/video/codec/AVDecoder.h>
#include<libminisip/media/video/codec/AVCoder.h>
#include<libminisip/media/video/codec/VideoEncoderCallback.h>
#include<libminisip/media/video/grabber/Grabber.h>

#include<libminisip/media/codecs/Codec.h>

#define MAX_SOURCES 256

class VideoEncoderCallback;
class MVideoCodec;
class VideoDisplay;
class Grabber;
class VideoMediaSource;
class AVDecoder;
class ImageMixer;
class RtpPacket;

//1920x1080x3 - uncompressed full-hd RGBA 24
#define MAX_ENCODED_FRAME_SIZE 6220800

class LIBMINISIP_API VideoMedia : public RealtimeMedia,
				  public VideoEncoderCallback{

	public:
		VideoMedia( MRef<Codec *> codec, MRef<VideoDisplay *> display, MRef<ImageMixer *> mixer, MRef<Grabber *> = NULL, uint32_t receivingWidth = 176, uint32_t receivingHeight=144 );

		virtual std::string getMemObjectType() const {return "VideoMedia";}

		virtual std::string getSdpMediaType();

		virtual void playData( const MRef<RtpPacket *> & rtpPacket );

		virtual void sendVideoData( byte_t * data, uint32_t length, uint32_t ts, bool marker=false );

		virtual void registerRealtimeMediaSender( MRef<RealtimeMediaStreamSender *> sender );
		virtual void unregisterRealtimeMediaSender( MRef<RealtimeMediaStreamSender *> sender );
		virtual void registerMediaSource( uint32_t ssrc, std::string callId );
		virtual void unregisterMediaSource( uint32_t ssrc );
		virtual void handleMHeader( MRef<SdpHeaderM *> m );



		uint8_t  getCodecgetSdpMediaType();
		  MRef<CodecState *> getCodecInstance ();
		


		MRef<VideoMediaSource *> getSource( uint32_t ssrc );
		void getImagesFromSources( MImage ** images, 
					uint32_t & nImagesToMix,
					uint32_t mainSource );
		void releaseImagesToSources( MImage ** images, 
					uint32_t nImages );

	private:
		MRef<Grabber *> grabber; // NULL if receive only
		MRef<VideoDisplay *> display;
		MRef<ImageMixer *> mixer;
		MRef<MVideoCodec *> codec;

		uint32_t receivingWidth;
		uint32_t receivingHeight;

		uint32_t sendingWidth;
		uint32_t sendingHeight;

		std::list<MRef<VideoMediaSource *> > sources;
		Mutex sourcesLock;
};

class LIBMINISIP_API VideoMediaSource : public MObject {
	public:
		VideoMediaSource( uint32_t ssrc, uint32_t width, uint32_t height );

		MImage * provideEmptyImage();
		MImage * provideFilledImage();

		void addEmptyImage( MImage * image );
		void addFilledImage( MImage * image );
		
		virtual void playData( const MRef<RtpPacket *> & packet ); 

		MRef<AVDecoder *> getDecoder();
		MRef<AVEncoder *> getEncoder();

		uint32_t ssrc;

		virtual std::string getMemObjectType() const { return "VideoMediaSource"; };

		friend class VideoMedia;
	private:
		/**
		 * @param flush Packet loss was detected, and previous
		 *              data should be flushed to the decoder.
		 */
		void addPacketToFrame( const MRef<RtpPacket *> & packet, bool flush );

		MRef<AVDecoder *> decoder;
		MRef<VideoDisplay *> display;

		uint32_t width;
		uint32_t height;

		byte_t frame[MAX_ENCODED_FRAME_SIZE];
		uint32_t index;
		uint32_t nbytesReceived;
		bool packetLoss;
		bool firstSeqNo;
//		uint16_t expectedSeqNo;
		uint16_t lastSeqNo;
		uint16_t lastPlayedSeqNo;
		std::list<MRef<RtpPacket*> > rtpReorderBuf;
		void enqueueRtp(const MRef<RtpPacket*> & rtp); //ordered list of packets left to play out
		void playSaved();



		std::list<MImage *> emptyImages;
		Mutex emptyImagesLock;

		std::list<MImage *> filledImages;
		Mutex filledImagesLock;

		MRef<RtpPacket *> savedPacket;
};


#endif
