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
 * 	SipTransactionServer.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/



#ifndef SIPTRANSACTIONNONINVITECLIENT_H
#define SIPTRANSACTIONNONINVITECLIENT_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipSMCommand.h>
#include"SipTransaction.h"

class SipDialog;
class SipRequest;

/**
	Implement a Non-INVITE Transaction Client,
	as defined in RFC 3261, section 17.1.2.
	
	It is composed of the following states:
		start, trying, proceeding, completed, terminated
	
	Transition functions between the states are axxx_description().
	
	There is a transition from any state into TERMINATED, defined in 
	the base class SipTransaction::a1000_xxxx()
*/
class SipTransactionNonInviteClient: public SipTransactionClient{
	public:
		SipTransactionNonInviteClient(MRef<SipStackInternal *> stackInternal, 
				int seq_no, 
				const std::string &cSeqMethod, 
				const std::string &callid);
                ~SipTransactionNonInviteClient();
		
		virtual std::string getMemObjectType() const {return "SipTransactionNonInvCli";}
		
		virtual std::string getName(){return "transaction_noninviteclient[branch="+getBranch()+",type="+getDebugTransType()+"]";}

		void setUpStateMachine();

	private:
		
		/**
			Transition from START to TRYNG.
			It takes the request from the TU and forwards it to the
			transport layer (send() ).
			Timer E is set (re-tx interval, for non reliable transport only)
			Timer F is set (transaction timeout).
		*/
		bool a0_start_trying_request(const SipSMCommand &command);
		
		/**
			Transition from TRYNG to PROCEEDING.
			If the response received is a 1xx non-final response.
			Cancel timeouts (E, F).
			Forward the response received to the TU.
		*/
		bool a1_trying_proceeding_1xx( const SipSMCommand &command);
		
		/**
			Transition from TRYNG to TERMINATED.
			If there is a transport error, or timer F (transaction timeout)
			fires, terminate the transaction. 
			Notify the TU about the transport error and the transaction_terminated.
		*/
		bool a2_trying_terminated_TimerFOrErr( const SipSMCommand &command);
		
		/**
			Transition from PROCEEDING to COMPLTED.
			The response received is a final non-1xx. 
			Set timer K (how long responses are absorbed).
			Forward the received response up to the TU.
		*/
		bool a3_proceeding_completed_non1xxresp( const SipSMCommand &command);
		
		/**
			No transition, state loop from PROCEEDING to PROCEEDING.
			Timer E has fired, indicating the need to re-tx the last
			request sent. 
			Reset Timer E again.
		*/
		bool a4_proceeding_proceeding_timerE( const SipSMCommand &command);
		
		/**
			No transition, state loop from PROCEEDING to PROCEEDING.
			If another non-final 1xx response is received while in PROCEEDING,
			forward it to the TU also (it may be a different 1xx as the first one).
			
		*/
		bool a5_proceeding_proceeding_1xx( const SipSMCommand &command);
		
		/**
			Transition from PROCEEDING to TERMINATED.
			Transaction timeout timer (F) is fired or transport error.
			Notify the TU about a transport error and about the 
			transaction being terminated.
		*/
		bool a6_proceeding_terminated_transperrOrTimerF( const SipSMCommand &command);
		
		/**
			Transition from TRYNG to COMPLETED.
			If the received response is final non-1xx response, 
			transit to the completed stated.
			Cancel timeouts (E and F).
			Forward the response to the TU.
			Set Timer K (response re-tx absorbion timeout).
		*/
		bool a7_trying_completed_non1xxresp(const SipSMCommand &command);
		
		/**
			No transition, loop from TRYNG to TRYING.
			When Timer F fires, it means we must re-tx the last response.
			Update Timer E (re-tx timeout for responses) according to the
			standard algorithm in the RFC.
			Forward the message to the transport layer (re-send).
		*/
		bool a8_trying_trying_timerE(const SipSMCommand &command);
		
		/**
			Transition from COMPLETED to TERMINATED.
			We are finished absorbing re-tx of responses.
			Notify the TU about the transaction_terminated.
		*/
		bool a9_completed_terminated_timerK(const SipSMCommand &command);

		bool a10_completed_completed_anyresp(const SipSMCommand &);

		MRef<SipRequest*> lastRequest;
		
		int timerE; //retransmission of the initial request
};

#endif
