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
                |        | Proceeding|        |a2:send response
                +------->|           |<-------+
                         |           |          Transport Err.
                         |           |          a4:Inform TU
                         |           |--------------->+
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
#include<libmsip/SipDialog.h>
#include<libmsip/SipTransactionInviteServer.h>
#include<libmsip/SipResponse.h>
#include<libmsip/SipAck.h>
#include<libmsip/SipTransactionUtils.h>
#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipCommandString.h>

#ifdef DEBUG_OUTPUT
#include<libmutil/termmanip.h>
#endif

/**
 * The first command a INVITE server transaction receives i a INVITE
 * packet. It forwards the packet to the TU/call. We send no 1xx since we
 * expect a response from the TU in less than 200 ms.
 */
bool SipTransactionInviteServer::a0_start_proceeding_INVITE( const SipSMCommand &command){
	
	if (transitionMatch(command, SipInvite::type, IGN, SipSMCommand::transaction)){
		SipSMCommand cmd(command);
		cmd.setDestination(SipSMCommand::TU);
		cmd.setSource(SipSMCommand::transaction);
		dialog->getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
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
 * error message. TODO: Implement an option to send 100 trying in action
 * "a0".
 */
bool SipTransactionInviteServer::a1_proceeding_proceeding_INVITE( const SipSMCommand &command){
	
	if (transitionMatch(command, SipInvite::type, SipSMCommand::remote, SipSMCommand::transaction)){
		MRef<SipResponse*> resp = lastResponse;
		if (resp.isNull()){
#ifdef DEBUG_OUTPUT
			merr << FG_ERROR << "Invite server transaction failed to deliver response before remote side retransmitted. Bug?"<< PLAIN << end;
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
	if (transitionMatch(command, SipResponse::type, SipSMCommand::TU, SipSMCommand::transaction, "1**")){
		lastResponse = MRef<SipResponse*>((SipResponse*)*command.getCommandPacket());
		send(command.getCommandPacket(), false);
		return true;
	}else{
		return false;
	}
}


/**
 * If a response (non 2xx/1xx) is received from the TU/Call, we send it to
 * the remote side. We save it in case we need to retransmit it later.
 * We set "timerG" to time out unless we receive and acknowledgement (ACK).
 * We set "timerH" to indicate when we give up retransmitting.
 */
bool SipTransactionInviteServer::a3_proceeding_completed_resp36( const SipSMCommand &command){
	
	if (transitionMatch(command, SipResponse::type, SipSMCommand::TU, SipSMCommand::transaction, "3**\n4**\n5**\n6**")){
		lastResponse = MRef<SipResponse*>((SipResponse*)*command.getCommandPacket());
		timerG = 500;
		requestTimeout(timerG, "timerG");
		requestTimeout(32000,"timerH");
		send(command.getCommandPacket(), false);
		return true;
	}else
		return false;
}

/**
 * If we receive a "transport_error" indication, we inform the TU and go to
 * the terminated state.
 * NOTE: There is currently no one giving this command to the transaction
 * layer (EE, r241).
 */
bool SipTransactionInviteServer::a4_proceeding_terminated_err( const SipSMCommand &command){

	if (transitionMatch(command, SipCommandString::transport_error)){
		
		SipSMCommand cmd( CommandString(callId, SipCommandString::transport_error), 
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

/**
 * If a 2xx response is received from the TU/Call it is sent to the remote
 * side and the transaction terminates. Note that the ACK is not yet
 * handled when a 2xx terminates the INVITE server transaction.
 */
bool SipTransactionInviteServer::a5_proceeding_terminated_2xx( const SipSMCommand &command){

	if (transitionMatch(command, SipResponse::type, SipSMCommand::TU, SipSMCommand::transaction, "2**")){
		lastResponse = MRef<SipResponse*>((SipResponse*)*command.getCommandPacket());
		
		send(command.getCommandPacket(), false);

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

/**
 * When we are in the completed state a response (3xx-6xx) has been
 * received from the TU/Call and it has been sent to the remote side.
 * If we receive the INVITE again, the response was lost. We retransmit it
 * again without informing the TU.
 */
bool SipTransactionInviteServer::a6_completed_completed_INVITE( const SipSMCommand &command){
	
	if (transitionMatch(command, SipInvite::type, SipSMCommand::remote, SipSMCommand::transaction)){
		MRef<SipResponse*> resp = lastResponse;
		
		send(MRef<SipMessage*>(*resp), false);
		return true;
	}else{
		return false;
	}
}

/**
 * If we receive an ACK, we go to the "confirmed" state
 * without informing the TU, and schedule for termination
 * in 64 seconds.
 */
bool SipTransactionInviteServer::a7_completed_confirmed_ACK( const SipSMCommand &command){
	
	if (transitionMatch(command, SipAck::type, SipSMCommand::remote, SipSMCommand::transaction)){
		cancelTimeout("timerG");
		cancelTimeout("timerH");
		requestTimeout(64000,"timerI");
		return true;
	}else{
		return false;
	}
}

/**
 * If timer G fires when in the "completed" state, we have not reveived an
 * "ACK" to our response (3xx-6xx). We resend the response and hope for an
 * "ACK".
 */
bool SipTransactionInviteServer::a8_completed_completed_timerG( const SipSMCommand &command){
	
	if (transitionMatch(command, "timerG")){
		MRef<SipResponse*> resp = lastResponse;
		timerG *= 2;
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

	if (transitionMatch(command, "timerH") || transitionMatch(command,SipCommandString::transport_error)){

		cancelTimeout("timerG");

		SipSMCommand cmd( CommandString(callId, SipCommandString::transport_error), 
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


bool SipTransactionInviteServer::a10_confirmed_terminated_timerI( const SipSMCommand &command){
	
	if (transitionMatch(command, "timerI")){
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
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransaction::a1000_cancel_transaction, 
			s_proceeding, s_terminated);
	new StateTransition<SipSMCommand,string>(this, "transition_cancel_transaction",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransaction::a1000_cancel_transaction, 
			s_completed, s_terminated);
	new StateTransition<SipSMCommand,string>(this, "transition_cancel_transaction",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransaction::a1000_cancel_transaction, 
			s_confirmed, s_terminated);
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

	setCurrentState(s_start);
}


SipTransactionInviteServer::SipTransactionInviteServer(MRef<SipDialog*> d, int seq_no, const string &branch,string callid) : 
		SipTransactionServer(d, seq_no, branch,callid),
		lastResponse(NULL),
		timerG(500)
{
	toaddr = dialog->getDialogConfig()->inherited.sipIdentity->sipProxy.sipProxyIpAddr;
	port = dialog->getDialogConfig()->inherited.sipIdentity->sipProxy.sipProxyPort;
	setUpStateMachine();
}

SipTransactionInviteServer::~SipTransactionInviteServer(){
}
	

