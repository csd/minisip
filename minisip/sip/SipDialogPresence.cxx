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
 * 	SipDialogPresence.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/


#include<assert.h>
#include"SipDialogPresence.h"
#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipResponse.h>
#include<libmsip/SipSubscribe.h>
#include<libmsip/SipNotify.h>
//#include<libmsip/SipBye.h>
//#include<libmsip/SipCancel.h>
//#include<libmsip/SipAck.h>
#include<libmsip/SipMessageTransport.h>
//#include<libmsip/SipTransactionInviteClientUA.h>
//#include<libmsip/SipTransactionInviteServerUA.h>
#include<libmsip/SipTransactionClient.h>
#include<libmsip/SipTransactionServer.h>
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
 |timerTO    |     set timerTO=2min|
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


void SipDialogPresence::createSubscribeClientTransaction(){
	int seqNo = requestSeqNo();
	MRef<SipTransaction*> subscribetrans = new SipTransactionClient(MRef<SipDialog *>(this), seqNo, callId);
//	subscribetrans->setSocket( getPhoneConfig()->proxyConnection );
	registerTransaction(subscribetrans);
	sendSubscribe(subscribetrans->getBranch());
}
   

bool SipDialogPresence::a0_start_trying_presence(const SipSMCommand &command){

	if (transitionMatch(command, SipCommandString::start_presence)){
#ifdef DEBUG_OUTPUT
		merr << "SipDialogPresence::a0: Presence toUri is: <"<< command.getCommandString().getParam()<< ">"<< end;
#endif
		toUri = MRef<SipIdentity*>( new SipIdentity(command.getCommandString().getParam()) );
		createSubscribeClientTransaction();
		return true;
	}else{
		return false;
	}

}

bool SipDialogPresence::a1_X_subscribing_200OK(const SipSMCommand &command){
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "2**")){
		MRef<SipResponse*> resp(  (SipResponse*)*command.getCommandPacket() );
		getDialogConfig().tag_foreign = command.getCommandPacket()->getHeaderTo()->getTag();

		MRef<SipHeaderExpires *> expireshdr = (SipHeaderExpires*)*resp->getHeaderOfType(SIP_HEADER_TYPE_EXPIRES);
		int timeout;
		if (expireshdr){
			timeout = expireshdr->getTimeout();
		}else{
			mdbg << "WARNING: SipDialogPresence did not contain any expires header - using 300 seconds"<<end;
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

bool SipDialogPresence::a2_trying_retrywait_transperror(const SipSMCommand &command){
	if (transitionMatch(command, SipCommandString::transport_error )){
		mdbg << "WARNING: Transport error when subscribing - trying again in five minutes"<< end;
		requestTimeout(300, "timerDoSubscribe");
		return true;
	}else{
		return false;
	}

}

bool SipDialogPresence::a4_X_trying_timerTO(const SipSMCommand &command){
	if (transitionMatch(command, "timerDoSubscribe")){
		createSubscribeClientTransaction();
		return true;
	}else{
		return false;
	}
}


bool SipDialogPresence::a5_subscribing_subscribing_NOTIFY(const SipSMCommand &command){
	return false;

}

bool SipDialogPresence::a6_subscribing_termwait_stoppresence(const SipSMCommand &command){
	return false;

}

bool SipDialogPresence::a7_termwait_terminated_notransactions(const SipSMCommand &command){
	if (transitionMatch(command, SipCommandString::no_transactions) ){
		SipSMCommand cmd(
				CommandString( callId, SipCommandString::call_terminated),
				SipSMCommand::TU,
				SipSMCommand::DIALOGCONTAINER);
		getDialogContainer()->enqueueCommand( cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE );
		return true;
	}else{
		return false;
	}
}

#if 0
 
bool SipDialogPresence::a0_start_callingnoauth_invite( const SipSMCommand &command)
{
	if (transitionMatch(command, SipCommandString::invite)){
#ifdef MINISIP_MEMDEBUG
		vc.setUser("WARNING - transaction");
#endif


		int seqNo = requestSeqNo();
		setLocalCalled(false);
		getDialogConfig().uri_foreign = command.getCommandString().getParam();

		MRef<SipTransaction*> invtrans = new SipTransactionInviteClientUA(MRef<SipDialog *>(this), seqNo, callId);
		
		invtrans->setSocket( getPhoneConfig()->proxyConnection );
		
		registerTransaction(invtrans);

		sendInvite(invtrans->getBranch());

		return true;
	}else{
		return false;
	}

}

bool SipDialogPresence::a1_callingnoauth_callingnoauth_18X( const SipSMCommand &command)
{
	
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "18*")){

	    MRef<SipResponse*> resp= (SipResponse*) *command.getCommandPacket();

	    ts.save( RINGING );
	    CommandString cmdstr(callId, SipCommandString::remote_ringing);
	    getDialogContainer()->getCallback()->sipcb_handleCommand(cmdstr);
	    getDialogConfig().tag_foreign = command.getCommandPacket()->getHeaderTo()->getTag();

	 //   MRef<SdpPacket*> sdp((SdpPacket*)*resp->getContent());
	   // if ( !sdp.isNull() ){
	    //	vc->getMediaSession()->setSdpAnswer( sdp );
	    //}
	    //		else{
	    //			mdbg << "WARNING: 200 OK did not contain any session description"<< end;
	    //		}

	    return true;
	}else{
	    return false;
	}
}


