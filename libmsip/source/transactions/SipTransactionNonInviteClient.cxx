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

#include<libmutil/massert.h>
#include<libmsip/SipTransactionNonInviteClient.h>
#include<libmsip/SipCommandDispatcher.h>
#include<libmsip/SipResponse.h>
#include<libmsip/SipTransactionUtils.h>
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
			timerE = sipStack->getTimers()->getE();
			requestTimeout(timerE, "timerE");
		}
		requestTimeout(sipStack->getTimers()->getF(), "timerF");

/* TODO/FIXME: This is a bit tricky - we don't know which identity is
 * currently being used (the getConfig() one will have a null one if
 * not within a dialog). Maybe the creator of the transaction should
 * explicitely add a route set instead of implicitely specifying it always?
 * -EE
 
		massert(getConfig());
		massert(getConfig()->sipIdentity);

		MRef<SipProxy*> proxy = getConfig()->sipIdentity->getSipProxy();
		
		if( proxy->sipProxyAddressString.size()>0){
			MRef<SipHeaderValue*> hdr;

			hdr = lastRequest->getHeaderValueNo( SIP_HEADER_TYPE_ROUTE, 0 );

			if( !hdr ){
				lastRequest->addRoute( proxy->sipProxyAddressString,
						       proxy->sipProxyPort, 
						       proxy->getTransport() );
			}
		}
*/
		send(*lastRequest,true);
		
		return true;
	}else{
		return false;
	}
}

bool SipTransactionNonInviteClient::a1_trying_proceeding_1xx( const SipSMCommand &command){
	
	if (transitionMatch(SipResponse::type, command, SipSMCommand::transport_layer,/*IGN*/ SipSMCommand::transaction_layer, "1**")){
		cancelTimeout("timerE");
		cancelTimeout("timerF");
		SipSMCommand cmd(
				command.getCommandPacket(), 
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer );
		dispatcher->enqueueCommand(cmd, HIGH_PRIO_QUEUE/*, PRIO_LAST_IN_QUEUE*/);
		
		return true;
	}else{
		
		return false;
	}
}

bool SipTransactionNonInviteClient::a2_trying_terminated_TimerFOrErr( const SipSMCommand &command){
	
	if (transitionMatch(command, SipCommandString::transport_error, SipSMCommand::transport_layer, SipSMCommand::transaction_layer) 
			|| transitionMatch(command, "timerF", SipSMCommand::transaction_layer, SipSMCommand::transaction_layer)){
		cancelTimeout("timerE");
		cancelTimeout("timerF");
		
		SipSMCommand cmd(
				CommandString( callId, SipCommandString::transport_error),
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer);
		dispatcher->enqueueCommand(cmd, HIGH_PRIO_QUEUE/*, PRIO_LAST_IN_QUEUE*/);

		SipSMCommand cmdterminated(
			CommandString( callId, SipCommandString::transaction_terminated),
			SipSMCommand::transaction_layer,
			SipSMCommand::dispatcher);
		dispatcher->enqueueCommand( cmdterminated, HIGH_PRIO_QUEUE/*, PRIO_FIRST_IN_QUEUE */);
		
		
		return true;
	}else{
		
		return false;
	}
}

bool SipTransactionNonInviteClient::a3_proceeding_completed_non1xxresp( const SipSMCommand &command){

	if (transitionMatch(SipResponse::type, command, SipSMCommand::transport_layer, SipSMCommand::transaction_layer, "2**\n3**\n4**\n5**\n6**")){
		
		MRef<SipResponse*> pack((SipResponse *)*command.getCommandPacket());
		cancelTimeout("timerE"); //no more retx of the request
		if( isUnreliable() ) //response re-tx timer
			requestTimeout(sipStack->getTimers()->getT4(),"timerK");
		else
			requestTimeout(0,"timerK");
		
		//forward to TU
		MRef<SipMessage*> pref(*pack);
		SipSMCommand cmd(pref, 
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer);
		dispatcher->enqueueCommand(cmd, HIGH_PRIO_QUEUE/*, PRIO_LAST_IN_QUEUE*/);

		
		return true;
	}else{
		
		return false;
	}
}

