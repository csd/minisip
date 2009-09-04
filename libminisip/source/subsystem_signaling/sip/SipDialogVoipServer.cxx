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
 * 	SipDialogVoipServer.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/
#include<config.h>

#include<libminisip/signaling/sip/SipDialogVoipServer.h>

#include<libmutil/massert.h>

#include<libmsip/SipTransitionUtils.h>
#include<libmsip/SipCommandString.h>
#include<libmsip/SipHeaderWarning.h>
#include<libmsip/SipHeaderContact.h>
#include<libmsip/SipHeaderFrom.h>
#include<libmsip/SipHeaderRAck.h>
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
#include"../..subsytem_media/msrp/MSRPSender.h"

#endif

using namespace std;


/*

 The responsibility of the voip invite server is to accept
 or reject an incoming call request. It should either end
 up in the in_call state or be terminated. The super class,
 SipDialogVoip handles calls after they have been established.

 This class adds two states, start and ringing. The in_call,
 termwait and terminated states are inherited from the super-class
 (they have dotted borders in the diagram below).

 The "dotted" states are implemented in SipDialogVoip.cxx.

 INVITE & !100rel
 a3001:transIR;  +---------------+
       send 180  |               |
      +----------|     start     |
      |          |               |
      |          +---------------+
      |                  |
      |                  | INVITE && supports 100rel
      |                  | a3007:transIR; send 183
      |                  |
      |                  V
      |          +---------------+
      |          |               |
      |          |    100rel     |----------+
      |          |               |          |
      |          +---------------+          |
      |                  | PRACK & 100rel   |
      |                  | a3008: send 200  |
      |                  |        send 180  |
      |                  V                  |
      |          +---------------+          |
      |          |               |          | 
      +--------->|    ringing    |----------+  reject
                 |               |          |  a3005:send40X
                 +---------------+          |
                         |                  |  CANCEL
                         | accept_invite &  |  a3004:transCancelResp
                         | authok           |
		         | a3002: send 200  |  BYE
		         V                  |  a3003:transByeResp
                 +. . . . . . . .+          |   
                 .               .          |  INVITE & a11 fail
                 .   in call     .          |  a3006: send 606
                 .               .          |   
                 +. . . . . . . .+          |
                                            |
                                            |
                                            |
                 +. . . . . . . .+          |
                 .               .          |
                 .   termwait    .<---------+
                 .               .
                 +. . . . . . . .+
                          
                                               
                                                 
                 +. . . . . . . .+
                 .               .
                 .  terminated   .
                 .               .
                 + . . . . . . . +


                 +---------------+
           +---->|               |<----+  ResendTimer1xx
           |     |      ANY      |     |  a3009: resend 1XX
           +-----|               |-----+
 PRACK & 100rel  +---------------+
 a3010: send 200 &
        cancel timer
   
*/

bool SipDialogVoipServer::a3007_start_100rel_INVITE( const SipSMCommand &command){
	if( !transitionMatch("INVITE", 
			     command, 
			     SipSMCommand::transaction_layer, 
			     SipSMCommand::dialog_layer) ||
	    !getSipStack()->getStackConfig()->use100Rel ){
		return false;
	}
		
	MRef<SipRequest*> inv = (SipRequest *)*command.getCommandPacket();

	if( rejectUnsupported( inv ) ){
		// Unsupported extension(s)
		return true;
	}

	if( !inv->supported("100rel") && !inv->requires("100rel") ){
		return false;
	}

	use100Rel = true;
	resendTimer1xx=getSipStack()->getTimers()->getA();
	requestTimeout(resendTimer1xx,"ResendTimer1xx");
	dialogState.rseqNo = rand() % (1<<31);
		
	setLastInvite(inv);
	dialogState.updateState( getLastInvite() );

	// Build peer uri used for authentication from remote uri,
	// but containing user and host only.
	SipUri peer(dialogState.remoteUri);
	string peerUri = peer.getProtocolId() + ":" + peer.getUserIpString();
		
	if(!sortMIME(*command.getCommandPacket()->getContent(), peerUri, 10)){
		merr << "No MIME match" << endl;
		return false;
	}

	sendSessionProgress();

	return true;
}

