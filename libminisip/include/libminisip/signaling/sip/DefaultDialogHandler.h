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

#ifndef DEFAULTDialogHANDLER_H
#define DEFAULTDialogHANDLER_H

#include<libminisip/libminisip_config.h>

#include<libmutil/MemObject.h>

#include<libmsip/SipDialog.h>
#include<libmsip/SipSMCommand.h>
#include<libmsip/SipDialogConfig.h>

#include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>
#ifdef P2T_SUPPORT
#	include<libminisip/signaling/p2t/GroupListServer.h>
#	include<libminisip/signaling/p2t/SipDialogP2T.h>
#endif

class SipDialogContainer;

/**
 * the final destination for a SipSMCommand if no dialog handled it before.
 * If even the DefaultDialogHandler cannot handle the command, it will be
 * discarded.
 */
class LIBMINISIP_API DefaultDialogHandler : public SipDefaultHandler {
	public:
		
		/**
		 * Constructor
		 * @param dContainer the Dialog Container
		 * @param conf       the dialog configuration
		 * @param pconf      the phone configuration
		 */
		DefaultDialogHandler(MRef<SipStack*> stack, 
				MRef<SipSoftPhoneConfiguration*> pconf, 
				MRef<SubsystemMedia*> subsystemMedia);
		
		virtual ~DefaultDialogHandler();

		virtual std::string getMemObjectType() const {return "DefaultDialogHandler";}

		virtual std::string getName();
		
		virtual bool handleCommand(const SipSMCommand &command);

		void handleCommand(std::string subsystem, const CommandString &cmd);

		CommandString handleCommandResp(std::string subsystem, const CommandString &cmd);


		
	private:
		minilist<ConfMember> connectList;

		MRef<SipStack*> sipStack;

		int outsideDialogSeqNo;

		
		bool handleCommandPacket(MRef<SipMessage*> pkt );
		bool handleCommandString(CommandString &command );
		
		MRef<SipSoftPhoneConfiguration*> phoneconf;
		MRef<SubsystemMedia*> subsystemMedia;
#ifdef P2T_SUPPORT
		
		/**
		 * Sets up a new P2T Session that is initiated from the local user.
		 * Creates the Group Member List and adds it to the Group List Server.
		 * Starts the SipDialogP2T and all SipDialogP2Tuser dialogs.
		 * @param command "p2tStartSession"
		 */
		void startP2TSession(const SipSMCommand &command);
		
		/**
		 * Sets up the first part of a new P2T Session initiated from a remote user.
		 * Starts the local Group List Server
		 * Downloads the Group Member List from the remote Group List Server
		 * and adds it to the local server.
		 * Starts the SipDialogP2T dialog and ONE SipDialogP2Tuser for the inviting
		 * user.
		 * Sends command to the GUI that have to accept this P2T invitation
		 * @param command received INVITE message
		 */
		void inviteP2Treceived(const SipSMCommand &command);
		
		/**
		 * Sets up the second part of a new P2T Session initiated from a remote user
		 * if the local user has accepted this call.
		 * Starts the remaining SipDialogP2Tuser dialogs.
		 * @param command "p2tSessionAccepted"
		 */
		void inviteP2Taccepted(const SipSMCommand &command);
		
		
		
		/**
		 * Reference to the Group List Server used for P2T Sessions.
		 * DefaultDialogHandler is responsible for starting and stoping the
		 * Group List Server.
		 */
		MRef<GroupListServer*> grpListServer;
		
		/**
		 * Sets a reference to the SipDialogP2T with the specified Group Identity
		 * @param GroupId   the Group Identity
		 * @param p2tDialog the reference to the call
		 * return true if a dialog was found
		 */
		bool getP2TDialog(std::string GroupId, MRef<SipDialogP2T*> &p2tDialog);
#endif
		
		/**
		 * Sets the value in the DialogConfig before a SipDialogP2Tuser or
		 * SipDialogP2TVoIP dialog will be started to the specified user and
		 * checks if the user has a correct formatted SIP URI
		 * @param user         SIP URI of the user
		 * @param dialogConfig the SipDialogConfig that will be 
		 *                     modified
		 * @return true if the SIP URI is valid
		 */
		bool modifyDialogConfig(std::string user, MRef<SipDialogConfig *> dialogConfig);

		void sendIMOk(MRef<SipRequest*> immessage);
		
		void sendIM(std::string msg, int seqno, std::string toUri);
			
		MRef<SipIdentity *> lookupTarget(const SipUri &uri);
};

#endif
