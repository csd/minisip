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

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef SESSION_H
#define SESSION_H

#include<libmutil/MemObject.h>
#include<libmutil/TimeoutProvider.h>
#include"../sip/SipDialogSecurityConfig.h"
#include "SessionRegistry.h"
#include"DtmfSender.h"
#include<libmikey/keyagreement.h>
#include<../sdp/SdpPacket.h>
class MediaStream;
class SdpHeaderM;
//class KeyAgreement;
class IPAddress;
class SessionRegistry;

class Session : public MObject{
	public:

		static SessionRegistry * registry;
		static MRef<KeyAgreement *> precomputedKa;

		Session( std::string localIp, SipDialogSecurityConfig &config );
		~Session();

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

                //DtmfSender dtmfSender;
                MRef<TimeoutProvider<DtmfEvent *, MRef<DtmfSender *> > *> dtmfTOProvider;
};

#endif
