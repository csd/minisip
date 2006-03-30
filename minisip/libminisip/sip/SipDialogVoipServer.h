/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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


#include<config.h>


#include<libmsip/SipDialog.h>
#include<libmsip/SipTransaction.h>
#include<libmsip/SipResponse.h>
#include<libmutil/StateMachine.h>

#include"SipSoftPhoneConfiguration.h"
#include"SipDialogVoip.h"

#ifdef IPSEC_SUPPORT
#include<../ipsec/MsipIpsecAPI.h>
#endif

class Session;
class SipDialogContainer;
class SipDialogConfig;
class LogEntry;

class SipDialogVoipServer: public SipDialogVoip{
	public:
#ifdef IPSEC_SUPPORT
		SipDialogVoipServer(MRef<SipStack*> stack, MRef<SipDialogConfig*> callconfig, MRef<SipSoftPhoneConfiguration*> phoneconf, MRef<Session *> mediaSession, string callId="", MRef<MsipIpsecAPI *> ipsecSession=NULL);
#else
		SipDialogVoipServer(MRef<SipStack*> stack, MRef<SipDialogConfig*> callconfig, MRef<SipSoftPhoneConfiguration*> phoneconf, MRef<Session *> mediaSession, string callId="");

#endif		
		virtual ~SipDialogVoipServer();

		virtual void dummy(){}

		virtual std::string getMemObjectType(){return "SipDialogVoipServer";}
		
		virtual string getName(){return "SipDialogVoipServer[callid="+dialogState.callId +"]";}

	private:
		
		void setUpStateMachine();
		
		void sendInviteOk(const string &branch);
		void sendReject(const string &branch);
		void sendRinging(const string &branch);
		void sendNotAcceptable(const string &branch);

		bool a3001_start_ringing_INVITE( const SipSMCommand &command);
		bool a3002_ringing_incall_accept( const SipSMCommand &command);
		bool a3003_ringing_termwait_BYE( const SipSMCommand &command);
		bool a3004_ringing_termwait_CANCEL( const SipSMCommand &command);
		bool a3005_ringing_termwait_reject( const SipSMCommand &command);
		bool a3006_start_termwait_INVITE( const SipSMCommand &command);
};

#endif
