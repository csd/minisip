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
 *          Johan Bilien <jobi@via.ecp.fr>
*/

/* Name
 * 	SipDialogP2Tuser.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * 	Florian Maurer, florian.maurer@floHweb.ch
 * Purpose
 * 
*/

#include <config.h>

#include<assert.h>
#include<libminisip/signaling/p2t/SipDialogP2Tuser.h>
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
#include<libminisip/signaling/sip/DefaultDialogHandler.h>
#include<libmutil/stringutils.h>
#include<libmutil/Timestamp.h>
#include<libmutil/termmanip.h>
#include<libmutil/dbg.h>
#include<libmsip/SipSMCommand.h>
#include <time.h>
#include<libminisip/gui/LogEntry.h>
#include<libmsip/SipCommandString.h>

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
  |                      v a23: [Xsend ACKX]                  |                |
  |              +---------------+                            |                |
  |              |               |<---------------------------+                |
  |              |   in call     |-------+                                     |
  |              |               |       |                                     |
  |              +---------------+       |                                     |
  |                      |               | bye                                 |
  |cancel                | BYE           | a6:TransByeInit                     |
  |a8: new TrnsCncl      V a5:new ByeResp|                                     |
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
 
 
bool SipDialogP2Tuser::a0_start_callingnoauth_invite( const SipSMCommand &command)
{
	if (transitionMatch(command, SipCommandString::invite)){

		//assert(dynamic_cast<SipDialogP2Tuser*>(sipStateMachine)!=NULL);
		
		//MRef<SipDialogP2Tuser *>vc= (SipDialogP2Tuser *)sipStateMachine;

#if 0
		
		if(  vc->getDialogConfig().inherited.secured ){
			KeyAgreement * ka = vc->getKeyAgreement();

			ts.save( MIKEY_START );
			string keyAgreementMessage = InitiatorCreate( 
					ka, 
					vc->getDialogConfig() );
			ts.save( MIKEY_END );

			vc->setKeyManagementMessage( keyAgreementMessage );
			vc->setKeyAgreement( ka );

			if( keyAgreementMessage == "" ){
				//TODO: tell the GUI
			}
		}
#endif

		//int seqNo = /*vc->*/requestSeqNo();
		++dialogState.seqNo;
		/*vc->*/setLocalCalled(false);
		/*vc->*/dialogState.remoteUri = command.getCommandString().getParam();

		MRef<SipTransaction*> invtrans = new SipTransactionInviteClientUA(getSipStack(), MRef<SipDialog *>(/* *vc */ this), dialogState.seqNo, dialogState.callId);
		
		invtrans->setSocket( /*vc->*/getPhoneConfig()->proxyConnection );
		
		/*vc->*/registerTransaction(invtrans);

		/*vc->*/sendInvite(invtrans->getBranch());

		return true;
	}else{
		return false;
	}

}

bool SipDialogP2Tuser::a1_callingnoauth_callingnoauth_18X( const SipSMCommand &command)
{
	
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "18*")){

	    MRef<SipResponse*> resp= (SipResponse*) *command.getCommandPacket();

	    //MRef<SipDialogP2Tuser *>vc= (SipDialogP2Tuser *)sipStateMachine;
#ifndef _MSC_VER
		ts.save( RINGING );
#endif
	    CommandString cmdstr(/*vc->*/getCallId(), SipCommandString::remote_ringing);
	    //vc->getDialogContainer()->getCallback()->sipcb_handleCommand(cmdstr);
	    /*vc->*/dialogState.remoteTag = command.getCommandPacket()->getHeaderValueTo()->getParameter("tag");

	    MRef<SdpPacket*> sdp((SdpPacket*)*resp->getContent());
	    if ( !sdp.isNull() ){
		/*vc->*/handleSdp( MRef<SdpPacket*>((SdpPacket*)*sdp) );
	    }
	    //		else{
	    //			mdbg << "WARNING: 200 OK did not contain any session description"<< end;
	    //		}

	    return true;
	}else{
	    return false;
	}
}


bool SipDialogP2Tuser::a2_callingnoauth_callingnoauth_1xx( const SipSMCommand &command)
{

	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "1**")){
		//MRef<SipDialogP2Tuser *> vc= (SipDialogP2Tuser *)sipStateMachine;
		/*vc->*/dialogState.remoteTag = command.getCommandPacket()->getHeaderValueTo()->getParameter("tag");
		return true;
	}else{
		return false;
	}
    
}



bool SipDialogP2Tuser::a3_callingnoauth_incall_2xx( const SipSMCommand &command)
{
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "2**")){

		//MRef<SipDialogP2Tuser *>vc= (SipDialogP2Tuser *)sipStateMachine;
		MRef<SipResponse*> resp(  (SipResponse*)*command.getCommandPacket() );

		/*vc->*/setLogEntry( new LogEntryOutgoingCompletedCall() );
		/*vc->*/getLogEntry()->start = time( NULL );
		/*vc->*/getLogEntry()->peerSipUri = resp->getFrom().getString();

		/*vc->*/dialogState.remoteTag = command.getCommandPacket()->getHeaderValueTo()->getParameter("tag");

#if 0
		/* Key Agreement response message treatment */
		if( vc->getDialogConfig().inherited.secured ){
			
			KeyAgreement * ka = vc->getKeyAgreement();
			int32_t kam = vc->getDialogConfig().inherited.ka_type;
			
			if( !InitiatorAuthenticate( 
				((SdpPacket*)*command.getCommandPacket()->getContent())->getKeyMgmt(),
				ka,
				vc->getDialogConfig()) ){
			 	merr << "KeyAgreement Response message authentication failed" << end;
				return false;
			}
			
			/* TODO: send that error message */
			string error_message = InitiatorParse( 
					((SdpPacket*)*command.getCommandPacket()->getContent())->getKeyMgmt(),
					ka,
					vc->getDialogConfig().inherited.secured,
					kam );

			vc->getDialogConfig().inherited.ka_type = kam;

			if( !vc->getDialogConfig().inherited.secured ){
				// Some error occured in the key agreement parsing
				return false;
			}

			/* At this point, we can compute the tgk */
			//vc->getKeyAgreement()->compute_tgk();
		}
#endif
						
#if 0
                CommandString cmdstr(vc->getCallId(), SipCommandString::invite_ok, "",(vc->getDialogConfig().inherited.secured?"secure":"unprotected"));
#else
                CommandString cmdstr(/*vc->*/getCallId(), SipCommandString::invite_ok, "","unprotected");
#endif
		/*vc->*/getDialogContainer()->getCallback()->sipcb_handleCommand(cmdstr);
		
#if 0
		if( vc->getDialogConfig().inherited.secured && vc->getDialogConfig().inherited.ka_type == KEY_MGMT_METHOD_MIKEY_DH ){
			ts.save( TGK_START );
			((KeyAgreementDH *)vc->getKeyAgreement())->computeTgk();
			ts.save( TGK_END );
		}
#endif
		
		if ( !resp->getContent().isNull() ){
			/*vc->*/handleSdp(MRef<SdpPacket*>((SdpPacket*)*resp->getContent()));
			//vc->openSoundCard();

			//vc->getSoundSender()->initCrypto();
			//vc->getSoundReceiver()->initCrypto();
			//vc->getSoundReceiver()->start();
			//vc->getSoundSender()->start();

			//vc->getSoundReceiver()->flush();
			/*vc->*/reportSipDialogP2T(P2T::STATUS_CONNECTED);
			
		
		}else{
			merr << "WARNING: 200 OK did not contain any session description"<< end;
		}
		
		return true;
	}else
		return false;
}

