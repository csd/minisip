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
 * 	SipTransactionNonInviteClient.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

/*
 From RFC3261 with action number added:

                                   |Request from TU
                                   |a0.send request
               a8: Timer E         V
               send request  +-----------+
                   +---------|           |-------------------+
                   |         |  Trying   |  a2: Timer F      |
                   +-------->|           |  or Transport Err.|
                             +-----------+  inform TU        |
                a7: 200-699     |  |                         |
                resp. to TU     |  |a1: 1xx                  |
                +---------------+  |resp. to TU              |
                |                  |                         |
                |   a4: Timer E    V       a6: Timer F       |
                |   send req +-----------+ or Transport Err. |
                |  +---------|           | inform TU         |
                |  |         |Proceeding |------------------>|
                |  +-------->|           |-----+             |
                |            +-----------+     |a5: 1xx      |
                |              |      ^        |resp to TU   |
                | a3: 200-699  |      +--------+             |
                | resp. to TU  |                             |
                |              |                             |
                |              V                             |
                |            +-----------+                   |
                |            |           |----+ a10:***      |
                |            | Completed |    |     -        |
                |            |           |<---+              |
                |            +-----------+                   |
                |              ^   |                         |
                |              |   | a9: Timer K             |
                +--------------+   | -                       |
                                   |                         |
                                   V                         |
             NOTE:           +-----------+                   |
                             |           |                   |
         transitions         | Terminated|<------------------+
         labeled with        |           |
         the event           +-----------+
         over the action
         to take

                 Figure 6: non-INVITE client transaction




*/

#include<config.h>

#include<libmutil/massert.h>
#include"SipTransactionNonInviteClient.h"
#include"../SipCommandDispatcher.h"
#include<libmsip/SipResponse.h>
#include<libmsip/SipTransitionUtils.h>
#include<libmsip/SipCommandString.h>
#include<libmsip/SipDialog.h>
#include<libmsip/SipDialogConfig.h>
#include<libmsip/SipTimers.h>

#ifdef DEBUG_OUTPUT
#include<libmutil/dbg.h>
#endif

using namespace std;

bool SipTransactionNonInviteClient::a0_start_trying_request( const SipSMCommand &command) {
	if (transitionMatch(SipMessage::anyType, command, SipSMCommand::dialog_layer, SipSMCommand::transaction_layer)){
#ifdef DEBUG_OUTPUT
		setDebugTransType(command.getCommandPacket()->getType() );
#endif
		lastRequest = dynamic_cast<SipRequest*>(*command.getCommandPacket());
		if( isUnreliable() ) {
			timerE = sipStackInternal->getTimers()->getE();
			requestTimeout(timerE, "timerE");
		}
		requestTimeout(sipStackInternal->getTimers()->getF(), "timerF");

		send(*lastRequest,true);
		
		return true;
	}else{
		return false;
	}
}

bool SipTransactionNonInviteClient::a1_trying_proceeding_1xx( const SipSMCommand &command){
	
	if (transitionMatch(SipResponse::type, 
			command, 
			SipSMCommand::transport_layer, 
			SipSMCommand::transaction_layer, 
			"1**"))
	{
		cancelTimeout("timerE");
		cancelTimeout("timerF");
		SipSMCommand cmd(
				command.getCommandPacket(), 
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer );
		dispatcher->enqueueCommand(cmd, HIGH_PRIO_QUEUE);
		
		return true;
	}else{
		
		return false;
	}
}

bool SipTransactionNonInviteClient::a2_trying_terminated_TimerFOrErr( const SipSMCommand &command){
	
	if (transitionMatch(command, 
				SipCommandString::transport_error, 
				SipSMCommand::transport_layer, 
				SipSMCommand::transaction_layer) 
			|| transitionMatch(command, 
				"timerF", 
				SipSMCommand::transaction_layer, 
				SipSMCommand::transaction_layer)){
		cancelTimeout("timerE");
		cancelTimeout("timerF");
		
		SipSMCommand cmd(
				CommandString( callId, SipCommandString::transport_error),
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer);
		dispatcher->enqueueCommand(cmd, HIGH_PRIO_QUEUE);

		SipSMCommand cmdterminated(
			CommandString( getTransactionId(), SipCommandString::transaction_terminated),
			SipSMCommand::transaction_layer,
			SipSMCommand::dispatcher);
		dispatcher->enqueueCommand( cmdterminated, HIGH_PRIO_QUEUE);
		
		
		return true;
	}else{
		
		return false;
	}
}

