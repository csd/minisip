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
 * 	SipTransactionNonInviteServer.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<libmutil/massert.h>
#include"SipTransactionNonInviteServer.h"
#include<libmsip/SipResponse.h>
#include<libmsip/SipTransitionUtils.h>
#include"../SipCommandDispatcher.h"
#include<libmsip/SipCommandString.h>
#include<libmsip/SipDialog.h>
#include<libmsip/SipDialogConfig.h>
#include<libmsip/SipTimers.h>

using namespace std;

bool SipTransactionNonInviteServer::a0_start_trying_request(
		const SipSMCommand &command)
{
	if (transitionMatch( getCSeqMethod(), 
			command, 
			SipSMCommand::transport_layer, 
			SipSMCommand::transaction_layer))
	{
		MRef<Socket*> sock = command.getCommandPacket()->getSocket();

		if( sock )
			setSocket( *sock );
		else
			setSocket( NULL );

		SipSMCommand cmd(command);
#ifdef DEBUG_OUTPUT
		/*server->*/setDebugTransType(command.getCommandPacket()->getType() );
#endif
		cmd.setSource(SipSMCommand::transaction_layer);
		cmd.setDestination(SipSMCommand::dialog_layer);
		
		dispatcher->enqueueCommand(cmd, HIGH_PRIO_QUEUE);
		
		return true;
	}else{
		
		return false;
	}
}

bool SipTransactionNonInviteServer::a1_trying_proceeding_1xx(
		const SipSMCommand &command)
{
	if (transitionMatch(SipResponse::type, 
			command, 
			SipSMCommand::dialog_layer,
			SipSMCommand::transaction_layer,
			"1**"))
	{
		lastResponse = MRef<SipResponse*>((SipResponse*)*command.getCommandPacket());
		send(command.getCommandPacket(), false); //Do not add via header to responses
							//they are copied from the request
		return true;
	}else{
		
		return false;
	}
}

bool SipTransactionNonInviteServer::a2_trying_completed_non1xxresp(
		const SipSMCommand &command)
{
	if (transitionMatch(SipResponse::type, 
			command, 
			SipSMCommand::dialog_layer, 
			SipSMCommand::transaction_layer, 
			"2**\n3**\n4**\n5**\n6**"))
	{
		
		lastResponse = MRef<SipResponse*>((SipResponse*)*command.getCommandPacket());
		send(command.getCommandPacket(), false); 		//Do not add via header to responses
		requestTimeout(/*64 * timerT1*/ sipStackInternal->getTimers()->getJ(), "timerJ");
		
		return true;
	}else{
		
		return false;
	}
}

bool SipTransactionNonInviteServer::a3_proceeding_completed_non1xxresp(
		const SipSMCommand &command)
{
	if (transitionMatch(SipResponse::type, 
			command, 
			SipSMCommand::dialog_layer, 
			SipSMCommand::transaction_layer, 
			"2**\n3**\n4**\n5**\n6**"))
	{
		lastResponse = MRef<SipResponse*>((SipResponse*)*command.getCommandPacket());
		send(command.getCommandPacket(), false); 		//Do not add via header to responses
		if( isUnreliable() )
			requestTimeout(sipStackInternal->getTimers()->getJ(), "timerJ");
		else 
			requestTimeout( 0, "timerJ");
		
		return true;
	}else{
		
		return false;
	}
}

bool SipTransactionNonInviteServer::a4_proceeding_proceeding_request(
		const SipSMCommand &command)
{
	merr << "CESC: SipTransNIS::a4 ... " << endl;
	if (command.getSource()!=SipSMCommand::transport_layer)
		return false;
	
	if (command.getType()!=SipSMCommand::COMMAND_PACKET){
		return false;
	}
	if (command.getType()==SipSMCommand::COMMAND_PACKET && 
			command.getCommandPacket()->getType()==SipResponse::type){ //NOTICE: if Response, return
		return false;
	}
	
	massert( !lastResponse.isNull());
	//We are re-sending last response, do not add via header	
	send(MRef<SipMessage*>(* lastResponse),false);
	
	
	return true;
}

bool SipTransactionNonInviteServer::a5_proceeding_proceeding_1xx(
		const SipSMCommand &command)
{
	if (transitionMatch(SipResponse::type, 
			command, 
			SipSMCommand::dialog_layer, 
			SipSMCommand::transaction_layer, 
			"1**"))
	{
		MRef<SipResponse*> pack( (SipResponse *)*command.getCommandPacket());
		lastResponse = pack;
		send(MRef<SipMessage*>(*pack), false);
		
		return true;
	}else{
		
		return false;
	}
}