bool SipDialogP2Tuser::a5_incall_termwait_BYE( const SipSMCommand &command)
{
	
	if (transitionMatch(command, SipBye::type, SipSMCommand::remote, IGN)){
		//MRef<SipDialogP2Tuser *> vc = (SipDialogP2Tuser *)sipStateMachine;
		MRef<SipBye*> bye = (SipBye*) *command.getCommandPacket();

/*
		mdbg << "log stuff"<< end;
		(dynamic_cast< LogEntrySuccess * >(*( vc->getLogEntry() ))
						   )->duration = 
			std::time( NULL ) - vc->getLogEntry()->start; 

*/
		/*vc->*/getLogEntry()->handle();

		MRef<SipTransaction*> byeresp = new SipTransactionNonInviteServer(getSipStack(),
				MRef<SipDialog*>(/* *vc */ this),
				bye->getCSeq(),bye->getFirstViaBranch(), dialogState.callId); //TODO: remove second argument

		/*vc->*/registerTransaction(byeresp);
		SipSMCommand cmd(command);
		cmd.setDestination(SipSMCommand::transaction);
		cmd.setSource(command.getSource());
		/*vc->*/getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);

		/*vc->*/sendByeOk(bye, byeresp->getBranch() );

		CommandString cmdstr(/*vc->*/getCallId(), SipCommandString::remote_hang_up);
		/*vc->*/getDialogContainer()->getCallback()->sipcb_handleCommand(cmdstr);

		//	vc->closeSoundCard();
		//vc->getSoundSender()->stop();
		//vc->getSoundReceiver()->stop();

		
#if 0
		/* FIXME: HACK for the */
		if( vc->getDialogContainer()->getPrecomputedKa() == NULL && vc->getDialogConfig().inherited.dh_enabled )
		{

			KeyAgreement * newKa = new KeyAgreementDH( vc->getDialogConfig().inherited.cert, vc->getDialogConfig().inherited.cert_db, DH_GROUP_OAKLEY5 );
			vc->getDialogContainer()->setPrecomputedKa( newKa );
		}
#endif

		/*vc->*/getP2TDialog()->removeUser(/*vc->*/dialogState.remoteUri, SipCommandString::remote_hang_up, /*vc->*/getCallId());
		/*vc->*/signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}


bool SipDialogP2Tuser::a6_incall_termwait_hangup(
//		State<SipSMCommand, string> *fromState,
//		State<SipSMCommand, string> *toState,
		const SipSMCommand &command)
{
	if (transitionMatch(command, SipCommandString::hang_up)){
		
		//MRef<SipDialogP2Tuser *>vc = (SipDialogP2Tuser *)sipStateMachine;
//		/*vc->*/setCurrentState(toState);
		//int bye_seq_no= /*vc->*/requestSeqNo();
		++dialogState.seqNo;
		
		
		MRef<SipTransaction*> byetrans( new SipTransactionNonInviteClient(getSipStack(), MRef<SipDialog*>(/* *vc */ this), dialogState.seqNo, dialogState.callId)); 

		/*vc->*/registerTransaction(byetrans);
		/*vc->*/sendBye(byetrans->getBranch(), dialogState.seqNo);
		

		/////BUG: Here. Receive Segmentation fault if dialog was started by a incoming INVITE message
/*		(dynamic_cast< LogEntrySuccess * >(*( vc->getLogEntry() ))
						   )->duration = 
			std::time( NULL ) - vc->getLogEntry()->start; 
		vc->getLogEntry()->handle();
*/

				
		//	vc->closeSoundCard();
		//vc->getSoundSender()->stop();
		//vc->getSoundReceiver()->stop();

#if 0
		/* FIXME: HACK for the */
		if( vc->getDialogContainer()->getPrecomputedKa() == NULL && vc->getDialogConfig().inherited.dh_enabled ){
			KeyAgreement * newKa = new KeyAgreementDH( vc->getDialogConfig().inherited.cert, vc->getDialogConfig().inherited.cert_db, DH_GROUP_OAKLEY5 );
			vc->getDialogContainer()->setPrecomputedKa( newKa );
		}
#endif


		//vc->getP2TDialog()->removeUser(vc->getDialogConfig().uri_foreign,"local_hangup", vc->getCallId());

		
		/*vc->*/signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}



bool SipDialogP2Tuser::a7_callingnoauth_termwait_CANCEL(
//		State<SipSMCommand, string> *fromState,
//		State<SipSMCommand, string> *toState,
		const SipSMCommand &command)
{
	if (transitionMatch(command, SipCancel::type, SipSMCommand::remote, IGN)){
		//MRef<SipDialogP2Tuser *> vc = (SipDialogP2Tuser *)sipStateMachine;
//		/*vc->*/setCurrentState(toState);

		MRef<SipTransaction*> cancelresp( 
				new SipTransactionNonInviteServer(
					getSipStack(),
					MRef<SipDialog*>(/* *vc */ this), 
					command.getCommandPacket()->getCSeq(), 
					command.getCommandPacket()->getFirstViaBranch(), dialogState.callId ));

		/*vc->*/registerTransaction(cancelresp);

		SipSMCommand cmd(command);
		cmd.setSource(SipSMCommand::TU);
		cmd.setDestination(SipSMCommand::transaction);
		/*vc->*/getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
	
		/*vc->*/getP2TDialog()->removeUser(/*vc->*/dialogState.remoteUri, "cancel_received",/*vc->*/getCallId());
		/*vc->*/signalIfNoTransactions();
		return true;
	}else{
		return false;
	}
}


bool SipDialogP2Tuser::a8_callingnoauth_termwait_cancel(
//		State<SipSMCommand, string> *fromState,
//		State<SipSMCommand, string> *toState,
		const SipSMCommand &command)
{
	if (transitionMatch(command, SipCommandString::cancel) || transitionMatch(command, SipCommandString::hang_up)){
		//MRef<SipDialogP2Tuser *> vc = (SipDialogP2Tuser *)sipStateMachine;
//		/*vc->*/setCurrentState(toState);

		MRef<SipTransaction*> canceltrans( new SipTransactionNonInviteClient(getSipStack(), MRef<SipDialog*>(/* *vc */ this), /*vc->*/dialogState.seqNo, dialogState.callId)); 

		/*vc->*/registerTransaction(canceltrans);
		/*vc->*/sendCancel(canceltrans->getBranch());

		/*vc->*/signalIfNoTransactions();
		//vc->getP2TDialog()->removeUser(vc->getDialogConfig().uri_foreign, "local_cancelled",vc->getCallId());
		return true;
	}else{
		return false;
	}
}

//Note: This is also used as: callingauth_terminated_36
bool SipDialogP2Tuser::a9_callingnoauth_termwait_36( const SipSMCommand &command)
{
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "3**\n4**\n5**\n6**")){
		//MRef<SipDialogP2Tuser *> vc = (SipDialogP2Tuser *)sipStateMachine;
		
		MRef<LogEntry *> rejectedLog( new LogEntryCallRejected() );
		rejectedLog->start = time( NULL );
		rejectedLog->peerSipUri = /*vc->*/dialogState.remoteUri;
		dynamic_cast<LogEntryFailure *>(*rejectedLog)->error =
			"Remote user rejected the call";
		/*vc->*/setLogEntry( rejectedLog );
		rejectedLog->handle();
		
		string reason="";
		if (sipResponseFilterMatch(MRef<SipResponse*>((SipResponse*)*command.getCommandPacket()),"404")){
                        CommandString cmdstr(/*vc->*/getCallId(), SipCommandString::remote_user_not_found);
			/*vc->*/getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr);
			reason=SipCommandString::remote_user_not_found;
                        
		}else if (sipResponseFilterMatch(MRef<SipResponse*>((SipResponse*)*command.getCommandPacket()),"606")){
                        
                        CommandString cmdstr( /*vc->*/getCallId(), SipCommandString::remote_unacceptable);
			/*vc->*/getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
			reason=SipCommandString::remote_unacceptable;
		}
		else if (sipResponseFilterMatch(MRef<SipResponse*>((SipResponse*)*command.getCommandPacket()),"4**")){
                        CommandString cmdstr( /*vc->*/getCallId(), SipCommandString::remote_reject);
			/*vc->*/getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
			reason=SipCommandString::remote_reject;
		}
		else{
			merr << "ERROR: received response in SipDialogP2Tuser"
				" that could not be handled (unimplemented)"<< end;
			reason="unexpected_response";
                }
		
		/*vc->*/signalIfNoTransactions();
		/*vc->*/getP2TDialog()->removeUser(dialogState.remoteUri,reason,/*vc->*/getCallId());
		return true;
	}else{
		return false;
	}
}


