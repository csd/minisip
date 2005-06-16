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

/*
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/


/* Name
 * 	SipTransactionInviteServer.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/



#ifndef SIPTRANSACTIONINVITESERVER_H
#define SIPTRANSACTIONINVITESERVER_H

#ifdef _MSC_VER
#ifdef LIBMSIP_EXPORTS
#define LIBMSIP_API __declspec(dllexport)
#else
#define LIBMSIP_API __declspec(dllimport)
#endif
#else
#define LIBMSIP_API
#endif

#include<libmsip/SipSMCommand.h>
#include<libmsip/SipTransaction.h>
#include<libmsip/SipInvite.h>
#include<libmsip/SipResponse.h>

class SipResponse;

class LIBMSIP_API SipTransactionInviteServer : public SipTransactionServer{
	public:
		SipTransactionInviteServer(MRef<SipStack *> stack, MRef<SipDialog*> d, int seq_no, const string &branch, string callid);
		
		virtual ~SipTransactionInviteServer();

		virtual string getName(){return "transaction_INVITE_responder[branch="+getBranch()+"]";}

		void setUpStateMachine();

		void sendTrying();
		void sendOk();
		void sendRinging();
		void sendReject();

	protected:
		MRef<SipResponse*> lastResponse;
		int timerG;

	private:
		bool a0_start_proceeding_INVITE( const SipSMCommand &command);
		bool a1_proceeding_proceeding_INVITE( const SipSMCommand &command);
		bool a2_proceeding_proceeding_1xx( const SipSMCommand &command);
		bool a3_proceeding_completed_resp36( const SipSMCommand &command);
		bool a4_proceeding_terminated_err( const SipSMCommand &command);
		bool a5_proceeding_terminated_2xx( const SipSMCommand &command);
		bool a6_completed_completed_INVITE( const SipSMCommand &command);
		bool a7_completed_confirmed_ACK( const SipSMCommand &command);
		bool a8_completed_completed_timerG( const SipSMCommand &command);
		bool a9_completed_terminated_errOrTimerH( const SipSMCommand &command);
		bool a10_confirmed_terminated_timerI( const SipSMCommand &command);
		
		bool user_has_accepted;
		bool user_has_rejected;

		string key_mgmt;
		int32_t key_mgmt_method;
};

#endif