bool SipTransactionNonInviteClient::a4_proceeding_proceeding_timerE( const SipSMCommand &command){
	
	if (transitionMatch(command, "timerE",SipSMCommand::transaction_layer,SipSMCommand::transaction_layer)){
		timerE *= 2;
		if( timerE > sipStack->getTimers()->getT2() ) 
			timerE = sipStack->getTimers()->getT2();
		requestTimeout(timerE,"timerE");
		massert(!lastRequest.isNull());
		timerE = sipStack->getTimers()->getT2();
		requestTimeout(timerE,"timerE");
		lastRequest->removeAllViaHeaders();
		send( *lastRequest, true);	//add via, we have removed all from previous request
		
		return true;
	}else{
		
		return false;
	}
}

bool SipTransactionNonInviteClient::a5_proceeding_proceeding_1xx( const SipSMCommand &command){
	
	if (transitionMatch(SipResponse::type, command, SipSMCommand::transport_layer, SipSMCommand::transaction_layer, "1**")){
		MRef<SipResponse*> pack((SipResponse *)*command.getCommandPacket());
		MRef<SipMessage*> pref(*pack);
		SipSMCommand cmd( pref, 
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer);
		cancelTimeout("timerE");

		dispatcher->enqueueCommand( cmd, HIGH_PRIO_QUEUE/*, PRIO_LAST_IN_QUEUE*/ );
		
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

		SipSMCommand cmd(
				CommandString(callId,SipCommandString::transport_error), 
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer);

		dispatcher->enqueueCommand( cmd, HIGH_PRIO_QUEUE/*, PRIO_LAST_IN_QUEUE*/ );

		SipSMCommand cmdterminated(
				CommandString( callId, SipCommandString::transaction_terminated),
				SipSMCommand::transaction_layer,
				SipSMCommand::dispatcher);
		dispatcher->enqueueCommand( cmdterminated, HIGH_PRIO_QUEUE/*, PRIO_FIRST_IN_QUEUE*/ );

		
		return true;
	}else{
		
		return false;
	}
}


bool SipTransactionNonInviteClient::a7_trying_completed_non1xxresp( const SipSMCommand &command) {
	if (transitionMatch(SipResponse::type, command, SipSMCommand::transport_layer, SipSMCommand::transaction_layer, "2**\n3**\n4**\n5**\n6**")){
		
		cancelTimeout("timerE");
		cancelTimeout("timerF");
		//send command to TU
		SipSMCommand cmd(
				command.getCommandPacket(), 
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer);
		dispatcher->enqueueCommand( cmd, HIGH_PRIO_QUEUE/*, PRIO_LAST_IN_QUEUE*/ );
		
		if( isUnreliable() ) 
			requestTimeout(sipStack->getTimers()->getK(), "timerK");
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
		if( timerE > sipStack->getTimers()->getT2() ) 
			timerE = sipStack->getTimers()->getT2();
		requestTimeout(timerE,"timerE");
		
		massert( !lastRequest.isNull());
		lastRequest->removeAllViaHeaders();
		send( *lastRequest, true);	// add via header because we have removed all previous ones
		
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
			CommandString( callId, SipCommandString::transaction_terminated),
			SipSMCommand::transaction_layer,
			SipSMCommand::dispatcher);
		dispatcher->enqueueCommand( cmd, HIGH_PRIO_QUEUE/*, PRIO_FIRST_IN_QUEUE*/ );
		
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

	setCurrentState(s_start);
}



SipTransactionNonInviteClient::SipTransactionNonInviteClient(
		MRef<SipStack *> stack,
		//MRef<SipDialog*> d, 
		int seq_no, 
		const string &cSeqMethod, 
		const string &callid) : 
			SipTransactionClient(stack, /*d,*/ seq_no, cSeqMethod, "", callid),
			lastRequest(NULL)
{
	
	//timers are set in the initial transition

//	MRef<SipCommonConfig *> conf = getConfig(); 
//	if (dialog){
//		conf = dialog->getDialogConfig()->inherited;
//	}else{
//		conf = sipStack->getStackConfig();
//	}

//	if( !conf ) { cerr << "SipTranNonInvCli: conf null" << endl; massert (0);}
//	if( !conf->sipIdentity ) { cerr << "SipTranNonInvCli: identity null" << endl; massert (0);}
//	if( conf->sipIdentity->getSipProxy()->sipProxyAddressString.size()<=0 ) { cerr << "SipTranNonInvCli: no ipaddr" << endl; massert (0);}
//	toaddr = conf->sipIdentity->getSipProxy()->sipProxyIpAddr->clone();
	//port = conf->sipIdentity->getSipProxy()->sipProxyPort;
	setUpStateMachine();
}

SipTransactionNonInviteClient::~SipTransactionNonInviteClient(){
}

