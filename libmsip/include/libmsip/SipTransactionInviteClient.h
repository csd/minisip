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

class LIBMSIP_API SipTransactionInviteClient: public SipTransactionClient{
	public:
		SipTransactionInviteClient(MRef<SipDialog*> d, int seq_no, string callid);
		
		virtual ~SipTransactionInviteClient();

		virtual string getName(){return "transaction_invite_client[branch="+getBranch()+"]";}

		void setUpStateMachine();

		void sendAck(MRef<SipResponse *>, string branch=""); //if no branch specified, use transaction branch attribute

	private:
		bool a0_start_calling_INVITE( const SipSMCommand &command);
		bool a1_calling_calling_timerA( const SipSMCommand &command);
		bool a2_calling_proceeding_1xx( const SipSMCommand &command);
		bool a3_calling_completed_resp36( const SipSMCommand &command);
		bool a4_calling_terminated_ErrOrTimerB( const SipSMCommand &command);
		bool a5_calling_terminated_2xx( const SipSMCommand &command);
		bool a6_proceeding_proceeding_1xx( const SipSMCommand &command);
		bool a7_proceeding_terminated_2xx( const SipSMCommand &command);
		bool a8_proceeding_completed_resp36( const SipSMCommand &command);
		bool a9_completed_completed_resp36( const SipSMCommand &command);
		bool a10_completed_terminated_TErr( const SipSMCommand &command);
		bool a11_completed_terminated_timerD( const SipSMCommand &command);
		
		MRef<SipInvite*> lastInvite;
//		int timerT1;
		int timerA;
};

#endif
