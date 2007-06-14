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

/* Copyright (C) 2004-2007
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *	    Joachim Orrblad <joachim@orrblad.com>
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/

/* Name
 * 	SipDialogVoipServer.h
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

#ifndef SipDialogVoipServer_H
#define SipDialogVoipServer_H

#include<libminisip/libminisip_config.h>

#include<libmutil/StateMachine.h>

#include<libmsip/SipDialog.h>
#include<libmsip/SipResponse.h>

#include<libminisip/signaling/sip/SipDialogVoip.h>

class Session;
class SipDialogContainer;
class SipDialogConfig;
class LogEntry;

class LIBMINISIP_API SipDialogVoipServer: public SipDialogVoip{
	public:
		SipDialogVoipServer(MRef<SipStack*> stack, 
				MRef<SipIdentity*> ident, 
				bool useStun,
				MRef<Session *> mediaSession, 
				std::string callId="");

		virtual ~SipDialogVoipServer();

		virtual std::string getMemObjectType() const {return "SipDialogVoipServer";}
		
		virtual std::string getName(){return "SipDialogVoipServer[callid="+dialogState.callId +"]";}

	protected:
		void sendRinging();
		bool isMatchingPrack( MRef<SipMessage*> provisional,
				      MRef<SipRequest*> prack );
		
	private:
		
		void setUpStateMachine();
		
		void sendInviteOk();
		void sendReject();
		void sendNotAcceptable();
		void sendSessionProgress();
		void sendPrackOk( MRef<SipRequest*> prack );

		bool a3001_start_ringing_INVITE( const SipSMCommand &command);
		bool a3002_ringing_incall_accept( const SipSMCommand &command);
		bool a3003_ringing_termwait_BYE( const SipSMCommand &command);
		bool a3004_ringing_termwait_CANCEL( const SipSMCommand &command);
		bool a3005_ringing_termwait_reject( const SipSMCommand &command);
		bool a3006_start_termwait_INVITE( const SipSMCommand &command);
		bool a3007_start_100rel_INVITE( const SipSMCommand &command);
		bool a3008_100rel_ringing_PRACK( const SipSMCommand &command);
		bool a3009_any_any_ResendTimer1xx( const SipSMCommand &command);
		bool a3010_any_any_PRACK( const SipSMCommand &command);

		bool use100Rel;
		int resendTimer1xx;
		MRef<SipMessage*> lastProvisional;
};

#endif
