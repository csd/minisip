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



#ifndef SIPTRANSACTIONINVITESERVERUA_H
#define SIPTRANSACTIONINVITESERVERUA_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipSMCommand.h>
#include"SipTransactionInviteServer.h"

class SipResponse;

/**
	Implement an INVITE Transaction Server UAC,
	as defined in RFC 3261, section 17.2.1
	
	It is composed of the following states:
		start, proceeding, completed, confirmed, terminated

	Transition functions between the states are axxx_description().
	
	There is a transition from any state into TERMINATED, defined in 
	the base class SipTransaction::a1000_xxxx()
	
	All transitions are the same as for the parent class, except for
	a5, which is substituted by a1001.
*/
class SipTransactionInviteServerUA: public SipTransactionInviteServer{
	public:
		SipTransactionInviteServerUA(MRef<SipStackInternal*> stackInternal, 
				int seq_no, 
				const std::string &cSeqMethod, 
				const std::string &branch, 
				const std::string &callid);
		
		virtual ~SipTransactionInviteServerUA();

		virtual std::string getMemObjectType() const {return "SipTransactionInvServerUA";}
		virtual std::string getName(){return "transaction_ua_invite_server[branch="+getBranch()+"]";}

		void changeStateMachine();

	private:
		/**
		Transiton from PROCEEDING to COMPLETED
		(substitutes a5 from the parent class).
		*/
		bool a1001_proceeding_completed_2xx( const SipSMCommand &command);
};

#endif
