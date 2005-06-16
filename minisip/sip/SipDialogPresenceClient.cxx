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
 * 	SipDialogPresenceClient.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/


#include<assert.h>
#include"SipDialogPresenceClient.h"
#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipResponse.h>
#include<libmsip/SipSubscribe.h>
#include<libmsip/SipNotify.h>
#include<libmsip/SipMessageTransport.h>
#include<libmsip/SipTransactionNonInviteClient.h>
#include<libmsip/SipTransactionNonInviteServer.h>
#include<libmsip/SipTransactionUtils.h>
#include<libmsip/SipDialog.h>
#include<libmsip/SipCommandString.h>
#include"DefaultDialogHandler.h"
#include<libmutil/itoa.h>
#include<libmutil/Timestamp.h>
#include<libmutil/termmanip.h>
#include<libmutil/dbg.h>
#include<libmsip/SipSMCommand.h>
#include <time.h>
#include"../minisip/LogEntry.h"
#include<libmsip/SipCommandString.h>
#include"../mediahandler/MediaHandler.h"
#include<libmutil/MemObject.h>
#include<libmsip/SipHeaderExpires.h>

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
	//int seqNo = requestSeqNo();
	++dialogState.seqNo;
	MRef<SipTransaction*> subscribetrans = new SipTransactionNonInviteClient(sipStack, MRef<SipDialog *>(this), dialogState.seqNo, dialogState.callId);
//	subscribetrans->setSocket( getPhoneConfig()->proxyConnection );
	registerTransaction(subscribetrans);
	sendSubscribe(subscribetrans->getBranch());
}
   

bool SipDialogPresenceClient::a0_start_trying_presence(const SipSMCommand &command){

	if (transitionMatch(command, SipCommandString::start_presence_client)){
#ifdef DEBUG_OUTPUT
		merr << "SipDialogPresenceClient::a0: Presence toUri is: <"<< command.getCommandString().getParam()<< ">"<< end;
#endif
		toUri = MRef<SipIdentity*>( new SipIdentity(command.getCommandString().getParam()) );
		createSubscribeClientTransaction();
		return true;
	}else{
		return false;
	}

}

bool SipDialogPresenceClient::a1_X_subscribing_200OK(const SipSMCommand &command){
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "2**")){
		MRef<SipResponse*> resp(  (SipResponse*)*command.getCommandPacket() );
		dialogState.remoteTag = command.getCommandPacket()->getHeaderValueTo()->getParameter("tag");

		//MRef<SipHeaderValueExpires *> expireshdr = (SipHeaderValueExpires*)
		//	*(resp->getHeaderOfType(SIP_HEADER_TYPE_EXPIRES)->getHeaderValue(0));
		assert(dynamic_cast<SipHeaderValueExpires*>(*resp->getHeaderValueNo(SIP_HEADER_TYPE_EXPIRES,0)));		
		MRef<SipHeaderValueExpires *> expireshdr = (SipHeaderValueExpires*)*resp->getHeaderValueNo(SIP_HEADER_TYPE_EXPIRES,0);
		int timeout;
		if (expireshdr){
			timeout = expireshdr->getTimeout();
		}else{
			mdbg << "WARNING: SipDialogPresenceClient did not contain any expires header - using 300 seconds"<<end;
			timeout = 300;
		}
		
		requestTimeout(timeout, "timerDoSubscribe");
		
#ifdef DEBUG_OUTPUT
		merr << "Subscribed for presence for user "<< toUri->getSipUri()<< end;
#endif
		return true;
	}else{
		return false;
	}
}

bool SipDialogPresenceClient::a2_trying_retrywait_transperror(const SipSMCommand &command){
	if (transitionMatch(command, SipCommandString::transport_error )){
		mdbg << "WARNING: Transport error when subscribing - trying again in five minutes"<< end;
		requestTimeout(300, "timerDoSubscribe");
		return true;
	}else{
		return false;
	}

}

bool SipDialogPresenceClient::a4_X_trying_timerTO(const SipSMCommand &command){
	if (transitionMatch(command, "timerDoSubscribe")){
		createSubscribeClientTransaction();
		return true;
	}else{
		return false;
	}
}

bool SipDialogPresenceClient::a5_subscribing_subscribing_NOTIFY(const SipSMCommand &command){
	if (transitionMatch(command, SipNotify::type, SipSMCommand::remote, IGN)){
		CommandString cmdstr(dialogState.callId, SipCommandString::remote_presence_update,"UNIMPLEMENTED_INFO");
		getDialogContainer()->getCallback()->sipcb_handleCommand(cmdstr);
		return true;
	}else{
		return false;
	}
}

