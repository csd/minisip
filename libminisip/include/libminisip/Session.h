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


/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef SESSION_H
#define SESSION_H

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
#include<libmutil/TimeoutProvider.h>
#include<libminisip/SipDialogSecurityConfig.h>
#include<libminisip/SessionRegistry.h>
#include<libminisip/DtmfSender.h>
#include<libminisip/SdpPacket.h>
#include<libmikey/keyagreement.h>
class MediaStream;
class SdpHeaderM;
class IPAddress;
class SessionRegistry;

class LIBMINISIP_API Session : public MObject{
	public:

                static SessionRegistry * registry;
                static MRef<KeyAgreement *> precomputedKa;

		Session( std::string localIp, SipDialogSecurityConfig &config );

                void unregister();

		void start();
		void stop();

		MRef<SdpPacket *> getSdpOffer();
		MRef<SdpPacket *> getSdpAnswer();
		
		/* returns whether or not we accept the answer */
		bool setSdpAnswer( MRef<SdpPacket *> answer, std::string peerUri );
		bool setSdpOffer ( MRef<SdpPacket *> offer, std::string peerUri );

		void addMediaStreamReceiver( MRef<MediaStream *> );
		void addMediaStreamSender( MRef<MediaStream *> );

		std::string getErrorString();
		uint16_t getErrorCode();

		bool isSecure();

		virtual std::string getMemObjectType(){return "Session";}

                std::string getCallId();
                void setCallId( const string callId );

                friend class DtmfSender;
                void sendDtmf( uint8_t symbol );


	private:
		/* Key management handling */
		std::string initiatorCreate();
		bool initiatorAuthenticate( std::string message );
		std::string initiatorParse();

		bool responderAuthenticate( std::string message );
		std::string responderParse();

		void addStreamsToKa( bool initiating=true );
		void setMikeyOffer();
		std::string peerUri;
		
		MRef<SdpPacket *> emptySdp();
		MRef<MediaStream *> matchFormat( MRef<SdpHeaderM *> m, uint32_t iFormat, IPAddress * remoteAddress );

		std::list< MRef<MediaStream *> > mediaStreamReceivers;
		std::list< MRef<MediaStream *> > mediaStreamSenders;
                Mutex mediaStreamSendersLock;

		MRef<KeyAgreement *> ka;
		std::string localIpString;
		MRef<SdpPacket *> sdpAnswer;
		bool secured;

		std::string errorString;
		uint16_t errorCode;
		SipDialogSecurityConfig securityConfig;

                std::string callId;

                DtmfSender dtmfSender;
                TimeoutProvider<DtmfEvent *, DtmfSender *> dtmfTOProvider;

};

#endif