bool SipDialogPresence::a2_callingnoauth_callingnoauth_1xx( const SipSMCommand &command)
{

	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "1**")){
		getDialogConfig().tag_foreign = command.getCommandPacket()->getHeaderTo()->getTag();
		return true;
	}else{
		return false;
	}
    
}

bool SipDialogPresence::a3_callingnoauth_incall_2xx( const SipSMCommand &command)
{
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "2**")){

		MRef<SipResponse*> resp(  (SipResponse*)*command.getCommandPacket() );

		setLogEntry( new LogEntryOutgoingCompletedCall() );
		getLogEntry()->start = std::time( NULL );
		getLogEntry()->peerSipUri = resp->getFrom().getString();

		getDialogConfig().tag_foreign = command.getCommandPacket()->getHeaderTo()->getTag();

						
                CommandString cmdstr(callId, SipCommandString::invite_ok, "",(getMediaSession()->isSecure()?"secure":"unprotected"));
		
		getDialogContainer()->getCallback()->sipcb_handleCommand(cmdstr);
		
		MRef<SdpPacket *> sdp = (SdpPacket*)*resp->getContent();
		if ( sdp ){
			if( !getMediaSession()->setSdpAnswer( sdp ) ){
				// Some error occured, session rejected
				return false;
			}

			getMediaSession()->start();
		
		}else{
			merr << "WARNING: 200 OK did not contain any session description"<< end;
		}
		
		return true;
	}else{
		return false;
	}
}

bool SipDialogPresence::a5_incall_termwait_BYE( const SipSMCommand &command)
{
	
	if (transitionMatch(command, SipBye::type, SipSMCommand::remote, IGN)){
		MRef<SipBye*> bye = (SipBye*) *command.getCommandPacket();

		//mdbg << "log stuff"<< end;
		if( getLogEntry() ){
			((LogEntrySuccess *)(*( getLogEntry() )))->duration = 
			std::time( NULL ) - getLogEntry()->start; 

			getLogEntry()->handle();
		}

		MRef<SipTransaction*> byeresp = new SipTransactionServer(MRef<SipDialog*>(this), bye->getCSeq(),bye->getLastViaBranch(), callId); //TODO: remove second argument

		registerTransaction(byeresp);
		SipSMCommand cmd(command);
		cmd.setDestination(SipSMCommand::transaction);
		cmd.setSource(command.getSource());
		getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);

		sendByeOk(bye, byeresp->getBranch() );

		CommandString cmdstr(callId, SipCommandString::remote_hang_up);
		getDialogContainer()->getCallback()->sipcb_handleCommand(cmdstr);

		getMediaSession()->stop();

		
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}


bool SipDialogPresence::a6_incall_termwait_hangup(
//		State<SipSMCommand, string> *fromState,
//		State<SipSMCommand, string> *toState,
		const SipSMCommand &command)
{
//	merr << "EEEEEEEE: a6: got command."<< end;
//	merr <<"EEEEE: type is "<< command.getType()<< end;
	if (transitionMatch(command, SipCommandString::hang_up)){
//		merr << "EEEEEE match"<< end;
//		setCurrentState(toState);
		int bye_seq_no= requestSeqNo();
		MRef<SipTransaction*> byetrans( new SipTransactionClient(MRef<SipDialog*>(this), bye_seq_no, callId)); 

		registerTransaction(byetrans);
		sendBye(byetrans->getBranch(), bye_seq_no);
		
		if (getLogEntry()){
			(dynamic_cast< LogEntrySuccess * >(*( getLogEntry() )))->duration = std::time( NULL ) - getLogEntry()->start; 
			getLogEntry()->handle();
		}
		
		getMediaSession()->stop();

		signalIfNoTransactions();
		return true;
	}else{
//		merr << "EEEEEE no match"<< end;
		return false;
	}
}



bool SipDialogPresence::a7_callingnoauth_termwait_CANCEL(
//		State<SipSMCommand, string> *fromState,
//		State<SipSMCommand, string> *toState,
		const SipSMCommand &command)
{
	if (transitionMatch(command, SipCancel::type, SipSMCommand::remote, IGN)){
//		setCurrentState(toState);

		MRef<SipTransaction*> cancelresp( 
				new SipTransactionServer(
					MRef<SipDialog*>(this), 
					command.getCommandPacket()->getCSeq(), 
					command.getCommandPacket()->getLastViaBranch(), callId ));

		registerTransaction(cancelresp);

		SipSMCommand cmd(command);
		cmd.setSource(SipSMCommand::TU);
		cmd.setDestination(SipSMCommand::transaction);
		getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);

		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}


