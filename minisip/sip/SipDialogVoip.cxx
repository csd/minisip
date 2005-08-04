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
 *	    Joachim Orrblad <joachim[at]orrblad.com>
*/

/* Name
 * 	SipDialogVoip.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<assert.h>
#include"SipDialogVoip.h"
#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipResponse.h>
#include<libmsip/SipBye.h>
#include<libmsip/SipCancel.h>
#include<libmsip/SipRefer.h>
#include<libmsip/SipAck.h>
#include<libmsip/SipMessageTransport.h>
#include<libmsip/SipTransactionInviteClientUA.h>
#include<libmsip/SipTransactionInviteServerUA.h>
#include<libmsip/SipTransactionNonInviteClient.h>
#include<libmsip/SipTransactionNonInviteServer.h>
#include<libmsip/SipTransactionUtils.h>
//#include<libmsip/SipDialog.h>
#include<libmsip/SipCommandString.h>
#include<libmsip/SipHeaderWarning.h>
#include<libmsip/SipHeaderContact.h>
#include<libmsip/SipHeaderRoute.h>
#include<libmsip/SipMIMEContent.h>
#include<libmsip/SipMessageContent.h>
//#include"DefaultDialogHandler.h"
#include<libmutil/itoa.h>
#include<libmutil/Timestamp.h>
#include<libmutil/termmanip.h>
#include<libmutil/dbg.h>
#include<libmsip/SipSMCommand.h>
#include <time.h>
#include"../minisip/LogEntry.h"

//#include"../mediahandler/MediaHandler.h"

#include <iostream>
#include<time.h>

using namespace std;


/*
                                                             a13:reject/send40X
                                                                  +------------+
                                                                  |            |
                 +---------------+   INVITE           +--------------+CANCEL   |
                 |               |   a10:transIR; 180 |              |a12:tCncl|
                 |     start     |------------------->|    ringing   |---------+
                 |               |                    |              |         |
a26transport_err +---------------+                    +--------------+         |
gui(failed)              |                                    |                |
  +----------------+     | invite                             |                |
  |                |     V a0: new TransInvite                |                |
  |              +---------------+                            |                |
  |         +----|               |----+                       |                |
  |     1xx |    |Calling_noauth |    | 180                   |                |
  |a2:(null)+--->|               |<---+ a1: gui(ringing)      |                |
  +--------------+---------------+                            |                |
  |                X   ^ |     |       2xx/a3: [Xsend ACKX]   |                |
  |    [XACK/a15X] +---+ |     +------------------------------+                |
  |                      |                                    |                |
  +----------------+     |  40X                               | accept_invite  |
  |                |     V  a20: send_auth                    | a11: send 200  |
  |              +---------------+                            |                |
  |         +----|               |----+                       |                |
  |     1xx |    |Calling_stored |    | 180                   |                |
  |a22:(nul)+--->|               |<---+ a21: gui(ringing)     |                |
  |              +---------------+                            |                |
  |                      |                                    |                |
  |                      | 2xx                                |                |
  |   refer              v a23: [Xsend ACKX]                  |                |
  |   a27        +---------------+                            |                |
  |  (1)---------|               |<---------------------------+                |
  |              |   in call     |-------+                                     |
  |     -------->|               |       |                                     |
  |              +---------------+       |                                     |
  |                      |               | bye                                 |
  |cancel                | BYE           | a6:TransByeInit                     |
  |a8/a: new TrnsCncl    V a5:new ByeResp|                                     |
  +------------->+---------------+       |                                     |
  |3-699/a9:gui()|               |       |                                     |
  +------------->|   termwait    |<------+-------------------------------------+
  |              |               |
  +------------->+---------------+
                         |
                         | a25: no_transactions
                         V phone(call_terminated)
                 +---------------+
                 |               |
                 |  terminated   |
                 |               |
                 +---------------+
   
   
   CANCEL
   a7:TrnsCnclRsp

   CALL TRANSFER init:

                                      refer
                 +---------------+    a27 new TrsnReferInit
                 |               | <-----------(1)
                 | transf_request|  3-699 a31
                 |               | ----------->(2)
                 +---------------+
                         |
                         | 202
                         | a28 new subscription
                         |
        NOTIFY 3-699     V          NOTIFY 1XX
        a30      +---------------+  a29 (NULL)
   (2) <---------|               |----------------+
                 | transf_pending|                |
   +-------------|               |<---------------+
   |             +---------------+
   |    BYE           |    | 
   |    a5:new ByeResp|    | bye 
   |                  |    | a6 TransByeInit
   |                  V    V
   |             +---------------+
   |NOTIFY 2XX   |               |
   |a32 TransByeI|  termwait     |
   +------------>|               |
                 +---------------+

   CALL TRANSFER answer:

                                      REFER
                 +---------------+    a33 new TrsnReferResp
                 |               | <-----------(1)
                 |transf_ask_user|  transfer_refuse 
                 |               |  a35 send 403
                 +---------------+ ----------->(2)
                         |
                         | transfer_accept
                         | a34 send 202, start new call
                         |
                         V  
                 +---------------+
                 |               |----+ transferred_call_progress
                 | transf_started|    | a36 send NOTIFY 
                 |               |<---+
                 +---------------+
       BYE           |    | 
       a5:new ByeResp|    | bye 
                     |    | a6 TransByeInit
                     V    V
                 +---------------+
                 |               |
                 |  termwait     |
                 |               |
                 +---------------+

   
*/
 
 
bool SipDialogVoip::a0_start_callingnoauth_invite( const SipSMCommand &command)
{
	if (transitionMatch(command, SipCommandString::invite)){
#ifndef _MSC_VER
		ts.save("a0_start_callingnoauth_invite");
#endif
		//int seqNo = requestSeqNo();
		++dialogState.seqNo;
//		setLocalCalled(false);
		localCalled=false;
		
		//set an "early" remoteUri ... we will update this later
		dialogState.remoteUri= command.getCommandString().getParam();

		MRef<SipTransaction*> invtrans = new SipTransactionInviteClientUA(sipStack, 
				MRef<SipDialog *>(this), 
				dialogState.seqNo, 
				dialogState.callId);
		
		invtrans->setSocket( phoneconf->proxyConnection );
		
 		notifyEarlyTermination = true; // set to true, once sent the first command, set to false
		
		registerTransaction(invtrans);

		sendInvite(invtrans->getBranch());

		return true;
	}else{
		return false;
	}
}

bool SipDialogVoip::a1_callingnoauth_callingnoauth_18X( const SipSMCommand &command)
{	
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "18*")){

		MRef<SipResponse*> resp= (SipResponse*) *command.getCommandPacket();

#ifndef _MSC_VER
		ts.save( RINGING );
#endif
		CommandString cmdstr(dialogState.callId, SipCommandString::remote_ringing);
		getDialogContainer()->getCallback()->sipcb_handleCommand(cmdstr);
	
		//We must maintain the dialog state. 
		dialogState.updateState( resp );
		
		//string peerUri = command.getCommandPacket()->getTo().getString();
		string peerUri = dialogState.remoteUri; //use the dialog state ...
	
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


bool SipDialogVoip::a2_callingnoauth_callingnoauth_1xx( const SipSMCommand &command)
{

	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "1**")){
		dialogState.updateState( MRef<SipResponse*>((SipResponse *)*command.getCommandPacket()) );
		return true;
	}else{
		return false;
	}
    
}

