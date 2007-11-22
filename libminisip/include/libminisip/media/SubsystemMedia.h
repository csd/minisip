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

/* Copyright (C) 2007
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
*/

#ifndef SUBSYSTEM_MEDIA
#define SUBSYSTEM_MEDIA

#include<libminisip/libminisip_config.h>

#include<libmutil/MessageRouter.h>
#include<libminisip/ipprovider/IpProvider.h>

class SipSoftPhoneConfiguration;
class Session;
class SipIdentity;

class LIBMINISIP_API SubsystemMedia: public CommandReceiver {

	public:
		/**
		 * Constructor, created on startup
		 * @param config reference to the softphone configuration
		 * @param ipProvider reference to the public IP provider, used
		 * for NAT traversal mechanisms
		 */
		SubsystemMedia( MRef<SipSoftPhoneConfiguration *> config, 
				MRef<IpProvider *> ipProvider, 
				MRef<IpProvider *> ip6Provider = NULL );

		virtual ~SubsystemMedia();
		
		/**
		 * @param command the command to handle
		 */
		void handleCommand(std::string subsystem, const CommandString & command );

		CommandString handleCommandResp(std::string subsystem, const CommandString& command);
		
                /**
                 * Set the callback (interface) to Minisip's message router.
                 * 
                 * The Media handler uses this callback to send message to other subsystems
                 * of Minisip. 
                 * 
                 * @param callback
                 *     The pointer to the message router object.
                 */
                void setMessageRouterCallback(MRef<CommandReceiver*> callback);
#if 0
                /**
                 * Get the callback (interface) to Minisip's message router.
                 * 
                 * The Media handler uses this callback to send message to other subsystems
                 * of Minisip. 
                 * 
                 * @returns
                 *     The pointer to the message router object.
                 */
                MRef<CommandReceiver *> getMessageRouterCallback() { return messageRouterCallback;}
#endif                       

		/**
		 * Creates a new media session, for use in a new VoIP call
		 * @param config the call specific configuration
		 * @param callId identifier shared with the SIP stack
		 * @returns a reference to the session created
		 */
		MRef<Session *> createSession( MRef<SipIdentity*> ident, std::string callId );


		virtual std::string getMemObjectType() const {return "SubsystemMedia";}



	private:
		void *mediaHandler;
};

#endif
