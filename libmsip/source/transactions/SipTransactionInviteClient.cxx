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


#include<assert.h>
#include<libmsip/SipTransactionInviteClient.h>
#include<libmsip/SipResponse.h>
#include<libmsip/SipAck.h>
#include<libmsip/SipTransactionUtils.h>
#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipDialog.h>
#include<libmsip/SipSMCommand.h>
#include<libmsip/SipCommandString.h>
#include<libmutil/MemObject.h>
#include<libmutil/CommandString.h>
#include<libmutil/dbg.h>

bool SipTransactionInviteClient::a0_start_calling_INVITE( const SipSMCommand &command){

	if (transitionMatch(command, SipInvite::type, SipSMCommand::TU, IGN)){
		//SipTransactionInviteClient *invtrans= (SipTransactionInviteClient *)sipStateMachine;
//		SipTransactionInviteClient *invtrans= dynamic_cast<SipTransactionInviteClient *>(sipStateMachine);
//		cerr << "XXXXXXxx a0_start_calling_INVITE: packet ptr is " << (int) (*command.getCommandPacket())<<endl;
//		/*invtrans->*/setLastInvite((SipInvite *) *command.getCommandPacket() );

		lastInvite = (SipInvite *) *command.getCommandPacket();
		/*invtrans->*/requestTimeout( /*invtrans->*/ /*getTimerT1()*/ timerT1, "timerA" );
		
		/*invtrans->*/requestTimeout( /*invtrans->*/ /*getTimerT1()*/ timerT1*64, "timerB" );
		/*invtrans->*/send( command.getCommandPacket() );
		return true;
	}else{
		return false;
	}
}


bool SipTransactionInviteClient::a1_calling_calling_timerA( const SipSMCommand &command){
	
	if (transitionMatch(command, "timerA")){
		timerA *= 2; 
		requestTimeout( timerA, "timerA" );
		
		send(MRef<SipMessage*>((SipMessage*)* lastInvite));
		
		return true;
	}else{
		return false;
	}
}

bool SipTransactionInviteClient::a2_calling_proceeding_1xx( const SipSMCommand &command){
	
	if (transitionMatch(command,SipResponse::type, SipSMCommand::remote, IGN, "1**")){
		cancelTimeout("timerA");
		cancelTimeout("timerB");
                SipSMCommand cmd(
                        command.getCommandPacket(), 
                        SipSMCommand::transaction, 
                        SipSMCommand::TU);
		dialog->getDialogContainer()->enqueueCommand( cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE );
		return true;
	}else{
		return false;
	}
}

bool SipTransactionInviteClient::a3_calling_completed_resp36( const SipSMCommand &command){

	if (transitionMatch(command, SipResponse::type, SipSMCommand::remote, IGN, "3**\n4**\n5**\n6**")){
		
		MRef<SipResponse*> resp( (SipResponse*) *command.getCommandPacket() );
		
		cancelTimeout("timerA");
		cancelTimeout("timerB");
		requestTimeout(64000,"timerD");
                SipSMCommand cmd( command.getCommandPacket(), 
                        SipSMCommand::transaction, 
                        SipSMCommand::TU);
		dialog->getDialogContainer()->enqueueCommand( cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE );
		
		
		sendAck(resp);
		
		return true;
	}else{
		return false;
	}
}

