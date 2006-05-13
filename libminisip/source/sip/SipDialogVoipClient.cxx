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

/* Copyright (C) 2004,2005,2006
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *	    Joachim Orrblad <joachim[at]orrblad.com>
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/

/* Name
 * 	SipDialogVoipClient.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/
#include<config.h>

#include<libminisip/sip/SipDialogVoipClient.h>

#include<libmutil/massert.h>

#include<libmsip/SipTransactionInviteClientUA.h>
#include<libmsip/SipTransactionInviteServerUA.h>
#include<libmsip/SipTransactionNonInviteClient.h>
#include<libmsip/SipTransactionNonInviteServer.h>
#include<libmsip/SipTransactionUtils.h>
#include<libmsip/SipCommandString.h>
#include<libmsip/SipHeaderWarning.h>
#include<libmsip/SipHeaderContact.h>
#include<libmsip/SipHeaderFrom.h>
#include<libmsip/SipHeaderRoute.h>
#include<libmsip/SipHeaderRequire.h>
#include<libmsip/SipHeaderRSeq.h>
#include<libmsip/SipHeaderTo.h>
#include<libmsip/SipMIMEContent.h>
#include<libmsip/SipMessageContent.h>
#include<libmutil/itoa.h>
#include<libmcrypto/base64.h>
#include<libmutil/Timestamp.h>
#include<libmutil/termmanip.h>
#include<libmutil/dbg.h>
#include<libmsip/SipSMCommand.h>
#include <time.h>
#include<libminisip/gui/LogEntry.h>

#include<libmutil/print_hex.h>
#include <iostream>
#include<time.h>

#ifdef _WIN32_WCE
#	include"../include/minisip_wce_extra_includes.h"
#endif

using namespace std;


/*
 This class is responsible for starting up a session and ending up in
 the in_call state.
   
 The "dotted" states are implemented in SipDialogVoip.cxx.

 TODO: Merge calling_noauth and calling_stored to one state


                 +---------------+
                 |               |
                 |     start     |
                 |               |
a26transport_err +---------------+
gui(failed)              |
  +----------------+     | invite
  |                |     V a2001: new TransInvite
  |              +---------------+
  |         +----|               |
  |     1xx |    |Calling_noauth |----+ 180
  |a3:(null)+--->|               |    | a2002: gui(ringing)
  +--------------+---------------+<---+
  |                      |     |       2xx/a2004:
  |   cancel|hangup      |     +------------------------------+
  |   a2014              |                                    |
  +----------------+     |  40X                               | accept_invite
  |                |     V  a2008: send_auth                  | a11: send 200
  |              +---------------+                            |
  |         +----|               |----+                       |
  |         |    |Calling_stored |    | 180                   |
  |    1XX  +--->|               |<---+ a2009: gui(ringing)   |
  |    a2010     +---------------+                            |
  |                      |                                    |
  |                      | 2xx                                |
  |                      v a2011:                             |
  |              +. . . . . . . .+                            |
  |              .               .<---------------------------+
  |              .   in call     .        
  |              .               .        
  |              +. . . . . . . .+        
  |                                           
  |CANCEL/cancel                                                 
  |a2005/a2006: new TrnsCncl                     
  +------------->+. . . . . . . .+        
  |33/a2007:gui()|               .        
  +------------->|   termwait    .        
  |trnsperr/a2013|               .
  +------------->+. . . . . . . .+
                          
                                               
                                                 
                 +. . . . . . . .+
                 .               .
                 .  terminated   .
                 .               .
                 +. . . . . . . .+
   

                 +---------------+
      2XX   +----|               |
      a2017 |    |    anyState   |
      ACK   +--->|               |
                 +---------------+
*/

bool SipDialogVoipClient::a2001_start_callingnoauth_invite( const SipSMCommand &command)
{
	if (transitionMatch(command, 
				SipCommandString::invite,
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer)){
#ifdef ENABLE_TS
		ts.save("a0_start_callingnoauth_invite");
#endif
		++dialogState.seqNo;
		
		//set an "early" remoteUri ... we will update this later
		dialogState.remoteUri= command.getCommandString().getParam();

/*		
		MRef<SipTransaction*> invtrans = new SipTransactionInviteClientUA(sipStack, 
				//MRef<SipDialog *>(this), 
				dialogState.seqNo,
				"INVITE",
				dialogState.callId);
		
		invtrans->setSocket( phoneconf->proxyConnection );
		
 		notifyEarlyTermination = true; // set to true, once sent the first command, set to false
		
		dispatcher->getLayerTransaction()->addTransaction(invtrans);
		//registerTransactionToDialog(invtrans);
*/
		sendInvite(""/*invtrans->getBranch()*/);

		return true;
	}else{
		return false;
	}
}