bool SipDialogVoip::a3_callingnoauth_incall_2xx( const SipSMCommand &command)
{
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "2**")){
#ifndef _MSC_VER
		ts.save("a3_callingnoauth_incall_2xx");
#endif
		MRef<SipResponse*> resp(  (SipResponse*)*command.getCommandPacket() );

		dialogState.updateState( resp );
		
		//string peerUri = resp->getFrom().getString();
		string peerUri = dialogState.remoteUri;
		
		setLogEntry( new LogEntryOutgoingCompletedCall() );
		getLogEntry()->start = time( NULL );
		getLogEntry()->peerSipUri = peerUri;


//FIXME: CESC: for now, route set is updated at the transaction layer		
		
		CommandString cmdstr(dialogState.callId, SipCommandString::invite_ok, "",
							(getMediaSession()->isSecure()?"secure":"unprotected"));
		
		getDialogContainer()->getCallback()->sipcb_handleCommand(cmdstr);
		
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

bool SipDialogVoip::a5_incall_termwait_BYE( const SipSMCommand &command)
{
	
	if (transitionMatch(command, SipBye::type, SipSMCommand::remote, IGN)){
		MRef<SipBye*> bye = (SipBye*) *command.getCommandPacket();

		//mdbg << "log stuff"<< end;
		if( getLogEntry() ){
			((LogEntrySuccess *)(*( getLogEntry() )))->duration = 
			time( NULL ) - getLogEntry()->start; 

			getLogEntry()->handle();
		}

		MRef<SipTransaction*> byeresp = new SipTransactionNonInviteServer(sipStack, 
				MRef<SipDialog*>(this), 
				bye->getCSeq(),
				bye->getLastViaBranch(), 
				dialogState.callId); //TODO: remove second argument

		registerTransaction(byeresp);
		SipSMCommand cmd(command);
		cmd.setDestination(SipSMCommand::transaction);
		cmd.setSource(command.getSource());
		getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);

		sendByeOk(bye, byeresp->getBranch() );

		CommandString cmdstr(dialogState.callId, SipCommandString::remote_hang_up);
		getDialogContainer()->getCallback()->sipcb_handleCommand(cmdstr);

		getMediaSession()->stop();

		
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}


bool SipDialogVoip::a6_incall_termwait_hangup( const SipSMCommand &command)
{
	if (transitionMatch(command, SipCommandString::hang_up)){
		//int bye_seq_no= requestSeqNo();
		++dialogState.seqNo;
		MRef<SipTransaction*> byetrans( new SipTransactionNonInviteClient(sipStack, 
				MRef<SipDialog*>(this), 
				dialogState.seqNo, 
				dialogState.callId)); 

		registerTransaction(byetrans);
		sendBye(byetrans->getBranch(), dialogState.seqNo);
		
		if (getLogEntry()){
			(dynamic_cast< LogEntrySuccess * >(*( getLogEntry() )))->duration = time( NULL ) - getLogEntry()->start; 
			getLogEntry()->handle();
		}
		
		getMediaSession()->stop();

		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}

bool SipDialogVoip::a7_callingnoauth_termwait_CANCEL( const SipSMCommand &command)
{
	if (transitionMatch(command, SipCancel::type, SipSMCommand::remote, IGN)){
//		setCurrentState(toState);

		MRef<SipTransaction*> cancelresp( 
				new SipTransactionNonInviteServer(
					sipStack,
					MRef<SipDialog*>(this), 
					command.getCommandPacket()->getCSeq(), 
					command.getCommandPacket()->getLastViaBranch(), 
					dialogState.callId ));

		registerTransaction(cancelresp);

		SipSMCommand cmd(command);
		cmd.setSource(SipSMCommand::TU);
		cmd.setDestination(SipSMCommand::transaction);
		getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);

		getMediaSession()->stop();
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}

bool SipDialogVoip::a8_callingnoauth_termwait_cancel( const SipSMCommand &command)
{
	if (transitionMatch(command, SipCommandString::cancel) || 
			transitionMatch(command, SipCommandString::hang_up)){
//		setCurrentState(toState);

		MRef<SipTransaction*> canceltrans( 
				new SipTransactionNonInviteClient(sipStack, 
					MRef<SipDialog*>( this ), 
					dialogState.seqNo, 
					dialogState.callId)); 

		registerTransaction(canceltrans);
		sendCancel(canceltrans->getBranch());

		getMediaSession()->stop();
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}

//Note: This is also used as: callingauth_terminated_36
bool SipDialogVoip::a9_callingnoauth_termwait_36( const SipSMCommand &command)
{
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "3**\n4**\n5**\n6**")){
		
		MRef<LogEntry *> rejectedLog( new LogEntryCallRejected() );
		rejectedLog->start = time( NULL );
		rejectedLog->peerSipUri = dialogState.remoteTag;
		
		if (sipResponseFilterMatch(MRef<SipResponse*>((SipResponse*)*command.getCommandPacket()),"404")){
                        CommandString cmdstr(dialogState.callId, SipCommandString::remote_user_not_found);
			getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr);
			((LogEntryFailure *)*rejectedLog)->error =
				"User not found";
			setLogEntry( rejectedLog );
			rejectedLog->handle();
                        
		}else if (sipResponseFilterMatch(MRef<SipResponse*>((SipResponse*)*command.getCommandPacket()),"606")){
			((LogEntryFailure *)*rejectedLog)->error =
				"User could not handle the call";
			setLogEntry( rejectedLog );
			rejectedLog->handle();
                        
                        CommandString cmdstr( dialogState.callId, SipCommandString::remote_unacceptable, command.getCommandPacket()->getWarningMessage());
			getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
		}
		else if (sipResponseFilterMatch(MRef<SipResponse*>((SipResponse*)*command.getCommandPacket()),"4**")){
			((LogEntryFailure *)*rejectedLog)->error =
				"User rejected the call";
			setLogEntry( rejectedLog );
			rejectedLog->handle();
                        CommandString cmdstr( dialogState.callId, SipCommandString::remote_reject);
			getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
		}
		else{
			merr << "ERROR: received response in SipDialogVoip"
				" that could not be handled (unimplemented)"<< end;
                }
		
		getMediaSession()->stop();
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}

bool SipDialogVoip::a10_start_ringing_INVITE( const SipSMCommand &command)
{
	if (transitionMatch(command, SipInvite::type, IGN, SipSMCommand::TU)){
		
		setLastInvite(MRef<SipInvite*>((SipInvite *)*command.getCommandPacket()));
		dialogState.updateState( getLastInvite() );
		
		//string peerUri = command.getCommandPacket()->getFrom().getString().substr(4);
		string peerUri = dialogState.remoteUri.substr(4);
		
	//	setLocalCalled(true);
		localCalled=true;
		
		getDialogConfig()->inherited->sipIdentity->setSipUri(command.getCommandPacket()->getHeaderValueTo()->getUri().getUserIpString().substr(4));
		
		//MRef<SipMessageContent *> Offer = *command.getCommandPacket()->getContent();
		if(!sortMIME(*command.getCommandPacket()->getContent(), peerUri, 10)){
			merr << "No MIME match" << end;
			return false;
		}
#ifdef IPSEC_SUPPORT
		// Check if IPSEC was required
		if (ipsecSession->required() && !ipsecSession->offered){
			cerr << "I require IPSEC or nothing at all!" << endl;
			return false;
		}
#endif
		
		MRef<SipTransaction*> ir( new SipTransactionInviteServerUA(
						sipStack,
						MRef<SipDialog*>( this ), 
						command.getCommandPacket()->getCSeq(),
						command.getCommandPacket()->getLastViaBranch(), 
						dialogState.callId) );

		registerTransaction(ir);
		//	command.setSource(SipSMCommand::TU);
		//	command.setDestination(SipSMCommand::transaction);

		//	mdbg << "^^^^ SipDialogVoip: re-handling packet for transaction to catch:"<<command<<end;

		SipSMCommand cmd(command);
		cmd.setDestination(SipSMCommand::transaction);

		getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);

		CommandString cmdstr(dialogState.callId, 
				SipCommandString::incoming_available, 
				dialogState.remoteUri, 
				(getMediaSession()->isSecure()?"secure":"unprotected")
				);
		getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
		sendRinging(ir->getBranch());
		
		if( getDialogConfig()->inherited->autoAnswer ){
			CommandString accept( dialogState.callId, SipCommandString::accept_invite );
			SipSMCommand sipcmd(accept, SipSMCommand::remote, SipSMCommand::TU);
			getDialogContainer()->enqueueCommand(sipcmd,HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
		}
		return true;
	}else{
		return false;
	}
}

bool SipDialogVoip::a11_ringing_incall_accept( const SipSMCommand &command)
{
	if (transitionMatch(command, SipCommandString::accept_invite)){
#ifndef _MSC_VER
		ts.save(USER_ACCEPT);
#endif

		CommandString cmdstr(dialogState.callId, 
				SipCommandString::invite_ok,"",
				(getMediaSession()->isSecure()?"secure":"unprotected")
				);
		getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );

		assert( !getLastInvite().isNull() );
		sendInviteOk(getLastInvite()->getDestinationBranch() );

		getMediaSession()->start();

		MRef<LogEntry *> logEntry = new LogEntryIncomingCompletedCall();

		logEntry->start = time( NULL );
		logEntry->peerSipUri = getLastInvite()->getFrom().getString();
		
		setLogEntry( logEntry );
		
		return true;
	}else{
		return false;
	}
}

