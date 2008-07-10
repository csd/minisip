/*
 Copyright (C) 2004-2006 the Minisip Team
 
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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2004-2006
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/

/* Name
 * 	SipDialogPresenceClient.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/
#include<config.h>

#include<libminisip/signaling/sip/SipDialogPresenceClient.h>

#include<libmutil/massert.h>
#include<libmsip/SipHeaderFrom.h>
#include<libmsip/SipHeaderTo.h>
#include<libmsip/SipResponse.h>
#include<libmsip/SipTransitionUtils.h>
#include<libmsip/SipDialog.h>
#include<libmsip/SipCommandString.h>
#include<libminisip/signaling/sip/DefaultDialogHandler.h>
#include<libmutil/stringutils.h>
#include<libmutil/Timestamp.h>
#include<libmutil/termmanip.h>
#include<libmutil/dbg.h>
#include<libmsip/SipSMCommand.h>
#include <time.h>
#include <stdlib.h>
#include<libminisip/gui/LogEntry.h>
#include<libmsip/SipCommandString.h>
#include<libminisip/media/SubsystemMedia.h>
#include<libmutil/MemObject.h>
#include<libmsip/SipHeaderSubscriptionState.h>

#ifdef _WIN32_WCE
#	include"../include/minisip_wce_extra_includes.h"
#endif

using namespace std;

/*
 Presence dialog for user "user@domain".

      +-------------+
      |   start     |
      +-------------+
             |
	     | a0: CommandString "presence" "user@domain"
	     |     new TransactionClient(SUBSCRIBE)
	     V
      +-------------+
 +--->|   trying    |--------------+
 |    +-------------+              |
 |           |                     |
 |a4:        | a2: transport_error |
 |timerTO    |     set timerTO=5min|
 |new TC(S)  V                     |  a1: 200 OK
 |    +-------------+              |      -
 |<---| retry_wait  |              |
 |    +-------------+              |
 |                                 |
 |	                           |
 |	                           |
 |  	                           |
 |    +-------------+<-------------+
 +----| subscribing |----+
      +-------------+    | a5: NOTIFY
a6:          |    ^      |     new TransactionServer ; gui(<presenceinfo>)
stop_presence|    +------+
 -           |     
             V 
      +-------------+
      |  termwait   |
      +-------------+
             |
	     | a7: no_transactions
	     |
	     V
      +-------------+
      | terminated  |
      +-------------+
      
*/


void SipDialogPresenceClient::createSubscribeClientTransaction(){
	++dialogState.seqNo;
	sendSubscribe();
}
   

bool SipDialogPresenceClient::a0_start_trying_presence(const SipSMCommand &command){

	if (transitionMatch(command, 
				SipCommandString::start_presence_client,
				SipSMCommand::transaction_layer,
				SipSMCommand::dialog_layer)){
#ifdef DEBUG_OUTPUT
		merr << "SipDialogPresenceClient::a0: Presence toUri is: <"<< command.getCommandString().getParam()<< ">"<< endl;
#endif
		toUri = MRef<SipIdentity*>( new SipIdentity(command.getCommandString().getParam()) );
		createSubscribeClientTransaction();
		return true;
	}else{
		return false;
	}

}

bool SipDialogPresenceClient::a1_X_subscribing_200OK(const SipSMCommand &command){
	if (transitionMatch(SipResponse::type, command, SipSMCommand::transaction_layer, SipSMCommand::dialog_layer, "2**")){
		MRef<SipResponse*> resp(  (SipResponse*)*command.getCommandPacket() );
		dialogState.remoteTag = command.getCommandPacket()->getHeaderValueTo()->getParameter("tag");

		MRef<SipHeaderValueSubscriptionState *> statehdr = (SipHeaderValueSubscriptionState*)*resp->getHeaderValueNo(SIP_HEADER_TYPE_SUBSCRIPTIONSTATE,0);

		int to;
		if (statehdr && statehdr->hasParameter("expires")){
			to = atoi(statehdr->getParameter("expires").c_str());
		}else{
			mdbg("signaling/sip") << "WARNING: SipDialogPresenceClient did not contain any expires header - using 300 seconds"<<endl;
			to = 300;
		}
		
		requestTimeout(to * 1000, "timerDoSubscribe");
		
#ifdef DEBUG_OUTPUT
		merr << "Subscribed for presence for user "<< toUri->getSipUri().getString()<< endl;
#endif
		return true;
	}else{
		return false;
	}
}

bool SipDialogPresenceClient::a2_trying_retrywait_transperror(const SipSMCommand &command){
	if (transitionMatch(command, 
				SipCommandString::transport_error,
				SipSMCommand::transaction_layer,
				SipSMCommand::dialog_layer )){
		mdbg("signaling/sip") << "WARNING: Transport error when subscribing - trying again in five minutes"<< endl;
		requestTimeout(300 * 1000, "timerDoSubscribe");
		return true;
	}else{
		return false;
	}

}

