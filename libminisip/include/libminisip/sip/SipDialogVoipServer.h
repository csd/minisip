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
 *	    Joachim Orrblad <joachim@orrblad.com>
*/

/* Name
 * 	SipDialogVoipServer.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#ifndef SipDialogVoipServer_H
#define SipDialogVoipServer_H

#include<libminisip/libminisip_config.h>

#include<libmutil/StateMachine.h>

#include<libmsip/SipDialog.h>
#include<libmsip/SipTransaction.h>
#include<libmsip/SipResponse.h>

#include<libminisip/sip/SipDialogVoip.h>
#include<libminisip/sip/SipSoftPhoneConfiguration.h>
#ifdef IPSEC_SUPPORT
#	include<libminisip/ipsec/MsipIpsecAPI.h>
#endif

class Session;
class SipDialogContainer;
class SipDialogConfig;
class LogEntry;

class SipDialogVoipServer: public SipDialogVoip{
	public:
#ifdef IPSEC_SUPPORT
		SipDialogVoipServer(MRef<SipStack*> stack, MRef<SipDialogConfig*> callconfig, MRef<SipSoftPhoneConfiguration*> phoneconf, MRef<Session *> mediaSession, std::string callId="", MRef<MsipIpsecAPI *> ipsecSession=NULL);
#else
		SipDialogVoipServer(MRef<SipStack*> stack, MRef<SipDialogConfig*> callconfig, MRef<SipSoftPhoneConfiguration*> phoneconf, MRef<Session *> mediaSession, std::string callId="");

#endif		
		virtual ~SipDialogVoipServer();

		virtual void dummy(){}

		virtual std::string getMemObjectType(){return "SipDialogVoipServer";}
		
		virtual std::string getName(){return "SipDialogVoipServer[callid="+dialogState.callId +"]";}

	private:
		
		void setUpStateMachine();
		
		void sendInviteOk(const std::string &branch);
		void sendReject(const std::string &branch);
		void sendRinging(const std::string &branch);
		void sendNotAcceptable(const std::string &branch);

		bool a3001_start_ringing_INVITE( const SipSMCommand &command);
		bool a3002_ringing_incall_accept( const SipSMCommand &command);
		bool a3003_ringing_termwait_BYE( const SipSMCommand &command);
		bool a3004_ringing_termwait_CANCEL( const SipSMCommand &command);
		bool a3005_ringing_termwait_reject( const SipSMCommand &command);
		bool a3006_start_termwait_INVITE( const SipSMCommand &command);
};

#endif