bool SipDialogVoip::a12_ringing_termwait_CANCEL( const SipSMCommand &command)
{

	if (transitionMatch(command, SipCancel::type, IGN, IGN)){

		//FIXME: is this correct - this should probably be handled
		//in the already existing transaction.
		MRef<SipTransaction*> cr( 
			new SipTransactionNonInviteServer(sipStack, 
				MRef<SipDialog*>(this), 
				command.getCommandPacket()->getCSeq(), 
				command.getCommandPacket()->getLastViaBranch(), 
				dialogState.callId) );
		registerTransaction(cr);

		SipSMCommand cmd(command);
		cmd.setDestination(SipSMCommand::transaction);

		//	mdbg << "^^^^ SipDialogVoip: re-handling packet for transaction to catch."<<end;
		getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);

		//	mdbg << "^^^^ SipDialogVoip: sending ok to cancel packet"<<end;
		/* Tell the GUI */
		CommandString cmdstr(dialogState.callId, SipCommandString::remote_cancelled_invite,"");
		getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );

		sendInviteOk(cr->getBranch() ); 

		getMediaSession()->stop();
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}


bool SipDialogVoip::a13_ringing_termwait_reject( const SipSMCommand &command)
{
	
	if (transitionMatch(command, SipCommandString::reject_invite) || transitionMatch(command,SipCommandString::hang_up)){


		sendReject( getLastInvite()->getDestinationBranch() );

		getMediaSession()->stop();
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}


bool SipDialogVoip::a16_start_termwait_INVITE( const SipSMCommand &command){
	
	if (transitionMatch(command, SipInvite::type, SipSMCommand::remote, SipSMCommand::TU)){

		setLastInvite(MRef<SipInvite*>((SipInvite *)*command.getCommandPacket()));
		
		dialogState.updateState( getLastInvite() );

		MRef<SipTransaction*> ir( new SipTransactionInviteServerUA(sipStack, 
						MRef<SipDialog*>(this), 
						command.getCommandPacket()->getCSeq(), 
						command.getCommandPacket()->getLastViaBranch(), 
						dialogState.callId ));

		registerTransaction(ir);

		SipSMCommand cmd(command);
		cmd.setDestination(SipSMCommand::transaction);

		getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);

		sendNotAcceptable( command.getCommandPacket()->getDestinationBranch() );

		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}

bool SipDialogVoip::a20_callingnoauth_callingauth_40X( const SipSMCommand &command){

	if (transitionMatch(command,SipResponse::type, IGN, SipSMCommand::TU, "407\n401")){
		
		MRef<SipResponse*> resp( (SipResponse*)*command.getCommandPacket() );

		dialogState.updateState( resp ); //nothing will happen ... 4xx responses do not update ...

		//int seqNo = requestSeqNo();
		++dialogState.seqNo;
		MRef<SipTransaction*> trans( 
			new SipTransactionInviteClientUA(sipStack, 
				MRef<SipDialog*>(this), 
				dialogState.seqNo, 
				dialogState.callId));
		registerTransaction(trans);
		
		realm = resp->getRealm();
		nonce = resp->getNonce();

		sendAuthInvite(trans->getBranch());

		return true;
	}else{
		return false;
	}
}


bool SipDialogVoip::a21_callingauth_callingauth_18X( const SipSMCommand &command){
	
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "18*")){
		MRef<SipResponse*> resp (  (SipResponse*)*command.getCommandPacket()  );
#ifndef _MSC_VER
		ts.save( RINGING );
#endif

		CommandString cmdstr(dialogState.callId, SipCommandString::remote_ringing);
		getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );

		dialogState.updateState( resp );

		//string peerUri = resp->getFrom().getString();
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

bool SipDialogVoip::a22_callingauth_callingauth_1xx( const SipSMCommand &command){
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "1**")){

		dialogState.updateState( MRef<SipResponse*> ( (SipResponse*)*command.getCommandPacket() ) );
		return true;
	}else{
		return false;
	}
}

bool SipDialogVoip::a23_callingauth_incall_2xx( const SipSMCommand &command){
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "2**")){
		MRef<SipResponse*> resp( (SipResponse*)*command.getCommandPacket() );
		
		dialogState.updateState( resp );
//CESC: for now, route set is updated at the transaction layer		
		
		//string peerUri = resp->getFrom().getString().substr(4);
		string peerUri = dialogState.remoteUri.substr(4);
		
		setLogEntry( new LogEntryOutgoingCompletedCall() );
		getLogEntry()->start = time( NULL );
		getLogEntry()->peerSipUri = peerUri;
		
		CommandString cmdstr(dialogState.callId, 
				SipCommandString::invite_ok, 
				"",
				(getMediaSession()->isSecure()?"secure":"unprotected")
				);
		getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );


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

