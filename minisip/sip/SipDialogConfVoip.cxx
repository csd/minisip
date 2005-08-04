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
 *	Johan Bilien <jobi@via.ecp.fr>
 *	Joachim Orrblad <joachim[at]orrblad.com>
 */	    

/* Name
 * 	SipDialogConfVoip.cxx
 * Authors
 * 	Erik Eliasson, eliasson@it.kth.se
 	Bilge Cetin <bilge[at]kth.se>
	Max Loubser <loubser[at]kth.se>
*/
 
#include<config.h>

#include<assert.h>
#include"SipDialogConfVoip.h"
#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipResponse.h>
#include<libmsip/SipBye.h>
#include<libmsip/SipCancel.h>
#include<libmsip/SipAck.h>
#include<libmsip/SipMessageTransport.h>
#include<libmsip/SipTransactionInviteClientUA.h>
#include<libmsip/SipTransactionInviteServerUA.h>
#include<libmsip/SipTransactionNonInviteClient.h>
#include<libmsip/SipTransactionNonInviteServer.h>
#include<libmsip/SipTransactionUtils.h>
#include<libmsip/SipDialog.h>
#include<libmsip/SipCommandString.h>
#include<libmsip/SipHeaderWarning.h>
#include<libmsip/SipMIMEContent.h>
#include<libmsip/SipMessageContent.h>
#include"DefaultDialogHandler.h"
#include"../minisip/MessageRouter.h"
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
  |                X   ^ |     |       2xx/a3: send ACK       |                |
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
  |                      v a23: [Xsend ACKX]                  |                |
  |              +---------------+                            |                |
  |          +---|               |<---------------------------+                |
  |  ACK            |   in call     |-------+                                     |
  |  a27     +---|               |       |                                     |
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
   
*/
 
 
bool SipDialogConfVoip::a0_start_callingnoauth_invite( const SipSMCommand &command)
{//cerr<<"************************************invite receivedqwe"<<endl;
	if (transitionMatch(command, SipCommandString::invite)){
#ifndef _MSC_VER
		ts.save("a0_start_callingnoauth_invite");
#endif
		//cerr<<"sending invite1*************"<<endl;
		//int seqNo = requestSeqNo();
		//cerr<<"dialogState.seqNo*************"+itoa(dialogState.seqNo)<<endl;
		++dialogState.seqNo;
//		setLocalCalled(false);
		localCalled=false;
		dialogState.remoteUri= command.getCommandString().getParam();
		//cerr<<"dialogState.callId*************"+dialogState.callId<<endl;
		//cerr<<"dialogState.seqNo*************"+itoa(dialogState.seqNo)<<endl;
		MRef<SipTransaction*> invtrans = new SipTransactionInviteClient(sipStack, MRef<SipDialog *>(this), dialogState.seqNo, dialogState.callId);
		
		invtrans->setSocket( phoneconf->proxyConnection );
		//cerr<<"sending invite3*************"<<endl;
		registerTransaction(invtrans);
		//cerr<<"sending invite*************"<<endl;
		/*CommandString cmdstr("", "myuri", getDialogConfig()->inherited.sipIdentity->getSipUri());
		
		cmdstr.setParam3(confId);
		
		//getDialogContainer()->getCallback()->sipcb_handleCommand(cmdstr);
		getDialogContainer()->getCallback()->sipcb_handleConfCommand( cmdstr );*/
		sendInvite(invtrans->getBranch());
		//MessageRouter * ptr=(MessageRouter *)(getDialogContainer()->getCallback());
		//adviceList=(ptr->getConferenceController()->getConnectedList());
		return true;
	}else{
		return false;
	}

}

bool SipDialogConfVoip::a1_callingnoauth_callingnoauth_18X( const SipSMCommand &command)
{	
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "18*")){

	    MRef<SipResponse*> resp= (SipResponse*) *command.getCommandPacket();

#ifndef _MSC_VER
	    ts.save( RINGING );
#endif
	    CommandString cmdstr(dialogState.callId, SipCommandString::remote_ringing);
	    cmdstr.setParam3(confId);
	    //getDialogContainer()->getCallback()->sipcb_handleCommand(cmdstr);
	    getDialogContainer()->getCallback()->sipcb_handleConfCommand( cmdstr );

	    //We must maintain the dialog state. We copy the remote tag and
	    //the remote sequence number
	    dialogState.remoteTag = command.getCommandPacket()->getHeaderValueTo()->getParameter("tag");
	    dialogState.remoteSeqNo = command.getCommandPacket()->getCSeq();
	    
	    string peerUri = command.getCommandPacket()->getFrom().getString();

	    MRef<SdpPacket*> sdp((SdpPacket*)*resp->getContent());
	    if ( !sdp.isNull() ){
		    //Early media
		    getMediaSession()->setSdpAnswer( sdp, peerUri );
	    }

	    return true;
	}
	else{
	    return false;
	}
}


bool SipDialogConfVoip::a2_callingnoauth_callingnoauth_1xx( const SipSMCommand &command)
{

	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "1**")){
		dialogState.remoteTag = command.getCommandPacket()->getHeaderValueTo()->getParameter("tag");
		return true;
	}else{
		return false;
	}
    
}