bool SipTransactionNonInviteClient::a3_proceeding_completed_non1xxresp( const SipSMCommand &command){

	if (transitionMatch(SipResponse::type, 
			command, 
			SipSMCommand::transport_layer, 
			SipSMCommand::transaction_layer, 
			"2**\n3**\n4**\n5**\n6**"))
	{
		
		MRef<SipResponse*> pack((SipResponse *)*command.getCommandPacket());
		cancelTimeout("timerE"); //no more retx of the request
		if( isUnreliable() ) //response re-tx timer
			requestTimeout(sipStackInternal->getTimers()->getT4(),"timerK");
		else
			requestTimeout(0,"timerK");
		
		//forward to TU
		MRef<SipMessage*> pref(*pack);
		SipSMCommand cmd(pref, 
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer);
		dispatcher->enqueueCommand(cmd, HIGH_PRIO_QUEUE);

		
		return true;
	}else{
		
		return false;
	}
}

bool SipTransactionNonInviteClient::a4_proceeding_proceeding_timerE( const SipSMCommand &command){
	
	if (transitionMatch(command, 
			"timerE",
			SipSMCommand::transaction_layer,
			SipSMCommand::transaction_layer))
	{
		timerE *= 2;
		if( timerE > sipStackInternal->getTimers()->getT2() ) 
			timerE = sipStackInternal->getTimers()->getT2();
		requestTimeout(timerE,"timerE");
		massert(!lastRequest.isNull());
		timerE = sipStackInternal->getTimers()->getT2();
		requestTimeout(timerE,"timerE");
		send( *lastRequest, false);
		
		return true;
	}else{
		
		return false;
	}
}

bool SipTransactionNonInviteClient::a5_proceeding_proceeding_1xx( const SipSMCommand &command){
	
	if (transitionMatch(SipResponse::type, 
			command, 
			SipSMCommand::transport_layer, 
			SipSMCommand::transaction_layer, 
			"1**"))
	{
		MRef<SipResponse*> pack((SipResponse *)*command.getCommandPacket());
		MRef<SipMessage*> pref(*pack);
		SipSMCommand cmd( pref, 
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer);
		cancelTimeout("timerE");

		dispatcher->enqueueCommand( cmd, HIGH_PRIO_QUEUE );
		
		return true;
	}else{
		
		return false;
	}
}

//TODO: make sure  transport_error is sent when it should be
bool SipTransactionNonInviteClient::a6_proceeding_terminated_transperrOrTimerF( const SipSMCommand &command){
		
	if (		transitionMatch(command, 
				SipCommandString::transport_error,
				SipSMCommand::transport_layer,
				SipSMCommand::transaction_layer) 
			|| transitionMatch(command, 
				"timerF",
				SipSMCommand::transaction_layer,
				SipSMCommand::transaction_layer)){

		cancelTimeout("timerE");
		cancelTimeout("timerF");

		SipSMCommand cmd(
				CommandString(callId,SipCommandString::transport_error), 
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer);

		dispatcher->enqueueCommand( cmd, HIGH_PRIO_QUEUE );

		SipSMCommand cmdterminated(
				CommandString( getTransactionId(), SipCommandString::transaction_terminated),
				SipSMCommand::transaction_layer,
				SipSMCommand::dispatcher);
		dispatcher->enqueueCommand( cmdterminated, HIGH_PRIO_QUEUE );

		
		return true;
	}else{
		
		return false;
	}
}


bool SipTransactionNonInviteClient::a7_trying_completed_non1xxresp( const SipSMCommand &command) {
	if (transitionMatch(SipResponse::type, 
			command, 
			SipSMCommand::transport_layer, 
			SipSMCommand::transaction_layer, 
			"2**\n3**\n4**\n5**\n6**"))
	{
		
		cancelTimeout("timerE");
		cancelTimeout("timerF");
		//send command to TU
		SipSMCommand cmd(
				command.getCommandPacket(), 
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer);
		dispatcher->enqueueCommand( cmd, HIGH_PRIO_QUEUE );
		
		if( isUnreliable() ) 
			requestTimeout(sipStackInternal->getTimers()->getK(), "timerK");
		else 
			requestTimeout( 0, "timerK");
			
		
		return true;
	}else{
		
		return false;
	}
}

bool SipTransactionNonInviteClient::a8_trying_trying_timerE( const SipSMCommand &command) {
		
	if (transitionMatch(command, 
				"timerE",
				SipSMCommand::transaction_layer,
				SipSMCommand::transaction_layer)){
		//no need to check if isUnreliable() ... timerE will never be started anyway
		timerE *= 2;
		if( timerE > sipStackInternal->getTimers()->getT2() ) 
			timerE = sipStackInternal->getTimers()->getT2();
		requestTimeout(timerE,"timerE");
		
		massert( !lastRequest.isNull());
		send( *lastRequest, false);
		
		return true;
	}else{
		
		return false;
	}
}

