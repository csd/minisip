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

#ifndef MEDIA_H
#define MEDIA_H

#include"config.h"

#include<libmutil/Mutex.h>

#include"../soundcard/SoundRecorderCallback.h"
#include"../soundcard/SoundIO.h"
#ifdef VIDEO_SUPPORT
#include"../video/codec/VideoEncoderCallback.h"
#endif

#include<../codecs/Codec.h>

class SoundIO;
class MediaStreamSender;
class MediaStreamReceiver;
class SdpHeaderM;
class RtpPacket;

typedef uint8_t byte_t;

class Media : public MObject{
	public:
		int getType();

		virtual std::string getSdpMediaType()=0;
		
		// pn430 Following two lines rewritten for multicodec
		//virtual uint8_t getRtpPayloadType();
		//virtual std::string getRtpMap();
		// pn430 The four new ones (incl. new multi-versions)
//		virtual uint8_t getCurrentRtpPayloadType();
		virtual std::list<uint8_t> getAllRtpPayloadTypes();
//		virtual std::string getCurrentRtpMap();
		virtual std::list<std::string> getAllRtpMaps();
		//pn507 Added for changing the default codec
//		virtual bool setCurrentCodec( uint8_t newCodecPayloadType );
		virtual MRef<Codec*> getCodec( uint8_t payloadType );

		virtual void playData( RtpPacket * rtpPacket )=0;
		virtual void sendData( byte_t * data, uint32_t length, uint32_t ts, bool marker=false );

		virtual void registerMediaSender( MRef<MediaStreamSender *> sender );
		virtual void unRegisterMediaSender( MRef<MediaStreamSender *> sender );
		virtual void registerMediaSource( uint32_t ssrc );
		virtual void unRegisterMediaSource( uint32_t ssrc );
		
		bool receive;
		bool send;
		
		std::list< MRef<Codec *> >  Media::getAvailableCodecs();

		std::list<std::string> getSdpAttributes();
		void addSdpAttribute( std::string attribute );
		
		virtual void handleMHeader( MRef<SdpHeaderM *> m );
		MRef<CodecState *> Media::createCodecInstance( uint8_t payloadType );
		
	protected:
                Media();
                Media( MRef<Codec *> defaultCodec );

		Media( std::list<MRef<Codec *> > codecList );
				
		std::list< MRef<Codec *> > codecList;
		Mutex codecListLock;
		
		std::list< MRef<MediaStreamSender *> > senders;
		Mutex sendersLock;
		Mutex sourcesLock;
		
		std::list<std::string> sdpAttributes;
};

#endif
