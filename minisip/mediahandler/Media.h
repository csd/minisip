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

#include<libmutil/MemObject.h>
#include<libmutil/Mutex.h>

#ifndef _MSC_VER
#include<stdint.h>
#else
#ifndef uint16_t
typedef unsigned short  uint16_t;
#endif
#ifndef uint32_t
typedef unsigned int  uint32_t;
#endif
#endif

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
		virtual uint8_t getRtpPayloadType();
		virtual std::string getRtpMap();

		virtual void playData( RtpPacket * rtpPacket )=0;
		virtual void sendData( byte_t * data, uint32_t length, uint32_t ts, bool marker=false );

		virtual void registerMediaSender( MRef<MediaStreamSender *> sender );
		virtual void unRegisterMediaSender( MRef<MediaStreamSender *> sender );
		virtual void registerMediaSource( uint32_t ssrc );
		virtual void unRegisterMediaSource( uint32_t ssrc );
		
		
		bool receive;
		bool send;
		
		std::list<std::string> getSdpAttributes();
		void addSdpAttribute( std::string attribute );
		
		virtual void handleMHeader( MRef<SdpHeaderM *> m );
	protected:
		Media( MRef<Codec *> codec );
		MRef<Codec *> codec;
		std::list< MRef<MediaStreamSender *> > senders;
		Mutex sendersLock;
		Mutex sourcesLock;
		
		std::list<std::string> sdpAttributes;
};

class AudioMedia : public Media, public SoundRecorderCallback, public Resampler{

	public:
		AudioMedia( MRef<SoundIO *> soundIo, MRef<Codec *> codec );
		virtual std::string getMemObjectType(){return "AudioMedia";}
		virtual std::string getSdpMediaType();
		

		virtual void playData( RtpPacket * rtpPacket );
		
		virtual void registerMediaSender( MRef<MediaStreamSender *> sender );
		virtual void unRegisterMediaSender( MRef<MediaStreamSender *> sender );
		virtual void registerMediaSource( uint32_t ssrc );
		virtual void unRegisterMediaSource( uint32_t ssrc );

		virtual void srcb_handleSound( void *samplearr );

		void startRinging( std::string ringtoneFile );
		void stopRinging();
	private:
		MRef<SoundIO *> soundIo;
		uint32_t seqNo;
		byte_t encoded[1600];
		short resampledData[160];
#ifdef IPAQ
		uint32_t iIPAQ;
		short saved[160];
#endif
};

#endif
