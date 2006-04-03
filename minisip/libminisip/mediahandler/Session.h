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

#ifndef SESSION_H
#define SESSION_H

#include<config.h>

#include<libmutil/MemObject.h>
#include<libmutil/TimeoutProvider.h>
#include"../sip/SipDialogSecurityConfig.h"
#include"DtmfSender.h"
#include<libmikey/keyagreement.h>
#include"../sdp/SdpPacket.h"

class MediaStreamReceiver;
class MediaStreamSender;
class SdpHeaderM;
//class KeyAgreement;
class IPAddress;
class SessionRegistry;

/**
 * The session class is a representation of the media session associated
 * with a VoIP call. It holds MediaStreams that handles incoming and
 * outgoing streams for a given medium. The session is also the interface
 * used by the SIP stack to send and receive session descriptions (SDP)
 * and to start or stop the media transmission
 */
class Session : public MObject{
	public:

		/**
		 * Global registry holding all the current media sessions, for
		 * access for instance from the user interface */
		static SessionRegistry * registry;

		/**
		 * Pre-computed parameters to optimize the next key exchange
		 */
		static MRef<KeyAgreement *> precomputedKa;

		/**
		 * Constructor, called by MediaHandler::createSession only
		 * @param localIp IP address to give as contact in session
		 * description
		 * @param config security related configuration for the call
		 */
		Session( std::string localIp, SipDialogSecurityConfig &config );
		
		/**
		 * Destructor.
		 */
		~Session();

		/**
		 * Removes this session from the global session registry
		 */
		void unregister();

		/**
		 * Starts the media transmission. Called by the SIP stack.
		 */
		void start();
		
		/**
		 * Stops the media transmission. Called by the SIP stack.
		 */
		void stop();

		/**
		 * Used by the SIP stack to query the session description 
		 * offer (SDP offer), upon session initiation.
		 * @returns a reference to the SDP object
		 */
		MRef<SdpPacket *> getSdpOffer();
		
		/**
		 * Used by the SIP stack to query the session description
		 * answer (SDP answer), when answering a call.
		 * @returns a reference to the SDP object
		 */
		MRef<SdpPacket *> getSdpAnswer();
		
		/**
		 * Used by the SIP stack to provide the media session
		 * with the peer's session description answer (SDP answer).
		 * The media session configures the media streams accordingly.
		 * @returns whether or not we can accept this session.
		 */
		bool setSdpAnswer( MRef<SdpPacket *> answer, std::string peerUri );
		/**
		 * Used by the SIP stack to provide the media session
		 * with the peer's session description offer (SDP offer).
		 * The media session configures the media streams accordingly.
		 * @returns whether or not we can accept this session.
		 */
		bool setSdpOffer ( MRef<SdpPacket *> offer, std::string peerUri );

		/**
		 * Adds a MediaStreamReceiver to this media session. Used
		 * by the media handler to add a media stream per
		 * available medium.
		 * @param r a reference to the MediaStreamReceiver object to add
		 */
		void addMediaStreamReceiver( MRef<MediaStreamReceiver *> r );
		
		/**
		 * Adds a MediaStreamSender to this media session. Used
		 * by the media handler to add a media stream per
		 * available medium.
		 * @param s a reference to the MediaStreamSender object to add
		 */
		void addMediaStreamSender( MRef<MediaStreamSender *> s );

		/**
		 * Returns an error description suitable for use
		 * in a SIP Warning: header, to explain why the
		 * session could not be accepted.
		 * @returns the description as a string
		 */
		std::string getErrorString();

		/**
		 * Returns an error code suitable for use in a SIP
		 * Warning: header, to explain why the session could not
		 * be accepted.
		 * @returns the error code
		 */
		uint16_t getErrorCode();

		/**
		 * Used to query whether or not the session is considered
		 * "secure".
		 * @returns whether or not the session is "secure"
		 */
		bool isSecure();

		virtual std::string getMemObjectType(){return "Session";}

		/**
		 * Returns the CallId identifier shared with the SIP
		 * stack.
		 * @returns the identifier as a string
		 */
		std::string getCallId();

		/**
		 * Change the CallId identifier for the session. Used
		 * during session transfer.
		 * @param callId the new identifier, as a string
		 */
		void setCallId( const string callId );

		friend class DtmfSender;
		
		/**
		 * Asks the media session to send a DTMF symbol to all
		 * the peers.
		 * @param symbol the DTMF symbol to send
		 */
		void sendDtmf( uint8_t symbol );

		/**
		 * Used to mute all the input to this session.
		 * @param mute whether or not the peers in this
		 * session should receive the media transmission
		 */
		void muteSenders (bool mute);
		
		/**
		Indicates whether the media stream senders are in muted 
		state or not. 
		IMPORTANT: it is not 100% reliable, as the senders may have
			been modified somewhere else (not via this Session)
		*/
		bool mutedSenders;
		
		/**
		Used to silence all the sources associated to this 
		Session. SoundSources receive the audio from the
		MediaStreamReceiver and act as a buffer. They are identified
		by the ssrc (SoundSource::getId)
		@param silence whether or not we want the sources silenced
		*/
		void silenceSources ( bool silence );
		
		/**
		Indicates whether the audio media sources are in silenced
		state or not. 
		IMPORTANT: it is not 100% reliable, as the sources may have
			been modified somewhere else (not via this Session)
		*/
		bool silencedSources;
		
#ifdef DEBUG_OUTPUT
		/**
		Return a debug string containing info about this Session
		*/
		virtual string getDebugString();
#endif
		
		/**
		Empty the media stream receivers list.
		*/
		void clearMediaStreamReceivers();
		
		/**
		Return a copy of the list to the media stream receivers
		*/
		std::list< MRef<MediaStreamReceiver *> > getMediaStreamReceivers() {
			return mediaStreamReceivers;
		}

		/**
		Return a copy of the list to the media stream senders
		*/
		std::list< MRef<MediaStreamSender *> > getMediaStreamSenders() {
			return mediaStreamSenders;
		}
		
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
		MRef<MediaStreamReceiver *> matchFormat( MRef<SdpHeaderM *> m, uint32_t iFormat, IPAddress * remoteAddress );

		std::list< MRef<MediaStreamReceiver *> > mediaStreamReceivers;
		std::list< MRef<MediaStreamSender *> > mediaStreamSenders;
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


#include "SessionRegistry.h"

#endif
