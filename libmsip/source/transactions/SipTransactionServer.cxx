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
 * 	SipTransactionServer.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<assert.h>
#include<libmsip/SipDialog.h>
#include<libmsip/SipTransactionServer.h>
#include<libmsip/SipResponse.h>
#include<libmsip/SipTransactionUtils.h>
#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipCommandString.h>

bool SipTransactionServer::a0_start_trying_request(
		const SipSMCommand &command)
{
	if (transitionMatch(command, IGN, SipSMCommand::remote, IGN)){
		SipSMCommand cmd(command);
#ifdef DEBUG_OUTPUT
		/*server->*/setDebugTransType(command.getCommandPacket()->getTypeString() );
#endif
		cmd.setSource(SipSMCommand::transaction);
		cmd.setDestination(SipSMCommand::TU);
		
		dialog->getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
		return true;
	}else{
		return false;
	}
}

bool SipTransactionServer::a1_trying_proceeding_1xx(
		const SipSMCommand &command)
{
	if (transitionMatch(command, SipResponse::type, SipSMCommand::TU, IGN, "1**")){
		lastResponse = MRef<SipResponse*>((SipResponse*)*command.getCommandPacket());
		send(command.getCommandPacket());
		return true;
	}else{
		return false;
	}
}

bool SipTransactionServer::a2_trying_completed_non1xxresp(
		const SipSMCommand &command)
{
	if (transitionMatch(command, SipResponse::type, SipSMCommand::TU, IGN, "2**\n3**\n4**\n5**\n6**")){
		
		lastResponse = MRef<SipResponse*>((SipResponse*)*command.getCommandPacket());
		send(command.getCommandPacket());
		requestTimeout(64 * timerT1, "timerJ");
		return true;
	}else{
		return false;
	}
}

bool SipTransactionServer::a3_proceeding_completed_non1xxresp(
		const SipSMCommand &command)
{
	if (transitionMatch(command, SipResponse::type, SipSMCommand::TU, IGN, "2**\n3**\n4**\n5**\n6**")){
		lastResponse = MRef<SipResponse*>((SipResponse*)*command.getCommandPacket());
		send(command.getCommandPacket());
		requestTimeout(64 * timerT1, "timerJ");
		return true;
	}else{
		return false;
	}
}

bool SipTransactionServer::a4_proceeding_proceeding_request(
		const SipSMCommand &command)
{
	if (command.getSource()!=SipSMCommand::remote)
		return false;
	
	if (command.getType()!=SipSMCommand::COMMAND_PACKET){
		return false;
	}
	if (command.getType()==SipSMCommand::COMMAND_PACKET && 
			command.getCommandPacket()->getType()==SipResponse::type){ //NOTICE: if Response, return
		return false;
	}
	
	assert( !lastResponse.isNull());
	send(MRef<SipMessage*>(* lastResponse));
	
	return true;
}

bool SipTransactionServer::a5_proceeding_proceeding_1xx(
		const SipSMCommand &command)
{
	if (transitionMatch(command, SipResponse::type, SipSMCommand::TU, IGN, "1**")){
		MRef<SipResponse*> pack( (SipResponse *)*command.getCommandPacket());
		lastResponse = pack;
		send(MRef<SipMessage*>(*pack));
		return true;
	}else
		return false;
}

bool SipTransactionServer::a6_proceeding_terminated_transperr(
		const SipSMCommand &command){
		
	if (transitionMatch(command, SipCommandString::transport_error)){
		//inform TU
                SipSMCommand cmd(
                        CommandString(callId,SipCommandString::transport_error), 
                        SipSMCommand::transaction, 
                        SipSMCommand::TU);

		dialog->getDialogContainer()->enqueueCommand( cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE );
		
		SipSMCommand cmdterminated(
			CommandString( callId, SipCommandString::transaction_terminated),
			SipSMCommand::transaction,
			SipSMCommand::TU);
		dialog->getDialogContainer()->enqueueCommand( cmdterminated, HIGH_PRIO_QUEUE, PRIO_FIRST_IN_QUEUE);
		
		return true;
	}else{
		return false;
	}
}


bool SipTransactionServer::a7_completed_completed_request(
		const SipSMCommand &command){
	
	if (command.getSource()!=SipSMCommand::remote)
		return false;
	
	if (command.getType()!=SipSMCommand::COMMAND_PACKET){
		return false;
	}
	if (command.getType()==SipSMCommand::COMMAND_PACKET && 
			command.getCommandPacket()->getType()==SipResponse::type){
		return false;
	}
	assert( !lastResponse.isNull());
	send(MRef<SipMessage*>(* lastResponse));
	return true;
}

bool SipTransactionServer::a8_completed_terminated_transperr(
		const SipSMCommand &command){

	if (transitionMatch(command, SipCommandString::transport_error)){
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

bool SipTransactionServer::a9_completed_terminated_timerJ(
		const SipSMCommand &command){
	
		
	if (transitionMatch(command, "timerJ")){
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

void SipTransactionServer::setUpStateMachine(){
	
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

	new StateTransition<SipSMCommand,string>(this, "transition_start_trying_request",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionServer::a0_start_trying_request, 
			s_start, s_trying);

	new StateTransition<SipSMCommand,string>(this, "transition_trying_proceeding_1xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionServer::a1_trying_proceeding_1xx, 
			s_trying, s_proceeding);

	new StateTransition<SipSMCommand,string>(this, "transition_trying_completed_non1xxresp",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionServer::a2_trying_completed_non1xxresp, 
			s_trying, s_completed);

	new StateTransition<SipSMCommand,string>(this, "transition_proceeding_completed_non1xxresp",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionServer::a3_proceeding_completed_non1xxresp, 
			s_proceeding, s_completed);

	new StateTransition<SipSMCommand,string>(this, "transition_proceeding_proceeding_request",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionServer::a4_proceeding_proceeding_request,
			s_proceeding, s_proceeding);

	new StateTransition<SipSMCommand,string>(this, "transition_proceeding_proceeding_1xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionServer::a5_proceeding_proceeding_1xx,
			s_proceeding, s_proceeding);

	new StateTransition<SipSMCommand,string>(this, "transition_proceeding_terminated_transperr",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionServer::a6_proceeding_terminated_transperr,
			s_proceeding, s_terminated);

	new StateTransition<SipSMCommand,string>(this, "transition_completed_completed_request",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionServer::a7_completed_completed_request,
			s_completed, s_completed);

	new StateTransition<SipSMCommand,string>(this, "transition_completed_terminated_transperr",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionServer::a8_completed_terminated_transperr,
			s_completed, s_terminated);

	new StateTransition<SipSMCommand,string>(this, "transition_completed_terminated_timerJ",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionServer::a9_completed_terminated_timerJ,
			s_completed, s_terminated);

	setCurrentState(s_start);
}

SipTransactionServer::SipTransactionServer(MRef<SipDialog*> call, int seq_no, const string &branch,string callid) : 
		SipTransaction("SipTransactionServer",call, seq_no, branch, callid),
		lastResponse(NULL),
		timerT1(500)
{
	dialog->getDialogConfig()->local_called=false;
	
	toaddr = dialog->getDialogConfig()->inherited.sipIdentity->sipProxy.sipProxyIpAddr;
	port = dialog->getDialogConfig()->inherited.sipIdentity->sipProxy.sipProxyPort;
	
	setUpStateMachine();
}

SipTransactionServer::~SipTransactionServer(){
}