bool SipDialogVoip::a24_calling_termwait_2xx( const SipSMCommand &command){
	
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "2**")){

		//int bye_seq_no= requestSeqNo();
		++dialogState.seqNo;
//		setCurrentState(toState);

		MRef<SipTransaction*> byetrans = 
			new SipTransactionNonInviteClient(sipStack, 
				MRef<SipDialog*>(this), 
				dialogState.seqNo, 
				dialogState.callId); 


		registerTransaction(byetrans);
		sendBye(byetrans->getBranch(), dialogState.seqNo);

		CommandString cmdstr(dialogState.callId, SipCommandString::security_failed);
		getDialogContainer()->getCallback()->sipcb_handleCommand(cmdstr);

		getMediaSession()->stop();
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}


bool SipDialogVoip::a25_termwait_terminated_notransactions( const SipSMCommand &command){
	if (transitionMatch(command, SipCommandString::no_transactions) ){
		lastInvite=NULL;
                
		SipSMCommand cmd(
				CommandString( dialogState.callId, 
					SipCommandString::call_terminated),
					SipSMCommand::TU,
					SipSMCommand::DIALOGCONTAINER);

		getDialogContainer()->enqueueCommand( cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE );
		/* Tell the GUI */
		CommandString cmdstr( dialogState.callId, SipCommandString::call_terminated);
		getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );

#ifdef IPSEC_SUPPORT
		if(ipsecSession){
			cerr << "Clearing" << endl;
			if(ipsecSession->stop() != 0)
				cerr << "Not all IPSEC parameters were confired cleared. Check and remove manually." << endl;
		}
#endif
		return true;
	}else{
		return false;
	}
}

//notify the gui that dialog is finished ... just some transactions need to 
//finish absorbing messages
//also, reply to the dialog container when extra hang_up messages are received
//  (this happens when you hang up a call from the gui and then exit minisip,
//  the sipdialogmanagement would send a hang up and receive no call_term_early ...
//  with this, it works).
bool SipDialogVoip::a25_termwait_termwait_early( const SipSMCommand &command){
	if( transitionMatch(command, SipCommandString::hang_up) ) {
		CommandString cmdstr( dialogState.callId, SipCommandString::call_terminated_early);
		/* Tell the GUI, once only */
		if( notifyEarlyTermination ) {
			getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
			notifyEarlyTermination = false;
		}
		SipSMCommand cmd( cmdstr, SipSMCommand::TU, SipSMCommand::DIALOGCONTAINER );
		getDialogContainer()->enqueueCommand( cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE ); //this is for the shutdown dialog 
		return true;
	}
	else if ( notifyEarlyTermination && transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "2**")){
		lastInvite=NULL;                
		//Notify the GUI and the dialog container ... 
		CommandString cmdstr( dialogState.callId, SipCommandString::call_terminated_early);
		
		/* Tell the GUI, once only */
		getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
		notifyEarlyTermination = false;
		
		//Tell the dialog container ... 
		SipSMCommand cmd( cmdstr, SipSMCommand::TU, SipSMCommand::DIALOGCONTAINER );
		getDialogContainer()->enqueueCommand( cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE ); //this is for the shutdown dialog 
		return true;
	}else{
		return false;
	}
}


bool SipDialogVoip::a26_callingnoauth_termwait_transporterror( const SipSMCommand &command){
	if (transitionMatch(command, SipCommandString::transport_error )){
		CommandString cmdstr(dialogState.callId, SipCommandString::transport_error);
		getDialogContainer()->getCallback()->sipcb_handleCommand(cmdstr);
		return true;
	}else{
		return false;
	}
}

//Copy of a8!
bool SipDialogVoip::a26_callingauth_termwait_cancel( const SipSMCommand &command)
{
	if (transitionMatch(command, SipCommandString::cancel) || transitionMatch(command, SipCommandString::hang_up)){
//		setCurrentState(toState);
		MRef<SipTransaction*> canceltrans( 
			new SipTransactionNonInviteClient(sipStack, 
				MRef<SipDialog*>(this), 
				dialogState.seqNo, 
				dialogState.callId)); 
		registerTransaction(canceltrans);
		sendCancel(canceltrans->getBranch());

		getMediaSession()->stop();
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}

bool SipDialogVoip::a27_incall_transferrequested_transfer( const SipSMCommand &command)
{
	if (transitionMatch(command, SipCommandString::user_transfer)){
		//int bye_seq_no= requestSeqNo();
		string referredUri = command.getCommandString().getParam();
		++dialogState.seqNo;
		MRef<SipTransaction*> refertrans( 
			new SipTransactionNonInviteClient(sipStack, 
				MRef<SipDialog*>(this), 
				dialogState.seqNo, 
				dialogState.callId)); 

		registerTransaction(refertrans);
		sendRefer(refertrans->getBranch(), dialogState.seqNo, referredUri);

		return true;
	}else{
		return false;
	}
}

bool SipDialogVoip::a28_transferrequested_transferpending_202( const SipSMCommand &command){
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "202")){
		MRef<SipResponse*> resp( (SipResponse*)*command.getCommandPacket() );

		CommandString cmdstr(dialogState.callId, 
				SipCommandString::transfer_pending
				);
		getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );

		return true;
	}else{
		return false;
	}
}

bool SipDialogVoip::a31_transferrequested_incall_36( const SipSMCommand &command)
{
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "3**\n4**\n5**\n6**")){
                
		CommandString cmdstr( dialogState.callId, 
			SipCommandString::transfer_refused, 
			command.getCommandPacket()->getWarningMessage());
		getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
		return true;
	}else{
		return false;
	}
}

bool SipDialogVoip::a33_incall_transferaskuser_REFER( const SipSMCommand &command)
{

	if (transitionMatch(command, SipRefer::type, IGN, IGN)){

		MRef<SipTransaction*> cr( 
			new SipTransactionNonInviteServer(sipStack, 
				MRef<SipDialog*>(this), 
				command.getCommandPacket()->getCSeq(), 
				command.getCommandPacket()->getLastViaBranch(), 
				dialogState.callId) );
		registerTransaction(cr);

		SipSMCommand cmd(command);
		cmd.setDestination(SipSMCommand::transaction);

		getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
		
		lastRefer = dynamic_cast<SipRefer *>(*command.getCommandPacket());

		/* Tell the GUI */
		CommandString cmdstr(dialogState.callId, 
				SipCommandString::transfer_requested,
				lastRefer->getReferredUri());
		getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
		return true;
	}else{
		return false;
	}
}

bool SipDialogVoip::a34_transferaskuser_transferstarted_accept( const SipSMCommand &command)
{
	if (transitionMatch(command, SipCommandString::user_transfer_accept)){

		sendReferOk(lastRefer->getDestinationBranch() );

		/* start a new call ... */
		string uri = lastRefer->getReferredUri();
		string newCallId = phoneconf->sip->invite( uri );

		/* Send the new callId to the GUI */
		CommandString cmdstr(dialogState.callId, SipCommandString::call_transferred, newCallId);
		getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );

		return true;
	}else{
		return false;
	}
}