bool SipDialogP2Tuser::a10_start_ringing_INVITE( const SipSMCommand &command)
{
	if (transitionMatch(command, SipInvite::type, IGN, SipSMCommand::TU)){
		//MRef<SipDialogP2Tuser *> vc= (SipDialogP2Tuser *)sipStateMachine;

		dialogState.remoteUri = command.getCommandPacket()->getHeaderValueFrom()->getUri().getUserName()+"@"+ 
			command.getCommandPacket()->getHeaderValueFrom()->getUri().getIp();

		//vc->getDialogConfig().inherited->userUri = command.getCommandPacket()->getHeaderTo()->getUri().getUserIpString().substr(4);
		/*vc->*/getDialogConfig()->inherited->sipIdentity->setSipUri(  command.getCommandPacket()->getHeaderValueTo()->getUri().getUserIpString() );

		/*vc->*/ /*setSeqNo(command.getCommandPacket()->getCSeq() );*/
		dialogState.seqNo = command.getCommandPacket()->getCSeq();
		
		/*vc->*/dialogState.remoteTag = command.getCommandPacket()->getHeaderValueFrom()->getParameter("tag");

		/*vc->*/setLocalCalled(true);
		/*vc->*/setLastInvite(MRef<SipInvite*>((SipInvite *)*command.getCommandPacket()));

#if 0
		KeyAgreement * ka = vc->getDialogContainer()->getPrecomputedKa();

		if( !ResponderAuthenticate( ((SdpPacket*)*command.getCommandPacket()->getContent())->getKeyMgmt(),
					ka, vc->getDialogConfig() ) ){
			// Authentication failed
#ifdef DEBUG_OUTPUT
			merr << "Authentication of the incoming MIKEY message failed" << end;
#endif
			return false;
		}
		vc->setKeyAgreement(ka);
#endif


		MRef<SipTransaction*> ir( new SipTransactionInviteServerUA(
						getSipStack(),
						MRef<SipDialog*>(/* *vc */), 
						command.getCommandPacket()->getCSeq(),
						command.getCommandPacket()->getFirstViaBranch(), dialogState.callId) );

		/*vc->*/registerTransaction(ir);

		//	command.setSource(SipSMCommand::TU);
		//	command.setDestination(SipSMCommand::transaction);

		//	mdbg << "^^^^ SipDialogP2Tuser: re-handling packet for transaction to catch:"<<command<<end;

		SipSMCommand cmd(command);
		cmd.setDestination(SipSMCommand::transaction);

		/*vc->*/getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);

		CommandString cmdstr(/* vc->*/getCallId(), 
				"p2tAddUser", 
				/*vc->*/dialogState.remoteUri, 
#if 0
				(/*vc->*/getDialogConfig()->inherited->secured?"secure":"unprotected")
#else
				"unprotected"
#endif
				);
		//vc->getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
		/*vc->*/sendRinging(ir->getBranch());
		return true;
	}else{
		return false;
	}
}

bool SipDialogP2Tuser::a11_ringing_incall_accept( const SipSMCommand &command)
{
	if (transitionMatch(command, SipCommandString::accept_invite)){
#ifndef _MSC_VER
		ts.save(USER_ACCEPT);
#endif
		//MRef<SipDialogP2Tuser *>vc= (SipDialogP2Tuser *)sipStateMachine;

#if 0
		if( vc->getDialogConfig().inherited->secured ){
			KeyAgreement * ka = vc->getKeyAgreement();
			int32_t kam = vc->getDialogConfig().inherited->ka_type;

			string responseMessage = ResponderParse( 
					((SdpPacket*)*vc->getLastInvite()->getContent())->getKeyMgmt(), 
					ka,
					vc->getDialogConfig().inherited->secured,
					kam );

			vc->setKeyManagementMessage( "mikey " + responseMessage );

			if( !vc->getDialogConfig().inherited->secured ){
				/* An error occured when parsing the key agreement message */
				return false;
			}

		}
#endif
		CommandString cmdstr(/*vc->*/getCallId(), 
				SipCommandString::invite_ok,"",
#if 0
				(/*vc->*/getDialogConfig()->inherited->secured?"secure":"unprotected")
#else
				"unprotected"
#endif
				);
		/*vc->*/getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );

		assert( !/*vc->*/getLastInvite().isNull() );
		/*vc->*/sendInviteOk(/*vc->*/getLastInvite()->getDestinationBranch() );

#if 0
		if( vc->getDialogConfig().inherited->secured && vc->getDialogConfig().inherited->ka_type == KEY_MGMT_METHOD_MIKEY_DH ){
			ts.save( TGK_START );
			((KeyAgreementDH *)vc->getKeyAgreement())->computeTgk();
			ts.save( TGK_END );
		}
#endif

		if ( !/*vc->*/getLastInvite()->getContent().isNull() ){
			/*vc->*/handleSdp(MRef<SdpPacket*>((SdpPacket*) * /*vc->*/getLastInvite()->getContent()));
			//vc->openSoundCard();
			//vc->getSoundSender()->initCrypto();
			//vc->getSoundReceiver()->initCrypto();
			//vc->getSoundReceiver()->start();
			//vc->getSoundSender()->start();

			//vc->getSoundReceiver()->flush();
			/*vc->*/reportSipDialogP2T(P2T::STATUS_CONNECTED);
			
		}else{
			merr << "WARNING: INVITE did not contain any session description"<< end;
		}


		return true;
	}else{
		return false;
	}
}

