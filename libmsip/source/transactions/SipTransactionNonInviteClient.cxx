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

#include<config.h>

#include<assert.h>
#include<libmsip/SipTransactionNonInviteClient.h>
#include<libmsip/SipResponse.h>
#include<libmsip/SipTransactionUtils.h>
#include<libmsip/SipCommandString.h>
#include<libmsip/SipDialog.h>
#include<libmsip/SipDialogConfig.h>

#ifdef DEBUG_OUTPUT
#include<libmutil/dbg.h>
#endif

bool SipTransactionNonInviteClient::a0_start_trying_request( const SipSMCommand &command) {
	if (transitionMatch(command, IGN, SipSMCommand::TU,IGN)){
#ifdef DEBUG_OUTPUT
		setDebugTransType(command.getCommandPacket()->getTypeString() );
#endif
		lastRequest = command.getCommandPacket();
		requestTimeout(timerE_ms, "timerE");
		requestTimeout(sipStack->getTimers()->getF(), "timerF");
		send(command.getCommandPacket(),true);
		return true;
	}else{
		return false;
	}
}

bool SipTransactionNonInviteClient::a1_trying_proceeding_1xx( const SipSMCommand &command){
	
	if (transitionMatch(command, SipResponse::type, SipSMCommand::remote, IGN, "1**")){
		cancelTimeout("timerE");
		cancelTimeout("timerF");
                SipSMCommand cmd(
                        command.getCommandPacket(), 
                        SipSMCommand::transaction, 
                        SipSMCommand::TU );

		dialog->getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
		return true;
	}else{
		return false;
	}
}

bool SipTransactionNonInviteClient::a2_trying_terminated_TimerFOrErr( const SipSMCommand &command){
	
	if (transitionMatch(command, SipCommandString::transport_error) 
			|| transitionMatch(command, "timerF")){
		cancelTimeout("timerE");
		cancelTimeout("timerF");
		
                SipSMCommand cmd(
                        CommandString( callId, SipCommandString::transport_error),
                        SipSMCommand::transaction, 
                        SipSMCommand::TU);
		dialog->getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);

		SipSMCommand cmdterminated(
			CommandString( callId, SipCommandString::transaction_terminated),
			SipSMCommand::transaction,
			SipSMCommand::TU);
		dialog->getDialogContainer()->enqueueCommand( cmdterminated, HIGH_PRIO_QUEUE, PRIO_FIRST_IN_QUEUE );
		
		return true;
	}else{
		return false;
	}
}

bool SipTransactionNonInviteClient::a3_proceeding_completed_non1xxresp( const SipSMCommand &command){

	if (transitionMatch(command, SipResponse::type, SipSMCommand::remote, IGN, "2**\n3**\n4**\n5**\n6**")){
		
		MRef<SipResponse*> pack((SipResponse *)*command.getCommandPacket());
		cancelTimeout("timerE");
                requestTimeout(sipStack->getTimers()->getK(), "timerK");
                MRef<SipMessage*> pref(*pack);
		
                SipSMCommand cmd(pref, 
                        SipSMCommand::transaction, 
                        SipSMCommand::TU);
		dialog->getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);

		return true;
	}else{
		return false;
	}
}

bool SipTransactionNonInviteClient::a4_proceeding_proceeding_timerE( const SipSMCommand &command){
	
	if (transitionMatch(command, "timerE")){
		timerE_ms *=2;
		if (timerE_ms>4000){
			timerE_ms=4000;
		}
		requestTimeout(timerE_ms,"timerE");
		assert(!lastRequest.isNull());
		lastRequest->removeAllViaHeaders();
		send( lastRequest,false);	//do not add via header when re-sending.	
		return true;
	}else{
		return false;
	}
}

bool SipTransactionNonInviteClient::a5_proceeding_proceeding_1xx( const SipSMCommand &command){
	
	if (transitionMatch(command, SipResponse::type, SipSMCommand::remote, IGN, "1**")){
		MRef<SipResponse*> pack((SipResponse *)*command.getCommandPacket());
                MRef<SipMessage*> pref(*pack);
                SipSMCommand cmd( pref, 
                        SipSMCommand::transaction, 
                        SipSMCommand::TU);
		cancelTimeout("timerE");

		dialog->getDialogContainer()->enqueueCommand( cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE );
		return true;
	}else
		return false;
}

