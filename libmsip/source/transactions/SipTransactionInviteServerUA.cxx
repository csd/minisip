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
 * 	SipTransactionIntiveServerUA.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
*/

/*
                               o Start
                               |
                               |INVITE
                               |a0:pass INV to TU
            INVITE             V send 100 if TU won't in 200ms
         a1:send response+-----------+
                +--------|           |--------+101-199 from TU
                |        | Proceeding|        |a2:send response
                +------->|           |<-------+
       2xx from TU       |           |          Transport Err.
       a1001:send resp   |           |          a4:Inform TU
     +------------------N|           |--------------->+
     |                   +-----------+                |
     |      300-699 from TU |     X 2xx from TU       |
     |   a3:send response   |     | a5:send response  |
     |                      |     +------------------>+
     |                      |                         |
     |      INVITE          V          Timer G fires  |
     |   a6:send response+-----------+a8:send response|
     |          +--------|           |--------+       |
     |          |        | Completed |        |       |
     +--------->+------->|           |<-------+       |
                         +-----------+                |
                            |     |                   |
                        ACK |     |                   |
                    a7: -   |     +------------------>+
                            |        Timer H fires    |
                            V        or Transport Err.|
                         +-----------+ a9:Inform TU   |
                         |           |                |
                         | Confirmed |                |
                         |           |                |
                         +-----------+                |
                               |                      |
                               |Timer I fires         |
                               |a10: -                |
                               |                      |
                               V                      |
                         +-----------+                |
                         |           |                |
                         | Terminated|<---------------+
                         |           |
                         +-----------+

 Note: a5 has been removed and a1001 has been added compared
       with the invite server transaction.
*/

#include<config.h>

#include<libmutil/massert.h>
#include"SipTransactionInviteServerUA.h"
#include<libmsip/SipResponse.h>
#include<libmsip/SipTransitionUtils.h>
#include"../SipCommandDispatcher.h"
#include<libmsip/SipDialog.h>
#include<libmsip/SipDialogConfig.h>

#ifdef DEBUG_OUTPUT
#include<libmutil/termmanip.h>
#endif

using namespace std;

bool SipTransactionInviteServerUA::a1001_proceeding_completed_2xx( const SipSMCommand &command){

	if (transitionMatch(SipResponse::type, 
			command, 
			SipSMCommand::dialog_layer, 
			SipSMCommand::transaction_layer, 
			"2**"))
	{
		cancelTimeout("timerRel1xxResend");
		lastResponse = MRef<SipResponse*>((SipResponse*)*command.getCommandPacket());
		
		if( isUnreliable() ) {
			timerG = sipStackInternal->getTimers()->getG();
			requestTimeout(timerG, "timerG");
		}
		requestTimeout(sipStackInternal->getTimers()->getH(),"timerH");
		
		send(command.getCommandPacket(), false);

		return true;
	}else{
		return false;
	}
}


void SipTransactionInviteServerUA::changeStateMachine(){


	MRef<State<SipSMCommand, string> *> s_proceeding = getState("proceeding");
	massert(s_proceeding);
	
	bool success = s_proceeding->removeTransition("transition_proceeding_terminated_2xx");
	if (!success){
		merr << "ERROR: Could not remove transition from state machine in SipTransactionInviteServerUA (BUGBUG!!)"<<endl;
		massert(0==1);
	}
	

	MRef<State<SipSMCommand, string> *>s_completed = getState("completed");
	massert(s_completed);
		
	new StateTransition<SipSMCommand,string>(this, "transition_proceeding_completed_2xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteServerUA::a1001_proceeding_completed_2xx,
			s_proceeding, s_completed);
}


SipTransactionInviteServerUA::SipTransactionInviteServerUA(MRef<SipStackInternal*> stack,
		int seq_no, 
		const string &cseqm, 
		const string &branch_,
		const string &callid) : 
			SipTransactionInviteServer(stack, seq_no, cseqm, branch_, callid)
{
	changeStateMachine();
}

SipTransactionInviteServerUA::~SipTransactionInviteServerUA(){
}

