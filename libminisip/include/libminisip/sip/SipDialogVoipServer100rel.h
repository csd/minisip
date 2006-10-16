/*
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2006
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
*/

/* Name
 * 	SipDialogVoipServer100rel.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 *  If a 100rel Voip server is used, it will send a
 *  183 session progress reliably before the 180
 *  ringing is transmitted. This is so that authentication
 *  is done before the phone starts ringing. We must
 *  have mutual authentication before the phone starts
 *  ringing to avoid ghost ringing when Alice policy
 *  does not accept Bob's information.
*/



#ifndef SipDialogVoipServer100rel_H
#define SipDialogVoipServer100rel_H


#include<config.h>


#include<libmsip/SipDialog.h>
#include<libmsip/SipTransaction.h>
#include<libmsip/SipResponse.h>
#include<libmutil/StateMachine.h>

#include<libminisip/sip/SipSoftPhoneConfiguration.h>
#include<libminisip/sip/SipDialogVoipServer.h>

class Session;
class SipDialogContainer;
class SipDialogConfig;
class LogEntry;

class SipDialogVoipServer100rel: public SipDialogVoipServer{
	public:
		SipDialogVoipServer100rel(MRef<SipStack*> stack, MRef<SipDialogConfig*> callconfig, MRef<SipSoftPhoneConfiguration*> phoneconf, MRef<Session *> mediaSession, std::string callId="");

		virtual ~SipDialogVoipServer100rel();

		virtual std::string getMemObjectType() const {return "SipDialogVoipServer100rel";}
		
		virtual std::string getName(){return "SipDialogVoipServer100rel[callid="+dialogState.callId +"]";}

	private:
		int _1xxResendTimer;
		MRef<SipMessage*> lastProgress;
		
		void setUpStateMachine();
		void sendSessionProgress(const std::string &branch);
		void resendSessionProgress();
		void sendPrackOk(const std::string &branch, MRef<SipMessage*> prack);
		
		bool a4001_start_100rel_100relINVITE( const SipSMCommand &command);
		bool a4002_100rel_ringing_PRACK( const SipSMCommand &command);
		bool a4003_100rel_100rel_1xxResendTimer( const SipSMCommand &command);
};

#endif