bool SipTransactionNonInviteServer::a6_proceeding_terminated_transperr(
		const SipSMCommand &command){
		
	if (transitionMatch(command, 
				SipCommandString::transport_error,
				SipSMCommand::transport_layer,
				SipSMCommand::transaction_layer)){

		

		//inform TU
		SipSMCommand cmd(
				CommandString(callId,SipCommandString::transport_error), 
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer);

		dispatcher->enqueueCommand( cmd, HIGH_PRIO_QUEUE );
		
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


bool SipTransactionNonInviteServer::a7_completed_completed_request(
		const SipSMCommand &command){
	
	
	if (command.getSource()!=SipSMCommand::transport_layer)
		return false;
	
	if (command.getType()!=SipSMCommand::COMMAND_PACKET){
		return false;
	}
	if (command.getType()==SipSMCommand::COMMAND_PACKET && 
			command.getCommandPacket()->getType()==SipResponse::type){
		return false;
	}
	massert( !lastResponse.isNull());
	send(MRef<SipMessage*>(* lastResponse), false);		//We are re-sending response, do not add via header
	
	return true;
}

bool SipTransactionNonInviteServer::a8_completed_terminated_transperr(
		const SipSMCommand &command){

	if (transitionMatch(command, 
				SipCommandString::transport_error,
				SipSMCommand::transport_layer,
				SipSMCommand::transaction_layer)){

		cancelTimeout("timerJ");

		SipSMCommand cmd(
				CommandString(callId,SipCommandString::transport_error), 
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer);

		dispatcher->enqueueCommand( cmd, HIGH_PRIO_QUEUE);
		
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

bool SipTransactionNonInviteServer::a9_completed_terminated_timerJ(
		const SipSMCommand &command){
	
		
	if (transitionMatch(command, 
				"timerJ",
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

void SipTransactionNonInviteServer::setUpStateMachine(){
	
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

	
	//



	new StateTransition<SipSMCommand,string>(this, "transition_start_trying_request",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionNonInviteServer::a0_start_trying_request, 
			s_start, s_trying);

	new StateTransition<SipSMCommand,string>(this, "transition_trying_proceeding_1xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionNonInviteServer::a1_trying_proceeding_1xx, 
			s_trying, s_proceeding);

	new StateTransition<SipSMCommand,string>(this, "transition_trying_completed_non1xxresp",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionNonInviteServer::a2_trying_completed_non1xxresp, 
			s_trying, s_completed);

	new StateTransition<SipSMCommand,string>(this, "transition_proceeding_completed_non1xxresp",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionNonInviteServer::a3_proceeding_completed_non1xxresp, 
			s_proceeding, s_completed);

	new StateTransition<SipSMCommand,string>(this, "transition_proceeding_proceeding_request",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionNonInviteServer::a4_proceeding_proceeding_request,
			s_proceeding, s_proceeding);

	new StateTransition<SipSMCommand,string>(this, "transition_proceeding_proceeding_1xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionNonInviteServer::a5_proceeding_proceeding_1xx,
			s_proceeding, s_proceeding);

	new StateTransition<SipSMCommand,string>(this, "transition_proceeding_terminated_transperr",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionNonInviteServer::a6_proceeding_terminated_transperr,
			s_proceeding, s_terminated);

	new StateTransition<SipSMCommand,string>(this, "transition_completed_completed_request",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionNonInviteServer::a7_completed_completed_request,
			s_completed, s_completed);

	new StateTransition<SipSMCommand,string>(this, "transition_completed_terminated_transperr",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionNonInviteServer::a8_completed_terminated_transperr,
			s_completed, s_terminated);

	new StateTransition<SipSMCommand,string>(this, "transition_completed_terminated_timerJ",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionNonInviteServer::a9_completed_terminated_timerJ,
			s_completed, s_terminated);

	setCurrentState(s_start);
}

SipTransactionNonInviteServer::SipTransactionNonInviteServer(MRef<SipStackInternal*> stack, 
		int seq_no, 
		const string &cseqm, 
		const string &branch_,
		const string &callid) : 
			SipTransactionServer(stack, seq_no, cseqm, branch_, callid),
			lastResponse(NULL)
{
	setUpStateMachine();
}

SipTransactionNonInviteServer::~SipTransactionNonInviteServer(){
}