bool SipDialogVoip::a35_transferaskuser_incall_refuse( const SipSMCommand &command)
{
	if (transitionMatch(command, SipCommandString::user_transfer_refuse)){

		sendReferReject(lastRefer->getDestinationBranch());

		return true;
	}else{
		return false;
	}
}


void SipDialogVoip::setUpStateMachine(){

	State<SipSMCommand,string> *s_start=new State<SipSMCommand,string>(this,"start");
	addState(s_start);

	State<SipSMCommand,string> *s_callingnoauth=new State<SipSMCommand,string>(this,"callingnoauth");
	addState(s_callingnoauth);

	State<SipSMCommand,string> *s_callingauth=new State<SipSMCommand,string>(this,"callingauth");
	addState(s_callingauth);

	State<SipSMCommand,string> *s_incall=new State<SipSMCommand,string>(this,"incall");
	addState(s_incall);

	State<SipSMCommand,string> *s_termwait=new State<SipSMCommand,string>(this,"termwait");
	addState(s_termwait);
	
	State<SipSMCommand,string> *s_terminated=new State<SipSMCommand,string>(this,"terminated");
	addState(s_terminated);
	
	State<SipSMCommand,string> *s_ringing=new State<SipSMCommand,string>(this,"ringing");
	addState(s_ringing);
	
	State<SipSMCommand,string> *s_transferrequested=new State<SipSMCommand,string>(this,"transferrequested");
	addState(s_transferrequested);
        
	State<SipSMCommand,string> *s_transferpending=new State<SipSMCommand,string>(this,"transferpending");
	addState(s_transferpending);
	
	State<SipSMCommand,string> *s_transferaskuser=new State<SipSMCommand,string>(this,"transferaskuser");
	addState(s_transferaskuser);
	
	State<SipSMCommand,string> *s_transferstarted=new State<SipSMCommand,string>(this,"transferstarted");
	addState(s_transferstarted);


	new StateTransition<SipSMCommand,string>(this, "transition_start_callingnoauth_invite",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a0_start_callingnoauth_invite, 
			s_start, s_callingnoauth);

	new StateTransition<SipSMCommand,string>(this, "transition_callingnoauth_callingnoauth_18X",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a1_callingnoauth_callingnoauth_18X, 
			s_callingnoauth, s_callingnoauth);

	new StateTransition<SipSMCommand,string>(this, "transition_callingnoauth_callingnoauth_1xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a2_callingnoauth_callingnoauth_1xx, 
			s_callingnoauth, s_callingnoauth);

	new StateTransition<SipSMCommand,string>(this, "transition_callingnoauth_incall_2xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a3_callingnoauth_incall_2xx, 
			s_callingnoauth, s_incall);
//this transition conflicts with a3 ... actually, they need the same messages, thus a3 prevails ... useless???  //CESC
//	new StateTransition<SipSMCommand,string>(this, "transition_callingauth_termwait_2xx",
//			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a24_calling_termwait_2xx,
//			s_callingnoauth, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_incall_termwait_BYE",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a5_incall_termwait_BYE,
			s_incall, s_termwait); 

	new StateTransition<SipSMCommand,string>(this, "transition_incall_termwait_hangup",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand& )) &SipDialogVoip::a6_incall_termwait_hangup,
			s_incall, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_callingnoauth_termwait_CANCEL",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a7_callingnoauth_termwait_CANCEL,
			s_callingnoauth, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_callingnoauth_termwait_cancel",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a8_callingnoauth_termwait_cancel,
			s_callingnoauth, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_callingnoauth_callingauth_40X",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a20_callingnoauth_callingauth_40X,
			s_callingnoauth, s_callingauth);

	new StateTransition<SipSMCommand,string>(this, "transition_callingnoauth_termwait_36",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a9_callingnoauth_termwait_36,
			s_callingnoauth, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_start_ringing_INVITE",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a10_start_ringing_INVITE,
			s_start, s_ringing);

	new StateTransition<SipSMCommand,string>(this, "transition_ringing_incall_accept",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a11_ringing_incall_accept,
			s_ringing, s_incall);

	new StateTransition<SipSMCommand,string>(this, "transition_ringing_termwait_CANCEL",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a12_ringing_termwait_CANCEL,
			s_ringing, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_ringing_termwait_reject",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a13_ringing_termwait_reject,
			s_ringing, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_start_termwait_INVITE",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a16_start_termwait_INVITE,
			s_start, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_callingauth_callingauth_18X",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a21_callingauth_callingauth_18X, 
			s_callingauth, s_callingauth);

	new StateTransition<SipSMCommand,string>(this, "transition_callingauth_callingauth_1xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a22_callingauth_callingauth_1xx, 
			s_callingauth, s_callingauth);

	new StateTransition<SipSMCommand,string>(this, "transition_callingauth_incall_2xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a23_callingauth_incall_2xx, 
			s_callingauth, s_incall);
//this transition conflicts with a3 ... actually, they need the same messages, thus a3 prevails ... useless???  //CESC
//	new StateTransition<SipSMCommand,string>(this, "transition_callingauth_termwait_2xx",
//			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a24_calling_termwait_2xx,
//			s_callingauth, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_callingauth_termwait_resp36",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a9_callingnoauth_termwait_36,
			s_callingauth, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_termwait_terminated_notransactions",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a25_termwait_terminated_notransactions,
			s_termwait, s_terminated);

	new StateTransition<SipSMCommand,string>(this, "transition_termwait_termwait_earlynotify",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a25_termwait_termwait_early,
			s_termwait, s_termwait);
	
	new StateTransition<SipSMCommand,string>(this, "transition_callingnoauth_termwait_transporterror",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a26_callingnoauth_termwait_transporterror,
			s_callingnoauth, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_callingauth_termwait_cancel",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a26_callingauth_termwait_cancel,
			s_callingauth, s_termwait);
	
	new StateTransition<SipSMCommand,string>(this, "transition_incall_transferrequested_transfer",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a27_incall_transferrequested_transfer,
			s_incall, s_transferrequested);
        
	new StateTransition<SipSMCommand,string>(this, "transition_transferrequested_transferpending_202",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a28_transferrequested_transferpending_202,
			s_transferrequested, s_transferpending);
	
	new StateTransition<SipSMCommand,string>(this, "transition_transferrequested_incall_36",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a31_transferrequested_incall_36,
			s_transferrequested, s_incall);
	
	new StateTransition<SipSMCommand,string>(this, "transition_transferpending_termwait_BYE",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a6_incall_termwait_hangup,
			s_transferpending, s_termwait);
	
	new StateTransition<SipSMCommand,string>(this, "transition_transferpending_termwait_bye",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a5_incall_termwait_BYE,
			s_transferpending, s_termwait);
	
	new StateTransition<SipSMCommand,string>(this, "transition_incall_transferaskuser_REFER",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a33_incall_transferaskuser_REFER,
			s_incall, s_transferaskuser);
	
	new StateTransition<SipSMCommand,string>(this, "transition_transferaskuser_transferstarted_accept",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a34_transferaskuser_transferstarted_accept,
			s_transferaskuser, s_transferstarted);
	
	new StateTransition<SipSMCommand,string>(this, "transition_transferaskuser_transferstarted_accept",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a34_transferaskuser_transferstarted_accept,
			s_transferaskuser, s_transferstarted);
        
	new StateTransition<SipSMCommand,string>(this, "transition_transferaskuser_incall_refuse",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a35_transferaskuser_incall_refuse,
			s_transferaskuser, s_incall);
	
	new StateTransition<SipSMCommand,string>(this, "transition_transferstarted_termwait_BYE",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a6_incall_termwait_hangup,
			s_transferstarted, s_termwait);
        
	new StateTransition<SipSMCommand,string>(this, "transition_transferstarted_termwait_bye",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoip::a5_incall_termwait_BYE,
			s_transferstarted, s_termwait);

	setCurrentState(s_start);
}