bool SipDialogPresence::a8_callingnoauth_termwait_cancel(
//		State<SipSMCommand, string> *fromState,
//		State<SipSMCommand, string> *toState,
		const SipSMCommand &command)
{
	if (transitionMatch(command, SipCommandString::cancel) || transitionMatch(command, SipCommandString::hang_up)){
//		setCurrentState(toState);

		MRef<SipTransaction*> canceltrans( new SipTransactionClient(MRef<SipDialog*>( this ), /*vc->*/getDialogConfig().seqNo, callId)); 

		registerTransaction(canceltrans);
		sendCancel(canceltrans->getBranch());

		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}

//Note: This is also used as: callingauth_terminated_36
bool SipDialogPresence::a9_callingnoauth_termwait_36( const SipSMCommand &command)
{
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "3**\n4**\n5**\n6**")){
		
		MRef<LogEntry *> rejectedLog( new LogEntryCallRejected() );
		rejectedLog->start = std::time( NULL );
		rejectedLog->peerSipUri = getDialogConfig().uri_foreign;
		
		if (sipResponseFilterMatch(MRef<SipResponse*>((SipResponse*)*command.getCommandPacket()),"404")){
                        CommandString cmdstr(callId, SipCommandString::remote_user_not_found);
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
                        
                        CommandString cmdstr( callId, SipCommandString::remote_unacceptable);
			getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
		}
		else if (sipResponseFilterMatch(MRef<SipResponse*>((SipResponse*)*command.getCommandPacket()),"4**")){
			((LogEntryFailure *)*rejectedLog)->error =
				"User rejected the call";
			setLogEntry( rejectedLog );
			rejectedLog->handle();
                        CommandString cmdstr( callId, SipCommandString::remote_reject);
			getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
		}
		else{
			merr << "ERROR: received response in SipDialogPresence"
				" that could not be handled (unimplemented)"<< end;
                }
		
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}

bool SipDialogPresence::a10_start_ringing_INVITE( const SipSMCommand &command)
{
	if (transitionMatch(command, SipInvite::type, IGN, SipSMCommand::TU)){

		getDialogConfig().uri_foreign = command.getCommandPacket()->getHeaderFrom()->getUri().getUserId()+"@"+ 
			command.getCommandPacket()->getHeaderFrom()->getUri().getIp();

		getDialogConfig().inherited.sipIdentity->setSipUri(command.getCommandPacket()->getHeaderTo()->getUri().getUserIpString().substr(4));

		setSeqNo(command.getCommandPacket()->getCSeq() );
		getDialogConfig().tag_foreign = command.getCommandPacket()->getHeaderFrom()->getTag();

		setLocalCalled(true);
		setLastInvite(MRef<SipInvite*>((SipInvite *)*command.getCommandPacket()));

		/* Give the SDP to the MediaSession */
		MRef<SdpPacket *> sdpOffer = 
			(SdpPacket*)*command.getCommandPacket()->getContent();
		
		if( sdpOffer ){
			if( !getMediaSession()->setSdpOffer( sdpOffer ) ){
				// The offer is rejected, send 6XX;
				return false;
			}
		}

		MRef<SipTransaction*> ir( new SipTransactionInviteServerUA(
						MRef<SipDialog*>( this ), 
						command.getCommandPacket()->getCSeq(),
						command.getCommandPacket()->getLastViaBranch(), callId) );

		registerTransaction(ir);

		//	command.setSource(SipSMCommand::TU);
		//	command.setDestination(SipSMCommand::transaction);

		//	mdbg << "^^^^ SipDialogPresence: re-handling packet for transaction to catch:"<<command<<end;

		SipSMCommand cmd(command);
		cmd.setDestination(SipSMCommand::transaction);

		getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);

		CommandString cmdstr(callId, 
				SipCommandString::incoming_available, 
				getDialogConfig().uri_foreign, 
				(getMediaSession()->isSecure()?"secure":"unprotected")
				);
		getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
		sendRinging(ir->getBranch());
		return true;
	}else{
		return false;
	}
}

bool SipDialogPresence::a11_ringing_incall_accept( const SipSMCommand &command)
{
	if (transitionMatch(command, SipCommandString::accept_invite)){
		ts.save(USER_ACCEPT);

		CommandString cmdstr(callId, 
				SipCommandString::invite_ok,"",
				(getMediaSession()->isSecure()?"secure":"unprotected")
				);
		getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );

		assert( !getLastInvite().isNull() );
		sendInviteOk(getLastInvite()->getDestinationBranch() );

		getMediaSession()->start();

		MRef<LogEntry *> logEntry = new LogEntryIncomingCompletedCall();

		logEntry->start = std::time( NULL );
		logEntry->peerSipUri = getLastInvite()->getFrom().getString();
		
		setLogEntry( logEntry );
		
		return true;
	}else{
		return false;
	}
}