bool SipDialogConfVoip::a3_callingnoauth_incall_2xx( const SipSMCommand &command)
{
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "2**")){
#ifndef _MSC_VER
		ts.save("a3_callingnoauth_incall_2xx");
#endif

		MRef<SipResponse*> resp(  (SipResponse*)*command.getCommandPacket() );
		lastResponse=resp;
		string peerUri = resp->getFrom().getString();
		setLogEntry( new LogEntryOutgoingCompletedCall() );
		getLogEntry()->start = time( NULL );
		getLogEntry()->peerSipUri = peerUri;

		dialogState.remoteTag = command.getCommandPacket()->getHeaderValueTo()->getParameter("tag");
		//parsing sdp part for conf:
		assert(dynamic_cast<SdpPacket*>(*resp->getContent())!=NULL);
		MRef<SdpPacket*> sdp = (SdpPacket*)*resp->getContent();
		string numToConnect = sdp->getSessionLevelAttribute("conf_#participants");
		int num = 0;

   //--- Convert each digit char and add into result.
   			int t=0;
			while (numToConnect[t] >= '0' && numToConnect[t] <='9') {
      				num = (num * 10) + (numToConnect[t] - '0');
      				t++;
   			}
			string users="";
			for(t=0;t<num;t++)
				//connectList[t]=  sdp->getSessionLevelAttribute("participant_"+itoa(t+1));
				users=users+sdp->getSessionLevelAttribute("participant_"+itoa(t+1))+";";
			//cerr<<"==============users: "+users<<endl;	
                
		
		CommandString cmdstr(dialogState.callId, SipCommandString::invite_ok, users,(getMediaSession()->isSecure()?"secure":"unprotected"),confId);
		
		
		
		//getDialogContainer()->getCallback()->sipcb_handleCommand(cmdstr);
		getDialogContainer()->getCallback()->sipcb_handleConfCommand( cmdstr );
		//cerr<<"****************************sendack is called**********************"<<endl;
		//sendAck(getLastInvite()->getDestinationBranch() );
		//BM
		MessageRouter * ptr=(MessageRouter *)(getDialogContainer()->getCallback());
		adviceList=(ptr->getConferenceController(confId)->getConnectedList());
		sendAck(getLastInvite()->getDestinationBranch());
		
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

bool SipDialogConfVoip::a5_incall_termwait_BYE( const SipSMCommand &command)
{
	
	if (transitionMatch(command, SipBye::type, SipSMCommand::remote, IGN)){
		MRef<SipBye*> bye = (SipBye*) *command.getCommandPacket();

		//mdbg << "log stuff"<< end;
		if( getLogEntry() ){
			((LogEntrySuccess *)(*( getLogEntry() )))->duration = 
			time( NULL ) - getLogEntry()->start; 

			getLogEntry()->handle();
		}

		MRef<SipTransaction*> byeresp = new SipTransactionNonInviteServer(sipStack, MRef<SipDialog*>(this), bye->getCSeq(),bye->getLastViaBranch(), dialogState.callId); //TODO: remove second argument

		registerTransaction(byeresp);
		SipSMCommand cmd(command);
		cmd.setDestination(SipSMCommand::transaction);
		cmd.setSource(command.getSource());
		getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
		
		sendByeOk(bye, byeresp->getBranch() );

		CommandString cmdstr(dialogState.callId, SipCommandString::remote_hang_up);
		cmdstr.setParam3(confId);
		//getDialogContainer()->getCallback()->sipcb_handleCommand(cmdstr);
		getDialogContainer()->getCallback()->sipcb_handleConfCommand( cmdstr );

		getMediaSession()->stop();

		
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}


bool SipDialogConfVoip::a6_incall_termwait_hangup( const SipSMCommand &command)
{
//	merr << "EEEEEEEE: a6: got command."<< end;
//	merr <<"EEEEE: type is "<< command.getType()<< end;
	if (transitionMatch(command, SipCommandString::hang_up)){
//		merr << "EEEEEE match"<< end;
//		setCurrentState(toState);
		//int bye_seq_no= requestSeqNo();
		++dialogState.seqNo;
		MRef<SipTransaction*> byetrans( new SipTransactionNonInviteClient(sipStack, MRef<SipDialog*>(this), dialogState.seqNo, dialogState.callId)); 

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
//		merr << "EEEEEE no match"<< end;
		return false;
	}
}

bool SipDialogConfVoip::a7_callingnoauth_termwait_CANCEL( const SipSMCommand &command)
{
	if (transitionMatch(command, SipCancel::type, SipSMCommand::remote, IGN)){
//		setCurrentState(toState);

		MRef<SipTransaction*> cancelresp( 
				new SipTransactionNonInviteServer(
					sipStack,
					MRef<SipDialog*>(this), 
					command.getCommandPacket()->getCSeq(), 
					command.getCommandPacket()->getLastViaBranch(), dialogState.callId ));

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

bool SipDialogConfVoip::a8_callingnoauth_termwait_cancel( const SipSMCommand &command)
{
	if (transitionMatch(command, SipCommandString::cancel) || transitionMatch(command, SipCommandString::hang_up)){
//		setCurrentState(toState);

		MRef<SipTransaction*> canceltrans( new SipTransactionNonInviteClient(sipStack, MRef<SipDialog*>( this ), dialogState.seqNo, dialogState.callId)); 

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
bool SipDialogConfVoip::a9_callingnoauth_termwait_36( const SipSMCommand &command)
{
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "3**\n4**\n5**\n6**")){
		
		MRef<LogEntry *> rejectedLog( new LogEntryCallRejected() );
		rejectedLog->start = time( NULL );
		rejectedLog->peerSipUri = dialogState.remoteTag;
		
		if (sipResponseFilterMatch(MRef<SipResponse*>((SipResponse*)*command.getCommandPacket()),"404")){
                        CommandString cmdstr(dialogState.callId, SipCommandString::remote_user_not_found);
			cmdstr.setParam3(confId);
			getDialogContainer()->getCallback()->sipcb_handleConfCommand(cmdstr);
			//getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr);
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
			cmdstr.setParam3(confId);
			//getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
			getDialogContainer()->getCallback()->sipcb_handleConfCommand( cmdstr );
		}
		else if (sipResponseFilterMatch(MRef<SipResponse*>((SipResponse*)*command.getCommandPacket()),"4**")){
			((LogEntryFailure *)*rejectedLog)->error =
				"User rejected the call";
			setLogEntry( rejectedLog );
			rejectedLog->handle();
                        CommandString cmdstr( dialogState.callId, SipCommandString::remote_reject);
			cmdstr.setParam3(confId);
			//getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
			getDialogContainer()->getCallback()->sipcb_handleConfCommand( cmdstr );
		}
		else{
			merr << "ERROR: received response in SipDialogConfVoip"
				" that could not be handled (unimplemented)"<< end;
                }
		
		getMediaSession()->stop();
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}

bool SipDialogConfVoip::a10_start_ringing_INVITE( const SipSMCommand &command)
{
	fprintf( stderr, "SipDialogConfVoip::a10_start_ringing_INVITE\n" );
	if (transitionMatch(command, SipInvite::type, IGN, SipSMCommand::TU)){

		dialogState.remoteUri = command.getCommandPacket()->getHeaderValueFrom()->getUri().getUserId()+"@"+ 
			command.getCommandPacket()->getHeaderValueFrom()->getUri().getIp();
		getDialogConfig()->inherited->sipIdentity->setSipUri(command.getCommandPacket()->getHeaderValueTo()->getUri().getUserIpString().substr(4));

		//We must maintain our dialog state. This is the first
		//message we receive for this dialog and we copy the remote
		//tag and the remote sequence number.
		dialogState.remoteTag = command.getCommandPacket()->getHeaderValueFrom()->getParameter("tag");
		dialogState.remoteSeqNo = command.getCommandPacket()->getCSeq();
		//dialogState.seqNo = command.getCommandPacket()->getCSeq(); (we do not need to set to the same at the remote one - remove)
//		setLocalCalled(true);
		localCalled=true;
		
		setLastInvite(MRef<SipInvite*>((SipInvite *)*command.getCommandPacket()));

		string peerUri = command.getCommandPacket()->getFrom().getString().substr(4);
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
		MRef<SipTransaction*> ir( new SipTransactionInviteServer(
						sipStack,
						MRef<SipDialog*>( this ), 
						command.getCommandPacket()->getCSeq(),
						command.getCommandPacket()->getLastViaBranch(), dialogState.callId) );

		registerTransaction(ir);
		//	command.setSource(SipSMCommand::TU);
		//	command.setDestination(SipSMCommand::transaction);

		//	mdbg << "^^^^ SipDialogVoip: re-handling packet for transaction to catch:"<<command<<end;

		SipSMCommand cmd(command);
		cmd.setDestination(SipSMCommand::transaction);

		getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
		if(type=="join")
		{
			string users=confId+";";
			//cerr<<"*************users "+itoa((*adviceList).size())<<endl;
			for(int t=0;t<(*adviceList).size();t++)
				users=users+ (((*adviceList)[t]).uri);
			//cerr<<"*************users "+users<<endl;}	
			/*CommandString cmdstr(dialogState.callId, 
					SipCommandString::incoming_available, 
					dialogState.remoteUri, 
					(getMediaSession()->isSecure()?"secure":"unprotected")
						);*/
			CommandString cmdstr(dialogState.callId, 
				"conf_join_received", 
				dialogState.remoteUri, 
				(getMediaSession()->isSecure()?"secure":"unprotected"),users
				);//bm
			getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
		}
		else
		{
			//cerr<<"a10 confIdidididididi "+confId<<endl;
			CommandString cmdstr(dialogState.callId, 
				"conf_connect_received", 
				dialogState.remoteUri, 
				(getMediaSession()->isSecure()?"secure":"unprotected"),confId);//bm
			getDialogContainer()->getCallback()->sipcb_handleConfCommand( cmdstr );
		}
		//getDialogContainer()->getCallback()->sipcb_handleConfCommand( cmdstr );
		
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

bool SipDialogConfVoip::a11_ringing_incall_accept( const SipSMCommand &command)
{
	
	if (transitionMatch(command, SipCommandString::accept_invite)){
#ifndef _MSC_VER
		ts.save(USER_ACCEPT);
#endif
		//bm
	numConnected=0;
	string users=command.getCommandString().getParam2();
	confId=command.getCommandString().getParam3();
	//cerr<<"users=command.getCommandString().getParam2() "+users<<endl;
	string line="";
	unsigned int i=0;
		while (users.length()!=0 &&!(i>(users.length()-1))){
			line+=users[i++];
			if(users[i]==';')
			{
				connectedList.push_back((ConfMember(line, "")));
				//connectedList[numConnected]=line;
				//cerr<< "line: " + line << endl;
				line="";
				
				numConnected++;
				assert(numConnected==connectedList.size());
				i++;
			}
		}
		
		/*
		for(int t=numConnected;t<10;t++)
			connectedList[t]="";	
		*/
		
	//bm
		CommandString cmdstr(dialogState.callId, 
				SipCommandString::invite_ok,dialogState.remoteUri,
				(getMediaSession()->isSecure()?"secure":"unprotected")
				);
		
		//getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
		//getDialogContainer()->getCallback()->sipcb_handleConfCommand( cmdstr );		

		assert( !getLastInvite().isNull() );
		sendInviteOk(getLastInvite()->getDestinationBranch() );
		CommandString cmdstr2("", "myuri", getDialogConfig()->inherited->sipIdentity->getSipUri());
		
		
		cmdstr2.setParam3(confId);
		//getDialogContainer()->getCallback()->sipcb_handleCommand(cmdstr);
		getDialogContainer()->getCallback()->sipcb_handleConfCommand( cmdstr2 );
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

bool SipDialogConfVoip::a12_ringing_termwait_CANCEL( const SipSMCommand &command)
{

	if (transitionMatch(command, SipCancel::type, IGN, IGN)){

		//FIXME: is this correct - this should probably be handled
		//in the already existing transaction.
		MRef<SipTransaction*> cr( new SipTransactionNonInviteServer(sipStack, MRef<SipDialog*>(this), command.getCommandPacket()->getCSeq(), command.getCommandPacket()->getLastViaBranch(), dialogState.callId) );
		registerTransaction(cr);

		SipSMCommand cmd(command);
		cmd.setDestination(SipSMCommand::transaction);

		//	mdbg << "^^^^ SipDialogVoip: re-handling packet for transaction to catch."<<end;
		getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);

		//	mdbg << "^^^^ SipDialogVoip: sending ok to cancel packet"<<end;
		/* Tell the GUI */
		CommandString cmdstr(dialogState.callId, SipCommandString::remote_cancelled_invite);
		cmdstr.setParam3(confId);
		//getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
		getDialogContainer()->getCallback()->sipcb_handleConfCommand( cmdstr );
		sendInviteOk(cr->getBranch() ); 

		getMediaSession()->stop();
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}


bool SipDialogConfVoip::a13_ringing_termwait_reject( const SipSMCommand &command)
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


bool SipDialogConfVoip::a16_start_termwait_INVITE( const SipSMCommand &command){
	
	if (transitionMatch(command, SipInvite::type, SipSMCommand::remote, SipSMCommand::TU)){

		setLastInvite(MRef<SipInvite*>((SipInvite *)*command.getCommandPacket()));

		MRef<SipTransaction*> ir( new SipTransactionInviteServer(sipStack, MRef<SipDialog*>(this), command.getCommandPacket()->getCSeq(), command.getCommandPacket()->getLastViaBranch(), dialogState.callId ));

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

bool SipDialogConfVoip::a20_callingnoauth_callingauth_40X( const SipSMCommand &command){

	if (transitionMatch(command,SipResponse::type, IGN, SipSMCommand::TU, "407\n401")){
		
		MRef<SipResponse*> resp( (SipResponse*)*command.getCommandPacket() );

		dialogState.remoteTag = command.getCommandPacket()->getHeaderValueTo()->getParameter("tag");

		//int seqNo = requestSeqNo();
		++dialogState.seqNo;
		MRef<SipTransaction*> trans( new SipTransactionInviteClient(sipStack, MRef<SipDialog*>(this), dialogState.seqNo, dialogState.callId));
		registerTransaction(trans);
		
		realm = resp->getRealm();
		nonce = resp->getNonce();

		sendAuthInvite(trans->getBranch());

		return true;
	}else{
		return false;
	}
}


bool SipDialogConfVoip::a21_callingauth_callingauth_18X( const SipSMCommand &command){
	
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "18*")){
		MRef<SipResponse*> resp (  (SipResponse*)*command.getCommandPacket()  );
#ifndef _MSC_VER
		ts.save( RINGING );
#endif

		CommandString cmdstr(dialogState.callId, SipCommandString::remote_ringing);
		cmdstr.setParam3(confId);
		//getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
		getDialogContainer()->getCallback()->sipcb_handleConfCommand( cmdstr );
		dialogState.remoteTag = command.getCommandPacket()->getHeaderValueTo()->getParameter("tag");

		string peerUri = resp->getFrom().getString();
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

bool SipDialogConfVoip::a22_callingauth_callingauth_1xx( const SipSMCommand &command){
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "1**")){
		dialogState.remoteTag = command.getCommandPacket()->getHeaderValueTo()->getParameter("tag");
		return true;
	}else{
		return false;
	}
}

bool SipDialogConfVoip::a23_callingauth_incall_2xx( const SipSMCommand &command){
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "2**")){
		MRef<SipResponse*> resp( (SipResponse*)*command.getCommandPacket() );
		
		string peerUri = resp->getFrom().getString().substr(4);
		setLogEntry( new LogEntryOutgoingCompletedCall() );
		getLogEntry()->start = time( NULL );
		getLogEntry()->peerSipUri = peerUri;

		dialogState.remoteTag = command.getCommandPacket()->getHeaderValueTo()->getParameter("tag");


		CommandString cmdstr(dialogState.callId, 
				SipCommandString::invite_ok, 
				"",
				(getMediaSession()->isSecure()?"secure":"unprotected"), confId);
		//getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
		getDialogContainer()->getCallback()->sipcb_handleConfCommand( cmdstr );

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

bool SipDialogConfVoip::a24_calling_termwait_2xx( const SipSMCommand &command){
	
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "2**")){

		//int bye_seq_no= requestSeqNo();
		++dialogState.seqNo;
//		setCurrentState(toState);

		MRef<SipTransaction*> byetrans = new SipTransactionNonInviteClient(sipStack, MRef<SipDialog*>(this), dialogState.seqNo, dialogState.callId); 


		registerTransaction(byetrans);
		sendBye(byetrans->getBranch(), dialogState.seqNo);

		CommandString cmdstr(dialogState.callId, SipCommandString::security_failed);
		cmdstr.setParam3(confId);
		//getDialogContainer()->getCallback()->sipcb_handleCommand(cmdstr);
		getDialogContainer()->getCallback()->sipcb_handleConfCommand( cmdstr );

		getMediaSession()->stop();
		signalIfNoTransactions();
		return true;
	} else{
		return false;
	}
}


bool SipDialogConfVoip::a25_termwait_terminated_notransactions( const SipSMCommand &command){
	if (transitionMatch(command, SipCommandString::no_transactions) ){
		lastInvite=NULL;
		SipSMCommand cmd(
				CommandString( dialogState.callId, SipCommandString::call_terminated),
				SipSMCommand::TU,
				SipSMCommand::DIALOGCONTAINER);

		getDialogContainer()->enqueueCommand( cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE );

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


bool SipDialogConfVoip::a26_callingnoauth_termwait_transporterror( const SipSMCommand &command){
	if (transitionMatch(command, SipCommandString::transport_error )){
		CommandString cmdstr(dialogState.callId, SipCommandString::transport_error);
		cmdstr.setParam3(confId);
		//getDialogContainer()->getCallback()->sipcb_handleCommand(cmdstr);
		getDialogContainer()->getCallback()->sipcb_handleConfCommand( cmdstr );
		return true;
	}else{
		return false;
	}
}

//Copy of a8!
bool SipDialogConfVoip::a26_callingauth_termwait_cancel( const SipSMCommand &command)
{
	if (transitionMatch(command, SipCommandString::cancel) || transitionMatch(command, SipCommandString::hang_up)){
//		setCurrentState(toState);
		MRef<SipTransaction*> canceltrans( new SipTransactionNonInviteClient(sipStack, MRef<SipDialog*>(this), dialogState.seqNo, dialogState.callId)); 
		registerTransaction(canceltrans);
		sendCancel(canceltrans->getBranch());

		getMediaSession()->stop();
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}


bool SipDialogConfVoip::a27_incall_incall_ACK( const SipSMCommand &command)
{
	if (transitionMatch(command, SipAck::type, SipSMCommand::remote, IGN)){
		//...
		//cerr << "Received ACK in SipDialogConfVoIP!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
		MRef<SipResponse*> resp(  (SipResponse*)*command.getCommandPacket() );
		assert(dynamic_cast<SdpPacket*>(*resp->getContent())!=NULL);
		MRef<SdpPacket*> sdp = (SdpPacket*)*resp->getContent();
		string numToConnect = sdp->getSessionLevelAttribute("conf_#participants");
		int num = 0;

   //--- Convert each digit char and add into result.
   			int t=0;
			while (numToConnect[t] >= '0' && numToConnect[t] <='9') {
      				num = (num * 10) + (numToConnect[t] - '0');
      				t++;
   			}
			string users="";
			for(t=0;t<num;t++)
				//connectList[t]=  sdp->getSessionLevelAttribute("participant_"+itoa(t+1));
				users=users+sdp->getSessionLevelAttribute("participant_"+itoa(t+1))+";";
			//cerr<<"==============users: "+users<<endl;	
                
		
		CommandString cmdstr(dialogState.callId, "invite_ack", users,(getMediaSession()->isSecure()?"secure":"unprotected"),confId);
		
		
		
		//getDialogContainer()->getCallback()->sipcb_handleCommand(cmdstr);
		getDialogContainer()->getCallback()->sipcb_handleConfCommand( cmdstr );
		
			
			
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


void SipDialogConfVoip::setUpStateMachine(){

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


	new StateTransition<SipSMCommand,string>(this, "transition_start_callingnoauth_invite",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogConfVoip::a0_start_callingnoauth_invite, 
			s_start, s_callingnoauth);

	new StateTransition<SipSMCommand,string>(this, "transition_callingnoauth_callingnoauth_18X",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogConfVoip::a1_callingnoauth_callingnoauth_18X, 
			s_callingnoauth, s_callingnoauth);

	new StateTransition<SipSMCommand,string>(this, "transition_callingnoauth_callingnoauth_1xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogConfVoip::a2_callingnoauth_callingnoauth_1xx, 
			s_callingnoauth, s_callingnoauth);

	new StateTransition<SipSMCommand,string>(this, "transition_callingnoauth_incall_2xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogConfVoip::a3_callingnoauth_incall_2xx, 
			s_callingnoauth, s_incall);

	new StateTransition<SipSMCommand,string>(this, "transition_callingauth_termwait_2xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogConfVoip::a24_calling_termwait_2xx,
			s_callingnoauth, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_incall_termwait_BYE",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogConfVoip::a5_incall_termwait_BYE,
			s_incall, s_termwait); 

	new StateTransition<SipSMCommand,string>(this, "transition_incall_termwait_hangup",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand& )) &SipDialogConfVoip::a6_incall_termwait_hangup,
			s_incall, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_callingnoauth_termwait_CANCEL",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogConfVoip::a7_callingnoauth_termwait_CANCEL,
			s_callingnoauth, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_callingnoauth_termwait_cancel",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogConfVoip::a8_callingnoauth_termwait_cancel,
			s_callingnoauth, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_callingnoauth_callingauth_40X",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogConfVoip::a20_callingnoauth_callingauth_40X,
			s_callingnoauth, s_callingauth);

	new StateTransition<SipSMCommand,string>(this, "transition_callingnoauth_termwait_36",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogConfVoip::a9_callingnoauth_termwait_36,
			s_callingnoauth, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_start_ringing_INVITE",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogConfVoip::a10_start_ringing_INVITE,
			s_start, s_ringing);

	new StateTransition<SipSMCommand,string>(this, "transition_ringing_incall_accept",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogConfVoip::a11_ringing_incall_accept,
			s_ringing, s_incall);

	new StateTransition<SipSMCommand,string>(this, "transition_ringing_termwait_CANCEL",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogConfVoip::a12_ringing_termwait_CANCEL,
			s_ringing, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_ringing_termwait_reject",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogConfVoip::a13_ringing_termwait_reject,
			s_ringing, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_start_termwait_INVITE",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogConfVoip::a16_start_termwait_INVITE,
			s_start, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_callingauth_callingauth_18X",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogConfVoip::a21_callingauth_callingauth_18X, 
			s_callingauth, s_callingauth);

	new StateTransition<SipSMCommand,string>(this, "transition_callingauth_callingauth_1xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogConfVoip::a22_callingauth_callingauth_1xx, 
			s_callingauth, s_callingauth);

	new StateTransition<SipSMCommand,string>(this, "transition_callingauth_incall_2xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogConfVoip::a23_callingauth_incall_2xx, 
			s_callingauth, s_incall);

	new StateTransition<SipSMCommand,string>(this, "transition_callingauth_termwait_2xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogConfVoip::a24_calling_termwait_2xx,
			s_callingauth, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_callingauth_termwait_resp36",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogConfVoip::a9_callingnoauth_termwait_36,
			s_callingauth, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_termwait_terminated_notransactions",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogConfVoip::a25_termwait_terminated_notransactions,
			s_termwait, s_terminated);

	new StateTransition<SipSMCommand,string>(this, "transition_callingnoauth_termwait_transporterror",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogConfVoip::a26_callingnoauth_termwait_transporterror,
			s_callingnoauth, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_callingauth_termwait_cancel",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogConfVoip::a26_callingauth_termwait_cancel,
			s_callingauth, s_termwait);

			
	new StateTransition<SipSMCommand,string>(this, "transition_incall_incall_ACK",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogConfVoip::a27_incall_incall_ACK,
			s_incall, s_incall);
	setCurrentState(s_start);
}


#ifdef IPSEC_SUPPORT
SipDialogConfVoip::SipDialogConfVoip(MRef<SipStack*> stack, MRef<SipDialogConfig*> callconfig, MRef<SipSoftPhoneConfiguration*> pconf, MRef<Session *> mediaSession, minilist<ConfMember> *list, string confid, string cid, MRef<MsipIpsecAPI *> ipsecSession) : 
                SipDialog(stack,callconfig),
                lastInvite(NULL), 
		phoneconf(pconf),
		mediaSession(mediaSession), ipsecSession(ipsecSession)
#else
SipDialogConfVoip::SipDialogConfVoip(MRef<SipStack*> stack, MRef<SipDialogConfig*> callconfig, MRef<SipSoftPhoneConfiguration*> pconf, MRef<Session *> mediaSession, minilist<ConfMember> *list,string confid, string cid) : 
                SipDialog(stack,callconfig),
                lastInvite(NULL), 
		phoneconf(pconf),
		mediaSession(mediaSession)
#endif

{
	confId=confid;
	numConnected= list->size();
	type="join";
	
	//cerr << "CONFDIALOG: Creating SipDialogConfVoip's receivedList" << endl;
	
	//this is the list you get/send as advice of who is in the conference. It will go to the GUI to be displayed to
	//the user to make a decision to join or not.
	//adviceList = new minilist<ConfMember>(*list);
	adviceList=list;

	
	//CommandString cmdstr("", "myuri", getDialogConfig()->inherited.sipIdentity->getSipUri());
		
		
		
		//getDialogContainer()->getCallback()->sipcb_handleCommand(cmdstr);
		//getDialogContainer()->getCallback()->sipcb_handleConfCommand( cmdstr );
	/*
	for(int t=0;t<10;t++)
	{
		if(t<=numConnected)
			connectedList[t]=list[t];
		else
			connectedList[t]="";
	}
	*/
	
	//cerr << "CONFDIALOG: "+ ((*list)[0]).uri << endl;
	//cerr << "CONFDIALOG: "+ ((*list)[1]).uri << endl;
	//cerr << "CONFDIALOG: "+itoa(numConnected)<< endl;
	
	if (cid=="")
		dialogState.callId = itoa(rand())+"@"+getDialogConfig()->inherited->externalContactIP;
	else
		dialogState.callId = cid;
	
	dialogState.localTag = itoa(rand());
	
	/* We will fill that later, once we know if that succeeded */
	logEntry = NULL;

	setUpStateMachine();
}
#ifdef IPSEC_SUPPORT
SipDialogConfVoip::SipDialogConfVoip(MRef<SipStack*> stack, MRef<SipDialogConfig*> callconfig, MRef<SipSoftPhoneConfiguration*> pconf, MRef<Session *> mediaSession, string confid, string cid, MRef<MsipIpsecAPI *> ipsecSession) : 
                SipDialog(stack,callconfig),
                lastInvite(NULL), 
		phoneconf(pconf),
		mediaSession(mediaSession), ipsecSession(ipsecSession)
#else
SipDialogConfVoip::SipDialogConfVoip(MRef<SipStack*> stack, MRef<SipDialogConfig*> callconfig, MRef<SipSoftPhoneConfiguration*> pconf, MRef<Session *> mediaSession, string confid, string cid) : 
                SipDialog(stack,callconfig),
                lastInvite(NULL), 
		phoneconf(pconf),
		mediaSession(mediaSession)
#endif

{
	confId=confid;
	//cerr<<"SDCVididididididididididdididi "+confId<<endl;
	//cerr << "CONFDIALOG: received"<< endl;
	type="connect";
	if (cid=="")
		dialogState.callId = itoa(rand())+"@"+getDialogConfig()->inherited->externalContactIP;
	else
		dialogState.callId = cid;
	
	dialogState.localTag = itoa(rand());
	
	/* We will fill that later, once we know if that succeeded */
	logEntry = NULL;

	setUpStateMachine();
}

SipDialogConfVoip::~SipDialogConfVoip(){	
}

/*
void SipDialogVoip::handleSdp(MRef<SdpPacket*> sdp){

}
*/

void SipDialogConfVoip::sendInvite(const string &branch){
	//	mdbg << "ERROR: SipDialogVoip::sendInvite() UNIMPLEMENTED"<< end;
	
	MRef<SipInvite*> inv;
	string keyAgreementMessage;

	inv= MRef<SipInvite*>(new SipInvite(
				branch,
				dialogState.callId,
				dialogState.remoteUri,
				//getDialogConfig().inherited.sipIdentity->sipProxy.sipProxyIpAddr->getString(),
				getDialogConfig()->inherited->sipIdentity->sipDomain,	//TODO: Change API - not sure if proxy or domain
				getDialogConfig()->inherited->sipIdentity->sipProxy.sipProxyPort,
//				getDialogConfig().inherited.localIpString,
				getDialogConfig()->inherited->externalContactIP,
				getDialogConfig()->inherited->getLocalSipPort(phoneconf->useSTUN),
				//getDialogConfig().inherited.userUri,
				getDialogConfig()->inherited->sipIdentity->getSipUri(),
				dialogState.seqNo,
				getDialogConfig()->inherited->transport));

	/* Get the session description from the Session */
		
//      There might be so that there are no SDP. Check!
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
	if(type=="join"){
		//cerr << "SDCV: modifyjoininvite"<< endl;
		modifyConfJoinInvite(inv);}
	else
		modifyConfConnectInvite(inv);
	//if(inv->is_ConfJoin())
		//cerr << "SDCV: confjoin was set!!"<< endl;
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
	//cerr<<"SDCV: "+scmd.getCommandString().getString()<<endl;
	getDialogContainer()->enqueueCommand(scmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
	setLastInvite(inv);
	//inv->checkAcceptContact();

}

void SipDialogConfVoip::sendAuthInvite(const string &branch){
	//	merr << "ERROR: SipDialogVoip::sendAuthInvite() UNIMPLEMENTED"<< end;
//	string call_id = getDialogConfig().callId;
	//SipInvite * inv;
	MRef<SipInvite*> inv;
	string keyAgreementMessage;

	inv= new SipInvite(
			branch,
			dialogState.callId,
			dialogState.remoteUri,
			//getDialogConfig().inherited.sipIdentity->sipProxy.sipProxyIpAddr->getString(),
			getDialogConfig()->inherited->sipIdentity->sipDomain,
			getDialogConfig()->inherited->sipIdentity->sipProxy.sipProxyPort,
//			getDialogConfig().inherited.localIpString,
			getDialogConfig()->inherited->externalContactIP,
			getDialogConfig()->inherited->getLocalSipPort(phoneconf->useSTUN),
			//getDialogConfig().inherited.userUri,
			getDialogConfig()->inherited->sipIdentity->getSipUri(),
			dialogState.seqNo,
//			requestSeqNo(),
			getDialogConfig()->inherited->sipIdentity->sipProxy.sipProxyUsername,
			nonce,
			realm,
			getDialogConfig()->inherited->sipIdentity->sipProxy.sipProxyPassword,
			getDialogConfig()->inherited->transport);

	inv->getHeaderValueFrom()->setParameter("tag",dialogState.localTag);
	if(type=="join")
		modifyConfJoinInvite(inv);
	else
		modifyConfConnectInvite(inv);
	
//      There might be so that there are no SDP. Check!
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


//#ifdef NEVERDEFINED_ERSADFS
void SipDialogConfVoip::sendAck(const string &branch){
/*	//	mdbg << "ERROR: SipDialogVoip::sendAck() UNIMPLEMENTED" << end;

	
	assert( !lastResponse.isNull());
	
	SipAck *ack = new SipAck(
			branch, 
			*lastResponse,
			dialogState.remoteUri,
			//getDialogConfig().inherited.sipIdentity->sipProxy.sipProxyIpAddr->getString());
			getDialogConfig()->inherited.sipIdentity->sipDomain);
	//TODO:
	//	ack.add_header( new SipHeaderRoute(getDialog()->getRouteSet() ) );
//	mdbg << "SipDialogVoip:sendAck(): sending ACK directly to remote" << end;

	//	if(socket == NULL){
	// No StreamSocket, create one or use UDP
//	Socket *sock=NULL;
	MRef<SdpPacket *> sdp;

	
	ack->setContent( *sdp );

//-------------------------------------------------------------------------------------------------------------//
	
	
	modifyConfAck(ack);
	MRef<SipMessage*> pref(*ack);
        SipSMCommand cmd( pref, SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand(cmd);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);

	

	
	//	}*/

	/*	if(dynamic_cast<StreamSocket *>(socket) != NULL){
	// A StreamSocket exists, try to use it
	mdbg << "Sending packet using existing StreamSocket"<<end;
	getDialog()->getDialogConfig().sipTransport->sendMessage(pack,(StreamSocket *)socket);
	return;
	}
	 */
	 
	MRef<SipAck *> ack = new SipAck(
		branch, 
		*lastResponse,
		dialogState.remoteUri,
		//getDialogConfig().inherited.sipIdentity->sipProxy.sipProxyIpAddr->getString());
		getDialogConfig()->inherited->sipIdentity->sipDomain);

//      There might be so that there are no SDP. Check!
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
		ack->setContent( *multi);
	}
	if (mikey && !mediaSession)
		ack->setContent( *mikey);
	if (!mikey && mediaSession)
		ack->setContent( *sdp );
#else
	
	ack->setContent( *sdp );
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
	///modifyConfOk(ack);
//	setLastResponse(ok);
	modifyConfAck(ack);
        MRef<SipMessage*> pref(*ack);
	
	IP4Address toaddr(getDialogConfig()->inherited->sipIdentity->sipProxy.sipProxyAddressString);
	getSipStack()->getSipTransportLayer()->sendMessage(pref,
					toaddr,
					getDialogConfig()->inherited->sipIdentity->sipProxy.sipProxyPort, 
					getDialogConfig()->inherited->sipIdentity->sipProxy.getTransport(), 
					string("ACK"),
					true);

	
	
	
//        SipSMCommand cmd( pref, SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand(cmd);
//	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_FIRST_IN_QUEUE);

}
//#endif

void SipDialogConfVoip::sendBye(const string &branch, int bye_seq_no){

	//string tmp = getDialogConfig().inherited.userUri;
	string tmp = getDialogConfig()->inherited->sipIdentity->getSipUri();
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
			dialogState.remoteUri,
			//getDialogConfig().inherited.userUri,
			getDialogConfig()->inherited->sipIdentity->getSipUri(),
			domain,
//			getDialogConfig().seqNo+1,
			bye_seq_no///,
			///localCalled
			);

	bye->getHeaderValueFrom()->setParameter("tag",dialogState.localTag);
	bye->getHeaderValueTo()->setParameter("tag",dialogState.remoteTag);

        MRef<SipMessage*> pref(*bye);
        SipSMCommand cmd( pref, SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand(cmd);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}

void SipDialogConfVoip::sendCancel(const string &branch){
	assert( !lastInvite.isNull());
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

void SipDialogConfVoip::sendInviteOk(const string &branch){
	MRef<SipResponse*> ok= new SipResponse(branch, 200,"OK", MRef<SipMessage*>(*getLastInvite()));	
	ok->getHeaderValueTo()->setParameter("tag",dialogState.localTag);

//      There might be so that there are no SDP. Check!
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
	
	modifyConfOk(ok);
//	
	//setLastResponse(ok);
        MRef<SipMessage*> pref(*ok);
        SipSMCommand cmd( pref, SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand(cmd);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}

void SipDialogConfVoip::sendByeOk(MRef<SipBye*> bye, const string &branch){
	MRef<SipResponse*> ok= new SipResponse( branch, 200,"OK", MRef<SipMessage*>(*bye) );
	ok->getHeaderValueTo()->setParameter("tag",dialogState.localTag);

//	setLastResponse(ok);
        MRef<SipMessage*> pref(*ok);
        SipSMCommand cmd( pref, SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand(cmd);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}

void SipDialogConfVoip::sendReject(const string &branch){
	MRef<SipResponse*> ringing = new SipResponse(branch,486,"Temporary unavailable", MRef<SipMessage*>(*getLastInvite()));	
	ringing->getHeaderValueTo()->setParameter("tag",dialogState.localTag);
//	setLastResponse(ringing);
        MRef<SipMessage*> pref(*ringing);
        SipSMCommand cmd( pref,SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand(cmd);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}

void SipDialogConfVoip::sendRinging(const string &branch){
	MRef<SipResponse*> ringing = new SipResponse(branch,180,"Ringing", MRef<SipMessage*>(*getLastInvite()));	
	ringing->getHeaderValueTo()->setParameter("tag",dialogState.localTag);
//	setLastResponse(ringing);
        MRef<SipMessage*> pref(*ringing);
        SipSMCommand cmd( pref, SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand(cmd);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}

void SipDialogConfVoip::sendNotAcceptable(const string &branch){
	MRef<SipResponse*> not_acceptable = new SipResponse(branch,606,"Not Acceptable", MRef<SipMessage*>(*getLastInvite()));	
	if( mediaSession && mediaSession->getErrorString() != "" ){
		not_acceptable->addHeader( 
			new SipHeader(new SipHeaderValueWarning(getDialogConfig()->inherited->externalContactIP, 399, mediaSession->getErrorString() ) ));
	}

	not_acceptable->getHeaderValueTo()->setParameter("tag",dialogState.localTag);
//	setLastResponse(not_acceptable);
        MRef<SipMessage*> pref(*not_acceptable);
        SipSMCommand cmd( pref, SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand(cmd);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}


bool SipDialogConfVoip::handleCommand(const SipSMCommand &c){
	mdbg << "SipDialogConfVoip::handleCommand got "<< c << end;

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
	
	mdbg << "SipDialogConfVoip::handlePacket() got "<< c << end;
	bool handled = SipDialog::handleCommand(c);
	
	if (!handled && c.getType()==SipSMCommand::COMMAND_STRING && c.getCommandString().getOp()==SipCommandString::no_transactions){
		return true;
	}
	
	if (c.getType()==SipSMCommand::COMMAND_STRING && dialogState.callId.length()>0){
		if (c.getCommandString().getDestinationId() == dialogState.callId ){
			mdbg << "Warning: SipDialogConfVoip ignoring command with matching call id"<< end;
			return true;
		}
	}
	if (c.getType()==SipSMCommand::COMMAND_PACKET && dialogState.callId.length()>0){
		if (c.getCommandPacket()->getCallId() == dialogState.callId){
			mdbg << "Warning: SipDialogConfVoip ignoring packet with matching call id"<< end;
			return true;
		}
	}

	
	return handled;
}

MRef<SipInvite*> SipDialogConfVoip::getLastInvite(){
    return lastInvite;
}

void SipDialogConfVoip::setLastInvite(MRef<SipInvite*> i){ 
    lastInvite = i; 
}

MRef<LogEntry *> SipDialogConfVoip::getLogEntry(){
	return logEntry;
}

void SipDialogConfVoip::setLogEntry( MRef<LogEntry *> logEntry ){
	this->logEntry = logEntry;
}

MRef<Session *> SipDialogConfVoip::getMediaSession(){
	return mediaSession;
}



#ifdef IPSEC_SUPPORT
MRef<MsipIpsecAPI *> SipDialogConfVoip::getIpsecSession(){
	return ipsecSession;
}
#endif


bool SipDialogConfVoip::sortMIME(MRef<SipMessageContent *> Offer, string peerUri, int type){
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
void SipDialogConfVoip::modifyConfJoinInvite(MRef<SipInvite*>inv){
	//Add Accept-Contact Header
	//inv->set_ConfJoin();
	inv->set_ConfJoin();	
	//Add SDP Session Level Attributes
	//cerr<<"modify join 1111111111111111"<<endl;
	assert(dynamic_cast<SdpPacket*>(*inv->getContent())!=NULL);
	MRef<SdpPacket*> sdp = (SdpPacket*)*inv->getContent();
	sdp->setSessionLevelAttribute("confId", confId);
	sdp->setSessionLevelAttribute("conf_#participants", itoa((*adviceList).size()));
	for(int t=0;t<numConnected;t++)
	{
		sdp->setSessionLevelAttribute("participant_"+itoa(t+1), ((*adviceList)[t]).uri);
	}
	//cerr<<"modify join 22222222222222222"<<endl;
}
void SipDialogConfVoip::modifyConfAck(MRef<SipAck*>ack){
	//Add Accept-Contact Header
	ack->set_Conf();
	//cerr<<"modify ack 1111111111111111"<<endl;	
	//Add SDP Session Level Attributes
	assert(dynamic_cast<SdpPacket*>(*ack->getContent())!=NULL);
	MRef<SdpPacket*> sdp = (SdpPacket*)*ack->getContent();
	int numParticipants=(*adviceList).size();
	for(int t=0;t<(*adviceList).size();t++)
	{
		//cerr<<"*adviceList))[t]).uri: "+(((*adviceList))[t]).uri<<endl;
		//cerr<<"dialogState.remoteUri "+dialogState.remoteUri<<endl;
		if((((*adviceList))[t]).uri!=dialogState.remoteUri)
		{
			sdp->setSessionLevelAttribute("participant_"+itoa(t+1), (((*adviceList))[t]).uri);
			numParticipants--;}
	}
	sdp->setSessionLevelAttribute("conf_#participants", itoa(numParticipants));
	//cerr<<"modify ack 22222222222222222"<<endl;
}
void SipDialogConfVoip::modifyConfOk(MRef<SipResponse*> ok){
	//Add Accept-Contact Header
	//ack->set_Conf();
		
	//Add SDP Session Level Attributes
	assert(dynamic_cast<SdpPacket*>(*ok->getContent())!=NULL);
	MRef<SdpPacket*> sdp = (SdpPacket*)*ok->getContent();
	sdp->setSessionLevelAttribute("conf_#participants", itoa(connectedList.size()));
	for(int t=0;t<connectedList.size();t++)
	{
		sdp->setSessionLevelAttribute("participant_"+itoa(t+1), ((connectedList)[t]).uri);
	}
}
void SipDialogConfVoip::modifyConfConnectInvite(MRef<SipInvite*>inv){
	//Add Accept-Contact Header
	inv->set_ConfConnect();
	assert(dynamic_cast<SdpPacket*>(*inv->getContent())!=NULL);
	MRef<SdpPacket*> sdp = (SdpPacket*)*inv->getContent();
	sdp->setSessionLevelAttribute("confId", confId);	
	//Add SDP Session Level Attributes
	//assert(dynamic_cast<SdpPacket*>(*inv->getContent())!=NULL);
	//MRef<SdpPacket*> sdp = (SdpPacket*)*inv->getContent();
	/*sdp->setSessionLevelAttribute("p2tGroupListServer", getDialogConfig()->inherited.externalContactIP + ":" + itoa(getPhoneConfig()->p2tGroupListServerPort));
	sdp->setSessionLevelAttribute("p2tGroupIdentity", getP2TDialog()->getGroupList()->getGroupIdentity());
	sdp->setSessionLevelAttribute("p2tGroupListProt","http/xml");*/	
}

