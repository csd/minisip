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

/**
	Implement an INVITE Transaction Server
	as defined in RFC 3261, section 17.2.1
	
	It is composed of the following states:
		start, proceeding, completed, confirmed, terminated
	
	Transition functions between the states are axxx_description().
	
	There is a transition from any state into TERMINATED, defined in 
	the base class SipTransaction::a1000_xxxx()
	
	This is a base class for an INVITE transaction server UA
	(the handling of 2xx responses changes).
*/
class LIBMSIP_API SipTransactionInviteServer : public SipTransactionServer{
	public:
		SipTransactionInviteServer(MRef<SipStack *> stack, MRef<SipDialog*> d, int seq_no, const string &branch, string callid);
		
		virtual ~SipTransactionInviteServer();

		virtual string getMemObjectType(){return "SipTransactionInvServer";}
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
		
		/**
		Transition from START to PROCEEDING
		The first command a INVITE server transaction receives is an INVITE
		packet. It forwards the packet to the TU/call. We send no 1xx since we
 		expect a response from the TU in less than 200 ms.
		*/
		bool a0_start_proceeding_INVITE( const SipSMCommand &command);
		
		/**
		Loop in PROCEEDING state
		If we receive the INVITE packet _again_, our response was probably lost.
		In that case we retransmit it. 
		Note: We expected that the TU/Call would answer before the remote side
		retransmitted. 
		TODO: Implement an option to send 100 trying in action "a0".
		*/
		bool a1_proceeding_proceeding_INVITE( const SipSMCommand &command);

		/**
		Loop in PROCEEDING state.
		If a "1xx" response is received from the TU(/call), 
		send it to the remote side and 
		save it in case we need to retransmit it.
		*/		
		bool a2_proceeding_proceeding_1xx( const SipSMCommand &command);
		
		/**
		Transition from PROCEEDING to COMPLETED
		If a response (non 2xx/1xx) is received from the TU/Call, we send it to
		the remote side. 
		We save it in case we need to retransmit it later.
		Set "timerH", wait time for an ACK.
		Set "timerG", response re-tx interval (only if unreliable transport)
		*/
		bool a3_proceeding_completed_resp36( const SipSMCommand &command);
		
		/**
		Transition from PROCEEDING to TERMINATED
		If we receive a "transport_error" indication, 
		we inform the TU and go to the terminated state.
		//FIXME: There is currently no one giving this command to the transaction
		layer (EE, r241).
		*/
		bool a4_proceeding_terminated_err( const SipSMCommand &command);

		/**
		Transiton from PROCEEDING to TERMINATED
		If a 2xx response is received from the TU/Call,
		  it is sent to the remote side 
		  and the transaction terminates. 
		Note that the ACK is not yet handled when a 
		  2xx terminates the INVITE server transaction.
		*/
		bool a5_proceeding_terminated_2xx( const SipSMCommand &command);
		
		/**
		Loop in COMPLETED state.
		If we receive the INVITE again from the TU, the response has been lost. 
		We retransmit the response again without informing the TU.
		*/
		bool a6_completed_completed_INVITE( const SipSMCommand &command);

		/**
		Transition from COMPLETED to CONFIRMED state.
		If we receive an ACK while in COMPLETED, we go to the "confirmed" state
		without informing the TU.
		Schedule for transaction termination using timer I.
		All other timers are cancelled.
		*/
		bool a7_completed_confirmed_ACK( const SipSMCommand &command);
		
		/**
		Loop in COMPLETED state.
		If timer G fires when in the "completed" state, we have not received an
		"ACK" to our response (3xx-6xx). 
		Resend the response and hope for an "ACK" before a transaction
		timeout (timer H).
		*/
		bool a8_completed_completed_timerG( const SipSMCommand &command);

		/**
		Transition from COMPLETED to TERMINATED
		If there is a transport error 
		or 
		if timerH fires, 
		we failed to receive an ACK after several re-sends. 
		We inform the TU/Call and go to the terminated state.
		*/
		bool a9_completed_terminated_errOrTimerH( const SipSMCommand &command);
		
		/**
		Transition from CONFIRMED to TERMINATED
		When timer I fires, we stop absorbing ACKs.
		Move to TERMINATED and inform TU.
		*/
		bool a10_confirmed_terminated_timerI( const SipSMCommand &command);
		
		bool user_has_accepted;
		bool user_has_rejected;

		string key_mgmt;
		int32_t key_mgmt_method;
};

#endif