bool SipDialogVoipServer::a3001_start_ringing_INVITE( const SipSMCommand &command)
{
	if (transitionMatch("INVITE", command, SipSMCommand::transaction_layer, SipSMCommand::dialog_layer)){
		MRef<SipRequest*> inv = (SipRequest *)*command.getCommandPacket();
		
		if( inv->requires("100rel") ){
			// TODO reject unsupported extension
			return false;
		}

		setLastInvite(inv);
		dialogState.updateState( inv );

		// Build peer uri used for authentication from remote uri,
		// but containing user and host only.
		SipUri peer(dialogState.remoteUri);
		string peerUri = peer.getProtocolId() + ":" + peer.getUserIpString();
		
		if(!sortMIME(*inv->getContent(), peerUri, 10)){
			merr << "No MIME match" << endl;
			return false;
		}
		
/*		MRef<SipHeaderValue*> identity = command.getCommandPacket()->getHeaderValueNo(SIP_HEADER_TYPE_IDENTITY, 0);
		MRef<SipHeaderValue*> identityinfo = command.getCommandPacket()->getHeaderValueNo(SIP_HEADER_TYPE_IDENTITYINFO, 0);
		
		bool identityVerified=false;
		if (identity && identityinfo){
			cerr << "IDENTITY: found identity and identity-info header values"<< endl;
			assert(dynamic_cast<SipHeaderValueIdentity*>( *identity));
			assert(dynamic_cast<SipHeaderValueIdentityInfo*>( *identityinfo));
			MRef<SipHeaderValueIdentity*> ident = (SipHeaderValueIdentity*) *identity;
			MRef<SipHeaderValueIdentity*> identinfo = (SipHeaderValueIdentity*) *identityinfo;

			cerr << "IDENTITY: algorithm is: <"<< identinfo->getParameter("alg") << ">"<< endl;
			
			//downloadCertificate( identinfo->getCertUri() );
			
			identityVerified = verifyIdentityHeader(ident);


			//TODO: check that the identity is rsa-sha1
			
			if (!identityVerified){
#ifdef DEBUG_OUTPUT
				cerr << "IDENTITY: the verification FAILED!"<< endl;
#endif
			}
			
			
		}else{
			cerr << "IDENTITY: did not find identity header value"<< endl;
		}
*/		
		CommandString cmdstr(dialogState.callId, 
				SipCommandString::incoming_available, 
				     getMediaSession()->getPeerUri(),
				(getMediaSession()->isSecure()?"secure":"unprotected")
				);
		getSipStack()->getCallback()->handleCommand("gui", cmdstr );

		sendRinging();
		
		if( getSipStack()->getStackConfig()->autoAnswer ){
			CommandString accept( dialogState.callId, SipCommandString::accept_invite );
			SipSMCommand sipcmd(accept, SipSMCommand::dialog_layer, SipSMCommand::dialog_layer);
			getSipStack()->enqueueCommand(sipcmd,HIGH_PRIO_QUEUE);
		}
		return true;
	}else{
		return false;
	}
}

bool SipDialogVoipServer::a3002_ringing_incall_accept( const SipSMCommand &command)
{
	if (transitionMatch(command, 
				SipCommandString::accept_invite,
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer)){
#ifdef ENABLE_TS
		ts.save(USER_ACCEPT);
#endif

		CommandString cmdstr(dialogState.callId, 
				SipCommandString::invite_ok,
				     getMediaSession()->getPeerUri(),
				(getMediaSession()->isSecure()?"secure":"unprotected")
				);
		getSipStack()->getCallback()->handleCommand("gui", cmdstr );

		massert( !getLastInvite().isNull() );
		sendInviteOk();

		getMediaSession()->start();

		MRef<LogEntry *> log= new LogEntryIncomingCompletedCall();

		log->start = time( NULL );
		log->peerSipUri = getLastInvite()->getFrom().getString();
		
		setLogEntry( log );
		
		return true;
	}else{
		return false;
	}
}