bool SipDialogVoipClient::a2002_callingnoauth_callingnoauth_18X( const SipSMCommand &command)
{	
	if (transitionMatchSipResponse("INVITE", 
				command, 
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer, 
				"18*")){

		MRef<SipResponse*> resp= (SipResponse*) *command.getCommandPacket();

#ifdef ENABLE_TS
		ts.save( RINGING );
#endif
		if (resp->requires("100rel") && resp->getStatusCode()!=100){
			if( !handleRel1xx( resp ) )
				// Ignore retransmission
				return true;
		}
		else{
			//We must maintain the dialog state. 
			dialogState.updateState( resp );
		}
		
		CommandString cmdstr(dialogState.callId, SipCommandString::remote_ringing);
		sipStack->getCallback()->handleCommand("gui", cmdstr);
	
		//string peerUri = command.getCommandPacket()->getTo().getString();
		string peerUri = dialogState.remoteUri; //use the dialog state ...

		MRef<SipMessageContent *> content = resp->getContent();
		if( !content.isNull() ){
			MRef<SdpPacket*> sdp((SdpPacket*)*content);
			//Early media	
			getMediaSession()->setSdpAnswer( sdp, peerUri );
		}

		return true;
	}else{
		return false;
	}
}


bool SipDialogVoipClient::a2003_callingnoauth_callingnoauth_1xx( const SipSMCommand &command)
{

	if (transitionMatchSipResponse("INVITE", command, SipSMCommand::transaction_layer, SipSMCommand::dialog_layer, "1**")){
		dialogState.updateState( MRef<SipResponse*>((SipResponse *)*command.getCommandPacket()) );

		MRef<SipResponse*> resp = (SipResponse*)*command.getCommandPacket();
		if (resp->requires("100rel") && resp->getStatusCode()!=100){
			if( !handleRel1xx( resp ) )
				// Ignore retransmission
				return true;
		}

		return true;
	}else{
		return false;
	}
    
}

bool SipDialogVoipClient::a2004_callingnoauth_incall_2xx( const SipSMCommand &command)
{
	if (transitionMatchSipResponse("INVITE", 
				command, 
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer, 
				"2**")){
#ifdef ENABLE_TS
		ts.save("a3_callingnoauth_incall_2xx");
#endif
		MRef<SipResponse*> resp(  (SipResponse*)*command.getCommandPacket() );
		
		string peerUri = dialogState.remoteUri;
		if(!sortMIME(*resp->getContent(), peerUri, 3))
			return false;

		dialogState.updateState( resp );
		sendAck();
		
		//string peerUri = resp->getFrom().getString();
		
		setLogEntry( new LogEntryOutgoingCompletedCall() );
		getLogEntry()->start = time( NULL );
		getLogEntry()->peerSipUri = peerUri;


//FIXME: CESC: for now, route set is updated at the transaction layer		
		
		CommandString cmdstr(dialogState.callId, SipCommandString::invite_ok, "",
							(getMediaSession()->isSecure()?"secure":"unprotected"));
		
		sipStack->getCallback()->handleCommand("gui", cmdstr);
		
#ifdef IPSEC_SUPPORT
		// Check if IPSEC was required
		if (ipsecSession->required() && !ipsecSession->offered)
			return false;
#endif
		return true;
	}else{
		return false;
	}
}

