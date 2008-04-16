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
 * 	SipTransactionIntiveResponder.cxx
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
                |        | Proceeding|        |a2:send response (if 100rel, set TimerRel1xx)
                +------->|           |<-------+
 a20:TimerRel1xx+--------|           |          Transport Err.
 resendRel1xx   |        |           |          a4:Inform TU
                +------->|           |--------------->+
                         +-----------+                |
            300-699 from TU |     |2xx from TU        |
         a3:send response   |     |a5:send response   |
                            |     +------------------>+
                            |                         |
            INVITE          V          Timer G fires  |
         a6:send response+-----------+a8:send response|
                +--------|           |--------+       |
                |        | Completed |        |       |
                +------->|           |<-------+       |
                         +-----------+                |
                            |     |                   |
                        ACK |     |                   |
                    a7: -   |     +------------------>+
                            |        Timer H fires    |
                            V        or Transport Err.|
                         +-----------+ a9:Inform TU   |
                    +----|           |                |
          a11:ACK   |    | Confirmed |                |
              -     +--->|           |                |
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

#include"SipTransactionInviteServer.h"
#include"SipTransactionNonInviteServer.h"
#include<libmsip/SipResponse.h>
#include<libmsip/SipTransitionUtils.h>
#include"../SipCommandDispatcher.h"
#include<libmsip/SipCommandString.h>
#include<libmsip/SipDialog.h>
#include<libmsip/SipDialogConfig.h>

#ifdef DEBUG_OUTPUT
#include<libmutil/termmanip.h>
#endif

using namespace std;

/**
 * The first command a INVITE server transaction receives is an INVITE
 * packet. It forwards the packet to the TU/call. We send no 1xx since we
 * expect a response from the TU in less than 200 ms.
 */