bool SipDialogP2Tuser::a12_ringing_termwait_CANCEL( const SipSMCommand &command)
{

	if (transitionMatch(command, SipCancel::type, IGN, IGN)){

		//MRef<SipDialogP2Tuser *> vc= (SipDialogP2Tuser *)sipStateMachine;

		//FIXME: is this correct - this should probably be handled
		//in the already existing transaction.
		MRef<SipTransaction*> cr( new
				SipTransactionNonInviteServer(getSipStack(),
					MRef<SipDialog*>(/* *vc */ this),
					command.getCommandPacket()->getCSeq(),
					command.getCommandPacket()->getFirstViaBranch(), dialogState.callId) );
		/*vc->*/registerTransaction(cr);

		SipSMCommand cmd(command);
		cmd.setDestination(SipSMCommand::transaction);

		//	mdbg << "^^^^ SipDialogP2Tuser: re-handling packet for transaction to catch."<<end;
		/*vc->*/getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);


		//	mdbg << "^^^^ SipDialogP2Tuser: sending ok to cancel packet"<<end;
		/* Tell the GUI */
		CommandString cmdstr(/*vc->*/getCallId(), SipCommandString::remote_cancelled_invite,"");
		/*vc->*/getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );

		/*vc->*/sendInviteOk(cr->getBranch() ); 

		/*vc->*/signalIfNoTransactions();
		/*vc->*/getP2TDialog()->removeUser(/*vc->*/dialogState.remoteUri, SipCommandString::remote_cancelled_invite,/*vc->*/getCallId());
		return true;
	}else{
		return false;
	}
}


bool SipDialogP2Tuser::a13_ringing_termwait_reject( const SipSMCommand &command)
{
	
	if (transitionMatch(command, SipCommandString::reject_invite) || transitionMatch(command,SipCommandString::hang_up)){

		//MRef<SipDialogP2Tuser *> vc= (SipDialogP2Tuser *)sipStateMachine;

		/*vc->*/sendReject( /*vc->*/getLastInvite()->getDestinationBranch() );

		/*vc->*/signalIfNoTransactions();
		/*vc->*/getP2TDialog()->removeUser(/*vc->*/dialogState.remoteUri, SipCommandString::reject_invite,/*vc->*/getCallId());
		return true;
	}else{
		return false;
	}
}


bool SipDialogP2Tuser::a16_start_termwait_INVITE( const SipSMCommand &command)
{
	
	if (transitionMatch(command, SipInvite::type, SipSMCommand::remote, SipSMCommand::TU)){

		//MRef<SipDialogP2Tuser *> vc= (SipDialogP2Tuser *)sipStateMachine;
		/*vc->*/setLastInvite(MRef<SipInvite*>((SipInvite *)*command.getCommandPacket()));

		MRef<SipTransaction*> ir( new SipTransactionInviteServerUA(getSipStack(),
					MRef<SipDialog*>( /* *vc */ this),
					command.getCommandPacket()->getCSeq(),
					command.getCommandPacket()->getFirstViaBranch(), dialogState.callId ));

		/*vc->*/registerTransaction(ir);

		SipSMCommand cmd(command);
		cmd.setDestination(SipSMCommand::transaction);

		/*vc->*/getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);

		/*vc->*/sendNotAcceptable( command.getCommandPacket()->getDestinationBranch() );

		/*vc->*/signalIfNoTransactions();
		/*vc->*/getP2TDialog()->removeUser(/*vc->*/dialogState.remoteUri, "send_notacceptable",/*vc->*/getCallId());
		return true;
	}else{
		return false;
	}
}

bool SipDialogP2Tuser::a20_callingnoauth_callingauth_40X( const SipSMCommand &command){

	if (transitionMatch(command,SipResponse::type, IGN, SipSMCommand::TU, "407\n401")){
		
		//MRef<SipDialogP2Tuser *> vc= (SipDialogP2Tuser *)sipStateMachine;
		MRef<SipResponse*> resp( (SipResponse*)*command.getCommandPacket() );

		/*vc->*/dialogState.remoteTag = command.getCommandPacket()->getHeaderValueTo()->getParameter("tag");

		//int seqNo = /*vc->*/requestSeqNo();
		++dialogState.seqNo;
		MRef<SipTransaction*> trans( new SipTransactionInviteClientUA(getSipStack(), MRef<SipDialog*>(/* *vc */ this), dialogState.seqNo, dialogState.callId));
		/*vc->*/registerTransaction(trans);
		
		/*vc->*/setRealm( resp->getRealm() );
		/*vc->*/setNonce( resp->getNonce() );
		/*vc->*/sendAuthInvite(trans->getBranch());

		return true;
	}else{
		return false;
	}
}


bool SipDialogP2Tuser::a21_callingauth_callingauth_18X(
		const SipSMCommand &command){
	
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "18*")){
		MRef<SipResponse*> resp (  (SipResponse*)*command.getCommandPacket()  );
		//MRef<SipDialogP2Tuser *> vc= (SipDialogP2Tuser *)sipStateMachine;

#ifndef _MSC_VER
		ts.save( RINGING );
#endif

		CommandString cmdstr(/*vc->*/getCallId(), SipCommandString::remote_ringing);
		/*vc->*/getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
		/*vc->*/dialogState.remoteTag = command.getCommandPacket()->getHeaderValueTo()->getParameter("tag");
		if ( !resp->getContent().isNull()){
			/*vc->*/handleSdp(MRef<SdpPacket*>((SdpPacket*)*resp->getContent()) );
		}
		//		else{
		//			merr << "WARNING: 200 OK did not contain any session description"<< end;
		//		}

		return true;
	}else{
		return false;
	}
}

bool SipDialogP2Tuser::a22_callingauth_callingauth_1xx(
		const SipSMCommand &command){
	
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "1**")){
		//SipDialogP2Tuser *vc= (SipDialogP2Tuser *)sipStateMachine;

		/*vc->*/dialogState.remoteTag= command.getCommandPacket()->getHeaderValueTo()->getParameter("tag");
		return true;
	}else{
		return false;
	}
}