#ifdef IPSEC_SUPPORT
SipDialogVoip::SipDialogVoip(MRef<SipStack*> stack, MRef<SipDialogConfig*> callconfig, MRef<SipSoftPhoneConfiguration*> pconf, MRef<Session *> mediaSession, string cid, MRef<MsipIpsecAPI *> ipsecSession) : 
		SipDialog(stack,callconfig),
		lastInvite(NULL), 
		phoneconf(pconf),
		mediaSession(mediaSession), ipsecSession(ipsecSession)
#else
SipDialogVoip::SipDialogVoip(MRef<SipStack*> stack, MRef<SipDialogConfig*> callconfig, MRef<SipSoftPhoneConfiguration*> pconf, MRef<Session *> mediaSession, string cid) : 
		SipDialog(stack,callconfig),
		lastInvite(NULL), 
		phoneconf(pconf),
		mediaSession(mediaSession)
#endif

{
	if (cid=="")
		dialogState.callId = itoa(rand())+"@"+getDialogConfig()->inherited->externalContactIP;
	else
		dialogState.callId = cid;
	
	dialogState.localTag = itoa(rand());
	
	/* We will fill that later, once we know if that succeeded */
	logEntry = NULL;

	setUpStateMachine();
}

SipDialogVoip::~SipDialogVoip(){	
	mediaSession->unregister();
}

/*
void SipDialogVoip::handleSdp(MRef<SdpPacket*> sdp){

}
*/

