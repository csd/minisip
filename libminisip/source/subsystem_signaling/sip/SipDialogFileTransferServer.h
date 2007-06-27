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
 * 	SipDialogFileTransferServer.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#ifndef SipDialogFileTransferServer_H
#define SipDialogFileTransferServer_H

#include<libminisip/libminisip_config.h>

#include<libmutil/StateMachine.h>
#include<libmutil/minilist.h>

#include<libmsip/SipDialog.h>
#include<libmsip/SipResponse.h>

#include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>

class Session;
class SipDialogContainer;
class SipDialogConfig;
class LogEntry;

class LIBMINISIP_API SipDialogFileTransferServer: public SipDialog{
	public:
		SipDialogFileTransferServer(MRef<SipStack*> dContainer, MRef<SipSoftPhoneConfiguration*> pconf, MRef<SipIdentity*> ident, bool use_stun, std::string callId);
		
		virtual ~SipDialogFileTransferServer();

		virtual std::string getMemObjectType() const {return "SipDialogFileTransferServer";}
		
		virtual  std::string getName(){return "SipDialogFileTransferServer[callid="+dialogState.callId +"]";}

		//virtual bool handleCommand(const SipSMCommand &command);

		void setUpStateMachine();
		
		void sendTrying();
		void sendReject(const std::string &branch);
		void sendFailureReport(const std::string &branch);
		void sendByeOk(MRef<SipRequest*>bye, const std::string &branch);

	private:
		//void sendNoticeToAll( std::string onlineStatus);
		//void sendNotice( std::string onlinestatus,  std::string user);
		//void sendSubscribeOk(MRef<SipRequest*> sub);
		//void removeUser( std::string user);
		//void addUser( std::string user);
		
		//void sendNotify(const  std::string &branch,  std::string toUri,  std::string callId);

		bool a0_start_trying_receiveINVITE(const SipSMCommand &command);
		bool a1_trying_receiving_accept(const SipSMCommand &command);
		bool a2_trying_termwait_rejecttransfer(const SipSMCommand &command);
		bool a3_receiving_termwait_BYE(const SipSMCommand &command);
		bool a4_receiving_termwait_failedtransfer(const SipSMCommand &command);
		bool a5_termwait_terminated_notransactions(const SipSMCommand &command);
		
		bool useSTUN;
		//minilist< std::string> subscribing_users;
		Mutex usersLock;

		std::string onlineStatus;
		MRef<SipRequest*> lastInvite;
		MRef<SipSoftPhoneConfiguration*> phoneConf;
};

#endif