bool SipDialogPresenceClient::a6_subscribing_termwait_stoppresence(const SipSMCommand &command){
	if (transitionMatch(command, SipCommandString::hang_up)){
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}

}

bool SipDialogPresenceClient::a7_termwait_terminated_notransactions(const SipSMCommand &command){
	if (transitionMatch(command, SipCommandString::no_transactions) ){
		SipSMCommand cmd( CommandString( dialogState.callId, SipCommandString::call_terminated),
				  SipSMCommand::TU,
				  SipSMCommand::DIALOGCONTAINER);
		getDialogContainer()->enqueueCommand( cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE );
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

	setCurrentState(s_start);
}



SipDialogPresenceClient::SipDialogPresenceClient(MRef<SipStack*> stack, 
		//const SipDialogConfig &callconfig, 
		MRef<SipDialogConfig*> callconfig,
		bool use_stun) : 
                	SipDialog(stack,callconfig),
			useSTUN(use_stun)
{

	//setCallId( itoa(rand())+"@"+getDialogConfig().inherited.externalContactIP);
	dialogState.callId = itoa(rand())+"@"+getDialogConfig()->inherited.externalContactIP;
	
	dialogState.localTag = itoa(rand());
	
	setUpStateMachine();
}

SipDialogPresenceClient::~SipDialogPresenceClient(){	
}

void SipDialogPresenceClient::sendSubscribe(const string &branch){
	
	MRef<SipSubscribe*> sub;
	int32_t localSipPort;

	if(getDialogConfig()->inherited.transport=="TCP")
		localSipPort = getDialogConfig()->inherited.localTcpPort;
	else if(getDialogConfig()->inherited.transport=="TLS")
		localSipPort = getDialogConfig()->inherited.localTlsPort;
	else{ /* UDP, may use STUN */
            if( /*phoneconf->*/useSTUN ){
		localSipPort = getDialogConfig()->inherited.externalContactUdpPort;
            } else {
                localSipPort = getDialogConfig()->inherited.localUdpPort;
            }
        }
	
	sub = MRef<SipSubscribe*>(new SipSubscribe(
				branch,
				dialogState.callId,
				toUri,
				getDialogConfig()->inherited.sipIdentity,
				///getDialogConfig()->inherited.localUdpPort,
//				im_seq_no
				dialogState.seqNo
				));

	sub->getHeaderValueFrom()->setParameter("tag",dialogState.localTag);

        MRef<SipMessage*> pktr(*sub);
#ifdef MINISIP_MEMDEBUG
	pktr.setUser("SipDialogPresenceClient");
#endif

        SipSMCommand scmd(
                pktr, 
                SipSMCommand::TU, 
                SipSMCommand::transaction
                );
	
	getDialogContainer()->enqueueCommand(scmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
//	setLastSubsc(inv);

}

bool SipDialogPresenceClient::handleCommand(const SipSMCommand &c){
	mdbg << "SipDialogPresenceClient::handleCommand got "<< c << end;
//	merr << "XXXSipDialogPresenceClient::handleCommand got "<< c << end;

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
	
//	if (c.getType()!=SipSMCommand::COMMAND_PACKET && 
//			c.getCommandPacket()->getCSeq()!= command_seq_no)
//		return false;
	
	mdbg << "SipDialogPresenceClient::handlePacket() got "<< c << end;
	merr << "SipDialogPresenceClient returning dialogs handleCommand"<< end;
	bool handled = SipDialog::handleCommand(c);
	
	if (!handled && c.getType()==SipSMCommand::COMMAND_STRING && c.getCommandString().getOp()==SipCommandString::no_transactions){
		return true;
	}
	
	if (c.getType()==SipSMCommand::COMMAND_STRING && dialogState.callId.length()>0){
		if (c.getCommandString().getDestinationId() == dialogState.callId ){
			mdbg << "Warning: SipDialogPresenceClient ignoring command with matching call id"<< end;
			return true;
		}
	}
	if (c.getType()==SipSMCommand::COMMAND_PACKET && dialogState.callId.length()>0){
		if (c.getCommandPacket()->getCallId() == dialogState.callId){
			mdbg << "Warning: SipDialogPresenceClient ignoring packet with matching call id"<< end;
			return true;
		}
	}
	
	return handled;
}


