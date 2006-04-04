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

#ifndef MEDIA_HANDLER_H
#define MEDIA_HANDLER_H

#include<config.h>

#include<libmutil/MemObject.h>

#include<libminisip/mediahandler/Media.h>
#include<libminisip/mediahandler/AudioMedia.h>
#include<libminisip/mediahandler/MediaStream.h>
#include<libminisip/mediahandler/Session.h>
#include<libminisip/mediahandler/RtpReceiver.h>
#include<libminisip/ipprovider/IpProvider.h>
#include<libmutil/CommandString.h>
#include<libmutil/MessageRouter.h>


class SipSoftPhoneConfiguration;
class IpProvider;


class MediaHandler : public virtual MObject, public SessionRegistry, public CommandReceiver {

	public:
		/**
		 * Constructor, created on startup
		 * @param config reference to the softphone configuration
		 * @param ipProvider reference to the public IP provider, used
		 * for NAT traversal mechanisms
		 */
		MediaHandler( MRef<SipSoftPhoneConfiguration *> config, MRef<IpProvider *> ipProvider );
		virtual ~MediaHandler();
		
		/**
		 * Creates a new media session, for use in a new VoIP call
		 * @param config the call specific configuration
		 * @param callId identifier shared with the SIP stack
		 * @returns a reference to the session created
		 */
		MRef<Session *> createSession( SipDialogSecurityConfig &config, std::string callId = "" );
		
		/**
		 * Registers a new media type (audio or video
		 * @param media a reference to the representation of the
		 * medium to add
		 */
		void registerMedia( MRef<Media *> media );

		/**
		 * Handles a command sent by the user interface
		 * @param command the command to handle
		 */
		void handleCommand(std::string subsystem, const CommandString & command );


		CommandString handleCommandResp(std::string subsystem, const CommandString& command);
		
		/**
		 * Provides the IP address given as contact to the external
		 * peers
		 * @returns a string containing the IP address
		 */
		std::string getExtIP();
		
		
		virtual std::string getMemObjectType(){return "MediaHandler";}

#ifdef DEBUG_OUTPUT	
		virtual std::string getDebugString();
#endif

#if 0
		/**
		True if all but one sender/media sessions are muted.
		If turned off, all ongoing sessions receive the audio from our mic.
		If turned on, only one source (the active one) will receive it.
		*/
		bool muteAllButOne;
#endif

	private:
		void init();

		std::list< MRef<Media *> > media;

		std::string ringtoneFile;

		MRef<AudioMedia *> audioMedia;
		MRef<IpProvider *> ipProvider;
		MRef<SipSoftPhoneConfiguration *> config;
		
		/**
		Looks for a Session with callid. If found, set the audio settings
		of that Session (side = "receivers" or "senders") to either be on
		or off.
		@param callid callid of the Session whose settings we want to deal with
		@param side either "receivers" (audio received from the network and heard
			on the headsets) or "senders" (audio read from the mic/soundcard and
			sent over the net).
		@param turnOn whether to turn on or off the setting of the specified session.
			If turnOn = true -> the receivers are actived (un-silenced) or the
			are activated (un-muted).
		*/
		void setSessionSoundSettings( std::string callid, std::string side, bool turnOn );

};

#endif
