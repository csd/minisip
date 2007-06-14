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

/* Name
 * 	SipDialogPresenceServer.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#ifndef SipDialogPresenceServer_H
#define SipDialogPresenceServer_H

#include<libminisip/libminisip_config.h>

#include<libmutil/StateMachine.h>
#include<libmutil/minilist.h>

#include<libmsip/SipDialog.h>
#include<libmsip/SipResponse.h>


class Session;
class SipDialogContainer;
class SipDialogConfig;
class LogEntry;

class LIBMINISIP_API SipDialogPresenceServer: public SipDialog{
	public:
		SipDialogPresenceServer(MRef<SipStack*> dContainer, MRef<SipIdentity*> ident, bool use_stun);
		
		virtual ~SipDialogPresenceServer();

		virtual std::string getMemObjectType() const {return "SipDialogPresenceServer";}
		
		virtual  std::string getName(){return "SipDialogPresenceServer[callid="+dialogState.callId +"]";}

		virtual bool handleCommand(const SipSMCommand &command);

		void setUpStateMachine();

	private:
		void sendNoticeToAll( std::string onlineStatus);
		void sendNotice( std::string onlinestatus,  std::string user);
		void sendSubscribeOk(MRef<SipRequest*> sub);
		void removeUser( std::string user);
		void addUser( std::string user);
		
		void sendNotify(std::string toUri,  std::string callId);

		bool a0_start_default_startpresenceserver(const SipSMCommand &command);
		bool a1_default_default_timerremovesubscriber(const SipSMCommand &command);
		bool a2_default_default_localpresenceupdated(const SipSMCommand &command);
		bool a3_default_termwait_stoppresenceserver(const SipSMCommand &command);
		bool a4_termwait_terminated_notransactions(const SipSMCommand &command);
		bool a5_default_default_SUBSCRIBE(const SipSMCommand &command);

		bool useSTUN;
		minilist< std::string> subscribing_users;
		Mutex usersLock;

		std::string onlineStatus;
};

#endif
