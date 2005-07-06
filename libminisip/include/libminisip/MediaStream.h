/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  
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
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


/* Copyright (C) 2004, 2005 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef MEDIA_STREAM_H
#define MEDIA_STREAM_H

#ifdef _MSC_VER
#ifdef LIBMINISIP_EXPORTS
#define LIBMINISIP_API __declspec(dllexport)
#else
#define LIBMINISIP_API __declspec(dllimport)
#endif
#else
#define LIBMINISIP_API
#endif


#include<libmutil/MemObject.h>
#include<libminisip/CryptoContext.h>
#include<libminisip/Media.h>
#include<libminisip/MediaStream.h>
#include<libminisip/Session.h>
#include<libminisip/RtpReceiver.h>

class KeyAgreement;
class UDPSocket;
class SdpHeaderM;
class SRtpPacket;
class IpProvider;

class LIBMINISIP_API MediaStream : public MObject{
	public:
		virtual void start() = 0;
		virtual void stop() = 0;

		/* SDP information */
		std::string getSdpMediaType();/* audio, video, appli;ation... */
		// pn501 New function for multiple codecs
		std::list<uint8_t> getAllRtpPayloadTypes();
		// pn501 New function for multiple codecs
		std::list<std::string> getAllRtpMaps();
		std::list<std::string> getSdpAttributes();

		virtual std::string getMemObjectType(){return "MediaStream";}
		bool disabled;
		
		virtual void setPort( uint16_t port )=0;
		virtual uint16_t getPort()=0;

		bool matches( MRef<SdpHeaderM *> m, uint32_t nFormat=0 );
		void addToM( MRef<SdpPacket*> packet, MRef<SdpHeaderM *> m );

		void setKeyAgreement( MRef<KeyAgreement *> ka );

		uint32_t getSsrc();

        MRef<Codec *> getSelectedCodec(){return selectedCodec;};

	protected:
		MRef<CryptoContext *> getCryptoContext( uint32_t ssrc );
		MediaStream( MRef<Media *> );
		MRef<Media *> media;
		uint32_t csbId;
		uint32_t ssrc;
                // FIXME used only in sender case
                uint8_t payloadType;
                MRef<Codec *> selectedCodec;

	private:
		MRef<CryptoContext *> initCrypto( uint32_t ssrc );
		MRef<KeyAgreement *> ka;
		Mutex kaLock;
		std::list< MRef<CryptoContext *> > cryptoContexts;
};

class LIBMINISIP_API MediaStreamReceiver : public MediaStream{ 
	public:
		MediaStreamReceiver( MRef<Media *> media, MRef<RtpReceiver *>, MRef<IpProvider *> ipProvider );
		virtual void start();
		virtual void stop();
		
		virtual void setPort( uint16_t port );
		virtual uint16_t getPort();

		void handleRtpPacket( SRtpPacket * packet );
		uint32_t getId();
		
	private:
		MRef<RtpReceiver *> rtpReceiver;
		uint32_t id;
		MRef<IpProvider *> ipProvider;
		uint16_t externalPort;

		void gotSsrc( uint32_t ssrc );

		std::list<uint32_t> ssrcList;
		Mutex ssrcListLock;

		bool running;
};

class LIBMINISIP_API MediaStreamSender : public MediaStream{ 
	public:
		MediaStreamSender( MRef<Media *> media, 
				   MRef<UDPSocket *> senderSock=NULL );
		virtual void start();
		virtual void stop();

		virtual void setPort( uint16_t port );
		virtual uint16_t getPort();
		
		void send( byte_t * data, uint32_t length, uint32_t * ts, bool marker = false, bool dtmf = false );
		void setRemoteAddress( IPAddress * remoteAddress );
		
	private:
		MRef<UDPSocket *> senderSock;
		uint16_t remotePort;
		uint16_t seqNo;
                uint32_t lastTs;
		IPAddress * remoteAddress;
                Mutex senderLock;
};

#endif