bool SipTransactionInviteServer::a0_start_proceeding_INVITE( const SipSMCommand &command){
	
	if (transitionMatch("INVITE", 
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
		cmd.setSource(SipSMCommand::transaction_layer);
		cmd.setDestination(SipSMCommand::dialog_layer);
//		cmd.setSource(SipSMCommand::transaction);
		dispatcher->enqueueCommand(cmd, HIGH_PRIO_QUEUE);
		
		//update dialogs route set ... needed to add route headers to the ACK we are going to send
		
		// TODO/XXX/FIXME: implement this in the TU instead!!! --EE
		//setDialogRouteSet( (SipRequest*)*command.getCommandPacket() );
		
		return true;
	}else{
		return false;
	}
}

/**
 * If we receive the INVITE packet _again_, our response was probably lost.
 * In that case we retransmit it. 
 * Note: We expected that the TU/Call would answer before the remote side
 * retransmitted. If this failed, and if DEBUG_OUTPUT then we display an
 * error message. 
 * TODO: Implement an option to send 100 trying in action "a0".
 */
bool SipTransactionInviteServer::a1_proceeding_proceeding_INVITE( const SipSMCommand &command){
	
	if (transitionMatch("INVITE", 
			command, 
			SipSMCommand::transport_layer, 
			SipSMCommand::transaction_layer))
	{
		MRef<SipResponse*> resp = lastResponse;
		if (resp.isNull()){
#ifdef DEBUG_OUTPUT
			merr << FG_ERROR << "Invite server transaction failed to deliver response before remote side retransmitted. Bug?"<< PLAIN << endl;
#endif
		}else{
			send(MRef<SipMessage*>(*resp), false);
		}
		return true;
	}else{
		return false;
	}
}

/**
 * If a "1xx" response is received from the TU(/call), send it to the
 * remote side and save it in case we need to retransmit it.
 */
bool SipTransactionInviteServer::a2_proceeding_proceeding_1xx( const SipSMCommand &command){
	if (transitionMatch(SipResponse::type, 
			command, 
			SipSMCommand::dialog_layer, 
			SipSMCommand::transaction_layer, 
			"1**"))
	{
		MRef<SipResponse*> resp = (SipResponse*)*command.getCommandPacket();
		lastResponse = resp;
		//no need for via header, it is copied from the request msg

		
#if 0
		if (resp->requires("100rel")){
			lastReliableResponse=resp;
			//The order we want (no race): 	
			//  1. Create PRACK server transaction
			//  2. Request timeout
			//  3. send 1xx message

			MRef<SipTransaction*> pracktrans = new SipTransactionNonInviteServer(sipStack,
					/*MRef<SipDialog*>(this)*/ dialog,
					resp->getCSeq()+1, // The PRACK transaction MUST be the next in sequence
					"PRACK",
					/*bye->getLastViaBranch()*/ "",
					dialog->dialogState.callId);
			
			dialog->registerTransaction(pracktrans);
			
			timerRel1xxResend = sipStack->getTimers()->getT1();
			requestTimeout(timerRel1xxResend,"timerRel1xxResend");
		}
#endif
		send(command.getCommandPacket(), false);
		
		return true;
	}else{
		return false;
	}
}


/**
 * If a response (non 2xx/1xx) is received from the TU/Call, we send it to
 * the remote side. We save it in case we need to retransmit it later.
 * Set "timerH", wait time for an ACK.
 * Set "timerG", response re-tx interval
 */
bool SipTransactionInviteServer::a3_proceeding_completed_resp36( const SipSMCommand &command){
	
	if (transitionMatch(SipResponse::type, 
			command, 
			SipSMCommand::dialog_layer, 
			SipSMCommand::transaction_layer, 
			"3**\n4**\n5**\n6**"))
	{
		cancelTimeout("timerRel1xxResend");
		lastResponse = MRef<SipResponse*>((SipResponse*)*command.getCommandPacket());		
		if( isUnreliable() ) {
			timerG = sipStackInternal->getTimers()->getG();
			requestTimeout(timerG, "timerG");
		}
		requestTimeout(sipStackInternal->getTimers()->getH(),"timerH");
		
		//no need for via header, it is copied from the request msg
		send(command.getCommandPacket(), false);
		return true;
	}else
		return false;
}

/**
 * If we receive a "transport_error" indication, we inform the TU and go to
 * the terminated state.
 * FIXME: There is currently no one giving this command to the transaction
 * layer (EE, r241).
 */
bool SipTransactionInviteServer::a4_proceeding_terminated_err( const SipSMCommand &command){

	if (transitionMatch(command, 
				SipCommandString::transport_error,
				SipSMCommand::transport_layer,
				SipSMCommand::transaction_layer)){
		cancelTimeout("timerRel1xxResend");
		
		SipSMCommand cmd( CommandString(callId, SipCommandString::transport_error), 
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

/**
 * If a 2xx response is received from the TU/Call it is sent to the remote
 * side and the transaction terminates. Note that the ACK is not yet
 * handled when a 2xx terminates the INVITE server transaction.
 */
bool SipTransactionInviteServer::a5_proceeding_terminated_2xx( const SipSMCommand &command){

	if (transitionMatch(SipResponse::type, 
				command, SipSMCommand::dialog_layer, 
				SipSMCommand::transaction_layer, 
				"2**")){

		cancelTimeout("timerRel1xxResend");
		lastResponse = MRef<SipResponse*>((SipResponse*)*command.getCommandPacket());
		
		//no need for via header, it is copied from the request msg
		send(command.getCommandPacket(), false);

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

/**
	If we receive the INVITE again from the TU, the response has been lost. 
	We retransmit the response again without informing the TU.
 */
bool SipTransactionInviteServer::a6_completed_completed_INVITE( const SipSMCommand &command){
	
	if (transitionMatch("INVITE", 
				command, 
				SipSMCommand::transport_layer, 
				SipSMCommand::transaction_layer)){
		MRef<SipResponse*> resp = lastResponse;
		send(MRef<SipMessage*>(*resp), false);
		return true;
	}else{
		return false;
	}
}

/**
	If we receive an ACK while in COMPLETED, we go to the "confirmed" state
	without informing the TU.
	Schedule for transaction termination using timer I.
	All other timers are cancelled.
 */
bool SipTransactionInviteServer::a7_completed_confirmed_ACK( const SipSMCommand &command){
	
	if (transitionMatch("ACK",
				command, 
				SipSMCommand::transport_layer, 
				SipSMCommand::transaction_layer)){
		cancelTimeout("timerG");//response re-tx 
		cancelTimeout("timerH"); //wait for ACK reception
		if( isUnreliable() )
			requestTimeout(sipStackInternal->getTimers()->getI(), "timerI");
		else
			requestTimeout( 0, "timerI");
		return true;
	}else{
		return false;
	}
}

/**
	If timer G fires when in the "completed" state, we have not received an
	"ACK" to our response (3xx-6xx). 
	Resend the response and hope for an "ACK" before a transaction
	timeout (timer H).
*/
bool SipTransactionInviteServer::a8_completed_completed_timerG( const SipSMCommand &command){
	
	if (transitionMatch(command, 
				"timerG",
				SipSMCommand::transaction_layer,
				SipSMCommand::transaction_layer)){
		MRef<SipResponse*> resp = lastResponse;
		timerG *= 2;
		if( timerG > sipStackInternal->getTimers()->getT2() )
			timerG = sipStackInternal->getTimers()->getT2();
		requestTimeout( timerG, "timerG");
		send(MRef<SipMessage*>(*resp), false);
		return true;
	}else{
		return false;
	}
}

/**
 * If there is a transport error or if timerH fires, we failed to receive
 * an ACK after several re-sends. We inform the TU/Call and go to the 
 * terminated state.
 */
bool SipTransactionInviteServer::a9_completed_terminated_errOrTimerH( const SipSMCommand &command){

	if (		transitionMatch(command, 
				"timerH",
				SipSMCommand::transaction_layer,
				SipSMCommand::transaction_layer) 
			|| transitionMatch(command,
				SipCommandString::transport_error,
				SipSMCommand::transport_layer,
				SipSMCommand::transaction_layer))
	{

		cancelTimeout("timerG");
		cancelTimeout("timerH");

		SipSMCommand cmd( CommandString(callId, SipCommandString::transport_error), 
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

/**
When timer I fires, we stop absorbing ACKs.
Move to TERMINATED and inform TU.
*/
bool SipTransactionInviteServer::a10_confirmed_terminated_timerI( const SipSMCommand &command){
	
	if (transitionMatch(command, 
				"timerI",
				SipSMCommand::transaction_layer,
				SipSMCommand::transaction_layer))
	{
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


/**
In the Confirmed state we absorb any ACK messages.
*/
bool SipTransactionInviteServer::a11_confirmed_confirmed_ACK( const SipSMCommand &command){
	
	if (transitionMatch("ACK",
				command, 
				SipSMCommand::transport_layer, 
				SipSMCommand::transaction_layer)){
		return true;
	}else{
		return false;
	}
}



bool SipTransactionInviteServer::a20_proceeding_proceeding_timerRel1xxResend( const SipSMCommand &command){
	
	if (transitionMatch(command, 
				"timerRel1xxResend",
				SipSMCommand::transaction_layer,
				SipSMCommand::transaction_layer)){

		timerRel1xxResend*=2;
		requestTimeout(timerRel1xxResend, "timerRel1xxResend");
		send(*lastResponse, false); // second parameter is "bool addVia"
		
		return true;
	}else{
		return false;
	}
}

void SipTransactionInviteServer::setUpStateMachine(){
		
	State<SipSMCommand,string> *s_start=new State<SipSMCommand,string>(this,"start");
	addState(s_start);

	State<SipSMCommand,string> *s_proceeding=new State<SipSMCommand,string>(this,"proceeding");
	addState(s_proceeding);

	State<SipSMCommand,string> *s_completed=new State<SipSMCommand,string>(this,"completed");
	addState(s_completed);

	State<SipSMCommand,string> *s_confirmed=new State<SipSMCommand,string>(this,"confirmed");
	addState(s_confirmed);
	
	State<SipSMCommand,string> *s_terminated=new State<SipSMCommand,string>(this,"terminated");
	addState(s_terminated);

	///Set up transitions to enable cancellation of this transaction
	new StateTransition<SipSMCommand,string>(this, "transition_cancel_transaction",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) 
				&SipTransaction::a1000_anyState_terminated_canceltransaction, 
			StateMachine<SipSMCommand,string>::anyState, s_terminated);

	//



	new StateTransition<SipSMCommand,string>(this, "transition_start_proceeding_INVITE",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteServer::a0_start_proceeding_INVITE, 
			s_start, s_proceeding);
	new StateTransition<SipSMCommand,string>(this, "transition_proceeding_proceeding_INVITE",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteServer::a1_proceeding_proceeding_INVITE, 
			s_proceeding, s_proceeding);

	new StateTransition<SipSMCommand,string>(this, "transition_proceeding_proceeding_1xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteServer::a2_proceeding_proceeding_1xx, 
			s_proceeding, s_proceeding);

	new StateTransition<SipSMCommand,string>(this, "transition_proceeding_completed_resp36",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteServer::a3_proceeding_completed_resp36, 
			s_proceeding, s_completed);

	new StateTransition<SipSMCommand,string>(this, "transition_proceeding_terminated_Err",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteServer::a4_proceeding_terminated_err,
			s_proceeding, s_terminated);

	new StateTransition<SipSMCommand,string>(this, "transition_proceeding_terminated_2xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteServer::a5_proceeding_terminated_2xx,
			s_proceeding, s_terminated);

	new StateTransition<SipSMCommand,string>(this, "transition_completed_completed_INVITE",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteServer::a6_completed_completed_INVITE,
			s_completed, s_completed);

	new StateTransition<SipSMCommand,string>(this, "transition_completed_confirmed_ACK",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteServer::a7_completed_confirmed_ACK,
			s_completed, s_confirmed);

	new StateTransition<SipSMCommand,string>(this, "transition_completed_completed_timerG",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteServer::a8_completed_completed_timerG,
			s_completed, s_completed);

	new StateTransition<SipSMCommand,string>(this, "transition_completed_terminated_errOrTimerH",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteServer::a9_completed_terminated_errOrTimerH,
			s_completed, s_terminated);

	new StateTransition<SipSMCommand,string>(this, "transition_confirmed_terminated_timerI",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteServer::a10_confirmed_terminated_timerI,
			s_confirmed, s_terminated);

	new StateTransition<SipSMCommand,string>(this, "a20_proceeding_proceeding_timerRel1xxResend",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteServer::a20_proceeding_proceeding_timerRel1xxResend,
			s_proceeding, s_proceeding);
		
	setCurrentState(s_start);
}


SipTransactionInviteServer::SipTransactionInviteServer(MRef<SipStackInternal*> stack, 
		int seq_no, 
		const string &cseqm, 
		const string &branch_,
		const string &callid) : 
			SipTransactionServer(stack, seq_no, cseqm, branch_, callid),
			lastResponse(NULL),
			timerG(500)
{
	setUpStateMachine();
}

SipTransactionInviteServer::~SipTransactionInviteServer(){
}

	
void SipTransactionInviteServer::setDialogRouteSet(MRef<SipRequest*> ) {
#if 0
	assert(dialog);
	if( dialog->dialogState.routeSet.size() == 0 ) {
		merr << "CESC: parent dialog has NO routeset" << end;
		/*SipMessage::getRouteSet returns the top-to-bottom ordered 
		Record-Route headers. 
		As a server, we can use this directly as route set (no reversing).
		*/
		dialog->dialogState.routeSet = inv->getRouteSet();
		for( list<string>::iterator iter = dialog->dialogState.routeSet.begin(); iter!=dialog->dialogState.routeSet.end(); iter++ ) {
			//merr << "CESC: SipTransINVCli:setrouteset:  " << (*iter) << end;
		}
	} else {
		//merr << "CESC: parent dialog already has a routeset" << end;
	}
#endif	
}