bool SipDialogPresence::a12_ringing_termwait_CANCEL( const SipSMCommand &command)
{

	if (transitionMatch(command, SipCancel::type, IGN, IGN)){

		//FIXME: is this correct - this should probably be handled
		//in the already existing transaction.
		MRef<SipTransaction*> cr( new SipTransactionServer(MRef<SipDialog*>(this), command.getCommandPacket()->getCSeq(), command.getCommandPacket()->getLastViaBranch(), callId) );
		registerTransaction(cr);

		SipSMCommand cmd(command);
		cmd.setDestination(SipSMCommand::transaction);

		//	mdbg << "^^^^ SipDialogPresence: re-handling packet for transaction to catch."<<end;
		getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);

		//	mdbg << "^^^^ SipDialogPresence: sending ok to cancel packet"<<end;
		/* Tell the GUI */
		CommandString cmdstr(callId, SipCommandString::remote_cancelled_invite,"");
		getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );

		sendInviteOk(cr->getBranch() ); 

		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}


bool SipDialogPresence::a13_ringing_termwait_reject( const SipSMCommand &command)
{
	
	if (transitionMatch(command, SipCommandString::reject_invite) || transitionMatch(command,SipCommandString::hang_up)){


		sendReject( getLastInvite()->getDestinationBranch() );

		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}


bool SipDialogPresence::a16_start_termwait_INVITE( const SipSMCommand &command){
	
	if (transitionMatch(command, SipInvite::type, SipSMCommand::remote, SipSMCommand::TU)){

		setLastInvite(MRef<SipInvite*>((SipInvite *)*command.getCommandPacket()));

		MRef<SipTransaction*> ir( new SipTransactionInviteServerUA(MRef<SipDialog*>(this), command.getCommandPacket()->getCSeq(), command.getCommandPacket()->getLastViaBranch(), callId ));

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

bool SipDialogPresence::a20_callingnoauth_callingauth_40X( const SipSMCommand &command){

	if (transitionMatch(command,SipResponse::type, IGN, SipSMCommand::TU, "407\n401")){
		
		MRef<SipResponse*> resp( (SipResponse*)*command.getCommandPacket() );

		getDialogConfig().tag_foreign = command.getCommandPacket()->getHeaderTo()->getTag();

		int seqNo = requestSeqNo();
		MRef<SipTransaction*> trans( new SipTransactionInviteClientUA(MRef<SipDialog*>(this), seqNo, callId));
		registerTransaction(trans);
		
		setRealm( resp->getRealm() );
		setNonce( resp->getNonce() );
		sendAuthInvite(trans->getBranch());

		return true;
	}else{
		return false;
	}
}


bool SipDialogPresence::a21_callingauth_callingauth_18X( const SipSMCommand &command){
	
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "18*")){
		MRef<SipResponse*> resp (  (SipResponse*)*command.getCommandPacket()  );
		ts.save( RINGING );

		CommandString cmdstr(callId, SipCommandString::remote_ringing);
		getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
		getDialogConfig().tag_foreign = command.getCommandPacket()->getHeaderTo()->getTag();
//		if ( !resp->getContent().isNull()){
//			MRef<SdpPacket *> sdp = (SdpPacket*)*resp->getContent();
//			vc->getMediaSession()->setSdpAnswer( sdp );
//
//		}
		//		else{
		//			merr << "WARNING: 200 OK did not contain any session description"<< end;
		//		}

		return true;
	}else{
		return false;
	}
}

bool SipDialogPresence::a22_callingauth_callingauth_1xx( const SipSMCommand &command){
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "1**")){
		getDialogConfig().tag_foreign = command.getCommandPacket()->getHeaderTo()->getTag();
		return true;
	}else{
		return false;
	}
}

bool SipDialogPresence::a23_callingauth_incall_2xx( const SipSMCommand &command){
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "2**")){
		MRef<SipResponse*> resp( (SipResponse*)*command.getCommandPacket() );
		
		setLogEntry( new LogEntryOutgoingCompletedCall() );
		getLogEntry()->start = std::time( NULL );
		getLogEntry()->peerSipUri = resp->getFrom().getString();

		getDialogConfig().tag_foreign = command.getCommandPacket()->getHeaderTo()->getTag();


		CommandString cmdstr(callId, 
				SipCommandString::invite_ok, 
				"",
				(getMediaSession()->isSecure()?"secure":"unprotected")
				);
		getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );


		MRef<SdpPacket *> sdp = (SdpPacket*)*resp->getContent();
		if ( sdp ){
			if( !getMediaSession()->setSdpAnswer( sdp ) ){
				// Some error occured, session rejected
				return false;
			}

			getMediaSession()->start();

		}else{
			merr << "WARNING: 200 OK did not contain any session description"<< end;
		}


		return true;
	}else{
		return false;
	}
}

bool SipDialogPresence::a24_calling_termwait_2xx(
//		State<SipSMCommand, string> *fromState,
//		State<SipSMCommand, string> *toState,
		const SipSMCommand &command){
	
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "2**")){

		int bye_seq_no= requestSeqNo();
//		setCurrentState(toState);

		MRef<SipTransaction*> byetrans = new SipTransactionClient(MRef<SipDialog*>(this), getDialogConfig().seqNo, callId); 


		registerTransaction(byetrans);
		sendBye(byetrans->getBranch(), bye_seq_no);

		CommandString cmdstr(callId, SipCommandString::security_failed);
		getDialogContainer()->getCallback()->sipcb_handleCommand(cmdstr);

		signalIfNoTransactions();
		return true;
	} else{
		return false;
	}
}