bool SipDialogP2Tuser::a23_callingauth_incall_2xx(
		const SipSMCommand &command){

	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "2**")){

		//SipDialogP2Tuser *vc= (SipDialogP2Tuser *)sipStateMachine;
		MRef<SipResponse*> resp( (SipResponse*)*command.getCommandPacket() );
		
		/*vc->*/setLogEntry( new LogEntryOutgoingCompletedCall() );
		/*vc->*/getLogEntry()->start = time( NULL );
		/*vc->*/getLogEntry()->peerSipUri = resp->getFrom().getString();

		/*vc->*/dialogState.remoteTag = command.getCommandPacket()->getHeaderValueTo()->getParameter("tag");

		
#if 0
		/* Key Agreement response message treatment */
		if( vc->getDialogConfig().inherited->secured ){

			KeyAgreement * ka = vc->getKeyAgreement();
			int32_t kam = vc->getDialogConfig().inherited->ka_type;

			if( !InitiatorAuthenticate( 
						((SdpPacket*)*command.getCommandPacket()->getContent())->getKeyMgmt(),
						ka,
						vc->getDialogConfig()) ){
				merr << "KeyAgreement Response message authentication failed" << end;
				return false;
			}

			/* TODO: send that error message */
			string error_message = InitiatorParse( 
					((SdpPacket*)*command.getCommandPacket()->getContent())->getKeyMgmt(),
					ka,
					vc->getDialogConfig().inherited->secured,
					kam );

			vc->getDialogConfig().inherited->ka_type = kam;

			if( !vc->getDialogConfig().inherited->secured ){
				// Some error occured in the key agreement parsing
				return false;
			}

			/* At this point, we can compute the tgk */
			//vc->getKeyAgreement()->compute_tgk();
		}
#endif


		CommandString cmdstr(/*vc->*/getCallId(), 
				SipCommandString::invite_ok, 
				"",
#if 0
				(/*vc->*/getDialogConfig()->inherited->secured?"secure":"unprotected")
#else
				"unprotected"
#endif
				);
		/*vc->*/getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );

#if 0
		if( vc->getDialogConfig().inherited->secured && vc->getDialogConfig().inherited->ka_type == KEY_MGMT_METHOD_MIKEY_DH ){
			ts.save( TGK_START );
			((KeyAgreementDH *)vc->getKeyAgreement())->computeTgk();
			ts.save( TGK_END );
		}
#endif

		if ( !resp->getContent().isNull()){
			/*vc->*/handleSdp(MRef<SdpPacket*>((SdpPacket*)*resp->getContent()));
			//vc->openSoundCard();

			//vc->getSoundSender()->initCrypto();
			//vc->getSoundReceiver()->initCrypto();
			//vc->getSoundReceiver()->start();
			//vc->getSoundSender()->start();

			//vc->getSoundReceiver()->flush();
			
			/*vc->*/reportSipDialogP2T(P2T::STATUS_CONNECTED);
			

		}else{
			merr << "WARNING: 200 OK did not contain any session description"<< end;
		}

		return true;
	}else{
		return false;
	}
}

bool SipDialogP2Tuser::a24_calling_termwait_2xx(
//		State<SipSMCommand, string> *fromState,
//		State<SipSMCommand, string> *toState,
		const SipSMCommand &command){
	
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "2**")){

		//MRef<SipDialogP2Tuser *> vc = (SipDialogP2Tuser *)sipStateMachine;
		//int bye_seq_no= /*vc->*/requestSeqNo();
		++dialogState.seqNo;
//		/*vc->*/setCurrentState(toState);

		MRef<SipTransaction*> byetrans = new SipTransactionNonInviteClient(getSipStack(), MRef<SipDialog*>(/* *vc */), /*vc->*/dialogState.seqNo, dialogState.callId); 


		/*vc->*/registerTransaction(byetrans);
		/*vc->*/sendBye(byetrans->getBranch(), dialogState.seqNo);

		CommandString cmdstr(/*vc->*/getCallId(), SipCommandString::security_failed);
		/*vc->*/getDialogContainer()->getCallback()->sipcb_handleCommand(cmdstr);

		/*vc->*/signalIfNoTransactions();
		/*vc->*/getP2TDialog()->removeUser(/*vc->*/dialogState.remoteUri, SipCommandString::security_failed,/*vc->*/getCallId());
		return true;
	} else{
		return false;
	}
}

bool SipDialogP2Tuser::a25_termwait_terminated_notransactions(
		const SipSMCommand &command){
	
	if (transitionMatch(command, SipCommandString::no_transactions) ){

		//MRef<SipDialogP2Tuser *> vc= (SipDialogP2Tuser *)sipStateMachine;

		/*vc->*/setLastInvite(NULL);

		SipSMCommand cmd(
				CommandString( /*vc->*/this->getCallId(), SipCommandString::call_terminated),
				SipSMCommand::TU,
				SipSMCommand::DIALOGCONTAINER);

		/*vc->*/getDialogContainer()->enqueueCommand( cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE );

		//vc->getSoundSender()->setDialog(NULL);
		//vc->getSoundReceiver()->setDialog(NULL);
		
		return true;
	}else{
		return false;
	}
}


bool SipDialogP2Tuser::a26_callingnoauth_termwait_transporterror(
		const SipSMCommand &command){

	if (transitionMatch(command, SipCommandString::transport_error )){
		//MRef<SipDialogP2Tuser *> vc= (SipDialogP2Tuser *)sipStateMachine;
		
		CommandString cmdstr(/*vc->*/getCallId(), SipCommandString::transport_error);
		/*vc->*/getDialogContainer()->getCallback()->sipcb_handleCommand(cmdstr);
	
		/*vc->*/getP2TDialog()->removeUser(/*vc->*/dialogState.remoteUri, SipCommandString::transport_error,/*vc->*/getCallId());
		return true;
	}else{
		return false;
	}
}