bool SipTransactionInviteClient::a4_calling_terminated_ErrOrTimerB( const SipSMCommand &command){

	if (transitionMatch(command, SipCommandString::transport_error) || transitionMatch(command, "timerB")){
		cancelTimeout("timerA");
		cancelTimeout("timerB");
		
		SipSMCommand cmd( 
				
				CommandString( 
					callId, 
					SipCommandString::transport_error
					), 
				SipSMCommand::transaction, 
				SipSMCommand::TU 
				);

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

bool SipTransactionInviteClient::a5_calling_terminated_2xx( const SipSMCommand &command){

	if (transitionMatch(command, SipResponse::type, SipSMCommand::remote, IGN, "2**")){
		
		cancelTimeout("timerA");
		cancelTimeout("timerB");
		
                SipSMCommand cmd( command.getCommandPacket(), 
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

bool SipTransactionInviteClient::a6_proceeding_proceeding_1xx( const SipSMCommand &command){
	
	if (transitionMatch(command, SipResponse::type, SipSMCommand::remote, IGN, "1**")){
                SipSMCommand cmd( command.getCommandPacket(), 
                        SipSMCommand::transaction, 
                        SipSMCommand::TU);

		dialog->getDialogContainer()->enqueueCommand( cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE );
		return true;
	}else{
		return false;
	}
}

//TODO: make sure  transport_error is sent when it should be
bool SipTransactionInviteClient::a7_proceeding_terminated_2xx( const SipSMCommand &command){
	
	if (transitionMatch(command, SipResponse::type, SipSMCommand::remote, IGN, "2**")){
		cancelTimeout("timerA");
		cancelTimeout("timerB");
		
                SipSMCommand cmd( command.getCommandPacket(), 
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

bool SipTransactionInviteClient::a8_proceeding_completed_resp36( const SipSMCommand &command){
		
	if (transitionMatch(command, SipResponse::type, SipSMCommand::remote, IGN, "3**\n4**\n5**\n6**")){
		MRef<SipResponse *> resp((SipResponse*)*command.getCommandPacket());
		cancelTimeout("timerA");
		cancelTimeout("timerB");
		requestTimeout(64000, "timerD");
                
                SipSMCommand cmd( command.getCommandPacket(), 
                        SipSMCommand::transaction, 
                        SipSMCommand::TU);
		dialog->getDialogContainer()->enqueueCommand( cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE );
		
		sendAck(resp);
		
		return true;
	}else{
		return false;
	}
}

bool SipTransactionInviteClient::a9_completed_completed_resp36( const SipSMCommand &command){

	if (transitionMatch(command,SipResponse::type, SipSMCommand::remote, IGN, "3**\n4**\n5**\n6**")){
		MRef<SipResponse *> resp((SipResponse*)*command.getCommandPacket());
		sendAck(resp);
		return true;
	}else{
		return false;
	}
}



bool SipTransactionInviteClient::a10_completed_terminated_TErr( const SipSMCommand &command){

	if (transitionMatch(command, SipCommandString::transport_error)){
		cancelTimeout("timerD");
		
		SipSMCommand cmd( CommandString( callId, SipCommandString::transport_error), 
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

bool SipTransactionInviteClient::a11_completed_terminated_timerD( const SipSMCommand &command){
	
	if (transitionMatch(command, "timerD")){
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

//	StateTransition<SipSMCommand,string> *transition_start_calling_INVITE=
		new StateTransition<SipSMCommand,string>(this,
				"transition_start_trying_INVITE",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteClient::a0_start_calling_INVITE, 
				s_start, s_calling
				);
	
//	StateTransition<SipSMCommand,string> *transition_calling_calling_timerA=
		new StateTransition<SipSMCommand,string>(this,
				"transition_calling_calling_timerA",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteClient::a1_calling_calling_timerA, 
				s_calling, s_calling
				);

//	StateTransition<SipSMCommand,string> *transition_calling_proceeding_1xx=
		new StateTransition<SipSMCommand,string>(this,
				"transition_calling_proceeding_1xx",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteClient::a2_calling_proceeding_1xx, 
				s_calling, s_proceeding
				);

//	StateTransition<SipSMCommand,string> *transition_calling_completed_resp36=
		new StateTransition<SipSMCommand,string>(this,
				"transition_calling_completed_resp36",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteClient::a3_calling_completed_resp36, 
				s_calling, s_completed
				);

//	StateTransition<SipSMCommand,string> *transition_calling_terminated_ErrOrTimerB=
		new StateTransition<SipSMCommand,string>(this,
				"transition_calling_terminated_ErrOrTimerB",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteClient::a4_calling_terminated_ErrOrTimerB,
				s_calling, s_terminated
				);
		
//	StateTransition<SipSMCommand,string> *transition_calling_terminated_2xx=
		new StateTransition<SipSMCommand,string>(this,
				"transition_calling_terminated_2xx",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteClient::a5_calling_terminated_2xx,
				s_calling, s_terminated
				);

//	StateTransition<SipSMCommand,string> *transition_proceeding_proceeding_1xx=
		new StateTransition<SipSMCommand,string>(this,
				"transition_proceeding_proceeding_1xx",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteClient::a6_proceeding_proceeding_1xx,
				s_proceeding, s_proceeding
				);
		
//	StateTransition<SipSMCommand,string> *transition_proceeding_terminated_2xx=
		new StateTransition<SipSMCommand,string>(this,
				"transition_proceeding_terminated_2xx",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteClient::a7_proceeding_terminated_2xx,
				s_proceeding, s_terminated
				);

//	StateTransition<SipSMCommand,string> *transition_proceeding_completed_resp36=
		new StateTransition<SipSMCommand,string>(this,
				"transition_proceeding_completed_resp36",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteClient::a8_proceeding_completed_resp36,
				s_proceeding, s_completed
				);

//	StateTransition<SipSMCommand,string> *transition_completed_completed_resp36=
		new StateTransition<SipSMCommand,string>(this,
				"transition_completed_completed_resp36",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteClient::a9_completed_completed_resp36,
				s_completed, s_completed
				);

//	StateTransition<SipSMCommand,string> *transition_completed_terminated_TErr=
		new StateTransition<SipSMCommand,string>(this,
				"transition_completed_terminated_TErr",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteClient::a10_completed_terminated_TErr,
				s_completed, s_terminated
				);
	
//	StateTransition<SipSMCommand,string> *transition_completed_terminated_timerD=
		new StateTransition<SipSMCommand,string>(this,
				"transition_completed_terminated_timerD",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipTransactionInviteClient::a11_completed_terminated_timerD,
				s_completed, s_terminated
				);
	
	setCurrentState(s_start);
}


SipTransactionInviteClient::SipTransactionInviteClient(MRef<SipDialog*> call, 
            int seq_no, string callid): 
		SipTransaction("SipTransactionInviteClient",call, "", callid),
		lastInvite(NULL),
		timerT1(500),
		timerA(500),
		command_seq_no(seq_no)
{
	toaddr = dialog->getDialogConfig().inherited.sipIdentity->sipProxy.sipProxyIpAddr;
	port = dialog->getDialogConfig().inherited.sipIdentity->sipProxy.sipProxyPort;
	
	setUpStateMachine();
}

SipTransactionInviteClient::~SipTransactionInviteClient(){
}

bool SipTransactionInviteClient::handleCommand(const SipSMCommand &c){
	
	if (! (c.getDestination()==SipSMCommand::transaction || c.getDestination()==SipSMCommand::ANY))
		return false;
	
	if (c.getType()==SipSMCommand::COMMAND_PACKET && c.getCommandPacket()->getCSeq()!= command_seq_no){
		return false;
	}
	return StateMachine<SipSMCommand,string>::handleCommand(c);
}

void SipTransactionInviteClient::sendAck(MRef<SipResponse*> resp, string br){
        MRef<SipMessage*> ref( *resp);
	MRef<SipAck*> ack= new SipAck( getBranch(), ref, dialog->getDialogConfig().uri_foreign, 
			dialog->getDialogConfig().inherited.sipIdentity->sipDomain
			); 
	//TODO:
	//ack.add_header( new SipHeaderRoute(getDialog()->getRouteSet() ) );
	send(MRef<SipMessage*>(*ack), br);
}

