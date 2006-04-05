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

#include<libminisip/mediahandler/MediaHandler.h>
#include<libminisip/video/ImageHandler.h>
#include<libminisip/video/display/VideoDisplay.h>
#include<libminisip/video/codec/AVDecoder.h>
#include<libminisip/video/codec/VideoEncoderCallback.h>
#include<libminisip/video/grabber/Grabber.h>

#include<libminisip/codecs/Codec.h>

#define MAX_SOURCES 256

class VideoEncoderCallback;
class VideoCodec;
class VideoDisplay;
class Grabber;
class VideoMediaSource;
class AVDecoder;
class ImageMixer;
class RtpPacket;

class VideoMedia : public Media, public VideoEncoderCallback{

	public:
		VideoMedia( MRef<Codec *> codec, MRef<VideoDisplay *> display, MRef<ImageMixer *> mixer, MRef<Grabber *> = NULL, uint32_t receivingWidth = 176, uint32_t receivingHeight=144 );
		virtual std::string getMemObjectType(){return "VideoMedia";}

		virtual std::string getSdpMediaType();

		virtual void playData( MRef<RtpPacket *> rtpPacket );

		virtual void sendVideoData( byte_t * data, uint32_t length, uint32_t ts, bool marker=false );

		virtual void registerMediaSender( MRef<MediaStreamSender *> sender );
		virtual void unRegisterMediaSender( MRef<MediaStreamSender *> sender );
		virtual void registerMediaSource( uint32_t ssrc );
		virtual void unRegisterMediaSource( uint32_t ssrc );
		virtual void handleMHeader( MRef<SdpHeaderM *> m );


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
		MRef<VideoCodec *> codec;

		uint32_t receivingWidth;
		uint32_t receivingHeight;

		uint32_t sendingWidth;
		uint32_t sendingHeight;

		std::list<MRef<VideoMediaSource *> > sources;
		Mutex sourcesLock;
};

class VideoMediaSource : public MObject {
	public:
		VideoMediaSource( uint32_t ssrc, uint32_t width, uint32_t height );

		MImage * provideEmptyImage();
		MImage * provideFilledImage();

		void addEmptyImage( MImage * image );
		void addFilledImage( MImage * image );
		
		virtual void playData( MRef<RtpPacket *> packet ); 

		MRef<AVDecoder *> getDecoder();

		uint32_t ssrc;

		virtual std::string getMemObjectType(){ return "VideoMediaSource"; };

		friend class VideoMedia;
	private:
		void addPacketToFrame( MRef<RtpPacket *> packet );

		MRef<AVDecoder *> decoder;
		MRef<VideoDisplay *> display;

		uint32_t width;
		uint32_t height;

		byte_t frame[100000];
		uint32_t index;
		bool packetLoss;
		uint16_t expectedSeqNo;


		std::list<MImage *> emptyImages;
		Mutex emptyImagesLock;

		std::list<MImage *> filledImages;
		Mutex filledImagesLock;

		MRef<RtpPacket *> savedPacket;
};


#endif
