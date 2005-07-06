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

/* Name
 * 	SipDialogPresenceClient.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/



#ifndef SipDialogPresenceClient_H
#define SipDialogPresenceClient_H

#ifdef _MSC_VER
#ifdef LIBMINISIP_EXPORTS
#define LIBMINISIP_API __declspec(dllexport)
#else
#define LIBMINISIP_API __declspec(dllimport)
#endif
#else
#define LIBMINISIP_API
#endif



#include<libmsip/SipDialog.h>
#include<libmsip/SipTransaction.h>
#include<libmsip/SipInvite.h>
#include<libmsip/SipBye.h>
#include<libmsip/SipResponse.h>
#include<libmutil/StateMachine.h>

#include<libminisip/SipSoftPhoneConfiguration.h>

class Session;
class SipDialogContainer;
class SipDialogConfig;
class LogEntry;

class LIBMINISIP_API SipDialogPresenceClient: public SipDialog{
	public:
		SipDialogPresenceClient(MRef<SipStack*> stack, MRef<SipDialogConfig*> callconfig, bool use_stun);
		
		virtual ~SipDialogPresenceClient();

		virtual std::string getMemObjectType(){return "SipDialogPresenceClient";}
		
		virtual string getName(){return "SipDialogPresenceClient[callid="+dialogState.callId +"]";}


		virtual bool handleCommand(const SipSMCommand &command);

		void setUpStateMachine();

/*

		MRef<SipInvite*> getLastInvite();
		
		void setLastInvite(MRef<SipInvite*> i);
		
		void sendInvite(const string &branch);
		void sendAuthInvite(const string &branch);
		void sendBye(const string &branch, int);
		void sendCancel(const string &branch);
		void sendInviteOk(const string &branch);
		void sendByeOk(MRef<SipBye*> bye, const string &branch);
		void sendReject(const string &branch);
		void sendRinging(const string &branch);
		void sendNotAcceptable(const string &branch);

		void registerSDP(uint32_t sourceId, MRef<SdpPacket*> sdppack);

		void handleSdp(MRef<SdpPacket*> );
                
		void setLocalCalled(bool lc){localCalled=lc;}
		
		void setNonce(const string &n){ nonce = n; }

		void setRealm(const string &r){ realm = r; }

		MRef<SipSoftPhoneConfiguration*> getPhoneConfig(){return phoneconf;}

		MRef<LogEntry *> getLogEntry();
		void setLogEntry( MRef<LogEntry *> );
*/
	private:
		
		void sendSubscribe(const string &branch);
		void createSubscribeClientTransaction();
		
		bool a0_start_trying_presence(const SipSMCommand &command);
		bool a1_X_subscribing_200OK(const SipSMCommand &command);
		bool a2_trying_retrywait_transperror(const SipSMCommand &command);
		bool a4_X_trying_timerTO(const SipSMCommand &command);
		bool a5_subscribing_subscribing_NOTIFY(const SipSMCommand &command);
		bool a6_subscribing_termwait_stoppresence(const SipSMCommand &command);
		bool a7_termwait_terminated_notransactions(const SipSMCommand &command);

//		MRef<SipSoftPhoneConfiguration*> phoneconf;

		MRef<SipIdentity *> toUri;
		bool useSTUN;
};

#endif