void SipDialogVoip::sendInvite(const string &branch){
	//	mdbg << "ERROR: SipDialogVoip::sendInvite() UNIMPLEMENTED"<< end;
	
	MRef<SipInvite*> inv;
	string keyAgreementMessage;
	inv= MRef<SipInvite*>(
		new SipInvite(
			branch,
			dialogState.callId,
			dialogState.remoteUri,
			//getDialogConfig().inherited.sipIdentity->sipProxy.sipProxyIpAddr->getString(),
			getDialogConfig()->inherited->sipIdentity->sipDomain,	//TODO: Change API - not sure if proxy or domain
			getDialogConfig()->inherited->sipIdentity->sipProxy.sipProxyPort,
	//		getDialogConfig().inherited.localIpString,
			getDialogConfig()->inherited->externalContactIP,
			getDialogConfig()->inherited->getLocalSipPort(phoneconf->useSTUN),
			//getDialogConfig().inherited.userUri,
			getDialogConfig()->inherited->sipIdentity->getSipUri(),
			dialogState.seqNo,
			getDialogConfig()->inherited->transport));

	/* Get the session description from the Session */
		
	//There might be so that there are no SDP. Check!
	MRef<SdpPacket *> sdp;
	if (mediaSession){
#ifndef _MSC_VER
		ts.save("getSdpOffer");
#endif
		sdp = mediaSession->getSdpOffer();
#ifndef _MSC_VER
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

//	mdbg << "SipDialogVoip::sendInvite(): sending INVITE to transaction"<<end;
//	ts.save( INVITE_END );
	MRef<SipMessage*> pktr(*inv);

	SipSMCommand scmd(
			pktr, 
			SipSMCommand::TU, 
			SipSMCommand::transaction
			);
	
//	scmd.setDispatched(true); //What was I thinking about here?? --EE
	
//	handleCommand(scmd);
	getDialogContainer()->enqueueCommand(scmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
	setLastInvite(inv);

}

void SipDialogVoip::sendAuthInvite(const string &branch){
	//	merr << "ERROR: SipDialogVoip::sendAuthInvite() UNIMPLEMENTED"<< end;
	//string call_id = getDialogConfig().callId;
	//SipInvite * inv;
	MRef<SipInvite*> inv;
	string keyAgreementMessage;

	//merr << "SipDialogVoip::sendAuthInv : dialogstate.remoteUri=" << dialogState.remoteUri << end;

	inv= new SipInvite(
		branch,
		dialogState.callId,
		dialogState.remoteUri,
		//getDialogConfig().inherited.sipIdentity->sipProxy.sipProxyIpAddr->getString(),
		getDialogConfig()->inherited->sipIdentity->sipDomain,
		getDialogConfig()->inherited->sipIdentity->sipProxy.sipProxyPort,
//		getDialogConfig().inherited.localIpString,
		getDialogConfig()->inherited->externalContactIP,
		getDialogConfig()->inherited->getLocalSipPort(phoneconf->useSTUN),
		//getDialogConfig().inherited.userUri,
		getDialogConfig()->inherited->sipIdentity->getSipUri(),
		dialogState.seqNo,
//		requestSeqNo(),
		getDialogConfig()->inherited->sipIdentity->sipProxy.sipProxyUsername,
		nonce,
		realm,
		getDialogConfig()->inherited->sipIdentity->sipProxy.sipProxyPassword,
		getDialogConfig()->inherited->transport);

	inv->getHeaderValueFrom()->setParameter("tag",dialogState.localTag);
	
	//There might be so that there are no SDP. Check!
	MRef<SdpPacket *> sdp;
	if (mediaSession){
#ifndef _MSC_VER
		ts.save("getSdpOffer");
#endif
		sdp = mediaSession->getSdpOffer();
#ifndef _MSC_VER
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

	//	/* Get the session description from the Session */
	//	MRef<SdpPacket *> sdp = mediaSession->getSdpOffer();
	//
	//	/* Add the latter to the INVITE message */
	//	inv->setContent( *sdp );
	
	MRef<SipMessage*> pref(*inv);
	SipSMCommand cmd(pref, SipSMCommand::TU, SipSMCommand::transaction);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
	setLastInvite(inv);

}


#ifdef NEVERDEFINED_ERSADFS
void SipDialogVoip::sendAck(string branch){
	//	mdbg << "ERROR: SipDialogVoip::sendAck() UNIMPLEMENTED" << end;
	assert( !lastResponse.isNull());
	SipAck *ack = new SipAck(
			branch, 
			*lastResponse,
			//getDialogConfig()->uri_foreign,
			dialogState.getRemoteTarget(),
			//getDialogConfig().inherited.sipIdentity->sipProxy.sipProxyIpAddr->getString());
			getDialogConfig()->inherited->sipIdentity->sipDomain);
	//TODO:
	//	ack.add_header( new SipHeaderRoute(getDialog()->getRouteSet() ) );
	//	mdbg << "SipDialogVoip:sendAck(): sending ACK directly to remote" << end;

	//	if(socket == NULL){
	// No StreamSocket, create one or use UDP
	//	Socket *sock=NULL;
	
	if(getDialogConfig()->proxyConnection == NULL){
		getDialogConfig()->inherited->sipTransport->sendMessage(ack,
				*(getDialogConfig()->inherited->sipIdentity->sipProxy.sipProxyIpAddr), //*toaddr,
				getDialogConfig()->inherited->sipProxy.proxyPort, //port, 
//				sock, //(Socket *)NULL, //socket, 
				getDialogConfig()->proxyConnection,
				"BUGBUGBUG",
				getDialogConfig()->inherited->transport
				);
	}else{
		// A StreamSocket exists, try to use it
		mdbg << "Sending packet using existing StreamSocket"<<end;
		getDialogConfig()->inherited->sipTransport->sendMessage(
				ack,
				(StreamSocket *)getDialogConfig()->proxyConnection, "BUGBUGBUG");
	}

	return;
	//	}

	/*	if(dynamic_cast<StreamSocket *>(socket) != NULL){
	// A StreamSocket exists, try to use it
	mdbg << "Sending packet using existing StreamSocket"<<end;
	getDialog()->getDialogConfig().sipTransport->sendMessage(pack,(StreamSocket *)socket);
	return;
	}
	 */
}
#endif

void SipDialogVoip::sendBye(const string &branch, int bye_seq_no){

	//string tmp = getDialogConfig().inherited.userUri;
	string tmp = getDialogConfig()->inherited->sipIdentity->getSipUri();
	uint32_t i = tmp.find("@");
	assert(i!=string::npos);
	i++;
	string domain;
	for ( ; i < tmp.length() ; i++)
		domain = domain+tmp[i];
	
	//merr << "SipDialogVoip::sendBye : dialogstate.remoteUri=" << dialogState.remoteUri << end;

//	mdbg << "///////////Creating bye with uri_foreign="<<getDialogConfig().uri_foreign << " and doman="<< domain<< end;
	MRef<SipBye*> bye = new SipBye(
			branch,
			getLastInvite(),
			//dialogState.remoteUri,
			dialogState.getRemoteTarget(),
			//getDialogConfig().inherited.userUri,
			getDialogConfig()->inherited->sipIdentity->getSipUri(),
			domain,
//			getDialogConfig().seqNo+1,
			bye_seq_no///,
			///localCalled
			);
	//add route headers, if needed
	if( dialogState.routeSet.size() > 0 ) {
		//merr << "SipDlgVoip:sendBYE : adding header route! " << end;
		MRef<SipHeaderValueRoute *> rset = new SipHeaderValueRoute (dialogState.routeSet);
		bye->addHeader(new SipHeader(*rset) );
	} else {
		//merr << "SipDlgVoip:sendBYE : dialog route set is EMPTY!!! " << end;
	}

	bye->getHeaderValueFrom()->setParameter("tag",dialogState.localTag);
	bye->getHeaderValueTo()->setParameter("tag",dialogState.remoteTag);

	MRef<SipMessage*> pref(*bye);
	SipSMCommand cmd( pref, SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand(cmd);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}

void SipDialogVoip::sendRefer(const string &branch, int refer_seq_no, const string referredUri){
/*
	string tmp = getDialogConfig()->inherited.sipIdentity->getSipUri();
	uint32_t i = tmp.find("@");
	assert(i!=string::npos);
	i++;
	string domain;
	for ( ; i < tmp.length() ; i++)
		domain = domain+tmp[i];
*/
	MRef<SipRefer*> refer = new SipRefer(
			branch,
			getLastInvite(),
			dialogState.getRemoteTarget(),
			getDialogConfig()->inherited->sipIdentity->getSipUri(),
			getDialogConfig()->inherited->sipIdentity->sipDomain,
			referredUri,
			refer_seq_no
			);

	refer->getHeaderValueFrom()->setParameter("tag",dialogState.localTag);
	refer->getHeaderValueTo()->setParameter("tag",dialogState.remoteTag);

	MRef<SipMessage*> pref(*refer);
	SipSMCommand cmd( pref, SipSMCommand::TU, SipSMCommand::transaction);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}

void SipDialogVoip::sendCancel(const string &branch){
	assert( !lastInvite.isNull());
	
	//merr << "SipDialogVoip::sendCancel : dialogstate.remoteUri=" << dialogState.remoteUri << end;
	
	MRef<SipCancel*> cancel = new SipCancel(
			branch,
			lastInvite,
			dialogState.remoteUri,
			//getDialogConfig().inherited.userUri,
			getDialogConfig()->inherited->sipIdentity->getSipUri(),
			//getDialogConfig().inherited.sipIdentity->sipProxy.sipProxyIpAddr->getString(),
			getDialogConfig()->inherited->sipIdentity->sipDomain///,
			///localCalled
			);

	cancel->getHeaderValueFrom()->setParameter("tag",dialogState.localTag);
	cancel->getHeaderValueTo()->setParameter("tag",dialogState.remoteTag);

	MRef<SipMessage*> pref(*cancel);
	SipSMCommand cmd( pref, SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand( cmd );
	getDialogContainer()->enqueueCommand( cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE );
}

void SipDialogVoip::sendInviteOk(const string &branch){
	MRef<SipResponse*> ok= new SipResponse(branch, 200,"OK", MRef<SipMessage*>(*getLastInvite()));	
	ok->getHeaderValueTo()->setParameter("tag",dialogState.localTag);
	
	MRef<SipHeaderValue *> contact = 
		new SipHeaderValueContact( 
			getDialogConfig()->inherited->sipIdentity->getSipUri(),
			getDialogConfig()->inherited->externalContactIP,
			getDialogConfig()->inherited->getLocalSipPort(phoneconf->useSTUN),
			"", getDialogConfig()->inherited->transport,
			-1); //set expires to -1, we do not use it (only in register)
	ok->addHeader( new SipHeader(*contact) );
	
	//There might be so that there are no SDP. Check!
	MRef<SdpPacket *> sdp;
	if (mediaSession){
#ifndef _MSC_VER
		ts.save("getSdpAnswer");
#endif
		sdp = mediaSession->getSdpAnswer();
#ifndef _MSC_VER
		ts.save("getSdpAnswer");
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

//	setLastResponse(ok);
	MRef<SipMessage*> pref(*ok);
	SipSMCommand cmd( pref, SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand(cmd);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}

void SipDialogVoip::sendReferOk(const string &branch){
	MRef<SipResponse*> ok= new SipResponse(branch, 202,"OK", MRef<SipMessage*>(*lastRefer));	
	ok->getHeaderValueTo()->setParameter("tag",dialogState.localTag);
	MRef<SipHeaderValue *> contact = 
		new SipHeaderValueContact( 
			getDialogConfig()->inherited->sipIdentity->getSipUri(),
			getDialogConfig()->inherited->externalContactIP,
			getDialogConfig()->inherited->getLocalSipPort(phoneconf->useSTUN),
			"", getDialogConfig()->inherited->transport,
			-1); //set expires to -1, we do not use it (only in register)
	ok->addHeader( new SipHeader(*contact) );

	MRef<SipMessage*> pref(*ok);
	SipSMCommand cmd( pref, SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand(cmd);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}

void SipDialogVoip::sendByeOk(MRef<SipBye*> bye, const string &branch){
	MRef<SipResponse*> ok= new SipResponse( branch, 200,"OK", MRef<SipMessage*>(*bye) );
	ok->getHeaderValueTo()->setParameter("tag",dialogState.localTag);

//	setLastResponse(ok);
	MRef<SipMessage*> pref(*ok);
	SipSMCommand cmd( pref, SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand(cmd);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}

void SipDialogVoip::sendReject(const string &branch){
	MRef<SipResponse*> ringing = new SipResponse(branch,486,"Temporary unavailable", MRef<SipMessage*>(*getLastInvite()));	
	ringing->getHeaderValueTo()->setParameter("tag",dialogState.localTag);
//	setLastResponse(ringing);
	MRef<SipMessage*> pref(*ringing);
	SipSMCommand cmd( pref,SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand(cmd);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}

void SipDialogVoip::sendReferReject(const string &branch){
	MRef<SipResponse*> forbidden = new SipResponse(branch,403,"Forbidden", MRef<SipMessage*>(*lastRefer));	
	forbidden->getHeaderValueTo()->setParameter("tag",dialogState.localTag);
//	setLastResponse(ringing);
	MRef<SipMessage*> pref(*forbidden);
	SipSMCommand cmd( pref,SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand(cmd);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}

void SipDialogVoip::sendRinging(const string &branch){
	MRef<SipResponse*> ringing = new SipResponse(branch,180,"Ringing", MRef<SipMessage*>(*getLastInvite()));	
	ringing->getHeaderValueTo()->setParameter("tag",dialogState.localTag);
	
	MRef<SipHeaderValue *> contact = 
		new SipHeaderValueContact( 
			getDialogConfig()->inherited->sipIdentity->getSipUri(),
			getDialogConfig()->inherited->externalContactIP,
			getDialogConfig()->inherited->getLocalSipPort(phoneconf->useSTUN),
			"", getDialogConfig()->inherited->transport,
			-1); //set expires to -1, we do not use it (only in register)
	ringing->addHeader( new SipHeader(*contact) );

//	setLastResponse(ringing);
	MRef<SipMessage*> pref(*ringing);
	SipSMCommand cmd( pref, SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand(cmd);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}

void SipDialogVoip::sendNotAcceptable(const string &branch){
	MRef<SipResponse*> not_acceptable = new SipResponse(branch,606,"Not Acceptable", MRef<SipMessage*>(*getLastInvite()));	
	if( mediaSession && mediaSession->getErrorString() != "" ){
		not_acceptable->addHeader( 
			new SipHeader(
				new SipHeaderValueWarning(
					getDialogConfig()->inherited->externalContactIP, 
					399, 
					mediaSession->getErrorString() ) ));
	}

	not_acceptable->getHeaderValueTo()->setParameter("tag",dialogState.localTag);
//	setLastResponse(not_acceptable);
	MRef<SipMessage*> pref(*not_acceptable);
	SipSMCommand cmd( pref, SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand(cmd);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}


bool SipDialogVoip::handleCommand(const SipSMCommand &c){
	mdbg << "SipDialogVoip::handleCommand got "<< c << end;

	if (c.getType()==SipSMCommand::COMMAND_STRING && dialogState.callId.length()>0){
		if (c.getCommandString().getDestinationId() != dialogState.callId )
			return false;
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
	
	mdbg << "SipDialogVoip::handlePacket() got "<< c << end;
	bool handled = SipDialog::handleCommand(c);
	
	if (!handled && c.getType()==SipSMCommand::COMMAND_STRING && c.getCommandString().getOp()==SipCommandString::no_transactions){
		return true;
	}
	
	if (c.getType()==SipSMCommand::COMMAND_STRING && dialogState.callId.length()>0){
		if (c.getCommandString().getDestinationId() == dialogState.callId ){
			mdbg << "Warning: SipDialogVoIP ignoring command with matching call id"<< end;
			return true;
		}
	}
	if (c.getType()==SipSMCommand::COMMAND_PACKET && dialogState.callId.length()>0){
		if (c.getCommandPacket()->getCallId() == dialogState.callId){
			mdbg << "Warning: SipDialogVoIP ignoring packet with matching call id"<< end;
			return true;
		}
	}

	
	return handled;
}

MRef<SipInvite*> SipDialogVoip::getLastInvite(){
	return lastInvite;
}

void SipDialogVoip::setLastInvite(MRef<SipInvite*> i){ 
	lastInvite = i; 
}

MRef<LogEntry *> SipDialogVoip::getLogEntry(){
	return logEntry;
}

void SipDialogVoip::setLogEntry( MRef<LogEntry *> logEntry ){
	this->logEntry = logEntry;
}

MRef<Session *> SipDialogVoip::getMediaSession(){
	return mediaSession;
}



#ifdef IPSEC_SUPPORT
MRef<MsipIpsecAPI *> SipDialogVoip::getIpsecSession(){
	return ipsecSession;
}
#endif


bool SipDialogVoip::sortMIME(MRef<SipMessageContent *> Offer, string peerUri, int type){
	if (Offer){
		if ( Offer->getContentType().substr(0,9) == "multipart"){
			MRef<SipMessageContent *> part;
			part = ((SipMimeContent*)*Offer)->popFirstPart();
			while( *part != NULL){
				sortMIME(part, peerUri, type);
				part = ((SipMimeContent*)*Offer)->popFirstPart();
			}
		}
#ifdef IPSEC_SUPPORT
		if( (Offer->getContentType()).substr(0,17) == "application/mikey"){
			switch (type){
				case 10:
					ts.save("setMikeyIpsecOffer");
					if(!getIpsecSession()->setMikeyIpsecOffer((SipMimeContent*)*Offer))
						return false;
					ts.save("setMikeyIpsecOffer");
					return true;
				case 3:
					ts.save("setMikeyIpsecAnswer");
					if(!getIpsecSession()->setMikeyIpsecAnswer((SipMimeContent*)*Offer))
						return false;
					ts.save("setMikeyIpsecAnswer");
					return true;
				default:
					return false;
			}
		}
#endif
		if( (Offer->getContentType()).substr(0,15) == "application/sdp"){
			switch (type){
				case 10:
#ifndef _MSC_VER
					ts.save("setSdpOffer");
#endif
					if( !getMediaSession()->setSdpOffer( (SdpPacket*)*Offer, peerUri ) )
						return false;
#ifndef _MSC_VER
					ts.save("setSdpOffer");
#endif
					return true;
				case 3:
#ifndef _MSC_VER
					ts.save("setSdpAnswer");
#endif
					if( !getMediaSession()->setSdpAnswer( (SdpPacket*)*Offer, peerUri ) )
						return false;
					getMediaSession()->start();
#ifndef _MSC_VER
					ts.save("setSdpAnswer");
#endif
					return true;
				default:
					merr << "No SDP match" << end;
					return false;
			}
		}
	}
	return true;
}
