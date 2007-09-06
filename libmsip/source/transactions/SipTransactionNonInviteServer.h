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



#ifndef SIPTRANSACTIONNONINVITESERVER_H
#define SIPTRANSACTIONNONINVITESERVER_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipSMCommand.h>
#include"SipTransaction.h"

class SipResponse;

/**
	Implement a Non-INVITE Transaction Server,
	as defined in RFC 3261, section 17.2.2.
	
	It is composed of the following states:
		start, trying, proceeding, completed, terminated
	
	Transition functions between the states are axxx_description().
	
	There is a transition from any state into TERMINATED, defined in 
	the base class SipTransaction::a1000_xxxx()
*/
class SipTransactionNonInviteServer: public SipTransactionServer{
	public:
		SipTransactionNonInviteServer(MRef<SipStackInternal*> stackInternal, 
				int seq_no, 
				const std::string &cSeqMethod, 
				const std::string &branch, 
				const std::string &callid);
		
		virtual ~SipTransactionNonInviteServer();

		virtual std::string getMemObjectType() const {return "SipTransactionNonInvServer";}
		virtual std::string getName(){return "transaction_noninviteserver[branch="+getBranch()+",type="+getDebugTransType()+"]";}

		void setUpStateMachine();

	private:
		/**
			Transition from START to TRYNG.
			It takes the received request and enqueues it,
			to be received by the TU (Transaction Unit).
		*/
		bool a0_start_trying_request(const SipSMCommand &command);
		
		/**
			Transition from TRYNG to PROCEEDING.
			If the command contains a 1xx response (to reply to the 
			initial request) forward it to the transport layer (send() )
		*/		
		bool a1_trying_proceeding_1xx(const SipSMCommand &command);
		
		/**
			Transition from TRYNG to COMPLETED.
			If the command contains a non-1xx final response (to reply to the 
			initial request) forward it to the transport layer (send() )
		*/
		bool a2_trying_completed_non1xxresp(const SipSMCommand &command);
		
		/**
			Transition from PROCEEDING to COMPLETED.
			If the command contains a non-1xx final response (we are 
			in proceeding, so we already have sent a 1xx response), forward
			it to the transport layer.
			Set TimerJ if transition is true (how long are we going to absorb
			request retx).
		*/
		bool a3_proceeding_completed_non1xxresp(const SipSMCommand &command);
		
		/**
			No state change, loop in PROCEEDING.
			Re-send the last response. 
			This happens when a re-tx of the request is received while in 
			PROCEEDING state (by command of the TU usually).
		*/
		bool a4_proceeding_proceeding_request(const SipSMCommand &command);
		
		/**
			No state change, loop in PROCEEDING.
			Send the new response contained in the command.
			(by command of the TU usually).
		*/
		bool a5_proceeding_proceeding_1xx(const SipSMCommand &command);
		
		/**
			Transition from PROCEEDING to TERMINATED.
			Transport error detected. 
			Notify the TU about the transport error, as well as from 
			the transaction_terminated.
		*/
		bool a6_proceeding_terminated_transperr(const SipSMCommand &command);
		
		/**
			No state change, loop in COMPLETED.
			Forward final responses to the transport layer for re-tx  whenever a 
			retransmission of the request is received.  
			Any other final responses passed by the TU to the server
			transaction MUST be discarded while in the "Completed" state	
		*/
		bool a7_completed_completed_request(const SipSMCommand &command);
		
		/**
			Transition from COMPLETED to TERMINATED.
			Transport error detected. 
			Notify the TU about the transport error, as well as from 
			the transaction_terminated.
		*/
		bool a8_completed_terminated_transperr(const SipSMCommand &command);
		
		/**
			Transition from COMPLETED to TERMINATED.
			Timer J fired, stop absorbing retx. 
			Notify the TU about the transaction_terminated.
		*/
		bool a9_completed_terminated_timerJ(const SipSMCommand &command);

		MRef<SipResponse*> lastResponse;

		//int timerT1;
		int timerJ;
};

#endif