bool SipDialogVoipServer::a3003_ringing_termwait_BYE( const SipSMCommand &command)
{
	
	if (transitionMatch("BYE", 
				command, 
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer) &&
	    dialogState.remoteTag != ""){
		MRef<SipRequest*> bye = (SipRequest*) *command.getCommandPacket();

		if( getLogEntry() ){
			((LogEntrySuccess *)(*( getLogEntry() )))->duration = 
			time( NULL ) - getLogEntry()->start; 

			getLogEntry()->handle();
		}

		sendByeOk( bye );

		CommandString cmdstr(dialogState.callId, SipCommandString::remote_hang_up);
		getSipStack()->getCallback()->handleCommand("gui", cmdstr);

		getMediaSession()->stop();

		
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}

bool SipDialogVoipServer::a3004_ringing_termwait_CANCEL( const SipSMCommand &command)
{

	if (transitionMatch("CANCEL", command, SipSMCommand::transaction_layer,
			    SipSMCommand::dialog_layer)) {
		MRef<SipRequest*> cancel =
			(SipRequest*)*command.getCommandPacket();

///		const string branch = cancel->getFirstViaBranch();
///
///		if (getLastInvite()->getFirstViaBranch() != branch)
///			return false;

		// Send 487 Request Cancelled for INVITE
		MRef<SipResponse*> cancelledResp = new SipResponse( 
				487,
				"Request Cancelled", 
				*getLastInvite() 
				);
		cancelledResp->getHeaderValueTo()->setParameter("tag",dialogState.localTag);
		MRef<SipMessage*> cancelledMsg(*cancelledResp);
		SipSMCommand cancelledCmd( cancelledMsg, SipSMCommand::dialog_layer,
				  SipSMCommand::transaction_layer);
		getSipStack()->enqueueCommand(cancelledCmd, HIGH_PRIO_QUEUE);

		// Send 200 OK for CANCEL
		MRef<SipResponse*> okResp = new SipResponse( 200,"OK", cancel );
		okResp->getHeaderValueTo()->setParameter("tag",dialogState.localTag);
		MRef<SipMessage*> okMsg(*okResp);
		SipSMCommand okCmd( okMsg, SipSMCommand::dialog_layer,
				  SipSMCommand::transaction_layer);
		//cr->handleCommand(okCmd);
		getSipStack()->enqueueCommand(okCmd, HIGH_PRIO_QUEUE);

		/* Tell the GUI */
		CommandString cmdstr(dialogState.callId, SipCommandString::remote_cancelled_invite,"");
		getSipStack()->getCallback()->handleCommand("gui", cmdstr );

		getMediaSession()->stop();
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}

bool SipDialogVoipServer::a3005_ringing_termwait_reject( const SipSMCommand &command)
{
	
	if (		transitionMatch(command, 
				SipCommandString::reject_invite,
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer) || 
			transitionMatch(command,
				SipCommandString::hang_up,
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer)){


		cerr <<"EEEE: detected reject"<<endl;
		sendReject();

		getMediaSession()->stop();
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}

//In case a3001 does not accept the invite
bool SipDialogVoipServer::a3006_start_termwait_INVITE( const SipSMCommand &command)
{	
	if (transitionMatch("INVITE", command, SipSMCommand::transaction_layer, SipSMCommand::dialog_layer)){

		setLastInvite(MRef<SipRequest*>((SipRequest *)*command.getCommandPacket()));
		
		dialogState.updateState( getLastInvite() );

		sendNotAcceptable( );

		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}

bool SipDialogVoipServer::isMatchingPrack( MRef<SipMessage*> provisional,
					   MRef<SipRequest*> prack ){
	MRef<SipHeaderValue *> value = prack->getHeaderValueNo( SIP_HEADER_TYPE_RACK, 0 );
	if( !value ){
		// TODO reject 481
		cerr << "No RAck" << endl;
		return false;
	}

	MRef<SipHeaderValueRAck *> rack = dynamic_cast<SipHeaderValueRAck*>(*value);
	if( !rack ){
		// TODO reject 481
		cerr << "Bad RAck" << endl;
		return false;
	}

	if( rack->getMethod() != provisional->getCSeqMethod() ||
	    rack->getCSeqNum() != provisional->getCSeq() ||
	    (unsigned)rack->getResponseNum() != dialogState.rseqNo ){
		// TODO reject 481
		cerr << "Non matching RAck" << endl;
		return false;
	}

	return true;
}

bool SipDialogVoipServer::a3008_100rel_ringing_PRACK( const SipSMCommand &command){
	if( ! (use100Rel &&
	       lastProvisional &&
	       transitionMatch("PRACK", 
			     command, 
			     SipSMCommand::transaction_layer, 
			     SipSMCommand::dialog_layer) ) ){
		return false;
	}

	MRef<SipRequest*> prack =
		dynamic_cast<SipRequest *>(*command.getCommandPacket());
	dialogState.updateState( prack );

	if( !isMatchingPrack( lastProvisional, prack ) ){
		return false;
	}

// 	cerr << "RAck ok, send 200 Ok" << endl;

	lastProvisional = NULL;
	sendPrackOk( prack );
		
	CommandString cmdstr(dialogState.callId, 
			     SipCommandString::incoming_available, 
			     getMediaSession()->getPeerUri(),
			     (getMediaSession()->isSecure()?"secure":"unprotected")
		);
	getSipStack()->getCallback()->handleCommand("gui", cmdstr );

	sendRinging();
		
	return true;
}

bool SipDialogVoipServer::a3009_any_any_ResendTimer1xx( const SipSMCommand &command){
	if( !transitionMatch(command, 
			     "ResendTimer1xx", 
			     SipSMCommand::dialog_layer, 
			     SipSMCommand::dialog_layer) ){
		return false;
	}

	if( !lastProvisional ){
		// Stop retransmissions.
		return true;
	}

	resendTimer1xx *=2;

	// Stop retransmissions after 64*T1
	if( resendTimer1xx >= 64 * getSipStack()->getTimers()->getA() ){
		MRef<SipResponse*> reject =
			createSipResponse( getLastInvite(), 504,
					   "Server Time-out" );
		sendSipMessage( *reject );
		CommandString cmdstr(dialogState.callId, SipCommandString::remote_cancelled_invite,"");
		getSipStack()->getCallback()->handleCommand("gui", cmdstr );

		getMediaSession()->stop();
		signalIfNoTransactions();
		return true;
	}

	sendSipMessage( lastProvisional );
	requestTimeout(resendTimer1xx,"ResendTimer1xx");
	return true;
}

bool SipDialogVoipServer::a3010_any_any_PRACK( const SipSMCommand &command){
	if( ! (use100Rel &&
	       transitionMatch("PRACK", 
			     command, 
			     SipSMCommand::transaction_layer, 
			     SipSMCommand::dialog_layer) ) ){
		return false;
	}
		
	MRef<SipRequest*> prack =
		dynamic_cast<SipRequest *>(*command.getCommandPacket());
	dialogState.updateState( prack );

	if( !isMatchingPrack( lastProvisional, prack ) ){
		return false;
	}
		
// 	cerr << "RAck ok, send 200 Ok" << endl;

	lastProvisional = NULL;
	sendPrackOk( prack );

	return true;
}

void SipDialogVoipServer::setUpStateMachine(){

	State<SipSMCommand,string> *s_start=new State<SipSMCommand,string>(this,"start");
	addState(s_start);

	State<SipSMCommand,string> *s_100rel=new State<SipSMCommand,string>(this,"100rel");
	addState(s_100rel);

	State<SipSMCommand,string> *s_ringing=new State<SipSMCommand,string>(this,"ringing");
	addState(s_ringing);

	MRef<State<SipSMCommand,string> *> s_incall = getState("incall");
	MRef<State<SipSMCommand,string> *> s_termwait= getState("termwait");
	MRef<State<SipSMCommand,string> *> s_any = anyState;


	new StateTransition<SipSMCommand,string>(this, "transition_start_100rel_INVITE",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipServer::a3007_start_100rel_INVITE,
			s_start, s_100rel);

	// Fallback to unreliable provisinal responses
	new StateTransition<SipSMCommand,string>(this, "transition_start_ringing_INVITE",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipServer::a3001_start_ringing_INVITE,
			s_start, s_ringing);

	new StateTransition<SipSMCommand,string>(this, "transition_ringing_incall_accept",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipServer::a3002_ringing_incall_accept,
			s_ringing, s_incall);

	new StateTransition<SipSMCommand,string>(this, "transition_incall_termwait_BYE",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipServer::a3003_ringing_termwait_BYE,
			s_ringing, s_termwait); 

	new StateTransition<SipSMCommand,string>(this, "transition_ringing_termwait_CANCEL",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipServer::a3004_ringing_termwait_CANCEL,
			s_ringing, s_termwait);
	
	new StateTransition<SipSMCommand,string>(this, "transition_ringing_termwait_reject",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipServer::a3005_ringing_termwait_reject,
			s_ringing, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_start_termwait_INVITEnothandled",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipServer::a3006_start_termwait_INVITE,
			s_start, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_100rel_ringing_PRACK",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipServer::a3008_100rel_ringing_PRACK,
			s_100rel, s_ringing);

	new StateTransition<SipSMCommand,string>(this, "transition_100rel_100rel_ResendTimer1xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipServer::a3009_any_any_ResendTimer1xx,
			s_any, s_any);

	new StateTransition<SipSMCommand,string>(this, "transition_ringing_ringing_PRACK",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipServer::a3010_any_any_PRACK,
			s_any, s_any);

	// 100rel -> termwait
	new StateTransition<SipSMCommand,string>(this, "transition_100rel_termwait_CANCEL",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipServer::a3004_ringing_termwait_CANCEL,
			s_100rel, s_termwait);
	
	new StateTransition<SipSMCommand,string>(this, "transition_100rel_termwait_reject",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipServer::a3005_ringing_termwait_reject,
			s_100rel, s_termwait);

	setCurrentState(s_start);
}


SipDialogVoipServer::SipDialogVoipServer(MRef<SipStack*> stack, MRef<SipIdentity*> ident, bool stun, MRef<Session *> s, string cid) : 
		SipDialogVoip(stack, ident, /*pconf*/ stun, s, cid),
		use100Rel( false ), resendTimer1xx( 0 )
{
	setUpStateMachine();
}

SipDialogVoipServer::~SipDialogVoipServer(){	
	mediaSession->unregister();
}


void SipDialogVoipServer::sendInviteOk(){
	MRef<SipResponse*> ok= new SipResponse(200,"OK", getLastInvite() );	
	ok->getHeaderValueTo()->setParameter("tag",dialogState.localTag);
	
	MRef<SipHeaderValue *> contact = 
		new SipHeaderValueContact( 
			getDialogConfig()->getContactUri(useStun),
			-1); //set expires to -1, we do not use it (only in register)
	ok->addHeader( new SipHeader(*contact) );

	if( !use100Rel ){

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
	}

	MRef<SipMessage*> pref(*ok);
	SipSMCommand cmd( pref, SipSMCommand::dialog_layer, SipSMCommand::transaction_layer);
	getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE );
}

void SipDialogVoipServer::sendReject(){
	MRef<SipResponse*> ringing = new SipResponse(486,"Temporary unavailable", getLastInvite());	
	ringing->getHeaderValueTo()->setParameter("tag",dialogState.localTag);
	MRef<SipMessage*> pref(*ringing);
	SipSMCommand cmd( pref,SipSMCommand::dialog_layer, SipSMCommand::transaction_layer);
	getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE );
}

void SipDialogVoipServer::sendRinging(){
  MRef<SipResponse*> ringing =
	  createSipResponse( getLastInvite(), 180, "Ringing" );

	if (use100Rel){
		ringing->addHeader(new SipHeader(new SipHeaderValueRequire("100rel")));
		dialogState.rseqNo++;
		ringing->addHeader(new SipHeader(new SipHeaderValueRSeq( dialogState.rseqNo )));
		lastProvisional = *ringing;
	}
	
	MRef<SipHeaderValue *> contact = 
		new SipHeaderValueContact( 
			getDialogConfig()->getContactUri(useStun),
			-1); //set expires to -1, we do not use it (only in register)
	ringing->addHeader( new SipHeader(*contact) );

	sendSipMessage( *ringing );
}

void SipDialogVoipServer::sendNotAcceptable(){
	MRef<SipResponse*> not_acceptable = new SipResponse(406,"Not Acceptable", getLastInvite());	
	if( mediaSession && mediaSession->getErrorString() != "" ){
		not_acceptable->addHeader( 
			new SipHeader(
				new SipHeaderValueWarning(
					getSipStack()->getStackConfig()->externalContactIP, 
					399, 
					mediaSession->getErrorString() ) ));
	}

	not_acceptable->getHeaderValueTo()->setParameter("tag",dialogState.localTag);
	MRef<SipMessage*> pref(*not_acceptable);
	SipSMCommand cmd( pref, SipSMCommand::dialog_layer, SipSMCommand::transaction_layer);
	getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE );
}

void SipDialogVoipServer::sendPrackOk( MRef<SipRequest*> prack ){
	MRef<SipResponse*> ok = createSipResponse( prack, 200, "OK" );
	MRef<SipHeaderValue *> contact = 
		new SipHeaderValueContact( 
			getDialogConfig()->getContactUri(useStun),
			-1); //set expires to -1, we do not use it (only in register)
	ok->addHeader( new SipHeader(*contact) );

	sendSipMessage( *ok );
}

void SipDialogVoipServer::sendSessionProgress(){
	MRef<SipResponse*> progress =
		createSipResponse( getLastInvite(), 183,"Session progress" );

	progress->addHeader(new SipHeader(new SipHeaderValueRequire("100rel")));

	dialogState.rseqNo++;
	progress->addHeader(new SipHeader(new SipHeaderValueRSeq( dialogState.rseqNo )));
	
	MRef<SipHeaderValue *> contact = 
		new SipHeaderValueContact( 
			getDialogConfig()->getContactUri(useStun),
			-1); //set expires to -1, we do not use it (only in register)
	progress->addHeader( new SipHeader(*contact) );

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
	progress->setContent( *sdp );

	lastProvisional = *progress;

	sendSipMessage( lastProvisional );
}

