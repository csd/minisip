/*
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2004, 2006
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *	    Joachim Orrblad <joachim[at]orrblad.com>
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/

/* Name
 * 	SipDialogVoipServer100rel.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/
#include<libminisip/sip/SipDialogVoipServer100rel.h>

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

 TODO: do not remove the old start_ringing stansaction, but instead
 place our before the old one.
 
 The "dotted" states are inherited from superclasses.

 
                 +. . . . . . . .+
                 .               .
                 .     start     .
                 .               .
                 +. . . . . . . .+
                         | 
                         | INVITE && supports 100rel
		         | a4001:transIR; send 183
                         V
                 +---------------+
                 |               |-----+
                 |    100rel     |     | 1xxResendTimer
                 |               |<----+ a4003: resend 183
                 +---------------+
                         |
                         | PRACK
                         | a4002: send 200; send 180
			 V
                 +. . . . . . . .+
                 .               .
                 .    ringing    .
                 .               .
                 +. . . . . . . .+
                         
*/


bool SipDialogVoipServer100rel::a4001_start_100rel_100relINVITE( const SipSMCommand &command)
{
	if (transitionMatch("INVITE", 
				command, 
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer)){
		
		MRef<SipRequest*> inv = (SipRequest *)*command.getCommandPacket();
		if (!inv->supported("100rel"))
			return false;
		
		_1xxResendTimer=sipStack->getTimers()->getA();
		requestTimeout(_1xxResendTimer,"1xxResendTimer");
		
		setLastInvite(inv);
		dialogState.updateState( getLastInvite() );
		
		string peerUri = dialogState.remoteUri;
		
		getDialogConfig()->inherited->sipIdentity->setSipUri(command.getCommandPacket()->getHeaderValueTo()->getUri().getUserIpString());
		
		if(!sortMIME(*command.getCommandPacket()->getContent(), peerUri, 10)){
			merr << "No MIME match" << end;
			return false;
		}
		
		//The transaction layer must receive to this message as
		//well. We re-post it with the transaction layer
		//as destination
//		SipSMCommand cmd(command);
//		cmd.setDestination(SipSMCommand::transaction);
//		getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);

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
		
/*		CommandString cmdstr(dialogState.callId, 
				SipCommandString::incoming_available, 
				dialogState.remoteUri, 
				(getMediaSession()->isSecure()?"secure":"unprotected")
				);
		getDialogContainer()->getCallback()->handleCommand("gui", cmdstr );
*/		
		sendSessionProgress(inv->getDestinationBranch());
		
//		if( getDialogConfig()->inherited->autoAnswer ){
//			CommandString accept( dialogState.callId, SipCommandString::accept_invite );
//			SipSMCommand sipcmd(accept, SipSMCommand::remote, SipSMCommand::TU);
//			getDialogContainer()->enqueueCommand(sipcmd,HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
//		}
		return true;
	}else{
		return false;
	}
}

bool SipDialogVoipServer100rel::a4002_100rel_ringing_PRACK( const SipSMCommand &command)
{
	if (transitionMatch("PRACK", 
				command, 
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer)){
		
		MRef<SipRequest*> prack = (SipRequest *)*command.getCommandPacket();
		dialogState.updateState( prack );

		dialogState.seqNo++;
		
		sendPrackOk(prack->getDestinationBranch(), *prack);
		
		CommandString cmdstr(dialogState.callId, 
				SipCommandString::incoming_available, 
				dialogState.remoteUri, 
				(getMediaSession()->isSecure()?"secure":"unprotected")
				);
		sipStack->getCallback()->handleCommand("gui", cmdstr );

		sendRinging(getLastInvite()->getDestinationBranch(),false); //we don't do th 180 reliably, only the 183
		
		return true;
	}else{
		return false;
	}
}

bool SipDialogVoipServer100rel::a4003_100rel_100rel_1xxResendTimer( const SipSMCommand &command)
{
	if (transitionMatch(command, 
				"1xxResendTimer", 
				SipSMCommand::dialog_layer, 
				SipSMCommand::dialog_layer) ){
		resendSessionProgress();
		_1xxResendTimer *=2;
		requestTimeout(_1xxResendTimer,"1xxResendTimer");
		return true;
	}else{
		return false;
	}
}