void SipDialogP2Tuser::setUpStateMachine(){

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


//	StateTransition<SipSMCommand,string> *transition_start_callingnoauth_invite=
		new StateTransition<SipSMCommand,string>(this,
				"transition_start_callingnoauth_invite",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2Tuser::a0_start_callingnoauth_invite, 
				s_start, s_callingnoauth
				);

//	StateTransition<SipSMCommand,string> *transition_callingnoauth_callingnoauth_18X=
		new StateTransition<SipSMCommand,string>(this,
				"transition_callingnoauth_callingnoauth_18X",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2Tuser::a1_callingnoauth_callingnoauth_18X, 
				s_callingnoauth, s_callingnoauth
				);

//	StateTransition<SipSMCommand,string> *transition_callingnoauth_callingnoauth_1xx=
		new StateTransition<SipSMCommand,string>(this,
				"transition_callingnoauth_callingnoauth_1xx",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2Tuser::a2_callingnoauth_callingnoauth_1xx, 
				s_callingnoauth, s_callingnoauth
				);

//	StateTransition<SipSMCommand,string> *transition_callingnoauth_incall_2xx=
		new StateTransition<SipSMCommand,string>(this,
				"transition_callingnoauth_incall_2xx",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2Tuser::a3_callingnoauth_incall_2xx, 
				s_callingnoauth, s_incall
				);

//	StateTransition<SipSMCommand,string> *transition_callingnoauth_termwait_2xx=
		new StateTransition<SipSMCommand,string>(this,
				"transition_callingauth_termwait_2xx",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2Tuser::a24_calling_termwait_2xx,
				s_callingnoauth, s_termwait
				);

//	StateTransition<SipSMCommand,string> *transition_incall_termwait_BYE=
		new StateTransition<SipSMCommand,string>(this,
				"transition_incall_termwait_BYE",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2Tuser::a5_incall_termwait_BYE,
				s_incall, s_termwait
				);

//	StateTransition<SipSMCommand,string> *transition_incall_termwait_hangup=
		new StateTransition<SipSMCommand,string>(this,
				"transition_incall_termwait_hangup",
				(bool (StateMachine<SipSMCommand,string>::*)(/*State<SipSMCommand, string> *,State<SipSMCommand, string> *,*/const SipSMCommand&)) &SipDialogP2Tuser::a6_incall_termwait_hangup,
				s_incall, s_termwait
				);

//	StateTransition<SipSMCommand,string> *transition_callingnoauth_termwait_CANCEL=
		new StateTransition<SipSMCommand,string>(this,
				"transition_callingnoauth_termwait_CANCEL",
				(bool (StateMachine<SipSMCommand,string>::*)(/*State<SipSMCommand, string> *,State<SipSMCommand, string> *,*/const SipSMCommand&)) &SipDialogP2Tuser::a7_callingnoauth_termwait_CANCEL,
				s_callingnoauth, s_termwait
				);

//	StateTransition<SipSMCommand,string> *transition_callingnoauth_termwait_cancel=
		new StateTransition<SipSMCommand,string>(this,
				"transition_callingnoauth_termwait_cancel",
				(bool (StateMachine<SipSMCommand,string>::*)(/*State<SipSMCommand, string> *,State<SipSMCommand, string> *,*/const SipSMCommand&)) &SipDialogP2Tuser::a8_callingnoauth_termwait_cancel,
				s_callingnoauth, s_termwait
				);

	
//	StateTransition<SipSMCommand,string> *transition_callingnoauth_callingauth_40X=
		new StateTransition<SipSMCommand,string>(this,
				"transition_callingnoauth_callingauth_40X",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2Tuser::a20_callingnoauth_callingauth_40X,
				s_callingnoauth, s_callingauth
				);
	

//	StateTransition<SipSMCommand,string> *transition_callingnoauth_termwait_36=
		new StateTransition<SipSMCommand,string>(this,
				"transition_callingnoauth_termwait_36",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2Tuser::a9_callingnoauth_termwait_36,
				s_callingnoauth, s_termwait
				);

//	StateTransition<SipSMCommand,string> *transition_start_ringing_INVITE=
		new StateTransition<SipSMCommand,string>(this,
				"transition_start_ringing_INVITE",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2Tuser::a10_start_ringing_INVITE,
				s_start, s_ringing
				);

//	StateTransition<SipSMCommand,string> *transition_ringing_incall_accept=
		new StateTransition<SipSMCommand,string>(this,
				"transition_ringing_incall_accept",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2Tuser::a11_ringing_incall_accept,
				s_ringing, s_incall
				);
			
//	StateTransition<SipSMCommand,string> *transition_ringing_termwait_CANCEL=
		new StateTransition<SipSMCommand,string>(this,
				"transition_ringing_termwait_CANCEL",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2Tuser::a12_ringing_termwait_CANCEL,
				s_ringing, s_termwait
				);
				
//	StateTransition<SipSMCommand,string> *transition_ringing_termwait_reject=
		new StateTransition<SipSMCommand,string>(this,
				"transition_ringing_termwait_reject",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2Tuser::a13_ringing_termwait_reject,
				s_ringing, s_termwait
				);

//	StateTransition<SipSMCommand,string> *transition_start_termwait_INVITE=
		new StateTransition<SipSMCommand,string>(this,
				"transition_start_termwait_INVITE",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2Tuser::a16_start_termwait_INVITE,
				s_start, s_termwait
				);

//	StateTransition<SipSMCommand,string> *transition_callingauth_callingauth_18X=
		new StateTransition<SipSMCommand,string>(this,
				"transition_callingauth_callingauth_18X",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2Tuser::a21_callingauth_callingauth_18X, 
				s_callingauth, s_callingauth
				);

//	StateTransition<SipSMCommand,string> *transition_callingauth_callingauth_1xx=
		new StateTransition<SipSMCommand,string>(this,
				"transition_callingauth_callingauth_1xx",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2Tuser::a22_callingauth_callingauth_1xx, 
				s_callingauth, s_callingauth
				);

//	StateTransition<SipSMCommand,string> *transition_callingauth_incall_2xx=
		new StateTransition<SipSMCommand,string>(this,
				"transition_callingauth_incall_2xx",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2Tuser::a23_callingauth_incall_2xx, 
				s_callingauth, s_incall
				);
	
//	StateTransition<SipSMCommand,string> *transition_callingauth_termwait_2xx=
		new StateTransition<SipSMCommand,string>(this,
				"transition_callingauth_termwait_2xx",
				(bool (StateMachine<SipSMCommand,string>::*)(/*State<SipSMCommand, string> *,State<SipSMCommand, string> *,*/const SipSMCommand&)) &SipDialogP2Tuser::a24_calling_termwait_2xx,
				s_callingauth, s_termwait
				);
        
//	StateTransition<SipSMCommand,string> *transition_callingauth_termwait_resp36=
		new StateTransition<SipSMCommand,string>(this,
				"transition_callingauth_termwait_resp36",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2Tuser::a9_callingnoauth_termwait_36,
				s_callingauth, s_termwait
				);

//	StateTransition<SipSMCommand,string> *transition_termwait_terminated_notransactions=
		new StateTransition<SipSMCommand,string>(this,
				"transition_termwait_terminated_notransactions",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2Tuser::a25_termwait_terminated_notransactions,
				s_termwait, s_terminated
				);
       
//	StateTransition<SipSMCommand,string> *transition_callingnoauth_termwait_transporterror=
		new StateTransition<SipSMCommand,string>(this,
				"transition_callingnoauth_termwait_transporterror",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2Tuser::a26_callingnoauth_termwait_transporterror,
				s_callingnoauth, s_termwait
				);


	setCurrentState(s_start);
}



SipDialogP2Tuser::SipDialogP2Tuser(MRef<SipStack*> stack, MRef<SipDialogConfig*> callconfig, 
	MRef<SipSoftPhoneConfiguration*> pconf, MRef<SipDialogP2T*>p2tDialog) : 
                SipDialog(stack,callconfig),
                lastInvite(NULL), 
		phoneconf(pconf)
{
	
	/*
	soundReceiver = new SoundReceiver(getPhoneConfig()->soundcard, NULL, getPhoneConfig());
	soundSender = new SoundSender(soundReceiver->getSocket(), 
			getPhoneConfig()->soundcard, 
			getPhoneConfig()->inherited->codecs[0], //FIXME
			NULL);
	soundSender->setDialog(this);
	soundReceiver->setDialog(this);
	*/

	
//	getDialogConfig().callId = itoa(rand())+"@"+getDialogConfig().inherited->localIpString;
	/*getDialogConfig().callId*/ dialogState.callId = itoa(rand())+"@"+getDialogConfig()->inherited->externalContactIP;
	
	/* We will fill that later, once we know if that succeeded */
	logEntry = NULL;

//	setKeyAgreement( getDialogContainer()->getPrecomputedKa() );
//
//	getDialogContainer()->setPrecomputedKa( NULL );
	
	//set p2tDialog
	this->p2tDialog=p2tDialog;
	
	//initialize private variable
	this->myIp=NULL;
	this->myRTPport=0;
	this->myRTCPport=0;
	this->myCodec=NULL;
	
	//start state machine
	setUpStateMachine();
}

