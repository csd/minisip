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

                               ostart
                               |
                               |INVITE from TU
             Timer A fires     |a0.INVITE sent
             a1.Reset A,       V                      Timer B fires
             INVITE sent +-----------+                or Transport Err.
               +---------|           |---------------+a4.inform TU
               |         |  Calling  |               |
               +-------->|           |-------------->|
                         +-----------+ 2xx           |
                            |  |       a5.2xx to TU  |
                            |  |                     |
    300-699 +---------------+  |a2. 1xx to TU        |
a3.ACK sent |                  |                     |
resp. to TU |  1xx             V                     |
            |a6.1xx to TU+-----------+               |
            |  +---------|           |               |
            |  |         |Proceeding |-------------->|
            |  +-------->|           | 2xx           |
            |            +-----------+ a7.2xx to TU  |
            |       300-699    |                     |
            |     a8.ACK sent, |                     |
            |       resp. to TU|                     |
            |                  |                     |      NOTE:
            |  300-699         V                     |
            | a9.ACK sent+-----------+Transport Err. |  transitions
            |  +---------|           |a10.Inform TU  |  labeled with
            |  |         | Completed |-------------->|  the event
            |  +-------->|           |               |  over the action
            |            +-----------+               |  to take
            |              ^   |                     |
            |              |   | Timer D fires       |
            +--------------+   | a11. -              |
                               V                     |
                         +-----------+               |
                         |           |               |
                         | Terminated|<--------------+
                         |           |
                         +-----------+

                 Figure 5: INVITE client transaction
*/
 
#include<config.h>


#include"SipTransactionInviteClient.h"
#include"SipTransactionNonInviteClient.h"
#include<libmsip/SipResponse.h>
#include<libmsip/SipHeaderRoute.h>
#include<libmsip/SipTransitionUtils.h>
#include"../SipCommandDispatcher.h"
#include<libmsip/SipSMCommand.h>
#include<libmsip/SipCommandString.h>
#include<libmsip/SipDialog.h>
#include<libmsip/SipTimers.h>
#include<libmsip/SipDialogConfig.h>
#include<libmsip/SipHeaderCSeq.h>
#include<libmutil/MemObject.h>
#include<libmutil/CommandString.h>
#include<libmsip/SipHeaderRequire.h>
#include<libmutil/dbg.h>

using namespace std;

/*
void SipTransactionInviteClient::rel1xxProcessing(MRef<SipResponse*> resp){
		// If the server requests PRACK, then we need to start a
		// client transaction to transmit it.

		int statusCode = resp->getStatusCode();
		
		if (statusCode>100 && statusCode<200 && resp->requires("100rel")){
			dialog->dialogState.seqNo++;
			MRef<SipTransaction*> trans =
				new SipTransactionNonInviteClient(sipStackInternal, dialog, dialog->dialogState.seqNo, "PRACK", dialog->dialogState.callId);
			dialog->registerTransaction(trans);
			sendAck(resp,trans->getBranch(), true); //last argument means that yes, we are ack:ing a 1xx -> PRACK
		}
}
*/

bool SipTransactionInviteClient::a0_start_calling_INVITE( const SipSMCommand &command){

	if (transitionMatch("INVITE", 
			command, 
			SipSMCommand::dialog_layer, 
			SipSMCommand::transaction_layer))
	{
		lastInvite = (SipRequest*) *command.getCommandPacket();
		if( isUnreliable() ) { // retx timer
			timerA = sipStackInternal->getTimers()->getA();
			requestTimeout( timerA , "timerA" );
		}
		
		requestTimeout( sipStackInternal->getTimers()->getB(), "timerB" ); //transaction timeout

		send( command.getCommandPacket(), true ); // add via header
		return true;
	}else{
		return false;
	}
}

bool SipTransactionInviteClient::a1_calling_calling_timerA( const SipSMCommand &command){
	
	if (transitionMatch(command, 
			"timerA",
			SipSMCommand::transaction_layer,
			SipSMCommand::transaction_layer))
	{
		timerA *= 2; //no upper limit ... well ... timer B sets it
		requestTimeout( timerA, "timerA" );
		
		send(MRef<SipMessage*>((SipMessage*)* lastInvite), false);
		
		return true;
	}else{
		return false;
	}
}

