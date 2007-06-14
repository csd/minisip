/*
 Copyright (C) 2004-2007 the Minisip Team
 
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

/* Copyright (C) 2004-2007
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

#include<libminisip/signaling/sip/SipDialogVoipClient.h>

#include<libmutil/massert.h>

#include<libmsip/SipTransitionUtils.h>
#include<libmsip/SipCommandString.h>
#include<libmsip/SipHeaderWarning.h>
#include<libmsip/SipHeaderContact.h>
#include<libmsip/SipHeaderFrom.h>
#include<libmsip/SipHeaderRoute.h>
#include<libmsip/SipHeaderRequire.h>
#include<libmsip/SipHeaderRSeq.h>
#include<libmsip/SipHeaderTo.h>
#include<libmsip/SipMessageContentMime.h>
#include<libmsip/SipMessageContent.h>
#include<libmutil/stringutils.h>
#include<libmcrypto/base64.h>
#include<libmutil/Timestamp.h>
#include<libmutil/termmanip.h>
#include<libmutil/dbg.h>
#include<libmsip/SipSMCommand.h>
#include <time.h>
#include<libminisip/gui/LogEntry.h>

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
  |     1xx |    |    calling    |----+ 180
  |   a2003 +--->|               |    | a2002: gui(ringing)
  +--------------+---------------+<---+
  |                      |  |  ^
  |                      |  |  | 401/407
  |                      |  +--+ a2008: auth
  |                      |
  |                      | 2xx
  |                      | a2004: send ACK
  |                      v        gui(invite_ok)
  |              +. . . . . . . .+
  |              .               .
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

bool SipDialogVoipClient::a2001_start_calling_invite( const SipSMCommand &command)
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

		sendInvite();

		return true;
	}else{
		return false;
	}
}

bool SipDialogVoipClient::a2002_calling_calling_18X( const SipSMCommand &command)
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
		bool doPrack = false;
		if (resp->requires("100rel") && resp->getStatusCode()!=100){
			if( !handleRel1xx( resp ) ){
				// Ignore retransmission
// 				cerr << "Ignore 18x retransmission" << endl;
				return true;
			}
			doPrack = true;
		}
		else{
			//We must maintain the dialog state. 
			dialogState.updateState( resp );
		}

		// Build peer uri used for authentication from remote uri,
		// but containing user and host only.
		SipUri peer(dialogState.remoteUri);
		string peerUri = peer.getProtocolId() + ":" + peer.getUserIpString();
		MRef<SipMessageContent *> content = resp->getContent();
		if( !content.isNull() ){
			MRef<SdpPacket*> sdp((SdpPacket*)*content);
			//Early media	
  			if( !sortMIME( content , peerUri, 3) ){
				// MIKEY failed
				// TODO reason header
				sendCancel();

				getMediaSession()->stop();
				signalIfNoTransactions();
				// Skip prack
// 				cerr << "Send cancel, skip prack" << endl;
				return true;
			}
		}

		if( doPrack )
			sendPrack(resp);

		if( resp->getStatusCode() == 180 ){
			CommandString cmdstr(dialogState.callId, SipCommandString::remote_ringing, getMediaSession()->getPeerUri(), (getMediaSession()->isSecure()?"secure":"unprotected"));
			getSipStack()->getCallback()->handleCommand("gui", cmdstr);
		}
	
		return true;
	}else{
		return false;
	}
}


bool SipDialogVoipClient::a2003_calling_calling_1xx( const SipSMCommand &command)
{

	if (transitionMatchSipResponse("INVITE", command, SipSMCommand::transaction_layer, SipSMCommand::dialog_layer, "1**")){
		dialogState.updateState( MRef<SipResponse*>((SipResponse *)*command.getCommandPacket()) );

		MRef<SipResponse*> resp = (SipResponse*)*command.getCommandPacket();
		if (resp->requires("100rel") && resp->getStatusCode()!=100){
			if( !handleRel1xx( resp ) ){
				// Ignore retransmission
// 				cerr << "Ignore 1xx retransmission" << endl;
				return true;
			}

			sendPrack(resp);
		}

		return true;
	}else{
		return false;
	}
    
}

bool SipDialogVoipClient::a2004_calling_incall_2xx( const SipSMCommand &command)
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
		
		// Build peer uri used for authentication from remote uri,
		// but containing user and host only.
		SipUri peer(dialogState.remoteUri);
		string peerUri = peer.getProtocolId() + ":" + peer.getUserIpString();
		dialogState.updateState( resp );
		sendAck();
		
		setLogEntry( new LogEntryOutgoingCompletedCall() );
		getLogEntry()->start = time( NULL );
		getLogEntry()->peerSipUri = peerUri;


//FIXME: CESC: for now, route set is updated at the transaction layer		
		
		bool ret = sortMIME(*resp->getContent(), peerUri, 3);
		if( !ret ) {
			// Fall through to a2012 terminating the
			// call and sending security_failed to the gui
			return false;
		}

		CommandString cmdstr(dialogState.callId, SipCommandString::invite_ok, getMediaSession()->getPeerUri(),
							(getMediaSession()->isSecure()?"secure":"unprotected"));
		
		getSipStack()->getCallback()->handleCommand("gui", cmdstr);
		
		return true;
	}else{
		return false;
	}
}

bool SipDialogVoipClient::a2005_calling_termwait_CANCEL( const SipSMCommand &command)
{
	if (transitionMatch("CANCEL", command, SipSMCommand::transaction_layer, SipSMCommand::dialog_layer)){
		getMediaSession()->stop();
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}

bool SipDialogVoipClient::a2006_calling_termwait_cancel( const SipSMCommand &command)
{
	if (		transitionMatch(command, 
				SipCommandString::cancel,
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer) 
			|| transitionMatch(command, 
				SipCommandString::hang_up,
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer)){

 		sendCancel();

		getMediaSession()->stop();
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}

bool SipDialogVoipClient::a2007_calling_termwait_36( const SipSMCommand &command)
{
	if (transitionMatchSipResponse("INVITE", command, SipSMCommand::transaction_layer, SipSMCommand::dialog_layer, "3**\n4**\n5**\n6**")){
		

		dialogState.isTerminated=true;

		MRef<LogEntry *> rejectedLog( new LogEntryCallRejected() );
		rejectedLog->start = time( NULL );
		rejectedLog->peerSipUri = dialogState.remoteTag;

		MRef<SipResponse*> resp = (SipResponse*)*command.getCommandPacket();

		if (sipResponseFilterMatch(resp,"404")){
                        CommandString cmdstr(dialogState.callId, SipCommandString::remote_user_not_found);
			assert(getSipStack());
			assert(getSipStack()->getCallback());
			getSipStack()->getCallback()->handleCommand("gui", cmdstr);
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
			getSipStack()->getCallback()->handleCommand( "gui", cmdstr );
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
			getSipStack()->getCallback()->handleCommand( "gui",cmdstr );
		}
		
		getMediaSession()->stop();
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}

bool SipDialogVoipClient::a2008_calling_calling_40X( const SipSMCommand &command){

	if (transitionMatchSipResponse("INVITE", command,SipSMCommand::transaction_layer, SipSMCommand::dialog_layer, "407\n401")){
		
		MRef<SipResponse*> resp( (SipResponse*)*command.getCommandPacket() );

		dialogState.updateState( resp ); //nothing will happen ... 4xx responses do not update ...

		if( !updateAuthentications( resp ) ){
			// Fall through to a2007_calling_termwait_36
			return false;
		}

		++dialogState.seqNo;
		sendInvite();

		return true;
	}else{
		return false;
	}
}

bool SipDialogVoipClient::a2012_calling_termwait_2xx( const SipSMCommand &command){
	
	if (transitionMatchSipResponse("INVITE", command, SipSMCommand::transaction_layer, SipSMCommand::dialog_layer, "2**")){

		++dialogState.seqNo;

		sendBye(dialogState.seqNo);

		CommandString cmdstr(dialogState.callId, SipCommandString::security_failed);
		getSipStack()->getCallback()->handleCommand("gui", cmdstr);

		getMediaSession()->stop();
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}

bool SipDialogVoipClient::a2013_calling_termwait_transporterror( const SipSMCommand &command){
	if (transitionMatch(command, 
				SipCommandString::transport_error,
				SipSMCommand::transaction_layer,
				SipSMCommand::dialog_layer )){
		CommandString cmdstr(dialogState.callId, SipCommandString::transport_error);
		getSipStack()->getCallback()->handleCommand("gui",cmdstr);
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

	State<SipSMCommand,string> *s_calling=new State<SipSMCommand,string>(this,"calling");
	addState(s_calling);

	MRef<State<SipSMCommand,string> *> s_incall = getState("incall");
	MRef<State<SipSMCommand,string> *> s_termwait= getState("termwait");

	new StateTransition<SipSMCommand,string>(this, "transition_any_any_2XX",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipClient::a2017_any_any_2XX,
			anyState, anyState);




	new StateTransition<SipSMCommand,string>(this, "transition_start_calling_invite",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipClient::a2001_start_calling_invite, 
			s_start, s_calling);

	new StateTransition<SipSMCommand,string>(this, "transition_calling_calling_18X",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipClient::a2002_calling_calling_18X, 
			s_calling, s_calling);

	new StateTransition<SipSMCommand,string>(this, "transition_calling_calling_1xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipClient::a2003_calling_calling_1xx, 
			s_calling, s_calling);

	new StateTransition<SipSMCommand,string>(this, "transition_calling_incall_2xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipClient::a2004_calling_incall_2xx, 
			s_calling, s_incall);

	// Must be added after the calling->incall transition since this is
	// the "fallback one" if we don't accept the 2XX reply (for example
	// authentication error)
	new StateTransition<SipSMCommand,string>(this, "transition_calling_termwait_2xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipClient::a2012_calling_termwait_2xx,
			s_calling, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_calling_termwait_CANCEL",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipClient::a2005_calling_termwait_CANCEL,
			s_calling, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_calling_termwait_cancel",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipClient::a2006_calling_termwait_cancel,
			s_calling, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_calling_calling_40X",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipClient::a2008_calling_calling_40X,
			s_calling, s_calling);

	new StateTransition<SipSMCommand,string>(this, "transition_calling_termwait_36",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipClient::a2007_calling_termwait_36,
			s_calling, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_calling_termwait_transporterror",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipClient::a2013_calling_termwait_transporterror,
			s_calling, s_termwait);
	
	
	setCurrentState(s_start);
}


SipDialogVoipClient::SipDialogVoipClient(MRef<SipStack*> stack, MRef<SipIdentity*> ident, bool stun, bool anat, MRef<Session *> s, string cid) : 
		SipDialogVoip(stack, ident, stun, s, cid),
		useAnat(anat)
{
	setUpStateMachine();
}

SipDialogVoipClient::~SipDialogVoipClient(){	
}


void SipDialogVoipClient::sendInvite(){
	MRef<SipRequest*> inv;
	string keyAgreementMessage;
	inv = SipRequest::createSipMessageInvite(
			dialogState.callId,
			SipUri(dialogState.remoteUri),
			getDialogConfig()->sipIdentity->getSipUri(),
			getDialogConfig()->getContactUri(useStun),
			dialogState.seqNo,
			getSipStack() ) ;

	addAuthorizations( inv );
	addRoute( inv );

	/* Get the session description from the Session */
		
	//There might be so that there are no SDP. Check!
	MRef<SdpPacket *> sdp;
	if (mediaSession){
		// Build peer uri used for authentication from remote uri,
		// but containing user and host only.
		SipUri peer(dialogState.remoteUri);
		string peerUri = peer.getProtocolId() + ":" + peer.getUserIpString();
#ifdef ENABLE_TS
		ts.save("getSdpOffer");
#endif
		bool anat =  useAnat;
		sdp = mediaSession->getSdpOffer( peerUri, anat );
#ifdef ENABLE_TS
		ts.save("getSdpOffer");
#endif
		if( !sdp ){
			// FIXME: this most probably means that the
			// creation of the MIKEY message failed, it 
			// should not happen
			merr << "Sdp was NULL in sendInvite" << endl;
			return; 
		}

		if( !sdp->getSessionLevelAttribute( "group" ).empty() ){
			inv->addHeader(new SipHeader(new SipHeaderValueRequire("sdp-anat")));
		}
	}
	
	/* Add the latter to the INVITE message */ // If it exists
	

