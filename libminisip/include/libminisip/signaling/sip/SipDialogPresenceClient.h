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
 * 	SipDialogPresenceClient.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#ifndef SipDialogPresenceClient_H
#define SipDialogPresenceClient_H

#include<libminisip/libminisip_config.h>

#include<libmutil/StateMachine.h>

#include<libmsip/SipDialog.h>
#include<libmsip/SipResponse.h>


class Session;
class SipDialogContainer;
class SipDialogConfig;
class LogEntry;

class LIBMINISIP_API SipDialogPresenceClient: public SipDialog{
	public:
		SipDialogPresenceClient(MRef<SipStack*> stack, MRef<SipIdentity*> ident, bool use_stun);
		
		virtual ~SipDialogPresenceClient();

		virtual std::string getMemObjectType() const {return "SipDialogPresenceClient";}
		
		virtual  std::string getName(){return "SipDialogPresenceClient[callid="+dialogState.callId +"]";}


		virtual bool handleCommand(const SipSMCommand &command);

		void setUpStateMachine();

	private:
		
		void sendSubscribe();
		void sendNotifyOk(MRef<SipRequest*> notify);
		void createSubscribeClientTransaction();
		
		bool a0_start_trying_presence(const SipSMCommand &command);
		bool a1_X_subscribing_200OK(const SipSMCommand &command);
		bool a2_trying_retrywait_transperror(const SipSMCommand &command);
		bool a4_X_trying_timerTO(const SipSMCommand &command);
		bool a5_subscribing_subscribing_NOTIFY(const SipSMCommand &command);
		bool a6_subscribing_termwait_stoppresence(const SipSMCommand &command);
		bool a7_termwait_terminated_notransactions(const SipSMCommand &command);
		bool a8_trying_trying_40X(const SipSMCommand &command);
		bool a9_trying_retry_wait_failure(const SipSMCommand &command);

		MRef<SipIdentity *> toUri;
		bool useSTUN;
};

#endif