bool SipDialogPresenceClient::a4_X_trying_timerTO(const SipSMCommand &command){
	if (transitionMatch(command, "timerDoSubscribe",
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer)){
		createSubscribeClientTransaction();
		return true;
	}else{
		return false;
	}
}

bool SipDialogPresenceClient::a5_subscribing_subscribing_NOTIFY(const SipSMCommand &command){
	if (transitionMatch("NOTIFY", command, SipSMCommand::transaction_layer, SipSMCommand::dialog_layer)){

		MRef<SipRequest*> notify = dynamic_cast<SipRequest*>(*command.getCommandPacket());
		sendNotifyOk(notify);

		CommandString cmdstr(dialogState.callId, SipCommandString::remote_presence_update,"UNIMPLEMENTED_INFO");
		getSipStack()->getCallback()->handleCommand("gui",cmdstr);
		return true;
	}else{
		return false;
	}
}

bool SipDialogPresenceClient::a6_subscribing_termwait_stoppresence(const SipSMCommand &command){
	if (transitionMatch(command, 
				SipCommandString::hang_up,
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer)){
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}

}

bool SipDialogPresenceClient::a7_termwait_terminated_notransactions(const SipSMCommand &command){
	if (transitionMatch(command, 
				SipCommandString::no_transactions,
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer) ){

		dialogState.isTerminated=true;

		SipSMCommand cmd( CommandString( dialogState.callId, SipCommandString::call_terminated),
				  SipSMCommand::dialog_layer,
				  SipSMCommand::dispatcher);
		getSipStack()->enqueueCommand( cmd, HIGH_PRIO_QUEUE );
		return true;
	}else{
		return false;
	}
}

bool SipDialogPresenceClient::a8_trying_trying_40X(const SipSMCommand &command){
	if (transitionMatchSipResponse("SUBSCRIBE", command,SipSMCommand::transaction_layer, SipSMCommand::dialog_layer, "407\n401")){
		
		MRef<SipResponse*> resp( (SipResponse*)*command.getCommandPacket() );

		dialogState.updateState( resp ); //nothing will happen ... 4xx responses do not update ...

		++dialogState.seqNo;

		if( !updateAuthentications( resp ) ){
			mdbg("signaling/sip") << "Auth failed" << endl;
			return true;
		}

		sendSubscribe();

		return true;
	}else{
		return false;
	}
}


bool SipDialogPresenceClient::a9_trying_retry_wait_failure(const SipSMCommand &command){
	if (transitionMatchSipResponse("SUBSCRIBE", command,SipSMCommand::transaction_layer, SipSMCommand::dialog_layer, "3**\n4**\n5**\n6**")){
		
		MRef<SipResponse*> resp( (SipResponse*)*command.getCommandPacket() );

		dialogState.updateState( resp ); //nothing will happen ...

		++dialogState.seqNo;

		return true;
	}else{
		return false;
	}
}


void SipDialogPresenceClient::setUpStateMachine(){

	State<SipSMCommand,string> *s_start = new State<SipSMCommand,string>(this,"start");
	addState(s_start);

	State<SipSMCommand,string> *s_trying = new State<SipSMCommand,string>(this,"trying");
	addState(s_trying);

	State<SipSMCommand,string> *s_retry_wait = new State<SipSMCommand,string>(this,"retry_wait");
	addState(s_retry_wait);

	State<SipSMCommand,string> *s_subscribing = new State<SipSMCommand,string>(this,"subscribing");
	addState(s_subscribing);

	State<SipSMCommand,string> *s_termwait=new State<SipSMCommand,string>(this,"termwait");
	addState(s_termwait);
	
	State<SipSMCommand,string> *s_terminated=new State<SipSMCommand,string>(this,"terminated");
	addState(s_terminated);

	
	new StateTransition<SipSMCommand,string>(this, "transition_start_trying_presence",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogPresenceClient::a0_start_trying_presence,
		s_start, s_trying);
	
 	new StateTransition<SipSMCommand,string>(this, "transition_trying_subscribing_200OK",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogPresenceClient::a1_X_subscribing_200OK,
		s_trying, s_subscribing);
       
 	new StateTransition<SipSMCommand,string>(this, "transition_trying_retrywait",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogPresenceClient::a2_trying_retrywait_transperror,
		s_trying, s_retry_wait);
 
 	new StateTransition<SipSMCommand,string>(this, "transition_retrywait_subscribing_200OK",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogPresenceClient::a1_X_subscribing_200OK,
		s_retry_wait, s_subscribing);

 	new StateTransition<SipSMCommand,string>(this, "transition_subscribing_trying_timerTO",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogPresenceClient::a4_X_trying_timerTO,
		s_subscribing, s_trying);
	
 	new StateTransition<SipSMCommand,string>(this, "transition_retrywait_trying_timerTO",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogPresenceClient::a4_X_trying_timerTO,
		s_subscribing, s_trying);



	//Should add this/similar transition to the trying case (when
	//updating registration, we should be ready to receive a NOTIFY).
	new StateTransition<SipSMCommand,string>(this, "transition_subscribing_subscribing_NOTIFY",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogPresenceClient::a5_subscribing_subscribing_NOTIFY,
			s_subscribing, s_subscribing);

	new StateTransition<SipSMCommand,string>(this, "transition_subscribing_termwait_stoppresence",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogPresenceClient::a6_subscribing_termwait_stoppresence,
			s_subscribing, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_termwait_terminated_notransactions",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogPresenceClient::a7_termwait_terminated_notransactions,
		s_termwait, s_terminated);

	new StateTransition<SipSMCommand,string>(this, "transition_termwait_terminated_notransactions",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogPresenceClient::a8_trying_trying_40X,
		s_trying, s_trying);

	new StateTransition<SipSMCommand,string>(this, "transition_termwait_terminated_notransactions",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogPresenceClient::a9_trying_retry_wait_failure,
		s_trying, s_retry_wait);

	setCurrentState(s_start);
}



