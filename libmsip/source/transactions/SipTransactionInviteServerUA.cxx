/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Copyright (C) 2004 
 *
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
        2xx from TU      |           |          Transport Err.
        send resp        |           |          a4:Inform TU
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

              Figure 7: INVITE server transaction
*/

#include<config.h>

#include<assert.h>
#include<libmsip/SipTransactionInviteServerUA.h>
#include<libmsip/SipResponse.h>
#include<libmsip/SipTransactionUtils.h>
#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipDialog.h>

#ifdef DEBUG_OUTPUT
#include<libmutil/termmanip.h>
#endif


bool SipTransactionInviteServerUA::a1001_proceeding_completed_2xx( const SipSMCommand &command){

	if (transitionMatch(command, SipResponse::type, SipSMCommand::TU, SipSMCommand::transaction, "2**")){
		lastResponse = MRef<SipResponse*>((SipResponse*)*command.getCommandPacket());
		
		timerG=500;
		requestTimeout( timerG,"timerG");
		
		requestTimeout(32000,"timerH");
		
		send(command.getCommandPacket());

		return true;
	}else{
		return false;
	}
}


void SipTransactionInviteServerUA::changeStateMachine(){


	State<SipSMCommand, string> *s_proceeding = getState("proceeding");
	assert(s_proceeding);
	
	bool success = s_proceeding->removeTransition("transition_proceeding_terminated_2xx");
	if (!success){
		merr << "ERROR: Could not remove transition from state machine in SipTransactionInviteServerUA (BUGBUG!!)"<<end;
		assert(0==1);
	}
	

	State<SipSMCommand, string> *s_completed = getState("completed");
	assert(s_completed);
		
		
	StateTransition<SipSMCommand,string> *transition_proceeding_completed_2xx=
		new StateTransition<SipSMCommand,string>(this,
				"transition_proceeding_completed_2xx",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteServerUA::a1001_proceeding_completed_2xx,
				s_proceeding, s_completed
				);
}


SipTransactionInviteServerUA::SipTransactionInviteServerUA(MRef<SipDialog*> d, int seq_no, const string &branch,string callid) : 
		SipTransactionInviteServer(d, seq_no, branch, callid)
{
	changeStateMachine();
}

SipTransactionInviteServerUA::~SipTransactionInviteServerUA(){
}