bool SipDialogPresence::a25_termwait_terminated_notransactions( const SipSMCommand &command){
	if (transitionMatch(command, SipCommandString::no_transactions) ){
		lastInvite=NULL;
		SipSMCommand cmd(
				CommandString( callId, SipCommandString::call_terminated),
				SipSMCommand::TU,
				SipSMCommand::DIALOGCONTAINER);

		getDialogContainer()->enqueueCommand( cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE );
		return true;
	}else{
		return false;
	}
}


bool SipDialogPresence::a26_callingnoauth_termwait_transporterror( const SipSMCommand &command){
	if (transitionMatch(command, SipCommandString::transport_error )){
		CommandString cmdstr(callId, SipCommandString::transport_error);
		getDialogContainer()->getCallback()->sipcb_handleCommand(cmdstr);
		return true;
	}else{
		return false;
	}
}


//Copy of a8!
bool SipDialogPresence::a26_callingauth_termwait_cancel(
//		State<SipSMCommand, string> *fromState,
//		State<SipSMCommand, string> *toState,
		const SipSMCommand &command)
{
	if (transitionMatch(command, SipCommandString::cancel) || transitionMatch(command, SipCommandString::hang_up)){
//		setCurrentState(toState);
		MRef<SipTransaction*> canceltrans( new SipTransactionClient(MRef<SipDialog*>(this), getDialogConfig().seqNo, callId)); 
		registerTransaction(canceltrans);
		sendCancel(canceltrans->getBranch());

		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}

#endif


void SipDialogPresence::setUpStateMachine(){

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
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogPresence::a0_start_trying_presence,
		s_start, s_trying);
	
 	new StateTransition<SipSMCommand,string>(this, "transition_trying_subscribing_200OK",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogPresence::a1_X_subscribing_200OK,
		s_trying, s_subscribing);
       
 	new StateTransition<SipSMCommand,string>(this, "transition_trying_retrywait",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogPresence::a2_trying_retrywait_transperror,
		s_trying, s_retry_wait);
 
 	new StateTransition<SipSMCommand,string>(this, "transition_retrywait_subscribing_200OK",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogPresence::a1_X_subscribing_200OK,
		s_retry_wait, s_subscribing);

 	new StateTransition<SipSMCommand,string>(this, "transition_subscribing_trying_timerTO",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogPresence::a4_X_trying_timerTO,
		s_subscribing, s_trying);
	
 	new StateTransition<SipSMCommand,string>(this, "transition_retrywait_trying_timerTO",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogPresence::a4_X_trying_timerTO,
		s_subscribing, s_trying);



	//Should add this/similar transition to the trying case (when
	//updating registration, we should be ready to receive a NOTIFY).
	new StateTransition<SipSMCommand,string>(this, "transition_subscribing_subscribing_NOTIFY",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogPresence::a5_subscribing_subscribing_NOTIFY,
			s_subscribing, s_subscribing);

	new StateTransition<SipSMCommand,string>(this, "transition_subscribing_termwait_stoppresence",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogPresence::a6_subscribing_termwait_stoppresence,
			s_subscribing, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_termwait_terminated_notransactions",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogPresence::a7_termwait_terminated_notransactions,
		s_termwait, s_terminated);


	setCurrentState(s_start);
}



SipDialogPresence::SipDialogPresence(MRef<SipDialogContainer*> dContainer, const SipDialogConfig &callconfig/*, MRef<SipSoftPhoneConfiguration*> pconf*/,MRef<TimeoutProvider<string, MRef<StateMachine<SipSMCommand,string>*> > *> tp, bool use_stun) : 
                SipDialog(dContainer,callconfig/*, pconf->timeoutProvider*/, tp),
//                lastInvite(NULL), 
//		phoneconf(pconf)
		useSTUN(use_stun)
{

	//setCallId( itoa(rand())+"@"+getDialogConfig().inherited.externalContactIP);
	callId = itoa(rand())+"@"+getDialogConfig().inherited.externalContactIP;
	
	getDialogConfig().tag_local=itoa(rand());
	
	setUpStateMachine();
}

SipDialogPresence::~SipDialogPresence(){	
}

/*
void SipDialogPresence::handleSdp(MRef<SdpPacket*> sdp){

}
*/

/*
void SipDialogPresence::setCallId(string callid){
	callId = callid;
}
*/

