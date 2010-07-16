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

#include<libminisip/signaling/sip/SipDialogConfVoip.h>

#include<libmutil/massert.h>

#include<libmsip/SipResponse.h>
#include<libmsip/SipTransitionUtils.h>
#include<libmsip/SipDialog.h>
#include<libmsip/SipCommandString.h>
#include<libmsip/SipHeaderTo.h>
#include<libmsip/SipHeaderAcceptContact.h>
#include<libmsip/SipHeaderFrom.h>
#include<libmsip/SipHeaderWarning.h>
#include<libmsip/SipMessageContentMime.h>
#include<libmsip/SipMessageContent.h>
#include<libminisip/signaling/sip/DefaultDialogHandler.h>
#include"../../../source/subsystem_media/Session.h"
#include<libminisip/signaling/conference/ConfMessageRouter.h>
#include<libmutil/stringutils.h>
#include<libmutil/Timestamp.h>
#include<libmutil/termmanip.h>
#include<libmutil/dbg.h>
#include<libmsip/SipSMCommand.h>
#include <time.h>
#include<libminisip/gui/LogEntry.h>
#include<libmsip/SipCommandString.h>
#include<libminisip/media/SubsystemMedia.h>
#include<libmutil/MemObject.h>
#include <iostream>
#include<time.h>

#ifdef _WIN32_WCE
#	include"../include/minisip_wce_extra_includes.h"
#endif

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
{
	if (transitionMatch(command, 
				SipCommandString::invite,
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer)){
#ifdef ENABLE_TS
		ts.save("a0_start_callingnoauth_invite");
#endif
		++dialogState.seqNo;
		localCalled=false;
		dialogState.remoteUri= command.getCommandString().getParam();

		sendInvite();
		return true;
	}else{
		return false;
	}

}