void SipDialogVoipServer100rel::setUpStateMachine(){

	State<SipSMCommand,string> *s_100rel=new State<SipSMCommand,string>(this,"100rel");
	addState(s_100rel);

	MRef<State<SipSMCommand,string> *> s_start= getState("start");
	MRef<State<SipSMCommand,string> *> s_ringing= getState("ringing");


        bool success = s_start->removeTransition("transition_start_ringing_INVITE");

	
	
        if (!success){
                merr << "ERROR: Could not remove transition from state machine in SipTransactionInviteServerUA (BUGBUG!!)"<<endl;
		massert(0==1);
        }


	new StateTransition<SipSMCommand,string>(this, "transition_start_100rel_100relINVITE",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipServer100rel::a4001_start_100rel_100relINVITE,
			s_start, s_100rel);
	
	new StateTransition<SipSMCommand,string>(this, "transition_100rel_ringing_PRACK",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipServer100rel::a4002_100rel_ringing_PRACK,
			s_100rel, s_ringing);
	
	new StateTransition<SipSMCommand,string>(this, "transition_100rel_100rel_1xxResendTimer",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogVoipServer100rel::a4003_100rel_100rel_1xxResendTimer,
			s_100rel, s_100rel);


	// We need to put the "fallback" transition last in the list of
	// transitions to be tried.
	MRef< StateTransition<SipSMCommand,string> *> trans = s_start->getTransition("transition_start_termwait_INVITEnothandled");
	success = s_start->removeTransition("transition_start_termwait_INVITEnothandled");
	massert(trans && success);
	s_start->register_transition(trans);
}


SipDialogVoipServer100rel::SipDialogVoipServer100rel(MRef<SipStack*> stack, MRef<SipDialogConfig*> callconfig, MRef<SipSoftPhoneConfiguration*> pconf, MRef<Session *> mediaSession, string cid) : 
		SipDialogVoipServer(stack, callconfig, pconf, mediaSession, cid) 
{
	setUpStateMachine();
}

SipDialogVoipServer100rel::~SipDialogVoipServer100rel(){	
}

void SipDialogVoipServer100rel::sendPrackOk(const string &branch, MRef<SipMessage*> prack){
	MRef<SipResponse*> ok= new SipResponse(branch, 200,"OK", prack);
	ok->getHeaderValueTo()->setParameter("tag",dialogState.localTag);
	
	MRef<SipHeaderValue *> contact = 
		new SipHeaderValueContact( 
			getDialogConfig()->inherited->sipIdentity->getSipUri(),
			getDialogConfig()->inherited->externalContactIP,
			getDialogConfig()->inherited->getLocalSipPort(phoneconf->useSTUN),
			"", getDialogConfig()->inherited->getTransport(),
			-1); //set expires to -1, we do not use it (only in register)
	ok->addHeader( new SipHeader(*contact) );
	
	MRef<SipMessage*> pref(*ok);
	SipSMCommand cmd( pref, SipSMCommand::dialog_layer, SipSMCommand::transaction_layer);
	sipStack->enqueueCommand(cmd, HIGH_PRIO_QUEUE);
}

void SipDialogVoipServer100rel::resendSessionProgress(){
	MRef<SipHeaderValueRSeq *> rseq = (SipHeaderValueRSeq*)*lastProgress->getHeaderValueNo( SIP_HEADER_TYPE_RSEQ, 0 );

	rseq->setRSeq( rseq->getRSeq() + 1 );

	SipSMCommand cmd( lastProgress, SipSMCommand::dialog_layer, SipSMCommand::transaction_layer);
	sipStack->enqueueCommand(cmd, HIGH_PRIO_QUEUE);
}

void SipDialogVoipServer100rel::sendSessionProgress(const string &branch){
	MRef<SipResponse*> progress = new SipResponse(branch,183,"Session progress", MRef<SipMessage*>(*getLastInvite()));	

	progress->addHeader(new SipHeader(new SipHeaderValueRequire("100rel"))); //EEEEEE
	
	progress->getHeaderValueTo()->setParameter("tag",dialogState.localTag);

	progress->addHeader(new SipHeader(new SipHeaderValueRSeq( rand() % (1<<31) )));
	
	MRef<SipHeaderValue *> contact = 
		new SipHeaderValueContact( 
			getDialogConfig()->inherited->sipIdentity->getSipUri(),
			getDialogConfig()->inherited->externalContactIP,
			getDialogConfig()->inherited->getLocalSipPort(phoneconf->useSTUN),
			"", getDialogConfig()->inherited->getTransport(),
			-1); //set expires to -1, we do not use it (only in register)
	progress->addHeader( new SipHeader(*contact) );

	lastProgress = *progress;

	MRef<SipMessage*> pref(*progress);
	SipSMCommand cmd( pref, SipSMCommand::dialog_layer, SipSMCommand::transaction_layer);
	sipStack->enqueueCommand(cmd, HIGH_PRIO_QUEUE);
}