bool SipDialogVoipClient::a2005_callingnoauth_termwait_CANCEL( const SipSMCommand &command)
{
	if (transitionMatch("CANCEL", command, SipSMCommand::transaction_layer, SipSMCommand::dialog_layer)){
//		setCurrentState(toState);

/*		MRef<SipTransaction*> cancelresp( 
				new SipTransactionNonInviteServer(
					sipStack,
					//MRef<SipDialog*>(this), 
					command.getCommandPacket()->getCSeq(), 
					command.getCommandPacket()->getCSeqMethod(), 
					command.getCommandPacket()->getLastViaBranch(), 
					dialogState.callId ));

		dispatcher->getLayerTransaction()->addTransaction(cancelresp);
		//registerTransactionToDialog(cancelresp);


		SipSMCommand cmd(command);
		cmd.setSource(SipSMCommand::dialog_layer);
		cmd.setDestination(SipSMCommand::transaction_layer);
		dispatcher->enqueueCommand(cmd, HIGH_PRIO_QUEUE);
*/
		getMediaSession()->stop();
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}

bool SipDialogVoipClient::a2006_callingnoauth_termwait_cancel( const SipSMCommand &command)
{
	if (		transitionMatch(command, 
				SipCommandString::cancel,
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer) 
			|| transitionMatch(command, 
				SipCommandString::hang_up,
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer)){
//		setCurrentState(toState);

/*
		string inv_branch = getLastInvite()->getFirstViaBranch();
		MRef<SipTransaction*> canceltrans( 
				new SipTransactionNonInviteClient(sipStack, 
					//MRef<SipDialog*>( this ), 
					dialogState.seqNo, 
					"CANCEL", 
					dialogState.callId)); 

		canceltrans->setBranch(inv_branch);
		dispatcher->getLayerTransaction()->addTransaction(canceltrans);
		//registerTransactionToDialog(canceltrans);
*/
 		sendCancel(""/*inv_branch*/);

		getMediaSession()->stop();
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}

//Note: This is also used as: callingauth_terminated_36
bool SipDialogVoipClient::a2007_callingnoauth_termwait_36( const SipSMCommand &command)
{
	if (transitionMatchSipResponse("INVITE", command, SipSMCommand::transaction_layer, SipSMCommand::dialog_layer, "3**\n4**\n5**\n6**")){
		
		MRef<LogEntry *> rejectedLog( new LogEntryCallRejected() );
		rejectedLog->start = time( NULL );
		rejectedLog->peerSipUri = dialogState.remoteTag;

		MRef<SipResponse*> resp = (SipResponse*)*command.getCommandPacket();

		if (sipResponseFilterMatch(resp,"404")){
                        CommandString cmdstr(dialogState.callId, SipCommandString::remote_user_not_found);
			assert(sipStack);
			assert(sipStack->getCallback());
			sipStack->getCallback()->handleCommand("gui", cmdstr);
			((LogEntryFailure *)*rejectedLog)->error =
				"User not found";
			setLogEntry( rejectedLog );
			rejectedLog->handle();
                        
		}else if (sipResponseFilterMatch(resp,"606")){
			((LogEntryFailure *)*rejectedLog)->error =
				"User could not handle the call";
			setLogEntry( rejectedLog );
			rejectedLog->handle();
                        
                        CommandString cmdstr( dialogState.callId, SipCommandString::remote_unacceptable, command.getCommandPacket()->getWarningMessage());
			sipStack->getCallback()->handleCommand( "gui", cmdstr );
		}
		else if (sipResponseFilterMatch(resp,"3**") ||
			 sipResponseFilterMatch(resp,"4**") ||
			 sipResponseFilterMatch(resp,"5**") ||
			 sipResponseFilterMatch(resp,"6**")){
			((LogEntryFailure *)*rejectedLog)->error =
				"User rejected the call";
			setLogEntry( rejectedLog );
			rejectedLog->handle();
                        CommandString cmdstr( dialogState.callId, SipCommandString::remote_reject);
			sipStack->getCallback()->handleCommand( "gui",cmdstr );
		}
		
		getMediaSession()->stop();
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}

bool SipDialogVoipClient::a2008_callingnoauth_callingauth_40X( const SipSMCommand &command){

	if (transitionMatchSipResponse("INVITE", command,SipSMCommand::transaction_layer, SipSMCommand::dialog_layer, "407\n401")){
		
		MRef<SipResponse*> resp( (SipResponse*)*command.getCommandPacket() );

		dialogState.updateState( resp ); //nothing will happen ... 4xx responses do not update ...

		++dialogState.seqNo;
/*
		MRef<SipTransaction*> trans( 
			new SipTransactionInviteClientUA(sipStack, 
				//MRef<SipDialog*>(this), 
				dialogState.seqNo, 
				"INVITE", 
				dialogState.callId));
		dispatcher->getLayerTransaction()->addTransaction(trans);
		//registerTransactionToDialog(trans);
*/		
		//realm = resp->getRealm();
		realm = resp->getAuthenticateProperty("realm");
		//nonce = resp->getNonce();
		nonce = resp->getAuthenticateProperty("nonce");

		sendAuthInvite(""/*trans->getBranch()*/);

		return true;
	}else{
		return false;
	}
}

bool SipDialogVoipClient::a2009_callingauth_callingauth_18X( const SipSMCommand &command){
	
	if (transitionMatchSipResponse("INVITE", command, SipSMCommand::transaction_layer, SipSMCommand::dialog_layer, "18*")){
		MRef<SipResponse*> resp (  (SipResponse*)*command.getCommandPacket()  );
#ifdef ENABLE_TS
		ts.save( RINGING );
#endif

		if (resp->requires("100rel") && resp->getStatusCode()!=100){
			if( !handleRel1xx( resp ) )
				// Ignore retransmission
				return true;
		}
		else{
			dialogState.updateState( resp );
		}

		CommandString cmdstr(dialogState.callId, SipCommandString::remote_ringing);
		sipStack->getCallback()->handleCommand("gui", cmdstr );

		string peerUri = dialogState.remoteUri;
		
		MRef<SdpPacket*> sdp((SdpPacket*)*resp->getContent());
		if ( !sdp.isNull() ){
			//Early media
			getMediaSession()->setSdpAnswer( sdp, peerUri );
		}

		return true;
	}else{
		return false;
	}
}

bool SipDialogVoipClient::a2010_callingauth_callingauth_1xx( const SipSMCommand &command){
	if (transitionMatchSipResponse("INVITE", command, SipSMCommand::transaction_layer, SipSMCommand::dialog_layer, "1**")){

		dialogState.updateState( MRef<SipResponse*> ( (SipResponse*)*command.getCommandPacket() ) );

		MRef<SipResponse*> resp = (SipResponse*)*command.getCommandPacket();
		if (resp->requires("100rel") && resp->getStatusCode()!=100){
			if( !handleRel1xx( resp ) )
				// Ignore retransmission
				return true;
		}

		return true;
	}else{
		return false;
	}
}

bool SipDialogVoipClient::a2011_callingauth_incall_2xx( const SipSMCommand &command){
	if (transitionMatchSipResponse("INVITE", command, SipSMCommand::transaction_layer, SipSMCommand::dialog_layer, "2**")){
		MRef<SipResponse*> resp( (SipResponse*)*command.getCommandPacket() );
		
		dialogState.updateState( resp );
		sendAck();

//CESC: for now, route set is updated at the transaction layer		
		
		//string peerUri = resp->getFrom().getString().substr(4);
		string peerUri = dialogState.remoteUri;
		
		setLogEntry( new LogEntryOutgoingCompletedCall() );
		getLogEntry()->start = time( NULL );
		getLogEntry()->peerSipUri = peerUri;
		
		CommandString cmdstr(dialogState.callId, 
				SipCommandString::invite_ok, 
				"",
				(getMediaSession()->isSecure()?"secure":"unprotected")
				);
		sipStack->getCallback()->handleCommand("gui", cmdstr );


		if(!sortMIME(*resp->getContent(), peerUri, 3))
			return false;
#ifdef IPSEC_SUPPORT
		// Check if IPSEC was required
		if (ipsecSession->required() && !ipsecSession->offered)
			return false;
#endif
		return true;
	}else{
		return false;
	}
}

bool SipDialogVoipClient::a2012_calling_termwait_2xx( const SipSMCommand &command){
	
	if (transitionMatchSipResponse("INVITE", command, SipSMCommand::transaction_layer, SipSMCommand::dialog_layer, "2**")){

		++dialogState.seqNo;

/*
		MRef<SipTransaction*> byetrans = 
			new SipTransactionNonInviteClient(sipStack, 
				//MRef<SipDialog*>(this), 
				dialogState.seqNo, 
				"BYE", 
				dialogState.callId); 

		dispatcher->getLayerTransaction()->addTransaction(byetrans);
		//registerTransactionToDialog(byetrans);
*/		
		sendBye(""/*byetrans->getBranch()*/, dialogState.seqNo);

		CommandString cmdstr(dialogState.callId, SipCommandString::security_failed);
		sipStack->getCallback()->handleCommand("gui", cmdstr);

		getMediaSession()->stop();
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}

bool SipDialogVoipClient::a2013_callingnoauth_termwait_transporterror( const SipSMCommand &command){
	if (transitionMatch(command, 
				SipCommandString::transport_error,
				SipSMCommand::transaction_layer,
				SipSMCommand::dialog_layer )){
		CommandString cmdstr(dialogState.callId, SipCommandString::transport_error);
		sipStack->getCallback()->handleCommand("gui",cmdstr);
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}

//Copy of a8!
bool SipDialogVoipClient::a2014_callingauth_termwait_cancel( const SipSMCommand &command)
{
	if (		transitionMatch(command, 
				SipCommandString::cancel,
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer) 
			|| transitionMatch(command, 
				SipCommandString::hang_up,
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer)){

//		setCurrentState(toState);
		string inv_branch = getLastInvite()->getFirstViaBranch();

/*	
		MRef<SipTransaction*> canceltrans( 
			new SipTransactionNonInviteClient(sipStack, 
				//MRef<SipDialog*>(this), 
				dialogState.seqNo, 
				"CANCEL", 
				dialogState.callId)); 
		canceltrans->setBranch(inv_branch);
		dispatcher->getLayerTransaction()->addTransaction(canceltrans);
		//registerTransactionToDialog(canceltrans);
*/
		sendCancel(""/*inv_branch*/);

		getMediaSession()->stop();
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}


bool SipDialogVoipClient::a2017_any_any_2XX( const SipSMCommand &command){
	if (transitionMatchSipResponse("INVITE",
				       command,
				       SipSMCommand::transport_layer,
				       SipSMCommand::dialog_layer,
				       "2**")){
		string state = getCurrentStateName();

		if( state == "terminated" )
			return false;

		MRef<SipMessage*> pack = command.getCommandPacket();

		if( dialogState.remoteTag != pack->getHeaderValueTo()->getParameter("tag") ){
			// Acknowledge and terminate 2xx from other fork
			sendAck();
			sendSipMessage( *createSipMessageBye() );
		}
		else if( state != "termwait" ){
			sendAck();
			
		}

		return true;
	}

	return false;
}


void SipDialogVoipClient::setUpStateMachine(){

	State<SipSMCommand,string> *s_start=new State<SipSMCommand,string>(this,"start");
	addState(s_start);

	State<SipSMCommand,string> *s_callingnoauth=new State<SipSMCommand,string>(this,"callingnoauth");
	addState(s_callingnoauth);

	State<SipSMCommand,string> *s_callingauth=new State<SipSMCommand,string>(this,"callingauth");
	addState(s_callingauth);

	MRef<State<SipSMCommand,string> *> s_incall = getState("incall");
	MRef<State<SipSMCommand,string> *> s_termwait= getState("termwait");

	new StateTransition<SipSMCommand,string>(this, "transition_any_any_2XX",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipClient::a2017_any_any_2XX,
			anyState, anyState);




	new StateTransition<SipSMCommand,string>(this, "transition_start_callingnoauth_invite",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipClient::a2001_start_callingnoauth_invite, 
			s_start, s_callingnoauth);

	new StateTransition<SipSMCommand,string>(this, "transition_callingnoauth_callingnoauth_18X",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipClient::a2002_callingnoauth_callingnoauth_18X, 
			s_callingnoauth, s_callingnoauth);

	new StateTransition<SipSMCommand,string>(this, "transition_callingnoauth_callingnoauth_1xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipClient::a2003_callingnoauth_callingnoauth_1xx, 
			s_callingnoauth, s_callingnoauth);

	new StateTransition<SipSMCommand,string>(this, "transition_callingnoauth_incall_2xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipClient::a2004_callingnoauth_incall_2xx, 
			s_callingnoauth, s_incall);

	// Must be added after the noauth->incall transition since this is
	// the "fallback one" if we don't accept the 2XX reply (for example
	// authentication error)
	new StateTransition<SipSMCommand,string>(this, "transition_callingauth_termwait_2xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipClient::a2012_calling_termwait_2xx,
			s_callingnoauth, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_callingnoauth_termwait_CANCEL",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipClient::a2005_callingnoauth_termwait_CANCEL,
			s_callingnoauth, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_callingnoauth_termwait_cancel",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipClient::a2006_callingnoauth_termwait_cancel,
			s_callingnoauth, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_callingnoauth_callingauth_40X",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipClient::a2008_callingnoauth_callingauth_40X,
			s_callingnoauth, s_callingauth);

	new StateTransition<SipSMCommand,string>(this, "transition_callingnoauth_termwait_36",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipClient::a2007_callingnoauth_termwait_36,
			s_callingnoauth, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_callingauth_callingauth_18X",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipClient::a2009_callingauth_callingauth_18X, 
			s_callingauth, s_callingauth);

	new StateTransition<SipSMCommand,string>(this, "transition_callingauth_callingauth_1xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipClient::a2010_callingauth_callingauth_1xx, 
			s_callingauth, s_callingauth);

	new StateTransition<SipSMCommand,string>(this, "transition_callingauth_incall_2xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipClient::a2011_callingauth_incall_2xx, 
			s_callingauth, s_incall);
	new StateTransition<SipSMCommand,string>(this, "transition_callingauth_termwait_2xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipClient::a2012_calling_termwait_2xx,
			s_callingauth, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_callingauth_termwait_resp36",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipClient::a2007_callingnoauth_termwait_36,
			s_callingauth, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_callingnoauth_termwait_transporterror",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipClient::a2013_callingnoauth_termwait_transporterror,
			s_callingnoauth, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_callingauth_termwait_cancel",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipClient::a2014_callingauth_termwait_cancel,
			s_callingauth, s_termwait);
	
	
	setCurrentState(s_start);
}


#ifdef IPSEC_SUPPORT
SipDialogVoipClient::SipDialogVoipClient(MRef<SipStack*> stack, MRef<SipDialogConfig*> callconfig, MRef<SipSoftPhoneConfiguration*> pconf, MRef<Session *> mediaSession, string cid, MRef<MsipIpsecAPI *> ipsecSession) : 
		SipDialogVoip(stack,callconfig,pconf,mediaSession,cid,ipsecSession), 
		ipsecSession(ipsecSession)
#else
SipDialogVoipClient::SipDialogVoipClient(MRef<SipStack*> stack, MRef<SipDialogConfig*> callconfig, MRef<SipSoftPhoneConfiguration*> pconf, MRef<Session *> mediaSession, string cid) : 
		SipDialogVoip(stack, callconfig, pconf, mediaSession, cid)  
#endif

{
	setUpStateMachine();
}

SipDialogVoipClient::~SipDialogVoipClient(){	
}


void SipDialogVoipClient::sendInvite(const string &branch){
	//	mdbg << "ERROR: SipDialogVoipClient::sendInvite() UNIMPLEMENTED"<< end;
	
	MRef<SipRequest*> inv;
	string keyAgreementMessage;
	//inv= MRef<SipInvite*>(
	inv = SipRequest::createSipMessageInvite(
			branch,
			dialogState.callId,
			dialogState.remoteUri,
			getDialogConfig()->inherited->sipIdentity->sipDomain,	//TODO: Change API - not sure if proxy or domain
			getDialogConfig()->inherited->sipIdentity->getSipProxy()->sipProxyPort,
			getDialogConfig()->inherited->externalContactIP,
			getDialogConfig()->inherited->getLocalSipPort(phoneconf->useSTUN),
			getDialogConfig()->inherited->sipIdentity->getSipUri(),
			dialogState.seqNo,
			getDialogConfig()->inherited->getTransport(),
			sipStack ) ;

	addRoute( inv );

	/* Get the session description from the Session */
		
	//There might be so that there are no SDP. Check!
	MRef<SdpPacket *> sdp;
	if (mediaSession){
#ifdef ENABLE_TS
		ts.save("getSdpOffer");
#endif
		sdp = mediaSession->getSdpOffer();
#ifdef ENABLE_TS
		ts.save("getSdpOffer");
#endif
		if( !sdp ){
			// FIXME: this most probably means that the
			// creation of the MIKEY message failed, it 
			// should not happen
			merr << "Sdp was NULL in sendInvite" << end;
			return; 
		}
	}
	
	/* Add the latter to the INVITE message */ // If it exists
	

//-------------------------------------------------------------------------------------------------------------//
#ifdef IPSEC_SUPPORT	
	// Create a MIKEY message for IPSEC if stated in the config file.
	MRef<SipMimeContent*> mikey;
	if (getIpsecSession()->required()){
		ts.save("getMikeyIpsecOffer");
		mikey = ipsecSession->getMikeyIpsecOffer();
		ts.save("getMikeyIpsecOffer");
		if (!mikey){
			merr << "Mikey was NULL" << end;
			merr << "Still some errors with IPSEC" << end;
			//return; 
		}
	}
	else
		mikey = NULL;
	MRef<SipMimeContent*> multi;
	if (mikey && mediaSession){
		multi = new SipMimeContent("multipart/mixed");
		multi->addPart(*mikey);
		multi->addPart(*sdp);
		inv->setContent( *multi);
	}
	if (mikey && !mediaSession)
		inv->setContent( *mikey);
	if (!mikey && mediaSession)
		inv->setContent( *sdp );
#else
	inv->setContent( *sdp );
#endif
//-------------------------------------------------------------------------------------------------------------//
	
	inv->getHeaderValueFrom()->setParameter("tag",dialogState.localTag );

//	mdbg << "SipDialogVoipClient::sendInvite(): sending INVITE to transaction"<<end;
//	ts.save( INVITE_END );
	MRef<SipMessage*> pktr(*inv);

	SipSMCommand scmd(
			pktr, 
			SipSMCommand::dialog_layer, 
			SipSMCommand::transaction_layer
			);
	
	dispatcher->enqueueCommand(scmd, HIGH_PRIO_QUEUE/*, PRIO_LAST_IN_QUEUE*/);
	setLastInvite(inv);

}

void SipDialogVoipClient::sendAck(){
	MRef<SipRequest*> ack = createSipMessageAck( getLastInvite() );

	// Send ACKs directly to the transport layer bypassing
	// the transaction layer
	SipSMCommand scmd(
			*ack,
			SipSMCommand::dialog_layer,
			SipSMCommand::transport_layer
			);
	
	dispatcher->enqueueCommand(scmd, HIGH_PRIO_QUEUE);
}

void SipDialogVoipClient::sendPrack(MRef<SipResponse*> rel100resp){
	
	MRef<SipRequest*> prack = createSipMessagePrack( rel100resp );
	sendSipMessage( *prack );
}

void SipDialogVoipClient::sendAuthInvite(const string &branch){
	//	merr << "ERROR: SipDialogVoipClient::sendAuthInvite() UNIMPLEMENTED"<< end;
	MRef<SipRequest*> inv;
	string keyAgreementMessage;

	//merr << "SipDialogVoip::sendAuthInv : dialogstate.remoteUri=" << dialogState.remoteUri << end;

	inv = SipRequest::createSipMessageInvite(
		branch,
		dialogState.callId,
		dialogState.remoteUri,
		getDialogConfig()->inherited->sipIdentity->sipDomain,
		getDialogConfig()->inherited->sipIdentity->getSipProxy()->sipProxyPort,
		getDialogConfig()->inherited->externalContactIP,
		getDialogConfig()->inherited->getLocalSipPort(phoneconf->useSTUN),
		getDialogConfig()->inherited->sipIdentity->getSipUri(),
		dialogState.seqNo,
		getDialogConfig()->inherited->sipIdentity->getSipProxy()->sipProxyUsername,
		nonce,
		realm,
		getDialogConfig()->inherited->sipIdentity->getSipProxy()->sipProxyPassword,
		getDialogConfig()->inherited->getTransport(),
		sipStack);

	addRoute( inv );

	inv->getHeaderValueFrom()->setParameter("tag",dialogState.localTag);

	//There might be so that there are no SDP. Check!
	MRef<SdpPacket *> sdp;
	if (mediaSession){
#ifdef ENABLE_TS
		ts.save("getSdpOffer");
#endif
		sdp = mediaSession->getSdpOffer();
#ifdef ENABLE_TS
		ts.save("getSdpOffer");
#endif
		if( !sdp ){
		// FIXME: this most probably means that the
		// creation of the MIKEY message failed, it 
		// should not happen
		merr << "Sdp was NULL in sendAuthInvite" << end;
		return; 
		}
	}
	
	/* Add the latter to the INVITE message */ // If it exists
	

//-------------------------------------------------------------------------------------------------------------//
#ifdef IPSEC_SUPPORT	
	// Create a MIKEY message for IPSEC if stated in the config file.
	MRef<SipMimeContent*> mikey;
	if (getIpsecSession()->required()){
		ts.save("getMikeyIpsecOffer");
		mikey = ipsecSession->getMikeyIpsecOffer();
		ts.save("getMikeyIpsecOffer");
		if (!mikey){
			merr << "Mikey was NULL" << end;
			merr << "Still some errors with IPSEC" << end;
			//return; 
		}
	}
	else
		mikey = NULL;
	MRef<SipMimeContent*> multi;
	if (mikey && mediaSession){
		multi = new SipMimeContent("multipart/mixed");
		multi->addPart(*mikey);
		multi->addPart(*sdp);
		inv->setContent( *multi);
	}
	if (mikey && !mediaSession)
		inv->setContent( *mikey);
	if (!mikey && mediaSession)
		inv->setContent( *sdp );
#else
	
	inv->setContent( *sdp );
#endif
//-------------------------------------------------------------------------------------------------------------//

	//	/* Get the session description from the Session */
	//	MRef<SdpPacket *> sdp = mediaSession->getSdpOffer();
	//
	//	/* Add the latter to the INVITE message */
	//	inv->setContent( *sdp );
	
	MRef<SipMessage*> pref(*inv);
	SipSMCommand cmd(pref, SipSMCommand::dialog_layer, SipSMCommand::transaction_layer);
	dispatcher->enqueueCommand(cmd, HIGH_PRIO_QUEUE/*, PRIO_LAST_IN_QUEUE*/);
	setLastInvite(inv);

}

void SipDialogVoipClient::sendInviteOk(const string &branch){
	MRef<SipResponse*> ok= new SipResponse(branch, 200,"OK", MRef<SipMessage*>(*getLastInvite()));	
	ok->getHeaderValueTo()->setParameter("tag",dialogState.localTag);
	
	MRef<SipHeaderValue *> contact = 
		new SipHeaderValueContact( 
			getDialogConfig()->inherited->sipIdentity->getSipUri(),
			getDialogConfig()->inherited->externalContactIP,
			getDialogConfig()->inherited->getLocalSipPort(phoneconf->useSTUN),
			"", getDialogConfig()->inherited->getTransport(),
			-1); //set expires to -1, we do not use it (only in register)
	ok->addHeader( new SipHeader(*contact) );
	
	//There might be so that there are no SDP. Check!
	MRef<SdpPacket *> sdp;
	if (mediaSession){
#ifdef ENABLE_TS
		ts.save("getSdpAnswer");
#endif
		sdp = mediaSession->getSdpAnswer();
#ifdef ENABLE_TS
		ts.save("getSdpAnswer");
#endif
		if( !sdp ){
		// FIXME: this most probably means that the
		// creation of the MIKEY message failed, it 
		// should not happen
		merr << "Sdp was NULL in sendInviteOk" << end;
		return; 
		}
	}
	
	/* Add the latter to the INVITE message */ // If it exists
	

//-------------------------------------------------------------------------------------------------------------//
#ifdef IPSEC_SUPPORT	
	// Create a MIKEY message for IPSEC if stated in the config file.
	MRef<SipMimeContent*> mikey;
	if (getIpsecSession()->required()){
		ts.save("getMikeyIpsecAnswer");
		mikey = ipsecSession->getMikeyIpsecAnswer();
		ts.save("getMikeyIpsecAnswer");
		if (!mikey){
			merr << "Mikey was NULL in sendInviteOk" << end;
			merr << "Still some errors with IPSEC" << end;
			//return; 
		}
	}
	else
		mikey = NULL;
	MRef<SipMimeContent*> multi;
	if (mikey && mediaSession){
		multi = new SipMimeContent("multipart/mixed");
		multi->addPart(*mikey);
		multi->addPart(*sdp);
		ok->setContent( *multi);
	}
	if (mikey && !mediaSession)
		ok->setContent( *mikey);
	if (!mikey && mediaSession)
		ok->setContent( *sdp );
#else
	
	ok->setContent( *sdp );
#endif
//-------------------------------------------------------------------------------------------------------------//
//	/* Get the SDP Answer from the MediaSession */
//	MRef<SdpPacket *> sdpAnswer = mediaSession->getSdpAnswer();
//
//	if( sdpAnswer ){
//		ok->setContent( *sdpAnswer );
//	}
//	/* if sdp is NULL, the offer was refused, send 606 */
//	// FIXME
//	else return; 

	MRef<SipMessage*> pref(*ok);
	SipSMCommand cmd( pref, SipSMCommand::dialog_layer, SipSMCommand::transaction_layer);
	dispatcher->enqueueCommand(cmd, HIGH_PRIO_QUEUE/*, PRIO_LAST_IN_QUEUE*/);
}

bool SipDialogVoipClient::handleRel1xx( MRef<SipResponse*> resp ){
	MRef<SipHeaderValue *> value = resp->getHeaderValueNo( SIP_HEADER_TYPE_RSEQ, 0 );

	if( !value )
		return false;

	MRef<SipHeaderValueRSeq *> rseq = dynamic_cast<SipHeaderValueRSeq*>( *value );
	uint32_t rseqNo = rseq->getRSeq();
		
	// First reliable provisional response
	// Next in-order reliable provisional response
	if( !(dialogState.rseqNo == (uint32_t)-1 ||
	      dialogState.rseqNo > rseqNo ) )
		return false;

	dialogState.updateState( resp );
	dialogState.seqNo++;
	dialogState.rseqNo = rseqNo;
	sendPrack(resp);

	return true;
}