bool SipDialogConfVoip::a1_callingnoauth_callingnoauth_18X( const SipSMCommand &command)
{	
	if (transitionMatch(SipResponse::type, command, IGN, SipSMCommand::dialog_layer, "18*")){

	    MRef<SipResponse*> resp= (SipResponse*) *command.getCommandPacket();

#ifdef ENABLE_TS
	    ts.save( RINGING );
#endif
	    CommandString cmdstr(dialogState.callId, SipCommandString::remote_ringing);
	    cmdstr.setParam3(confId);
	    getSipStack()->getCallback()->handleCommand("sip_conf", cmdstr );

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

	if (transitionMatch(SipResponse::type, command, IGN, SipSMCommand::dialog_layer, "1**")){
		dialogState.remoteTag = command.getCommandPacket()->getHeaderValueTo()->getParameter("tag");
		return true;
	}else{
		return false;
	}
    
}

bool SipDialogConfVoip::a3_callingnoauth_incall_2xx( const SipSMCommand &command)
{
	if (transitionMatch(SipResponse::type, command, IGN, SipSMCommand::dialog_layer, "2**")){
#ifdef ENABLE_TS
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
		massert(dynamic_cast<SdpPacket*>(*resp->getContent())!=NULL);
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
		getSipStack()->getCallback()->handleCommand("sip_conf", cmdstr );
		//cerr<<"****************************sendack is called**********************"<<endl;
		//sendAck(getLastInvite()->getDestinationBranch() );
		//BM
		MRef<ConfMessageRouter*> ptr= confCallback;//(ConfMessageRouter*) *(getDialogContainer()->getConfCallback());
		adviceList=(ptr->getConferenceController(confId)->getConnectedList());
		sendAck();
		
		if(!sortMIME(*resp->getContent(), peerUri, 3))
			return false;
			
		return true;
	}else{
		return false;
	}
}

bool SipDialogConfVoip::a5_incall_termwait_BYE( const SipSMCommand &command)
{
	
	if (transitionMatch("BYE", command, SipSMCommand::transaction_layer, SipSMCommand::dialog_layer)){
		MRef<SipRequest*> bye = (SipRequest*) *command.getCommandPacket();

		if( getLogEntry() ){
			((LogEntrySuccess *)(*( getLogEntry() )))->duration = 
			time( NULL ) - getLogEntry()->start; 

			getLogEntry()->handle();
		}

		sendByeOk( bye );

		CommandString cmdstr(dialogState.callId, SipCommandString::remote_hang_up);
		cmdstr.setParam3(confId);
		getSipStack()->getCallback()->handleCommand("sip_conf", cmdstr );

		getMediaSession()->stop();

		
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}


bool SipDialogConfVoip::a6_incall_termwait_hangup( const SipSMCommand &command)
{


printf("1 ------------------------------------------------ SipDialogConfVoip.cxx \n");
	if (transitionMatch(command, 
				SipCommandString::hang_up,
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer)){
		++dialogState.seqNo;
		sendBye( dialogState.seqNo );
		
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

bool SipDialogConfVoip::a7_callingnoauth_termwait_CANCEL( const SipSMCommand &command)
{
	if (transitionMatch("CANCEL", command, SipSMCommand::transaction_layer, IGN)){
		getMediaSession()->stop();
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}

bool SipDialogConfVoip::a8_callingnoauth_termwait_cancel( const SipSMCommand &command)
{

printf("2 ------------------------------------------------ SipDialogConfVoip.cxx \n");
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

//Note: This is also used as: callingauth_terminated_36
bool SipDialogConfVoip::a9_callingnoauth_termwait_36( const SipSMCommand &command)
{
	if (transitionMatch(SipResponse::type, command, IGN, SipSMCommand::dialog_layer, "3**\n4**\n5**\n6**")){
		
		MRef<LogEntry *> rejectedLog( new LogEntryCallRejected() );
		rejectedLog->start = time( NULL );
		rejectedLog->peerSipUri = dialogState.remoteTag;
		
		if (sipResponseFilterMatch(MRef<SipResponse*>((SipResponse*)*command.getCommandPacket()),"404")){
                        CommandString cmdstr(dialogState.callId, SipCommandString::remote_user_not_found);
			cmdstr.setParam3(confId);
			getSipStack()->getCallback()->handleCommand("sip_conf", cmdstr);
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
			getSipStack()->getCallback()->handleCommand("sip_conf", cmdstr );
		}
		else if (sipResponseFilterMatch(MRef<SipResponse*>((SipResponse*)*command.getCommandPacket()),"4**")){
			((LogEntryFailure *)*rejectedLog)->error =
				"User rejected the call";
			setLogEntry( rejectedLog );
			rejectedLog->handle();
                        CommandString cmdstr( dialogState.callId, SipCommandString::remote_reject);
			cmdstr.setParam3(confId);
			getSipStack()->getCallback()->handleCommand("sip_conf", cmdstr );
		}
		else{
			merr << "ERROR: received response in SipDialogConfVoip"
				" that could not be handled (unimplemented)"<< endl;
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
	if (transitionMatch("INVITE", command, IGN, SipSMCommand::dialog_layer)){

		dialogState.remoteUri = command.getCommandPacket()->getHeaderValueFrom()->getUri().getUserName()+"@"+ 
			command.getCommandPacket()->getHeaderValueFrom()->getUri().getIp();
		getDialogConfig()->sipIdentity->setSipUri( 
			command.getCommandPacket()->getHeaderValueTo()->getUri().getUserIpString() );

		//We must maintain our dialog state. This is the first
		//message we receive for this dialog and we copy the remote
		//tag and the remote sequence number.
		dialogState.remoteTag = command.getCommandPacket()->getHeaderValueFrom()->getParameter("tag");
		dialogState.remoteSeqNo = command.getCommandPacket()->getCSeq();
		localCalled=true;
		
		setLastInvite(MRef<SipRequest*>((SipRequest*)*command.getCommandPacket()));

		string peerUri = command.getCommandPacket()->getFrom().getString();
		//MRef<SipMessageContent *> Offer = *command.getCommandPacket()->getContent();
		if(!sortMIME(*command.getCommandPacket()->getContent(), peerUri, 10)){
			merr << "No MIME match" << endl;
			return false;
		}

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
			getSipStack()->getCallback()->handleCommand("gui", cmdstr );
		}
		else
		{
			//cerr<<"a10 confIdidididididi "+confId<<endl;
			CommandString cmdstr(dialogState.callId, 
				"conf_connect_received", 
				dialogState.remoteUri, 
				(getMediaSession()->isSecure()?"secure":"unprotected"),confId);//bm
			getSipStack()->getCallback()->handleCommand("sip_conf", cmdstr );
		}
		
		sendRinging();
		
		if( getSipStack()->getStackConfig()->autoAnswer ){
			CommandString accept( dialogState.callId, SipCommandString::accept_invite );
			SipSMCommand sipcmd(accept, SipSMCommand::transaction_layer, SipSMCommand::dialog_layer);
			getSipStack()->enqueueCommand(sipcmd,HIGH_PRIO_QUEUE );
		}
		return true;
	}else{
		return false;
	}
}

bool SipDialogConfVoip::a11_ringing_incall_accept( const SipSMCommand &command)
{
	
	if (transitionMatch(command, 
				SipCommandString::accept_invite,
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer)){
#ifdef ENABLE_TS
		ts.save(USER_ACCEPT);
#endif
		//bm
	numConnected=0;
	string users=command.getCommandString().getParam2();
	confId=command.getCommandString().getParam3();
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
				massert(numConnected==connectedList.size());
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
		
		massert( !getLastInvite().isNull() );
		sendInviteOk();
		CommandString cmdstr2("", "myuri", getDialogConfig()->sipIdentity->getSipUri().getString());
		
		
		cmdstr2.setParam3(confId);
		getSipStack()->getCallback()->handleCommand("sip_conf", cmdstr2 );
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

bool SipDialogConfVoip::a12_ringing_termwait_CANCEL( const SipSMCommand &command)
{

	if (transitionMatch("CANCEL", command, IGN, IGN)){

		/* Tell the GUI */
		CommandString cmdstr(dialogState.callId, SipCommandString::remote_cancelled_invite);
		cmdstr.setParam3(confId);
		getSipStack()->getCallback()->handleCommand("sip_conf", cmdstr );
		sendInviteOk(); 

		getMediaSession()->stop();
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}


bool SipDialogConfVoip::a13_ringing_termwait_reject( const SipSMCommand &command)
{

printf("3 ------------------------------------------------ SipDialogConfVoip.cxx \n");
	
	if (		transitionMatch(command, 
				SipCommandString::reject_invite,
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer) 
			|| transitionMatch(command,
				SipCommandString::hang_up,
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer)){


		sendReject();

		getMediaSession()->stop();
		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}


bool SipDialogConfVoip::a16_start_termwait_INVITE( const SipSMCommand &command){
	
	if (transitionMatch("INVITE", command, SipSMCommand::transaction_layer, SipSMCommand::dialog_layer)){

		setLastInvite(MRef<SipRequest*>((SipRequest *)*command.getCommandPacket()));

		sendNotAcceptable();

		signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}

bool SipDialogConfVoip::a20_callingnoauth_callingauth_40X( const SipSMCommand &command){

	if (transitionMatch(SipResponse::type, command, IGN, SipSMCommand::dialog_layer, "407\n401")){
		
		MRef<SipResponse*> resp( (SipResponse*)*command.getCommandPacket() );

		dialogState.remoteTag = command.getCommandPacket()->getHeaderValueTo()->getParameter("tag");

		++dialogState.seqNo;
		realm=resp->getAuthenticateProperty("realm");
		nonce=resp->getAuthenticateProperty("nonce");

		updateAuthentications( resp );
		sendInvite();

		return true;
	}else{
		return false;
	}
}


bool SipDialogConfVoip::a21_callingauth_callingauth_18X( const SipSMCommand &command){
	
	if (transitionMatch(SipResponse::type, command, IGN, SipSMCommand::dialog_layer, "18*")){
		MRef<SipResponse*> resp (  (SipResponse*)*command.getCommandPacket()  );
#ifdef ENABLE_TS
		ts.save( RINGING );
#endif

		CommandString cmdstr(dialogState.callId, SipCommandString::remote_ringing);
		cmdstr.setParam3(confId);
		getSipStack()->getCallback()->handleCommand("sip_conf", cmdstr );
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
	if (transitionMatch(SipResponse::type, command, IGN, SipSMCommand::dialog_layer, "1**")){
		dialogState.remoteTag = command.getCommandPacket()->getHeaderValueTo()->getParameter("tag");
		return true;
	}else{
		return false;
	}
}

bool SipDialogConfVoip::a23_callingauth_incall_2xx( const SipSMCommand &command){
	if (transitionMatch(SipResponse::type, command, IGN, SipSMCommand::dialog_layer, "2**")){
		MRef<SipResponse*> resp( (SipResponse*)*command.getCommandPacket() );
		
		string peerUri = resp->getFrom().getString();
		setLogEntry( new LogEntryOutgoingCompletedCall() );
		getLogEntry()->start = time( NULL );
		getLogEntry()->peerSipUri = peerUri;

		dialogState.remoteTag = command.getCommandPacket()->getHeaderValueTo()->getParameter("tag");


		CommandString cmdstr(dialogState.callId, 
				SipCommandString::invite_ok, 
				"",
				(getMediaSession()->isSecure()?"secure":"unprotected"), confId);
		getSipStack()->getCallback()->handleCommand("sip_conf", cmdstr );

		if(!sortMIME(*resp->getContent(), peerUri, 3))
			return false;

		return true;
	}else{
		return false;
	}
}

bool SipDialogConfVoip::a24_calling_termwait_2xx( const SipSMCommand &command){
	
	if (transitionMatch(SipResponse::type, command, IGN, SipSMCommand::dialog_layer, "2**")){

		++dialogState.seqNo;

		sendBye( dialogState.seqNo);

		CommandString cmdstr(dialogState.callId, SipCommandString::security_failed);
		cmdstr.setParam3(confId);
		getSipStack()->getCallback()->handleCommand("sip_conf", cmdstr );

		getMediaSession()->stop();
		signalIfNoTransactions();
		return true;
	} else{
		return false;
	}
}


bool SipDialogConfVoip::a25_termwait_terminated_notransactions( const SipSMCommand &command){
	if (transitionMatch(command, 
				SipCommandString::no_transactions,
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer) ){
		lastInvite=NULL;
		dialogState.isTerminated=true;
		SipSMCommand cmd(
				CommandString( dialogState.callId, SipCommandString::call_terminated),
				SipSMCommand::dialog_layer,
				SipSMCommand::dispatcher);

		getSipStack()->enqueueCommand( cmd, HIGH_PRIO_QUEUE );

		return true;
	}else{
		return false;
	}
}


bool SipDialogConfVoip::a26_callingnoauth_termwait_transporterror( const SipSMCommand &command){
	if (transitionMatch(command, 
				SipCommandString::transport_error,
				SipSMCommand::transaction_layer,
				SipSMCommand::dialog_layer )){
		CommandString cmdstr(dialogState.callId, SipCommandString::transport_error);
		cmdstr.setParam3(confId);
		getSipStack()->getCallback()->handleCommand("sip_conf", cmdstr );
		return true;
	}else{
		return false;
	}
}

//Copy of a8!
bool SipDialogConfVoip::a26_callingauth_termwait_cancel( const SipSMCommand &command)
{

printf("5 ------------------------------------------------ SipDialogConfVoip.cxx \n");
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


bool SipDialogConfVoip::a27_incall_incall_ACK( const SipSMCommand &command)
{
	if (transitionMatch("ACK", command, SipSMCommand::transaction_layer, IGN)){
		//...
		//cerr << "Received ACK in SipDialogConfVoIP!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
		MRef<SipResponse*> resp(  (SipResponse*)*command.getCommandPacket() );
		massert(dynamic_cast<SdpPacket*>(*resp->getContent())!=NULL);
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
		
		
		
		getSipStack()->getCallback()->handleCommand("sip_conf", cmdstr );
		
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


SipDialogConfVoip::SipDialogConfVoip(MRef<ConfMessageRouter*> confCb, MRef<SipStack*> stack, MRef<SipIdentity*> ident, bool stun, MRef<Session *> s, minilist<ConfMember> *conflist,string confid, string cid) : 
                SipDialog(stack,ident,cid),
		confCallback(confCb),
                lastInvite(NULL), 
		mediaSession(s),
		useStun(stun)
{
	confId=confid;
	numConnected= conflist->size();
	type="join";
	
	//cerr << "CONFDIALOG: Creating SipDialogConfVoip's receivedList" << endl;
	
	//this is the list you get/send as advice of who is in the conference. It will go to the GUI to be displayed to
	//the user to make a decision to join or not.
	//adviceList = new minilist<ConfMember>(*list);
	adviceList=conflist;

	
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
	
	/* We will fill that later, once we know if that succeeded */
	logEntry = NULL;

	setUpStateMachine();
}
SipDialogConfVoip::SipDialogConfVoip(MRef<ConfMessageRouter*> confCb, MRef<SipStack*> stack, MRef<SipIdentity*> ident, bool stun, MRef<Session *> s, string confid, string cid) : 
                SipDialog(stack,ident, cid),
		confCallback(confCb),
                lastInvite(NULL), 
		mediaSession(s),
		useStun(stun)
{
	confId=confid;
	//cerr<<"SDCVididididididididididdididi "+confId<<endl;
	//cerr << "CONFDIALOG: received"<< endl;
	type="connect";
	
	/* We will fill that later, once we know if that succeeded */
	logEntry = NULL;

	setUpStateMachine();
}

SipDialogConfVoip::~SipDialogConfVoip(){	
}

void SipDialogConfVoip::sendInvite(){
	MRef<SipRequest*> inv;
	string keyAgreementMessage;

	//inv= MRef<SipInvite*>(new SipInvite(
	inv = SipRequest::createSipMessageInvite(
				dialogState.callId,
				SipUri(dialogState.remoteUri),
				getDialogConfig()->sipIdentity->getSipUri(),
				getDialogConfig()->getContactUri(useStun),
				dialogState.seqNo,
				getSipStack()) ;

	addAuthorizations( inv );
	addRoute( inv );

	/* Get the session description from the Session */
		
//      There might be so that there are no SDP. Check!
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
		merr << "Sdp was NULL in sendInvite" << endl;
		return; 
		}
	}
	
	/* Add the latter to the INVITE message */ // If it exists
	

//-------------------------------------------------------------------------------------------------------------//
	inv->setContent( *sdp );
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
                SipSMCommand::dialog_layer, 
                SipSMCommand::transaction_layer
                );
	
//	scmd.setDispatched(true); //What was I thinking about here?? --EE
	
//	handleCommand(scmd);
	//cerr<<"SDCV: "+scmd.getCommandString().getString()<<endl;
	getSipStack()->enqueueCommand(scmd, HIGH_PRIO_QUEUE);
	setLastInvite(inv);
	//inv->checkAcceptContact();

}


//#ifdef NEVERDEFINED_ERSADFS
void SipDialogConfVoip::sendAck(){
/*	//	mdbg << "ERROR: SipDialogVoip::sendAck() UNIMPLEMENTED" << end;

	
	massert( !lastResponse.isNull());
	
	SipAck *ack = new SipAck(
			branch, 
			*lastResponse,
			dialogState.remoteUri,
			//getDialogConfig().inherited.sipIdentity->getSipRegistrar()->sipProxyIpAddr->getString());
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
        SipSMCommand cmd( pref, SipSMCommand::dialog_layer, SipSMCommand::transaction_layer);
//	handleCommand(cmd);
	dispatcher->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);

	

	
	//	}*/

	/*	if(dynamic_cast<StreamSocket *>(socket) != NULL){
	// A StreamSocket exists, try to use it
	mdbg << "Sending packet using existing StreamSocket"<<end;
	getDialog()->getDialogConfig().sipTransport->sendMessage(pack,(StreamSocket *)socket);
	return;
	}
	 */
	 
	//MRef<SipAck *> ack = new SipAck(
	MRef<SipRequest*> ack = SipRequest::createSipMessageAck(
		lastInvite,
		lastResponse
		);

//      There might be so that there are no SDP. Check!
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
		merr << "Sdp was NULL in sendInvite" << endl;
		return; 
		}
	}
	
	/* Add the latter to the INVITE message */ // If it exists
	

//-------------------------------------------------------------------------------------------------------------//
	ack->setContent( *sdp );
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
	const list<SipUri> &routes = getDialogConfig()->sipIdentity->getRouteSet();

	ack->addRoutes( routes );

        MRef<SipMessage*> pref(*ack);
	sendSipMessage(pref);
//	getSipStack()->getLayerTransport()->sendMessage(pref,
//							   string("ACK"),
//							   true);
	
	
}
//#endif

void SipDialogConfVoip::sendBye( int bye_seq_no ){

	MRef<SipRequest*> bye = createSipMessageBye();

        MRef<SipMessage*> pref(*bye);
        SipSMCommand cmd( pref, SipSMCommand::dialog_layer, SipSMCommand::transaction_layer);
	getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE );
}

void SipDialogConfVoip::sendCancel(){
	massert( !lastInvite.isNull());
	//MRef<SipCancel*> cancel = new SipCancel(
	MRef<SipRequest*> cancel = SipRequest::createSipMessageCancel(
			lastInvite
// 			dialogState.remoteUri
			);

	cancel->getHeaderValueFrom()->setParameter("tag",dialogState.localTag);
	cancel->getHeaderValueTo()->setParameter("tag",dialogState.remoteTag);

        MRef<SipMessage*> pref(*cancel);
        SipSMCommand cmd( pref, SipSMCommand::dialog_layer, SipSMCommand::transaction_layer);
	getSipStack()->enqueueCommand( cmd, HIGH_PRIO_QUEUE );
}

void SipDialogConfVoip::sendInviteOk(){
	MRef<SipResponse*> ok= new SipResponse( 200,"OK", getLastInvite());	
	ok->getHeaderValueTo()->setParameter("tag",dialogState.localTag);

//      There might be so that there are no SDP. Check!
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
		merr << "Sdp was NULL in sendInvite" << endl;
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
	
	modifyConfOk(ok);
//	
        MRef<SipMessage*> pref(*ok);
        SipSMCommand cmd( pref, SipSMCommand::dialog_layer, SipSMCommand::transaction_layer);
	getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE );
}

void SipDialogConfVoip::sendByeOk( MRef<SipRequest*> bye ){
	MRef<SipResponse*> ok= new SipResponse( 200,"OK", bye );
	ok->getHeaderValueTo()->setParameter("tag",dialogState.localTag);

        MRef<SipMessage*> pref(*ok);
        SipSMCommand cmd( pref, SipSMCommand::dialog_layer, SipSMCommand::transaction_layer);
	getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE );
}

void SipDialogConfVoip::sendReject(){
	MRef<SipResponse*> ringing = new SipResponse( 486, "Temporary unavailable", getLastInvite() );	
	ringing->getHeaderValueTo()->setParameter("tag",dialogState.localTag);
        MRef<SipMessage*> pref(*ringing);
        SipSMCommand cmd( pref,SipSMCommand::dialog_layer, SipSMCommand::transaction_layer);
	getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE );
}

void SipDialogConfVoip::sendRinging(){
	MRef<SipResponse*> ringing = new SipResponse( 180, "Ringing", getLastInvite() );	
	ringing->getHeaderValueTo()->setParameter("tag",dialogState.localTag);
        MRef<SipMessage*> pref(*ringing);
        SipSMCommand cmd( pref, SipSMCommand::dialog_layer, SipSMCommand::transaction_layer);
	getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE );
}

void SipDialogConfVoip::sendNotAcceptable(){
	MRef<SipResponse*> not_acceptable = new SipResponse( 606, "Not Acceptable", getLastInvite());	
	if( mediaSession && mediaSession->getErrorString() != "" ){
		not_acceptable->addHeader( 
			new SipHeader(new SipHeaderValueWarning(getSipStack()->getStackConfig()->externalContactIP, 399, mediaSession->getErrorString() ) ));
	}

	not_acceptable->getHeaderValueTo()->setParameter("tag",dialogState.localTag);
        MRef<SipMessage*> pref(*not_acceptable);
        SipSMCommand cmd( pref, SipSMCommand::dialog_layer, SipSMCommand::transaction_layer);
	getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE );
}


bool SipDialogConfVoip::handleCommand(const SipSMCommand &c){
	mdbg("signaling/sip") << "SipDialogConfVoip::handleCommand got "<< c << endl;

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
	
	mdbg("signaling/sip") << "SipDialogConfVoip::handlePacket() got "<< c << endl;
	bool handled = SipDialog::handleCommand(c);
	
	if (!handled && c.getType()==SipSMCommand::COMMAND_STRING && c.getCommandString().getOp()==SipCommandString::no_transactions){
		return true;
	}
	
	if (c.getType()==SipSMCommand::COMMAND_STRING && dialogState.callId.length()>0){
		if (c.getCommandString().getDestinationId() == dialogState.callId ){
			mdbg("signaling/sip") << "Warning: SipDialogConfVoip ignoring command with matching call id"<< endl;
			return true;
		}
	}
	if (c.getType()==SipSMCommand::COMMAND_PACKET && dialogState.callId.length()>0){
		if (c.getCommandPacket()->getCallId() == dialogState.callId){
			mdbg("signaling/sip") << "Warning: SipDialogConfVoip ignoring packet with matching call id"<< endl;
			return true;
		}
	}

	
	return handled;
}

MRef<SipRequest*> SipDialogConfVoip::getLastInvite(){
    return lastInvite;
}

void SipDialogConfVoip::setLastInvite(MRef<SipRequest*> i){ 
    lastInvite = i; 
}

MRef<LogEntry *> SipDialogConfVoip::getLogEntry(){
	return logEntry;
}

void SipDialogConfVoip::setLogEntry( MRef<LogEntry *> l){
	this->logEntry = l;
}

MRef<Session *> SipDialogConfVoip::getMediaSession(){
	return mediaSession;
}


bool SipDialogConfVoip::sortMIME(MRef<SipMessageContent *> Offer, string peerUri, int type_){
	if (Offer){
		if ( Offer->getContentType().substr(0,9) == "multipart"){
			MRef<SipMessageContent *> part;
			part = ((SipMessageContentMime*)*Offer)->popFirstPart();
			while( *part != NULL){
				sortMIME(part, peerUri, type_);
				part = ((SipMessageContentMime*)*Offer)->popFirstPart();
			}
		}

		if( (Offer->getContentType()).substr(0,15) == "application/sdp"){
			switch (type_){
				case 10:
#ifdef ENABLE_TS
					ts.save("setSdpOffer");
#endif
					if( !getMediaSession()->setSdpOffer( (SdpPacket*)*Offer, peerUri ) )
						return false;
#ifdef ENABLE_TS
					ts.save("setSdpOffer");
#endif
					return true;
				case 3:
#ifdef ENABLE_TS
					ts.save("setSdpAnswer");
#endif
					if( !getMediaSession()->setSdpAnswer( (SdpPacket*)*Offer, peerUri ) )
						return false;
					getMediaSession()->start();
#ifdef ENABLE_TS
					ts.save("setSdpAnswer");
#endif
					return true;
				default:
					merr << "No SDP match" << endl;
					return false;
			}
		}
	}
	return true;
}
void SipDialogConfVoip::modifyConfJoinInvite(MRef<SipRequest*>inv){
	//Add Accept-Contact Header
	//inv->set_ConfJoin();
	MRef<SipHeaderValueAcceptContact*> acp = new SipHeaderValueAcceptContact("+sip.confjoin=\"TRUE\"",false,true);
	inv->addHeader(new SipHeader(*acp) );
			
	//Add SDP Session Level Attributes
	//cerr<<"modify join 1111111111111111"<<endl;
	massert(dynamic_cast<SdpPacket*>(*inv->getContent())!=NULL);
	MRef<SdpPacket*> sdp = (SdpPacket*)*inv->getContent();
	sdp->setSessionLevelAttribute("confId", confId);
	sdp->setSessionLevelAttribute("conf_#participants", itoa((*adviceList).size()));
	for(int t=0;t<numConnected;t++)
	{
		sdp->setSessionLevelAttribute("participant_"+itoa(t+1), ((*adviceList)[t]).uri);
	}
	//cerr<<"modify join 22222222222222222"<<endl;
}
void SipDialogConfVoip::modifyConfAck(MRef<SipRequest*>ack){
	//Add Accept-Contact Header
	ack->addHeader(new SipHeader(new SipHeaderValueAcceptContact("+sip.conf=\"TRUE\"",true,false)) );

	//cerr<<"modify ack 1111111111111111"<<endl;	
	//Add SDP Session Level Attributes
	massert(dynamic_cast<SdpPacket*>(*ack->getContent())!=NULL);
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
	massert(dynamic_cast<SdpPacket*>(*ok->getContent())!=NULL);
	MRef<SdpPacket*> sdp = (SdpPacket*)*ok->getContent();
	sdp->setSessionLevelAttribute("conf_#participants", itoa(connectedList.size()));
	for(int t=0;t<connectedList.size();t++)
	{
		sdp->setSessionLevelAttribute("participant_"+itoa(t+1), ((connectedList)[t]).uri);
	}
}
void SipDialogConfVoip::modifyConfConnectInvite(MRef<SipRequest*>inv){
	//Add Accept-Contact Header
	//inv->set_ConfConnect();
        MRef<SipHeaderValueAcceptContact*> acp = new SipHeaderValueAcceptContact("+sip.confconnect=\"TRUE\"",true,false);
        inv->addHeader(new SipHeader(*acp) );

	massert(dynamic_cast<SdpPacket*>(*inv->getContent())!=NULL);
	MRef<SdpPacket*> sdp = (SdpPacket*)*inv->getContent();
	sdp->setSessionLevelAttribute("confId", confId);	
	//Add SDP Session Level Attributes
	//massert(dynamic_cast<SdpPacket*>(*inv->getContent())!=NULL);
	//MRef<SdpPacket*> sdp = (SdpPacket*)*inv->getContent();
	/*sdp->setSessionLevelAttribute("p2tGroupListServer", getDialogConfig()->inherited.externalContactIP + ":" + itoa(getPhoneConfig()->p2tGroupListServerPort));
	sdp->setSessionLevelAttribute("p2tGroupIdentity", getP2TDialog()->getGroupList()->getGroupIdentity());
	sdp->setSessionLevelAttribute("p2tGroupListProt","http/xml");*/	
}