bool SipTransactionInviteClient::a2_calling_proceeding_1xx( const SipSMCommand &command){
	
	if (transitionMatch(SipResponse::type, 
				command, 
				SipSMCommand::transport_layer,
				SipSMCommand::transaction_layer, 
				"1**"))
	{

		cancelTimeout("timerA");
		cancelTimeout("timerB");
		SipSMCommand cmd(
				command.getCommandPacket(), 
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer);
		dispatcher->enqueueCommand( cmd, HIGH_PRIO_QUEUE );
		
		MRef<SipResponse*> resp =(SipResponse *)*command.getCommandPacket();
		
		//assert(dialog);
		//TODO/XXX/FIXME: Do this in the TU instead
		//dialog->dialogState.updateState( resp );

		//rel1xxProcessing(resp);
		
		return true;
	}else{
		return false;
	}
}

bool SipTransactionInviteClient::a3_calling_completed_resp36( const SipSMCommand &command){

	if (transitionMatch(SipResponse::type, 
				command, 
				SipSMCommand::transport_layer, 
				SipSMCommand::transaction_layer, 
				"3**\n4**\n5**\n6**")){
		
		MRef<SipResponse*> resp( (SipResponse*) *command.getCommandPacket() );
		
		cancelTimeout("timerA");
		cancelTimeout("timerB");
		if( isUnreliable() )
			requestTimeout(sipStackInternal->getTimers()->getD(),"timerD");
		else 
			requestTimeout( 0,"timerD");
		SipSMCommand cmd( command.getCommandPacket(), 
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer);
		dispatcher->enqueueCommand( cmd, HIGH_PRIO_QUEUE );
		
		//assert(dialog);
		//TODO/XXX/FIXME: Do this in the TU instead
		//dialog->dialogState.updateState( resp );
		
		sendAck(resp);
		
		return true;
	}else{
		return false;
	}
}

