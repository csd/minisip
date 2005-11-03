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



#ifndef SIPTRANSACTIONINVITEICLIENTUA_H
#define SIPTRANSACTIONINVITEICLIENTUA_H

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
#include<libmsip/SipTransactionInviteClient.h>

/**
	Implement an INVITE Transaction Client UAC,
	as defined in RFC 3261, section 17.1.1
	
	It is composed of the following states:
		start, calling, proceeding, completed, terminated
	
	Transition functions between the states are axxx_description().
	
	There is a transition from any state into TERMINATED, defined in 
	the base class SipTransaction::a1000_xxxx()
	
	All transitions are the same as for the parent class, except for
	a5 and a7, which are substituted by a1001 and 1002.
	Transition a1003 is new.
*/
class LIBMSIP_API SipTransactionInviteClientUA: public SipTransactionInviteClient{
	public:
		SipTransactionInviteClientUA(MRef<SipStack *> stack, MRef<SipDialog*> d, int seq_no, const string &cSeqMethod, string callid);
		
		virtual ~SipTransactionInviteClientUA();

		void changeStateMachine();

		virtual string getMemObjectType(){return "SipTransactionInviteClientUA";}

		virtual string getName(){return "transaction_ua_invite_client[branch="+getBranch()+"]";}
	private:
		/**
			Transition from CALLING to COMPLETED
			It subsitutes the transition from CALLING to TERMINATED that 
			   exhists in its parent class.
			If a final 2xx response is received, we move to the COMPLETED
			state. 
			Timer A is cancelled (no more retx)
			Timer B is also cancelled.
			Set Timer D (wait time for response re-tx)
			Forward the 2xx to the TU.
			Send an ACK (transport layer) ... //FIXME:shouldn't it be the TU sending it?
		*/
		bool a1001_calling_completed_2xx( const SipSMCommand &command);
		
		/**
			Transition from PROCEEDING to COMPLETED
			Same as a1001, but from the PROCEEDING state.
			Send an ACK (transport layer) ... //FIXME:shouldn't it be the TU sending it?		
		*/
		bool a1002_proceeding_completed_2xx( const SipSMCommand &command);

		/**
			Loop in COMPLETED state.
			While in COMPLETED state, resend an ACK for each 2xx response 
			that we receive. Do not notify the TU.
		*/
		bool a1003_completed_completed_2xx( const SipSMCommand &command);
};

#endif
