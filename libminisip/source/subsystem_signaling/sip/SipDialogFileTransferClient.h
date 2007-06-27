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

/* Copyright (C) 2004-2006
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/

/* Name
 * 	SipDialogFileTransferClient.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#ifndef SipDialogFileTransferClient_H
#define SipDialogFileTransferClient_H

#include<libminisip/libminisip_config.h>

#include<libmutil/StateMachine.h>

#include<libmsip/SipDialog.h>
#include<libmsip/SipResponse.h>

#include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>

//#include"../../subsystem_media/msrp/MSRPSender.h"

#include"../../subsystem_media/msrp/MSRPMessage.h"

class Session;
class SipDialogContainer;
class SipDialogConfig;
class LogEntry;

class LIBMINISIP_API SipDialogFileTransferClient: public SipDialog{
	public:
		SipDialogFileTransferClient(MRef<SipStack*> stack, MRef<SipSoftPhoneConfiguration*> pconf, MRef<SipIdentity*> ident, bool use_stun);
		
		virtual ~SipDialogFileTransferClient();

		virtual std::string getMemObjectType() const {return "SipDialogFileTransferClient";}
		
		virtual  std::string getName(){return "SipDialogFileTransferClient[callid="+dialogState.callId +"]";}


		//virtual bool handleCommand(const SipSMCommand &command);

		void setUpStateMachine();
		void sendAck();
		void sendBye(const std::string &branch);

	private:
		
		//void sendSubscribe(const  std::string &branch);
		//void sendNotifyOk(MRef<SipRequest*> notify);
		//void createSubscribeClientTransaction();
		
		bool a0_start_trying_invite(const SipSMCommand &command);
		bool a1_trying_trying_100(const SipSMCommand &command);
		bool a2_trying_transfering_200OK(const SipSMCommand &command);
		bool a3_transfering_termwait_transperror(const SipSMCommand &command);
		bool a4_trying_termwait_4XX(const SipSMCommand &command);
		bool a5_transfering_termwait_MSRPDONE(const SipSMCommand &command);
		bool a6_termwait_terminated_notransactions(const SipSMCommand &command);

		MRef<SipRequest*> lastInvite;
		MRef<SipIdentity *> toUri;
		bool useSTUN;

		MRef<MSRPSender*> msrpSender;

		MRef<SipSoftPhoneConfiguration*> phoneConf;
};

#endif