bool SipTransactionNonInviteClient::a9_completed_terminated_timerK( const SipSMCommand &command) {
	if (transitionMatch(command, 
				"timerK",
				SipSMCommand::transaction_layer,
				SipSMCommand::transaction_layer)){
		SipSMCommand cmd(
			CommandString( getTransactionId(), SipCommandString::transaction_terminated),
			SipSMCommand::transaction_layer,
			SipSMCommand::dispatcher);
		dispatcher->enqueueCommand( cmd, HIGH_PRIO_QUEUE );
		
		return true;
	}else{
		
		return false;
	}
}

bool SipTransactionNonInviteClient::a10_completed_completed_anyresp( const SipSMCommand &command) {
	if (transitionMatch(SipResponse::type, 
			command, 
			SipSMCommand::transport_layer, 
			SipSMCommand::transaction_layer, 
			"1**\n2**\n3**\n4**\n5**\n6**"))
	{
		return true;
	}else{
		return false;
	}
}

void SipTransactionNonInviteClient::setUpStateMachine(){
	
	State<SipSMCommand,string> *s_start=new State<SipSMCommand,string>(this,"start");
	addState(s_start);

	State<SipSMCommand,string> *s_trying=new State<SipSMCommand,string>(this,"trying");
	addState(s_trying);

	State<SipSMCommand,string> *s_proceeding=new State<SipSMCommand,string>(this,"proceeding");
	addState(s_proceeding);

	State<SipSMCommand,string> *s_completed=new State<SipSMCommand,string>(this,"completed");
	addState(s_completed);

	State<SipSMCommand,string> *s_terminated=new State<SipSMCommand,string>(this,"terminated");
	addState(s_terminated);

	
	///Set up transitions to enable cancellation of this transaction
	new StateTransition<SipSMCommand,string>(this, "transition_cancel_transaction",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransaction::a1000_anyState_terminated_canceltransaction, 
			StateMachine<SipSMCommand,string>::anyState, s_terminated);

	

	new StateTransition<SipSMCommand,string>(this, "transition_start_trying_request",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionNonInviteClient::a0_start_trying_request, 
			s_start, s_trying);
	
	new StateTransition<SipSMCommand,string>(this, "transition_trying_proceeding_1xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionNonInviteClient::a1_trying_proceeding_1xx, 
			s_trying, s_proceeding);

	new StateTransition<SipSMCommand,string>(this, "transition_trying_terminated_TimerFOrErr",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionNonInviteClient::a2_trying_terminated_TimerFOrErr, 
			s_trying, s_terminated);

	new StateTransition<SipSMCommand,string>(this, "transition_proceeding_completed_non1xxresp",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionNonInviteClient::a3_proceeding_completed_non1xxresp, 
			s_proceeding, s_completed);

	new StateTransition<SipSMCommand,string>(this, "transition_proceeding_proceeding_timerE",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionNonInviteClient::a4_proceeding_proceeding_timerE,
			s_proceeding, s_proceeding);

	new StateTransition<SipSMCommand,string>(this, "transition_proceeding_proceeding_1xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionNonInviteClient::a5_proceeding_proceeding_1xx,
			s_proceeding, s_proceeding);

	new StateTransition<SipSMCommand,string>(this, "transition_proceeding_terminated_transperrOrTimerF",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionNonInviteClient::a6_proceeding_terminated_transperrOrTimerF,
			s_proceeding, s_terminated);

	new StateTransition<SipSMCommand,string>(this, "transition_trying_completed_non1xxresp",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionNonInviteClient::a7_trying_completed_non1xxresp,
			s_trying, s_completed);

	new StateTransition<SipSMCommand,string>(this, "transition_trying_trying_timerE",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionNonInviteClient::a8_trying_trying_timerE,
			s_trying, s_trying);

	new StateTransition<SipSMCommand,string>(this, "transition_completed_terminated_timerK",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionNonInviteClient::a9_completed_terminated_timerK,
			s_completed, s_terminated);

	new StateTransition<SipSMCommand,string>(this, "transition_completed_completed_anyresp",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionNonInviteClient::a10_completed_completed_anyresp,
			s_completed, s_completed);


	setCurrentState(s_start);
}



SipTransactionNonInviteClient::SipTransactionNonInviteClient(
		MRef<SipStackInternal *> stack,
		int seq_no, 
		const string &cseqm, 
		const string &callid) : 
			SipTransactionClient(stack, seq_no, cseqm, "", callid),
			lastRequest(NULL)
{
	
	//timers are set in the initial transition

	setUpStateMachine();
}

SipTransactionNonInviteClient::~SipTransactionNonInviteClient(){
}