/*
void SipDialogPresence::sendInvite(const string &branch){
	//	mdbg << "ERROR: SipDialogPresence::sendInvite() UNIMPLEMENTED"<< end;
	
	MRef<SipInvite*> inv;
	string keyAgreementMessage;
	int32_t localSipPort;

	if(getDialogConfig().inherited.transport=="TCP")
		localSipPort = getDialogConfig().inherited.localTcpPort;
	else if(getDialogConfig().inherited.transport=="TLS")
		localSipPort = getDialogConfig().inherited.localTlsPort;
	else{ // UDP, may use STUN 
            if( getPhoneConfig()->useSTUN ){
		localSipPort = getDialogConfig().inherited.externalContactUdpPort;
            } else {
                localSipPort = getDialogConfig().inherited.localUdpPort;
            }
        }
	
	inv= MRef<SipInvite*>(new SipInvite(
				branch,
				callId,
				getDialogConfig().uri_foreign,
				//getDialogConfig().inherited.sipIdentity->sipProxy.sipProxyIpAddr->getString(),
				getDialogConfig().inherited.sipIdentity->sipDomain,	//TODO: Change API - not sure if proxy or domain
				getDialogConfig().inherited.sipIdentity->sipProxy.sipProxyPort,
//				getDialogConfig().inherited.localIpString,
				getDialogConfig().inherited.externalContactIP,
				localSipPort,
				//getDialogConfig().inherited.userUri,
				getDialogConfig().inherited.sipIdentity->getSipUri(),
				getDialogConfig().seqNo,
				getDialogConfig().inherited.transport));

	// Get the session description from the Session 
	MRef<SdpPacket *> sdp = mediaSession->getSdpOffer();

	if( !sdp ){
		// FIXME: this most probably means that the
		// creation of the MIKEY message failed, it 
		// should not happen
		merr << "Sdp was NULL in sendInvite" << end;
		return; 
	}

	// Add the latter to the INVITE message 
	inv->setContent( *sdp );
	
#ifdef MINISIP_MEMDEBUG
	inv.setUser("SipDialogPresence");
#endif
	inv->getHeaderFrom()->setTag(getDialogConfig().tag_local);

//	mdbg << "SipDialogPresence::sendInvite(): sending INVITE to transaction"<<end;
//	ts.save( INVITE_END );
        MRef<SipMessage*> pktr(*inv);
#ifdef MINISIP_MEMDEBUG
	pktr.setUser("SipDialogPresence");
#endif

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
*/