SipDialogP2Tuser::~SipDialogP2Tuser(){	
}

void SipDialogP2Tuser::handleSdp(MRef<SdpPacket*> /*sdp*/){
#ifdef OLD_MEDIA
	int codec=0;
	Codec *c=NULL;
//	for (unsigned i=0; i< getDialogContainer()->getPhoneConfig()->inherited->codecs.size(); i++)
	for (unsigned i=0; i< getPhoneConfig()->inherited->codecs.size(); i++)
//		if (getDialogContainer()->getPhoneConfig()->inherited->codecs[i]->get_sdp_media_type()==codec)
		if (getPhoneConfig()->inherited->codecs[i]->getSdpMediaType()==codec)
//			c = getDialogContainer()->getPhoneConfig()->inherited->codecs[i];
			c = getPhoneConfig()->inherited->codecs[i];
	assert(c!=NULL);
        int port;
        
	myIp = sdp->getRemoteAddr(port);
	myRTPport = port;
	myRTCPport = port + 1;
	myCodec = c;
	
	
	//IPAddress *ip = sdp->getRemoteAddr(port);
	//soundSender->setRemoteAddr( ip, port );

	//soundSender->setCodec(c);
	
	//soundReceiver->registerSoundSource(-2); //FIXME: 
#endif

}


