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
 * 	SipTransactionClient.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<assert.h>
#include<libmsip/SipTransactionClient.h>
#include<libmsip/SipResponse.h>
#include<libmsip/SipTransactionUtils.h>
#include<libmsip/SipDialog.h>
#include<libmsip/SipCommandString.h>

#ifdef DEBUG_OUTPUT
#include<libmutil/dbg.h>
#endif

bool SipTransactionClient::a0_start_trying_request( const SipSMCommand &command)
{
	if (transitionMatch(command, IGN, SipSMCommand::TU,IGN)){
#ifdef DEBUG_OUTPUT
		setDebugTransType(command.getCommandPacket()->getTypeString() );
#endif
		lastRequest = command.getCommandPacket();
		requestTimeout(500, "timerE");
		requestTimeout(64000, "timerF");
		send(command.getCommandPacket());
		return true;
	}else{
		return false;
	}
}

bool SipTransactionClient::a1_trying_proceeding_1xx( const SipSMCommand &command){
	
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

bool SipTransactionClient::a2_trying_terminated_TimerFOrErr( const SipSMCommand &command){
	
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

bool SipTransactionClient::a3_proceeding_completed_non1xxresp( const SipSMCommand &command){

	if (transitionMatch(command, SipResponse::type, SipSMCommand::remote, IGN, "2**\n3**\n4**\n5**\n6**")){
		
		MRef<SipResponse*> pack((SipResponse *)*command.getCommandPacket());
                requestTimeout(60000,"timerK");
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

bool SipTransactionClient::a4_proceeding_proceeding_timerE( const SipSMCommand &command){
	
	if (transitionMatch(command, "timerE")){
		assert(!lastRequest.isNull());
		lastRequest->removeAllViaHeaders();
		send( lastRequest);	
		return true;
	}else{
		return false;
	}
}

bool SipTransactionClient::a5_proceeding_proceeding_1xx( const SipSMCommand &command){
	
	if (transitionMatch(command, SipResponse::type, SipSMCommand::remote, IGN, "1**")){
		MRef<SipResponse*> pack((SipResponse *)*command.getCommandPacket());
                MRef<SipMessage*> pref(*pack);
                SipSMCommand cmd( pref, 
                        SipSMCommand::transaction, 
                        SipSMCommand::TU);

		dialog->getDialogContainer()->enqueueCommand( cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE );
		return true;
	}else
		return false;
}

//TODO: make sure  transport_error is sent when it should be
bool SipTransactionClient::a6_proceeding_terminated_transperrOrTimerF( const SipSMCommand &command){
		
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


bool SipTransactionClient::a7_trying_completed_non1xxresp( const SipSMCommand &command)
{
	if (transitionMatch(command, SipResponse::type, SipSMCommand::remote, IGN, "2**\n3**\n4**\n5**\n6**")){
		
		cancelTimeout("timerE");
		cancelTimeout("timerF");
                SipSMCommand cmd(
                        command.getCommandPacket(), 
                        SipSMCommand::transaction, 
                        SipSMCommand::TU);

		dialog->getDialogContainer()->enqueueCommand( cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE );
		
		requestTimeout(5000, "timerK");
		return true;
	}else{
		return false;
	}
}

bool SipTransactionClient::a8_trying_trying_timerE( const SipSMCommand &command)
{
		
	if (transitionMatch(command, "timerE")){
		requestTimeout(1000,"timerE");
		assert( !lastRequest.isNull());
		lastRequest->removeAllViaHeaders();
		send( lastRequest);	
		return true;
	}else{
		return false;
	}
}

bool SipTransactionClient::a9_completed_terminated_timerK( const SipSMCommand &command)
{
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

void SipTransactionClient::setUpStateMachine(){
	
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


	StateTransition<SipSMCommand,string> *transition_start_trying_request=
		new StateTransition<SipSMCommand,string>(this,
				"transition_start_trying_request",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionClient::a0_start_trying_request, 
				s_start, s_trying);
	
	StateTransition<SipSMCommand,string> *transition_trying_proceeding_1xx=
		new StateTransition<SipSMCommand,string>(this,
				"transition_trying_proceeding_1xx",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionClient::a1_trying_proceeding_1xx, 
				s_trying, s_proceeding
				);

	StateTransition<SipSMCommand,string> *transition_trying_terminated=
		new StateTransition<SipSMCommand,string>(this,
				"transition_trying_terminated_TimerFOrErr",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionClient::a2_trying_terminated_TimerFOrErr, 
				s_trying, s_terminated
				);

	StateTransition<SipSMCommand,string> *transition_proceeding_completed_resp=
		new StateTransition<SipSMCommand,string>(this,
				"transition_proceeding_completed_non1xxresp",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionClient::a3_proceeding_completed_non1xxresp, 
				s_proceeding, s_completed
				);

	StateTransition<SipSMCommand,string> *transition_proceeding_proceeding_timerE=
		new StateTransition<SipSMCommand,string>(this,
				"transition_proceeding_proceeding_timerE",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionClient::a4_proceeding_proceeding_timerE,
				s_proceeding, s_proceeding
				);
		
	StateTransition<SipSMCommand,string> *transition_proceeding_proceeding_1xx=
		new StateTransition<SipSMCommand,string>(this,
				"transition_proceeding_proceeding_1xx",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionClient::a5_proceeding_proceeding_1xx,
				s_proceeding, s_proceeding
				);

	StateTransition<SipSMCommand,string> *transition_proceeding_terminated_transperrOrTimerF=
		new StateTransition<SipSMCommand,string>(this,
				"transition_proceeding_terminated_transperrOrTimerF",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionClient::a6_proceeding_terminated_transperrOrTimerF,
				s_proceeding, s_terminated
				);
		
	StateTransition<SipSMCommand,string> *transition_trying_completed_non1xxresp=
		new StateTransition<SipSMCommand,string>(this,
				"transition_trying_completed_non1xxresp",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionClient::a7_trying_completed_non1xxresp,
				s_trying, s_completed
				);

	StateTransition<SipSMCommand,string> *transition_trying_trying_timerE=
		new StateTransition<SipSMCommand,string>(this,
				"transition_trying_trying_timerE",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionClient::a8_trying_trying_timerE,
				s_trying, s_trying
				);

	StateTransition<SipSMCommand,string> *transition_completed_terminated_timerK=
		new StateTransition<SipSMCommand,string>(this,
				"transition_completed_terminated_timerK",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionClient::a9_completed_terminated_timerK,
				s_completed, s_terminated
				);
		
	setCurrentState(s_start);
}



SipTransactionClient::SipTransactionClient(
		MRef<SipDialog*> d, 
		int seq_no, string callid) : 
			SipTransaction("SipTransactionClient",d, "", callid),
			lastRequest(NULL),
			timerT1(500),
			command_seq_no(seq_no)
{
	toaddr = dialog->getDialogConfig().inherited.sipIdentity->sipProxy.sipProxyIpAddr;
	port = dialog->getDialogConfig().inherited.sipIdentity->sipProxy.sipProxyPort;
	setUpStateMachine();
}

SipTransactionClient::~SipTransactionClient(){
}

bool SipTransactionClient::handleCommand(const SipSMCommand &c){

	if (! (c.getDestination()==SipSMCommand::transaction || c.getDestination()==SipSMCommand::ANY))
		return false;
	
	if (c.getType()==SipSMCommand::COMMAND_PACKET && 
			c.getCommandPacket()->getCSeq()!= command_seq_no){
		return false;
	}
	return StateMachine<SipSMCommand,string>::handleCommand(c);
}