void SipDialogPresence::sendSubscribe(const string &branch){
	
	MRef<SipSubscribe*> sub;
	int32_t localSipPort;

	if(getDialogConfig().inherited.transport=="TCP")
		localSipPort = getDialogConfig().inherited.localTcpPort;
	else if(getDialogConfig().inherited.transport=="TLS")
		localSipPort = getDialogConfig().inherited.localTlsPort;
	else{ /* UDP, may use STUN */
            if( /*phoneconf->*/useSTUN ){
		localSipPort = getDialogConfig().inherited.externalContactUdpPort;
            } else {
                localSipPort = getDialogConfig().inherited.localUdpPort;
            }
        }
	
	sub = MRef<SipSubscribe*>(new SipSubscribe(
				branch,
				callId,
				toUri,
				getDialogConfig().inherited.sipIdentity,
				getDialogConfig().inherited.localUdpPort,
//				im_seq_no
				getDialogConfig().seqNo
				));

	sub->getHeaderFrom()->setTag(getDialogConfig().tag_local);

        MRef<SipMessage*> pktr(*sub);
#ifdef MINISIP_MEMDEBUG
	pktr.setUser("SipDialogPresence");
#endif

        SipSMCommand scmd(
                pktr, 
                SipSMCommand::TU, 
                SipSMCommand::transaction
                );
	
	getDialogContainer()->enqueueCommand(scmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
//	setLastSubsc(inv);

}

#if 0
void SipDialogPresence::sendAuthInvite(const string &branch){
	//	merr << "ERROR: SipDialogPresence::sendAuthInvite() UNIMPLEMENTED"<< end;
//	string call_id = getDialogConfig().callId;
	//SipInvite * inv;
	MRef<SipInvite*> inv;
	string keyAgreementMessage;
	int32_t localSipPort;

	if(getDialogConfig().inherited.transport=="TCP")
		localSipPort = getDialogConfig().inherited.localTcpPort;
	else if(getDialogConfig().inherited.transport=="TLS")
		localSipPort = getDialogConfig().inherited.localTlsPort;
	else{ /* UDP, may use STUN */
            if( getPhoneConfig()->useSTUN ){
		localSipPort = getDialogConfig().inherited.externalContactUdpPort;
            } else {
                localSipPort = getDialogConfig().inherited.localUdpPort;
            }
        }

	
		inv= new SipInvite(
				branch,
				callId,
				getDialogConfig().uri_foreign,
				//getDialogConfig().inherited.sipIdentity->sipProxy.sipProxyIpAddr->getString(),
				getDialogConfig().inherited.sipIdentity->sipDomain,
				getDialogConfig().inherited.sipIdentity->sipProxy.sipProxyPort,
//				getDialogConfig().inherited.localIpString,
				getDialogConfig().inherited.externalContactIP,
				localSipPort,
				//getDialogConfig().inherited.userUri,
				getDialogConfig().inherited.sipIdentity->getSipUri(),
				getDialogConfig().seqNo,
//				requestSeqNo(),
				getDialogConfig().inherited.sipIdentity->sipProxy.sipProxyUsername,
				nonce,
				realm,
				getDialogConfig().inherited.sipIdentity->sipProxy.sipProxyPassword,
				getDialogConfig().inherited.transport);

	inv->getHeaderFrom()->setTag(getDialogConfig().tag_local);
	
	/* Get the session description from the Session */
	MRef<SdpPacket *> sdp = mediaSession->getSdpOffer();

	/* Add the latter to the INVITE message */
	inv->setContent( *sdp );

        MRef<SipMessage*> pref(*inv);
        SipSMCommand cmd(pref, SipSMCommand::TU, SipSMCommand::transaction);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
	setLastInvite(inv);

}


#ifdef NEVERDEFINED_ERSADFS
void SipDialogPresence::sendAck(string branch){
	//	mdbg << "ERROR: SipDialogPresence::sendAck() UNIMPLEMENTED" << end;
	assert( !lastResponse.isNull());
	SipAck *ack = new SipAck(
			branch, 
			*lastResponse,
			getDialogConfig().uri_foreign,
			//getDialogConfig().inherited.sipIdentity->sipProxy.sipProxyIpAddr->getString());
			getDialogConfig().inherited.sipIdentity->sipDomain);
	//TODO:
	//	ack.add_header( new SipHeaderRoute(getDialog()->getRouteSet() ) );
//	mdbg << "SipDialogPresence:sendAck(): sending ACK directly to remote" << end;

	//	if(socket == NULL){
	// No StreamSocket, create one or use UDP
//	Socket *sock=NULL;
	
	if(getDialogConfig().proxyConnection == NULL){
		getDialogConfig().inherited.sipTransport->sendMessage(ack,
				*(getDialogConfig().inherited.sipIdentity->sipProxy.sipProxyIpAddr), //*toaddr,
				getDialogConfig().inherited.sipProxy.proxyPort, //port, 
//				sock, //(Socket *)NULL, //socket, 
				getDialogConfig().proxyConnection,
				"BUGBUGBUG",
				getDialogConfig().inherited.transport
				);
	}else{
		// A StreamSocket exists, try to use it
		mdbg << "Sending packet using existing StreamSocket"<<end;
		getDialogConfig().inherited.sipTransport->sendMessage(
				ack,
				(StreamSocket *)getDialogConfig().proxyConnection, "BUGBUGBUG");
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

void SipDialogPresence::sendBye(const string &branch, int bye_seq_no){

	//string tmp = getDialogConfig().inherited.userUri;
	string tmp = getDialogConfig().inherited.sipIdentity->getSipUri();
	uint32_t i = tmp.find("@");
	assert(i!=string::npos);
	i++;
	string domain;
	for ( ; i < tmp.length() ; i++)
		domain = domain+tmp[i];

//	mdbg << "///////////Creating bye with uri_foreign="<<getDialogConfig().uri_foreign << " and doman="<< domain<< end;
	MRef<SipBye*> bye = new SipBye(
			branch,
			getLastInvite(),
			getDialogConfig().uri_foreign,
			//getDialogConfig().inherited.userUri,
			getDialogConfig().inherited.sipIdentity->getSipUri(),
			domain,
//			getDialogConfig().seqNo+1,
			bye_seq_no,
			localCalled
			);

	bye->getHeaderFrom()->setTag(getDialogConfig().tag_local);
	bye->getHeaderTo()->setTag(getDialogConfig().tag_foreign);

        MRef<SipMessage*> pref(*bye);
        SipSMCommand cmd( pref, SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand(cmd);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}

void SipDialogPresence::sendCancel(const string &branch){
	assert( !lastInvite.isNull());
	MRef<SipCancel*> cancel = new SipCancel(
			branch,
			lastInvite,
			getDialogConfig().uri_foreign,
			//getDialogConfig().inherited.userUri,
			getDialogConfig().inherited.sipIdentity->getSipUri(),
			//getDialogConfig().inherited.sipIdentity->sipProxy.sipProxyIpAddr->getString(),
			getDialogConfig().inherited.sipIdentity->sipDomain,
			localCalled
			);

	cancel->getHeaderFrom()->setTag(getDialogConfig().tag_local);
	cancel->getHeaderTo()->setTag(getDialogConfig().tag_foreign);

        MRef<SipMessage*> pref(*cancel);
        SipSMCommand cmd( pref, SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand( cmd );
	getDialogContainer()->enqueueCommand( cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE );

}

void SipDialogPresence::sendInviteOk(const string &branch){
	MRef<SipResponse*> ok= new SipResponse(branch, 200,"OK", MRef<SipMessage*>(*getLastInvite()));	
	ok->getHeaderTo()->setTag(getDialogConfig().tag_local);


	/* Get the SDP Answer from the MediaSession */
	MRef<SdpPacket *> sdpAnswer = mediaSession->getSdpAnswer();

	if( sdpAnswer ){
		ok->setContent( *sdpAnswer );
	}
	/* if sdp is NULL, the offer was refused, send 606 */
	// FIXME
	else return; 

//	setLastResponse(ok);
        MRef<SipMessage*> pref(*ok);
        SipSMCommand cmd( pref, SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand(cmd);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}

void SipDialogPresence::sendByeOk(MRef<SipBye*> bye, const string &branch){
	MRef<SipResponse*> ok= new SipResponse( branch, 200,"OK", MRef<SipMessage*>(*bye) );
	ok->getHeaderTo()->setTag(getDialogConfig().tag_local);

//	setLastResponse(ok);
        MRef<SipMessage*> pref(*ok);
        SipSMCommand cmd( pref, SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand(cmd);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}

void SipDialogPresence::sendReject(const string &branch){
	MRef<SipResponse*> ringing = new SipResponse(branch,486,"Temporary unavailable", MRef<SipMessage*>(*getLastInvite()));	
	ringing->getHeaderTo()->setTag(getDialogConfig().tag_local);
//	setLastResponse(ringing);
        MRef<SipMessage*> pref(*ringing);
        SipSMCommand cmd( pref,SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand(cmd);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}

void SipDialogPresence::sendRinging(const string &branch){
	MRef<SipResponse*> ringing = new SipResponse(branch,180,"Ringing", MRef<SipMessage*>(*getLastInvite()));	
	ringing->getHeaderTo()->setTag(getDialogConfig().tag_local);
//	setLastResponse(ringing);
        MRef<SipMessage*> pref(*ringing);
        SipSMCommand cmd( pref, SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand(cmd);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}

void SipDialogPresence::sendNotAcceptable(const string &branch){
	MRef<SipResponse*> not_acceptable = new SipResponse(branch,606,"Not Acceptable", MRef<SipMessage*>(*getLastInvite()));	
#ifdef MINISIP_MEMDEBUG
	not_acceptable.setUser("SipDialogPresence");
#endif
	not_acceptable->getHeaderTo()->setTag(getDialogConfig().tag_local);
//	setLastResponse(not_acceptable);
        MRef<SipMessage*> pref(*not_acceptable);
#ifdef MINISIP_MEMDEBUG
	pref.setUser("SipDialogPresence");
#endif
        SipSMCommand cmd( pref, SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand(cmd);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}
#endif


bool SipDialogPresence::handleCommand(const SipSMCommand &c){
	mdbg << "SipDialogPresence::handleCommand got "<< c << end;
	merr << "XXXSipDialogPresence::handleCommand got "<< c << end;

	if (c.getType()==SipSMCommand::COMMAND_STRING && callId.length()>0){
		if (c.getCommandString().getDestinationId() != callId ){
			cerr << "SipDialogPresence returning false based on callId"<< endl;
			return false;
		}
	}
	
	if (c.getType()==SipSMCommand::COMMAND_PACKET  && callId.length()>0){
		if (c.getCommandPacket()->getCallId() != callId ){
			return false;
		}
		if (c.getType()!=SipSMCommand::COMMAND_PACKET && 
				c.getCommandPacket()->getCSeq()!= getDialogConfig().seqNo){
			return false;
		}
	
	}
	
//	if (c.getType()!=SipSMCommand::COMMAND_PACKET && 
//			c.getCommandPacket()->getCSeq()!= command_seq_no)
//		return false;
	
	mdbg << "SipDialogPresence::handlePacket() got "<< c << end;
	merr << "SipDialogPresence returning dialogs handleCommand"<< end;
	bool handled = SipDialog::handleCommand(c);
	
	if (!handled && c.getType()==SipSMCommand::COMMAND_STRING && c.getCommandString().getOp()==SipCommandString::no_transactions){
		return true;
	}
	
	if (c.getType()==SipSMCommand::COMMAND_STRING && callId.length()>0){
		if (c.getCommandString().getDestinationId() == callId ){
			mdbg << "Warning: SipDialogPresence ignoring command with matching call id"<< end;
			return true;
		}
	}
	if (c.getType()==SipSMCommand::COMMAND_PACKET && callId.length()>0){
		if (c.getCommandPacket()->getCallId() == callId){
			mdbg << "Warning: SipDialogPresence ignoring packet with matching call id"<< end;
			return true;
		}
	}
	
	return handled;
}

#if 0
MRef<SipInvite*> SipDialogPresence::getLastInvite(){
    return lastInvite;
}

void SipDialogPresence::setLastInvite(MRef<SipInvite*> i){ 
    lastInvite = i; 
#ifdef MINISIP_MEMDEBUG
    lastInvite.setUser("SipDialogPresence::lastInvite");
#endif
}

MRef<LogEntry *> SipDialogPresence::getLogEntry(){
	return logEntry;
}

void SipDialogPresence::setLogEntry( MRef<LogEntry *> logEntry ){
	this->logEntry = logEntry;
}

MRef<Session *> SipDialogPresence::getMediaSession(){
	return mediaSession;
}

#endif