void SipDialogP2Tuser::sendInvite(const string &branch){
	//	mdbg << "ERROR: SipDialogP2Tuser::sendInvite() UNIMPLEMENTED"<< end;
//	string call_id = getDialogConfig().callId;
	MRef<SipInvite*> inv;
	string keyAgreementMessage;

		inv= MRef<SipInvite*>(new SipInvite(
				branch,
				/*getDialogConfig().callId*/ dialogState.callId,
				dialogState.remoteUri,
				getDialogConfig()->inherited->sipIdentity->getSipRegistrar()->sipProxyIpAddr->getString(),
				getDialogConfig()->inherited->sipIdentity->getSipRegistrar()->sipProxyPort,
//				getDialogConfig().inherited->localIpString,
				getDialogConfig()->inherited->externalContactIP,
				getDialogConfig()->inherited->getLocalSipPort(getPhoneConfig()->useSTUN),
				//getDialogConfig().inherited->userUri,
				getDialogConfig()->inherited->sipIdentity->getSipUri(),
				dialogState.seqNo,
//				requestSeqNo(),
//				getSoundReceiver()->getSocket()->get_port(),
//				getP2TDialog()->getRTPPort(),
				//                              getDialog()->getDialogConfig().localMediaPort, 
				/*getDialogConfig().inherited->codecs,*/
				getDialogConfig()->inherited->transport) );
	
	inv->getHeaderValueFrom()->setParameter("tag",dialogState.localTag);
	
	//add P2T stuff to the invite message
	modifyP2TInvite(inv);
	

//	mdbg << "SipDialogP2Tuser::sendInvite(): sending INVITE to transaction"<<end;
//	ts.save( INVITE_END );
        MRef<SipMessage*> pktr(*inv);

        SipSMCommand scmd(
                pktr, 
                SipSMCommand::TU, 
                SipSMCommand::transaction
                );
	
//	scmd.setDispatched(true); // What was I thinging about here?? --EE
	
//	handleCommand(scmd);
	getDialogContainer()->enqueueCommand(scmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
	setLastInvite(inv);

}

void SipDialogP2Tuser::sendAuthInvite(const string &branch){
	//	merr << "ERROR: SipDialogP2Tuser::sendAuthInvite() UNIMPLEMENTED"<< end;
//	string call_id = getDialogConfig().callId;
	//SipInvite * inv;
	MRef<SipInvite*> inv;
	string keyAgreementMessage;

	inv= new SipInvite(
			branch,
			/*getDialogConfig().callId*/ dialogState.callId,
			dialogState.remoteUri,
			getDialogConfig()->inherited->sipIdentity->getSipRegistrar()->sipProxyIpAddr->getString(),
			getDialogConfig()->inherited->sipIdentity->getSipRegistrar()->sipProxyPort,
			//				getDialogConfig().inherited->localIpString,
			getDialogConfig()->inherited->externalContactIP,
			getDialogConfig()->inherited->getLocalSipPort(getPhoneConfig()->useSTUN),
			//getDialogConfig().inherited->userUri,
			getDialogConfig()->inherited->sipIdentity->getSipUri(),
			dialogState.seqNo,
			//				requestSeqNo(),
			getDialogConfig()->inherited->sipIdentity->getSipRegistrar()->sipProxyUsername,
			nonce,
			realm,
			getDialogConfig()->inherited->sipIdentity->getSipRegistrar()->sipProxyPassword,
			//				getSoundReceiver()->getSocket()->get_port(),
			//getP2TDialog()->getRTPPort(),
			//                              getDialog()->getDialogConfig().localMediaPort, 
			/*getDialogConfig().inherited->codecs,*/
			getDialogConfig()->inherited->transport);

	inv->getHeaderValueFrom()->setParameter("tag",dialogState.localTag);

//	mdbg << "SipDialogP2Tuser::sendInvite(): sending INVITE to transaction"<<end;
#ifndef _MSC_VER
	ts.save( INVITE_END );
#endif
	//add p2t stuff to the INVITE message
	modifyP2TInvite(inv);
	
	MRef<SipMessage*> pref(*inv);
        SipSMCommand cmd(pref, SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand(cmd);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
	setLastInvite(inv);

}


#ifdef NEVERDEFINED_ERSADFS
void SipDialogP2Tuser::sendAck(string branch){
	//	mdbg << "ERROR: SipDialogP2Tuser::sendAck() UNIMPLEMENTED" << end;
	assert( !lastResponse.isNull());
	SipAck *ack = new SipAck(
			branch, 
			*lastResponse,
			getDialogConfig()->uri_foreign,
			getDialogConfig()->inherited->sipIdentity->getSipRegistrar()->sipProxyIpAddr->getString());
	//TODO:
	//	ack.add_header( new SipHeaderRoute(getDialog()->getRouteSet() ) );
//	mdbg << "SipDialogP2Tuser:sendAck(): sending ACK directly to remote" << end;

	//	if(socket == NULL){
	// No StreamSocket, create one or use UDP
//	Socket *sock=NULL;
	
	if(getDialogConfig()->proxyConnection == NULL){
		getDialogConfig()->inherited->sipTransport->sendMessage(ack,
				*(getDialogConfig()->inherited->sipIdentity->getSipRegistrar()->sipProxyIpAddr), //*toaddr,
				getDialogConfig()->inherited->proxyPort, //port, 
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

void SipDialogP2Tuser::sendBye(const string &branch, int bye_seq_no){

	//string tmp = getDialogConfig().inherited->userUri;
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
			//getDialogConfig().inherited->userUri,
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

void SipDialogP2Tuser::sendCancel(const string &branch){
	assert( !lastInvite.isNull());
	MRef<SipCancel*> cancel = new SipCancel(
			branch,
			lastInvite,
			dialogState.remoteUri,
			//getDialogConfig().inherited->userUri,
			getDialogConfig()->inherited->sipIdentity->getSipUri(),
			getDialogConfig()->inherited->sipIdentity->getSipRegistrar()->sipProxyIpAddr->getString()///,
			///localCalled
			);

	cancel->getHeaderValueFrom()->setParameter("tag",dialogState.localTag);
	cancel->getHeaderValueTo()->setParameter("tag",dialogState.remoteTag);

        MRef<SipMessage*> pref(*cancel);
        SipSMCommand cmd( pref, SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand( cmd );
	getDialogContainer()->enqueueCommand( cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE );

}

void SipDialogP2Tuser::sendInviteOk(const string &branch){
	MRef<SipResponse*> ok= new SipResponse(branch, 200,"OK", MRef<SipMessage*>(*getLastInvite()));	
	ok->getHeaderValueTo()->setParameter("tag",dialogState.localTag);


#ifdef OLD_MEDIA
	int32_t codecmatch = ((SdpPacket*)*getLastInvite()->getContent())->getCodecMatch(getDialogConfig()->inherited->codecs);
	vector<Codec *> orderedcodecs = getDialogConfig()->inherited->codecs;
	for (unsigned i=0; i<orderedcodecs.size(); i++)
		if (orderedcodecs[i]->getSdpMediaType()==codecmatch){
			Codec *tmp = orderedcodecs[0];
			orderedcodecs[0]=orderedcodecs[i];
			orderedcodecs[i]=tmp;
		}

	//getSoundSender()->setCodec(orderedcodecs[0]);
 	myCodec=orderedcodecs[0];
	
	if (getDialogConfig()->inherited->secured && (getDialogConfig()->inherited->ka_type & KEY_MGMT_METHOD_MIKEY))
//		ok->setContent( new SdpPacket(getDialogConfig().inherited->localIpString, 
		ok->setContent( new SdpPacket(getDialogConfig()->inherited->externalContactIP, 
//					getSoundReceiver()->getSocket()->get_port(), 
					getP2TDialog()->getRTPPort(), 
					orderedcodecs, 
					key_mgmt) );
	else
//		ok->setContent( new SdpPacket(getDialogConfig().inherited->localIpString, 
		ok->setContent( new SdpPacket(getDialogConfig()->inherited->externalContactIP, 
//					getSoundReceiver()->getSocket()->get_port(), 
					getP2TDialog()->getRTPPort(),
					orderedcodecs) );

#endif
//	setLastResponse(ok);
        MRef<SipMessage*> pref(*ok);
        SipSMCommand cmd( pref, SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand(cmd);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}

void SipDialogP2Tuser::sendByeOk(MRef<SipBye*> bye, const string &branch){
	MRef<SipResponse*> ok= new SipResponse( branch, 200,"OK", MRef<SipMessage*>(*bye) );
	ok->getHeaderValueTo()->setParameter("tag",dialogState.localTag);

//	setLastResponse(ok);
        MRef<SipMessage*> pref(*ok);
        SipSMCommand cmd( pref, SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand(cmd);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}

void SipDialogP2Tuser::sendReject(const string &branch){
	MRef<SipResponse*> ringing = new SipResponse(branch,486,"Temporary unavailable", MRef<SipMessage*>(*getLastInvite()));	
	ringing->getHeaderValueTo()->setParameter("tag",dialogState.localTag);
//	setLastResponse(ringing);
        MRef<SipMessage*> pref(*ringing);
        SipSMCommand cmd( pref,SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand(cmd);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}

void SipDialogP2Tuser::sendRinging(const string &branch){
	MRef<SipResponse*> ringing = new SipResponse(branch,180,"Ringing", MRef<SipMessage*>(*getLastInvite()));	
	ringing->getHeaderValueTo()->setParameter("tag",dialogState.localTag);
//	setLastResponse(ringing);
        MRef<SipMessage*> pref(*ringing);
        SipSMCommand cmd( pref, SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand(cmd);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}

void SipDialogP2Tuser::sendNotAcceptable(const string &branch){
	MRef<SipResponse*> not_acceptable = new SipResponse(branch,606,"Not Acceptable", MRef<SipMessage*>(*getLastInvite()));	
	not_acceptable->getHeaderValueTo()->setParameter("tag",dialogState.localTag);
//	setLastResponse(not_acceptable);
        MRef<SipMessage*> pref(*not_acceptable);
        SipSMCommand cmd( pref, SipSMCommand::TU, SipSMCommand::transaction);
//	handleCommand(cmd);
	getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}


bool SipDialogP2Tuser::handleCommand(const SipSMCommand &c){
	mdbg << "SipDialogP2Tuser::handleCommand got "<< c << end;

	if (c.getType()==SipSMCommand::COMMAND_PACKET  && getCallId().length()>0){
		if (c.getCommandPacket()->getCallId()!=getCallId()){
			return false;
		}
		if (c.getType()!=SipSMCommand::COMMAND_PACKET && 
				c.getCommandPacket()->getCSeq()!= dialogState.seqNo){
			return false;
		}
	
	}
	
	//check callID also for CommandString
	if(c.getType()==SipSMCommand::COMMAND_STRING){
		if (c.getCommandString().getDestinationId()!= getCallId())
			return false;
	}
	
//	if (c.getType()!=SipSMCommand::COMMAND_PACKET && 
//			c.getCommandPacket()->getCSeq()!= command_seq_no)
//		return false;
	
//	mdbg << "SipDialogP2Tuser::handlePacket() got "<< c << end;
	return SipDialog::handleCommand(c);
}

MRef<SipInvite*> SipDialogP2Tuser::getLastInvite(){
    return lastInvite;
}

void SipDialogP2Tuser::setLastInvite(MRef<SipInvite*> i){ 
    lastInvite = i; 
}

MRef<LogEntry *> SipDialogP2Tuser::getLogEntry(){
	return logEntry;
}

void SipDialogP2Tuser::setLogEntry( MRef<LogEntry *> logEntry ){
	this->logEntry = logEntry;
}

void SipDialogP2Tuser::modifyP2TInvite(MRef<SipInvite*>inv){
	//Add Accept-Contact Header
	inv->set_P2T();
		
	//Add SDP Session Level Attributes
	assert(dynamic_cast<SdpPacket*>(*inv->getContent())!=NULL);
	MRef<SdpPacket*> sdp = (SdpPacket*)*inv->getContent();
	sdp->setSessionLevelAttribute("p2tGroupListServer", getDialogConfig()->inherited->externalContactIP + ":" + itoa(getPhoneConfig()->p2tGroupListServerPort));
	sdp->setSessionLevelAttribute("p2tGroupIdentity", getP2TDialog()->getGroupList()->getGroupIdentity());
	sdp->setSessionLevelAttribute("p2tGroupListProt","http/xml");	
}

void SipDialogP2Tuser::reportSipDialogP2T(int status){
	getP2TDialog()->modifyUser(dialogState.remoteUri, myIp, myRTPport, myRTCPport, myCodec, getCallId());
}