bool SipTransactionInviteClient::a4_calling_terminated_ErrOrTimerB( const SipSMCommand &command){

	if (		transitionMatch(command, 
				SipCommandString::transport_error,
				SipSMCommand::transport_layer,
				SipSMCommand::transaction_layer) 
			|| transitionMatch(command, 
				"timerB",
				SipSMCommand::transaction_layer,
				SipSMCommand::transaction_layer)){
		cancelTimeout("timerA");
		cancelTimeout("timerB");
		
		CommandString terr( callId, SipCommandString::transport_error) ;
		terr["tid"] = getTransactionId();

		SipSMCommand cmd( terr, 
			SipSMCommand::transaction_layer, 
			SipSMCommand::dialog_layer
			);

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

bool SipTransactionInviteClient::a5_calling_terminated_2xx( const SipSMCommand &command){

	if (transitionMatch(SipResponse::type, 
				command, 
				SipSMCommand::transport_layer, 
				SipSMCommand::transaction_layer, 
				"2**")){
		
		cancelTimeout("timerA");
		cancelTimeout("timerB");
		
		SipSMCommand cmd( command.getCommandPacket(), 
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer);
		dispatcher->enqueueCommand( cmd, HIGH_PRIO_QUEUE );


		//FIXME/XXX/TODO: Implement setDialogRouteSet in TU instead
		//-EE
		
		//update dialogs route set ... needed to add route headers to the ACK we are going to send
		//setDialogRouteSet( (SipResponse*)*command.getCommandPacket() ); 
		
		//assert(dialog);
		//dialog->dialogState.updateState( (MRef<SipResponse*>((SipResponse *)*command.getCommandPacket()) ) );
		
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

bool SipTransactionInviteClient::a6_proceeding_proceeding_1xx( const SipSMCommand &command){
	
	if (transitionMatch(SipResponse::type, 
				command, 
				SipSMCommand::transport_layer, 
				SipSMCommand::transaction_layer, 
				"1**")){
		SipSMCommand cmd( command.getCommandPacket(), 
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer);

		MRef<SipResponse*> resp = (SipResponse *)*command.getCommandPacket();

		dispatcher->enqueueCommand( cmd, HIGH_PRIO_QUEUE );
		
		//assert(dialog);
		//TODO/XXX/FIXME: Do this in the TU instead
		//dialog->dialogState.updateState( resp );

		//rel1xxProcessing(resp);
		
		return true;
	}else{
		return false;
	}
}

//TODO: make sure  transport_error is sent when it should be
bool SipTransactionInviteClient::a7_proceeding_terminated_2xx( const SipSMCommand &command){
	
	if (transitionMatch(SipResponse::type, 
				command, 
				SipSMCommand::transport_layer, 
				SipSMCommand::transaction_layer, 
				"2**")){
		
		cancelTimeout("timerA");
		cancelTimeout("timerB");
		
		SipSMCommand cmd( command.getCommandPacket(), 
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer);
		dispatcher->enqueueCommand( cmd, HIGH_PRIO_QUEUE );

		//update dialogs route set ... needed to add route headers to the ACK we are going to send
		//setDialogRouteSet( (SipResponse*)*command.getCommandPacket() ); 
		//assert(dialog);
		//TODO/XXX/FIXME: In Tu instead
		//dialog->dialogState.updateState( (MRef<SipResponse*>((SipResponse *)*command.getCommandPacket()) ) );
		
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

bool SipTransactionInviteClient::a8_proceeding_completed_resp36( const SipSMCommand &command){
		
	if (transitionMatch(SipResponse::type, 
				command, 
				SipSMCommand::transport_layer, 
				SipSMCommand::transaction_layer, 
				"3**\n4**\n5**\n6**")){
		
		MRef<SipResponse *> resp((SipResponse*)*command.getCommandPacket());
		cancelTimeout("timerA");
		cancelTimeout("timerB");
		if( isUnreliable() )
			requestTimeout(sipStackInternal->getTimers()->getD(),"timerD");
		else 
			requestTimeout( 0,"timerD");
                
		SipSMCommand cmd( command.getCommandPacket(), 
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer);
		dispatcher->enqueueCommand( cmd, HIGH_PRIO_QUEUE );
		
		//assert(dialog);
		//IN TU instead TODO/XXX/FIXME
		//dialog->dialogState.updateState( resp );
		
		sendAck(resp);
		
		return true;
	}else{
		return false;
	}
}

bool SipTransactionInviteClient::a9_completed_completed_resp36( const SipSMCommand &command){

	if (transitionMatch(SipResponse::type, 
				command, 
				SipSMCommand::transport_layer, 
				SipSMCommand::transaction_layer, 
				"3**\n4**\n5**\n6**")){
		MRef<SipResponse *> resp((SipResponse*)*command.getCommandPacket());
		sendAck(resp);
		return true;
	}else{
		return false;
	}
}

bool SipTransactionInviteClient::a10_completed_terminated_TErr( const SipSMCommand &command){

	if (transitionMatch(command, 
				SipCommandString::transport_error,
				SipSMCommand::transaction_layer,
				SipSMCommand::transaction_layer)){

		cancelTimeout("timerD");
		
		SipSMCommand cmd( CommandString( callId, SipCommandString::transport_error), 
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

bool SipTransactionInviteClient::a11_completed_terminated_timerD( const SipSMCommand &command){
	
	if (transitionMatch(command, 
				"timerD",
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


void SipTransactionInviteClient::setUpStateMachine(){
		
	State<SipSMCommand,string> *s_start=new State<SipSMCommand,string>(this,"start");
	addState(s_start);

	State<SipSMCommand,string> *s_calling=new State<SipSMCommand,string>(this,"calling");
	addState(s_calling);
	
	State<SipSMCommand,string> *s_proceeding=new State<SipSMCommand,string>(this,"proceeding");
	addState(s_proceeding);

	State<SipSMCommand,string> *s_completed=new State<SipSMCommand,string>(this,"completed");
	addState(s_completed);

	State<SipSMCommand,string> *s_terminated=new State<SipSMCommand,string>(this,"terminated");
	addState(s_terminated);

	//Set up cancel transitions
	new StateTransition<SipSMCommand,string>(this, "transition_cancel_transaction",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) 
				&SipTransaction::a1000_anyState_terminated_canceltransaction, 
			StateMachine<SipSMCommand,string>::anyState, s_terminated);

	//

	new StateTransition<SipSMCommand,string>(this, "transition_start_calling_INVITE",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteClient::a0_start_calling_INVITE, 
			s_start, s_calling);

	new StateTransition<SipSMCommand,string>(this, "transition_calling_calling_timerA",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteClient::a1_calling_calling_timerA, 
			s_calling, s_calling);

	new StateTransition<SipSMCommand,string>(this, "transition_calling_proceeding_1xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteClient::a2_calling_proceeding_1xx, 
			s_calling, s_proceeding);

	new StateTransition<SipSMCommand,string>(this, "transition_calling_completed_resp36",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteClient::a3_calling_completed_resp36, 
			s_calling, s_completed);

	new StateTransition<SipSMCommand,string>(this, "transition_calling_terminated_ErrOrTimerB",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteClient::a4_calling_terminated_ErrOrTimerB,
			s_calling, s_terminated);

	new StateTransition<SipSMCommand,string>(this, "transition_calling_terminated_2xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteClient::a5_calling_terminated_2xx,
			s_calling, s_terminated);

	new StateTransition<SipSMCommand,string>(this, "transition_proceeding_proceeding_1xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteClient::a6_proceeding_proceeding_1xx,
			s_proceeding, s_proceeding);

	new StateTransition<SipSMCommand,string>(this, "transition_proceeding_terminated_2xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteClient::a7_proceeding_terminated_2xx,
			s_proceeding, s_terminated);

	new StateTransition<SipSMCommand,string>(this, "transition_proceeding_completed_resp36",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteClient::a8_proceeding_completed_resp36,
			s_proceeding, s_completed);

	new StateTransition<SipSMCommand,string>(this, "transition_completed_completed_resp36",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteClient::a9_completed_completed_resp36,
			s_completed, s_completed);

	new StateTransition<SipSMCommand,string>(this, "transition_completed_terminated_TErr",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteClient::a10_completed_terminated_TErr,
			s_completed, s_terminated);

	new StateTransition<SipSMCommand,string>(this, "transition_completed_terminated_timerD",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteClient::a11_completed_terminated_timerD,
			s_completed, s_terminated);

	setCurrentState(s_start);
}


SipTransactionInviteClient::SipTransactionInviteClient(MRef<SipStackInternal*> stack, 
		int seq_no, 
		const string &cseqm, 
		const string &callid): 
			SipTransactionClient(stack, seq_no, cseqm, "", callid),
		lastInvite(NULL)
{
	timerA=sipStackInternal->getTimers()->getA();
	setUpStateMachine();
}

SipTransactionInviteClient::~SipTransactionInviteClient(){
}

void SipTransactionInviteClient::setDialogRouteSet(MRef<SipResponse *> ) {
#if 0
	assert(dialog);
	if( dialog->dialogState.routeSet.size() == 0 ) {
		//merr << "CESC: parent dialog has NO routeset" << end;
		/*SipMessage::getRouteSet returns the uris in the record-route headers,
		in the top to bottom order.
		We need to reverse the order of those uris (as client).*/
		dialog->dialogState.routeSet = resp->getRouteSet();
		dialog->dialogState.routeSet.reverse();
		//for( list<string>::iterator iter = dialog->dialogState.routeSet.begin(); iter!=dialog->dialogState.routeSet.end(); iter++ ) {
		//	merr << "CESC: SipTransINVCli:setrouteset:  " << (*iter) << end;
		//}
	} else {
		//merr << "CESC: parent dialog already has a routeset" << end;
	}
#endif
}


void SipTransactionInviteClient::sendAck(MRef<SipResponse*> resp, bool provisional){

	MRef<SipRequest*> ack= SipRequest::createSipMessageAck( 
			lastInvite,
			resp,
			provisional
			);

	if (provisional){
		//TODO/XXX/FIXME: What is this needed for? --EE
		//int seq = dialog->dialogState.seqNo++;
		//((SipHeaderValueCSeq*)*ack->getHeaderValueNo(SIP_HEADER_TYPE_CSEQ, 0))->setCSeq(seq);
	}
		
	send(MRef<SipMessage*>(*ack), true, lastInvite->getBranch() );
}