//-------------------------------------------------------------------------------------------------------------//
	inv->setContent( *sdp );
//-------------------------------------------------------------------------------------------------------------//
	
	inv->getHeaderValueFrom()->setParameter("tag",dialogState.localTag );

//	ts.save( INVITE_END );
	MRef<SipMessage*> pktr(*inv);

	SipSMCommand scmd(
			pktr, 
			SipSMCommand::dialog_layer, 
			SipSMCommand::transaction_layer
			);
	
	getSipStack()->enqueueCommand(scmd, HIGH_PRIO_QUEUE);
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
	
	getSipStack()->enqueueCommand(scmd, HIGH_PRIO_QUEUE);
}

void SipDialogVoipClient::sendPrack(MRef<SipResponse*> rel100resp){
	
	MRef<SipRequest*> prack = createSipMessagePrack( rel100resp );
	sendSipMessage( *prack );
}

void SipDialogVoipClient::sendInviteOk(){
	MRef<SipResponse*> ok= new SipResponse(200,"OK", getLastInvite());	
	ok->getHeaderValueTo()->setParameter("tag",dialogState.localTag);
	
	MRef<SipHeaderValue *> contact = 
		new SipHeaderValueContact( 
			getDialogConfig()->getContactUri(useStun),
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
		merr << "Sdp was NULL in sendInviteOk" << endl;
		return; 
		}
	}
	
	/* Add the latter to the INVITE message */ // If it exists
	

//-------------------------------------------------------------------------------------------------------------//
	ok->setContent( *sdp );
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
	getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE);
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
	      rseqNo > dialogState.rseqNo ) )
		return false;

	dialogState.updateState( resp );
	dialogState.seqNo++;
	dialogState.rseqNo = rseqNo;

	return true;
}