SipDialogPresenceClient::SipDialogPresenceClient(MRef<SipStack*> stack, 
		MRef<SipIdentity*> ident,
		bool use_stun) : 
                	SipDialog(stack,ident, ""),
			useSTUN(use_stun)
{
	setUpStateMachine();
}

SipDialogPresenceClient::~SipDialogPresenceClient(){	
}

void SipDialogPresenceClient::sendSubscribe(){
	
	MRef<SipRequest*> sub ;
	
	sub = SipRequest::createSipMessageSubscribe(
				dialogState.callId,
				toUri->getSipUri(),
				getDialogConfig()->sipIdentity->getSipUri(),
				getDialogConfig()->getContactUri(useSTUN),
				dialogState.seqNo
				);

	sub->getHeaderValueFrom()->setParameter("tag",dialogState.localTag);

	addAuthorizations( sub );
	addRoute( sub );

        MRef<SipMessage*> pktr(*sub);

        SipSMCommand scmd(
                pktr, 
                SipSMCommand::dialog_layer, 
                SipSMCommand::transaction_layer
                );
	
	getSipStack()->enqueueCommand(scmd, HIGH_PRIO_QUEUE );

}

void SipDialogPresenceClient::sendNotifyOk(MRef<SipRequest*> notify){
	MRef<SipResponse*> ok= createSipResponse(notify, 200, "OK");

	MRef<SipMessage*> pref(*ok);
	SipSMCommand cmd( pref, SipSMCommand::dialog_layer, SipSMCommand::transaction_layer);
	getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE);
}

bool SipDialogPresenceClient::handleCommand(const SipSMCommand &c){
	mdbg("signaling/sip") << "SipDialogPresenceClient::handleCommand got "<< c << endl;

	if (c.getType()==SipSMCommand::COMMAND_STRING && dialogState.callId.length()>0){
		if (c.getCommandString().getDestinationId() != dialogState.callId ){
			cerr << "SipDialogPresenceClient returning false based on callId"<< endl;
			return false;
		}
	}
	
	if (c.getType()==SipSMCommand::COMMAND_PACKET  && dialogState.callId.length()>0){
		if (c.getCommandPacket()->getCallId() != dialogState.callId ){
			return false;
		}
		if (c.getType()!=SipSMCommand::COMMAND_PACKET && 
				c.getCommandPacket()->getCSeq()!= dialogState.seqNo){
			return false;
		}
	
	}
	
	mdbg("signaling/sip") << "SipDialogPresenceClient::handlePacket() got "<< c << endl;
	bool handled = SipDialog::handleCommand(c);
	
	if (!handled && c.getType()==SipSMCommand::COMMAND_STRING && c.getCommandString().getOp()==SipCommandString::no_transactions){
		return true;
	}
	
	if (c.getType()==SipSMCommand::COMMAND_STRING && dialogState.callId.length()>0){
		if (c.getCommandString().getDestinationId() == dialogState.callId ){
			mdbg("signaling/sip") << "Warning: SipDialogPresenceClient ignoring command with matching call id"<< endl;
			return true;
		}
	}
	if (c.getType()==SipSMCommand::COMMAND_PACKET && dialogState.callId.length()>0){
		if (c.getCommandPacket()->getCallId() == dialogState.callId){
			mdbg("signaling/sip") << "Warning: SipDialogPresenceClient ignoring packet with matching call id"<< endl;
			return true;
		}
	}
	
	return handled;
}


