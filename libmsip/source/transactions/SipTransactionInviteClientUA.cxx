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
 * 	SipTransactionIntiveInitiator.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/


/*
                                start
                               o
                               |INVITE from TU
             Timer A fires     |INVITE sent
             Reset A,          V                      Timer B fires
             INVITE sent +-----------+                or Transport Err.
               +---------|           |---------------+inform TU
               |         |  Calling  |---------------)---+
               +-------->|           |X------------->|   N
                         +-----------+ 2xx           |   | a1001
                            |  |       2xx to TU     |   | 2XX
                            |  |1xx                  |   | 2XX to TU
    300-699 +---------------+  |1xx to TU            |   | ACK sent
   ACK sent |                  |                     |   |
resp. to TU |  1xx             V                     |   |
            3A
            |  1xx to TU  -----------+               |   |
            |  +---------|           |               |   |
            |  |         |Proceeding |-X------------>|   |
            |  +-------->|           | 2xx           |   |
            |            +-----------+ 2xx to TU     |   |
            |       300-699    | N 2XX  a1002        |   |
            |       ACK sent,  | | 2XX to TU         |   |
            |       resp. to TU| | ACK sent          |   |
            |                  | |  +----------------)---+  NOTE:
            |  300-699         V V  V                |
            |  ACK sent  +-----------+Transport Err. |  transitions
            |  +---------|           |Inform TU      |  labeled with
            |  |         | Completed |-------------->|  the event
            |  +-------->|           |<--+           |  over the action
            |            +-----------+   |2XX        |  to take
            |              ^   |  N      |ACK sent   |
            |              |   |  +------+           |
            |              |   |                     |
            |              |   | Timer D fires       |
            +--------------+   | -                   |
                               |                     |
                               V                     |
                         +-----------+               |
                         |           |               |
                         | Terminated|<--------------+
                         |           |
                         +-----------+


*/

 
#include<config.h>

#include<assert.h>
#include<libmsip/SipDialog.h>
#include<libmsip/SipTransactionInviteClientUA.h>
#include<libmsip/SipResponse.h>
#include<libmsip/SipTransactionUtils.h>
#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipSMCommand.h>
#include<libmutil/MemObject.h>
#include<libmutil/dbg.h>


	
bool SipTransactionInviteClientUA::a1001_calling_completed_2xx( const SipSMCommand &command) {
	if (transitionMatch(command, SipResponse::type, SipSMCommand::remote, IGN, "2**")){
		MRef<SipResponse *> resp((SipResponse*) *command.getCommandPacket());
		
		cancelTimeout("timerA");
		cancelTimeout("timerB");
		requestTimeout(64000, "timerD");

		SipSMCommand cmd( command.getCommandPacket(), 
				SipSMCommand::transaction, 
				SipSMCommand::TU);
		dialog->getDialogContainer()->enqueueCommand( cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE );

		sendAck(resp, getBranch()/*+"ACK"*/);
		
		return true;
	}else{
		return false;
	}
}
	
bool SipTransactionInviteClientUA::a1002_proceeding_completed_2xx( const SipSMCommand &command) {
	if (transitionMatch(command, SipResponse::type, SipSMCommand::remote, IGN, "2**")){
		MRef<SipResponse *> resp((SipResponse*)*command.getCommandPacket());
		cancelTimeout("timerA");
		cancelTimeout("timerB");
		requestTimeout(64000, "timerD");
		
		SipSMCommand cmd( command.getCommandPacket(), 
				SipSMCommand::transaction, 
				SipSMCommand::TU);
		dialog->getDialogContainer()->enqueueCommand( cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE );

		sendAck(resp, getBranch()/*+"ACK"*/);

		return true;
	}else{
		return false;
	}
}
	
bool SipTransactionInviteClientUA::a1003_completed_completed_2xx( const SipSMCommand &command) {
	if (transitionMatch(command, SipResponse::type, SipSMCommand::remote, IGN, "2**")){
		MRef<SipResponse *> resp((SipResponse*)*command.getCommandPacket());

		sendAck(resp, getBranch()/*+"ACK"*/);

		return true;
	}else{
		return false;
	}
}


void SipTransactionInviteClientUA::changeStateMachine(){
	MRef<State<SipSMCommand, string> *> s_calling = getState("calling");
	bool success = s_calling->removeTransition("transition_calling_terminated_2xx");
	if (!success)
		merr << "ERROR: Could not remove transition from state machine in SipTransactionInviteClientUA (BUGBUG!!)"<< end;

	
	MRef<State<SipSMCommand, string> *>s_proceeding = getState("proceeding");
	success = s_proceeding->removeTransition("transition_proceeding_terminated_2xx");
	if (!success)
		merr << "ERROR: Could not remove transition(2) from state machine in SipTransactionInviteClientUA (BUGBUG!!)"<< end;
	

	MRef<State<SipSMCommand, string> *> s_completed= getState("completed");


	
	new StateTransition<SipSMCommand,string>(this, "transition_calling_completed_2xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteClientUA::a1001_calling_completed_2xx,
			s_calling, s_completed);

	new StateTransition<SipSMCommand,string>(this, "transition_proceeding_completed_2xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteClientUA::a1002_proceeding_completed_2xx,
			s_proceeding, s_completed);

	new StateTransition<SipSMCommand,string>(this, "transition_completed_completed_2xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteClientUA::a1003_completed_completed_2xx,
			s_completed, s_completed);
}


SipTransactionInviteClientUA::SipTransactionInviteClientUA(MRef<SipDialog*> call, 
            int seq_no,string callid): 
		SipTransactionInviteClient(call, seq_no, callid)
{
	changeStateMachine();
}

SipTransactionInviteClientUA::~SipTransactionInviteClientUA(){
}

