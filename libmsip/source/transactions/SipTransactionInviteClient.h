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
 * 	SipTransactionInviteInitiator.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/



#ifndef SIPTRANSACTIONINVITEICLIENT_H
#define SIPTRANSACTIONINVITEICLIENT_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipSMCommand.h>
#include"SipTransaction.h"

class SipResponse;

/**
	Implement an INVITE Transaction Client,
	as defined in RFC 3261, section 17.1.1
	
	It is composed of the following states:
		start, calling, proceeding, completed, terminated
	
	Transition functions between the states are axxx_description().
	
	There is a transition from any state into TERMINATED, defined in 
	the base class SipTransaction::a1000_xxxx()
	
	This is a base class for an INVITE transaction client UA
	(the handling of 2xx responses changes).
*/
class SipTransactionInviteClient: public SipTransactionClient{
	public:
		SipTransactionInviteClient(MRef<SipStackInternal*> stackInternal, 
				int seq_no, 
				const std::string &cSeqMethod, 
				const std::string &callid);
		
		virtual ~SipTransactionInviteClient();

		virtual std::string getMemObjectType() const {return "SipTransactionInvCli";}
		virtual std::string getName(){return "transaction_invite_client[branch="+getBranch()+"]";}

		void setUpStateMachine();
		
		/**
			Update the parent dialog's route set. 
			For now, only responses >= 200 update the route-set.
			FIXME: PRACKs too
			FIXME: Actually the route set would be better set in the dialog state machine.
				But it is not possible until the ACK for 2xx responses are handled/sent by
				the dialog/TU and not by the transaction (in violation of the RFC).
		*/
		void setDialogRouteSet(MRef<SipResponse *> resp);

		void sendAck(MRef<SipResponse *>, bool provisional=false); //if no branch specified, use transaction branch attribute

	private:
		
		/**
			Transition from START to CALLING
			It receives the INVITE message to send from the TU.
			Start timer A (retx timer), only for unreliable protocols.
			Start timer B, transaction timeout.
			Send the INVITE message (transport layer).
		*/
		bool a0_start_calling_INVITE( const SipSMCommand &command);
		
		/**
			No Transition, loop CALLING to CALLING
			Timer A goes off, thus we re-tx the msg and reset the timer
			using the backoff algorithm (multiply by 2).
		*/
		bool a1_calling_calling_timerA( const SipSMCommand &command);
		
		/**
			Transition from CALLING to PROCEEDING
			If a non final 1xx response is received, we move to the proceeding
			state. 
			Timer A is cancelled (no more retx)
			Timer B is also cancelled, thus we stay in this state until we receive
			some response from the proxy.
			Forward the response to the TU.
		*/
		bool a2_calling_proceeding_1xx( const SipSMCommand &command);
		
		/**
			Transition from CALLING to COMPLETED
			If a final response 3xx,4xx,5xx or 6xx is received, we move to the COMPLETED
			state. 
			Timer A is cancelled (no more retx)
			Timer B is also cancelled.
			Timer D is set (response absorbtion timeout)
			Forward the response to the TU.
		*/
		bool a3_calling_completed_resp36( const SipSMCommand &command);
		
		/**
			Transition from CALLING to TERMINATED
			A transport error, or timer B (transaction timeout), have happened.
			Notify the TU about this and that the transaction_terminated.
			Timer A is cancelled (no more retx)
			Timer B is also cancelled.
		*/
		bool a4_calling_terminated_ErrOrTimerB( const SipSMCommand &command);
		
		/**
			Transition from CALLING to TERMINATED
			If a final 2xx response is received, we move to the TERMINATED
			state. 
			Forward the 2xx to the TU and notify the TU about transaction_terminated.
			Timer A is cancelled (no more retx)
			Timer B is also cancelled.
			
			Note: this transition does not exhist for INVITE client UA transaction
		*/
		bool a5_calling_terminated_2xx( const SipSMCommand &command);
		
		/**
			Loop Transition in PROCEEDING
			If a non-final 1xx response is received, just notify the TU 
			about the reception of such.
			Forward the response to the TU.
		*/
		bool a6_proceeding_proceeding_1xx( const SipSMCommand &command);
		
		/**
			Transition from PROCEEDING to TERMINATED
			Same as a5 transition.
			
			Note: this transition does not exhist for INVITE client UA transaction.
		*/
		bool a7_proceeding_terminated_2xx( const SipSMCommand &command);
		
		/**
			Transition from PROCEEDING to COMPLETED
			If a final 3xx,4xx,5xx or 6xx response is received, we move to the COMPLETED
			state. 
			Notify the TU about the reception of such response.
			Timer A is cancelled (no more retx)
			Timer B is also cancelled.
			Timer D is set (response absorbtion timeout)
		*/
		bool a8_proceeding_completed_resp36( const SipSMCommand &command);
		
		/**
			Loop Transition in COMPLETED
			If a final 3xx,4xx, 5xx or 6xx response is received, we resend the
			ACK we previously have sent. 
			Do NOT forward this responses to the TU.
		*/
		bool a9_completed_completed_resp36( const SipSMCommand &command);
		
		/**
			Transition from COMPLETED to TERMINATED
			Transport Error transition. Notify the TU about the transport error
			and transaction_terminated.
			Timer D is cancelled (response absorbtion timeout)
			Forward the response to the TU.
		*/
		bool a10_completed_terminated_TErr( const SipSMCommand &command);
		
		/**
			Transition from COMPLETED to TERMINATED
			Timer D goes off, thus we have absorbed enough re-txd responses.
			Notify TU about transaction_terminated.
		*/
		bool a11_completed_terminated_timerD( const SipSMCommand &command);

		/**
		 * If the remote UA requires reliable transmission of 1xx
		 * responses we need to do some extra processing. This
		 * method should be called when ever receiving a 1XX
		 * response to setup the reliable transmission (100rel SIP
		 * extension, RFC3262)
		 */
		//void rel1xxProcessing(MRef<SipResponse*> resp);
		
		MRef<SipRequest*> lastInvite;
//		int timerT1;
		int timerA;
};

#endif