//TODO: make sure  transport_error is sent when it should be
bool SipTransactionNonInviteClient::a6_proceeding_terminated_transperrOrTimerF( const SipSMCommand &command){
		
	if (transitionMatch(command, SipCommandString::transport_error) || transitionMatch(command, "timerF")){

		SipSMCommand cmd(
				CommandString(callId,SipCommandString::transport_error), 
				SipSMCommand::transaction, 
				SipSMCommand::TU);

		dialog->getDialogContainer()->enqueueCommand( cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE );

		SipSMCommand cmdterminated(
				CommandString( callId, SipCommandString::transaction_terminated),
				SipSMCommand::transaction,
				SipSMCommand::TU);
		dialog->getDialogContainer()->enqueueCommand( cmdterminated, HIGH_PRIO_QUEUE, PRIO_FIRST_IN_QUEUE );

		return true;
	}else{
		return false;
	}
}


bool SipTransactionNonInviteClient::a7_trying_completed_non1xxresp( const SipSMCommand &command) {
	if (transitionMatch(command, SipResponse::type, SipSMCommand::remote, IGN, "2**\n3**\n4**\n5**\n6**")){
		
		cancelTimeout("timerE");
		cancelTimeout("timerF");
                SipSMCommand cmd(
                        command.getCommandPacket(), 
                        SipSMCommand::transaction, 
                        SipSMCommand::TU);

		dialog->getDialogContainer()->enqueueCommand( cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE );
		
		requestTimeout(sipStack->getTimers()->getK(), "timerK");
		return true;
	}else{
		return false;
	}
}

bool SipTransactionNonInviteClient::a8_trying_trying_timerE( const SipSMCommand &command) {
		
	if (transitionMatch(command, "timerE")){
		timerE_ms *= 2;
		if (timerE_ms>4000){
			timerE_ms=4000;
		}
		
		requestTimeout(timerE_ms, "timerE");
		
		assert( !lastRequest.isNull());
		lastRequest->removeAllViaHeaders();
		send( lastRequest, false);		// do not add via header when re-sending 
		return true;
	}else{
		return false;
	}
}

bool SipTransactionNonInviteClient::a9_completed_terminated_timerK( const SipSMCommand &command) {
	if (transitionMatch(command, "timerK")){
		SipSMCommand cmd(
			CommandString( callId, SipCommandString::transaction_terminated),
			SipSMCommand::transaction,
			SipSMCommand::TU);
		dialog->getDialogContainer()->enqueueCommand( cmd, HIGH_PRIO_QUEUE, PRIO_FIRST_IN_QUEUE );
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
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransaction::a1000_cancel_transaction, 
			s_trying, s_terminated);
	new StateTransition<SipSMCommand,string>(this, "transition_cancel_transaction",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransaction::a1000_cancel_transaction, 
			s_proceeding, s_terminated);
	new StateTransition<SipSMCommand,string>(this, "transition_cancel_transaction",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransaction::a1000_cancel_transaction, 
			s_completed, s_terminated);
	//



	///Set up transitions to enable cancellation of this transaction
	new StateTransition<SipSMCommand,string>(this, "transition_cancel_transaction",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransaction::a1000_cancel_transaction, 
			s_start, s_terminated);
	new StateTransition<SipSMCommand,string>(this, "transition_cancel_transaction",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransaction::a1000_cancel_transaction, 
			s_trying, s_terminated);
	new StateTransition<SipSMCommand,string>(this, "transition_cancel_transaction",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransaction::a1000_cancel_transaction, 
			s_proceeding, s_terminated);
	new StateTransition<SipSMCommand,string>(this, "transition_cancel_transaction",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransaction::a1000_cancel_transaction, 
			s_completed, s_terminated);
	//

		

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

	setCurrentState(s_start);
}



SipTransactionNonInviteClient::SipTransactionNonInviteClient(
		MRef<SipStack *> stack,
		MRef<SipDialog*> d, 
		int seq_no, string callid) : 
			SipTransactionClient(stack, d, seq_no, "", callid),
			lastRequest(NULL)
{
	timerE_ms = stack->getTimers()->getE();

	MRef<SipCommonConfig *> conf;
	if (dialog){
		conf = dialog->getDialogConfig()->inherited;
	}else{
		conf = sipStack->getStackConfig();
	}

	toaddr = conf->sipIdentity->sipProxy.sipProxyIpAddr;
	port = conf->sipIdentity->sipProxy.sipProxyPort;
	setUpStateMachine();
}

SipTransactionNonInviteClient::~SipTransactionNonInviteClient(){
}

