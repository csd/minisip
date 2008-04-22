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
 * 	SipDialogFileTransferServer.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include"SipDialogFileTransferServer.h"
#include<libmsip/SipHeaderFrom.h>
#include<libmsip/SipHeaderTo.h>
#include<libmsip/SipResponse.h>
#include<libmsip/SipTransitionUtils.h>
#include<libmsip/SipDialog.h>
#include<libmsip/SipCommandString.h>
#include<libminisip/signaling/sip/DefaultDialogHandler.h>
#include<libmutil/stringutils.h>
#include<libmutil/Timestamp.h>
#include<libmutil/Thread.h>
#include<libmutil/termmanip.h>
#include<libmutil/dbg.h>
#include<libmsip/SipSMCommand.h>
#include <time.h>
#include<libminisip/gui/LogEntry.h>
#include<libmsip/SipCommandString.h>
#include<libminisip/media/MediaHandler.h>
#include<libmutil/MemObject.h>
#include<libmsip/SipHeaderExpires.h>
#include<libminisip/signaling/sip/PresenceMessageContent.h>
#include<libminisip/ipprovider/SimpleIpProvider.h>

#include<libminisip/signaling/sdp/SdpHeaderA.h>
#include<libminisip/signaling/sdp/SdpHeaderM.h>

#ifdef _WIN32_WCE
#	include"../include/minisip_wce_extra_includes.h"
#endif

#include"../../subsystem_media/msrp/MSRPMessage.h"


using namespace std;

/*
 Presence dialog for user "user@domain".

      +-------------+
      |   s_start   |
      +-------------+
             | start_filetransfer_server
	     | a0: receive invite
             |
             V  
      +-------------+
      |   trying    |------------------------+
      +-------------+			     |	
      	     |				     |	 
             | 				     |
 	     | a1: 200 OK		     |
 	     |				     |
             V				     |
      +-------------+			     |
 +----|  receiving  |			     | a2: 415      		
 |    +-------------+			     |     reject whole transfer
 |	     |				     |
 |a4:	     | a3: receiving BYE request     |
 |fail       |				     |
 |report     V				     |
 |    +-------------+	     		     |
 +--->|  term_wait  |<-----------------------+
      +-------------+			
  	     | a5: no_transactions	
  	     | 
             V
      +-------------+
      | terminated  |
      +-------------+
*/

bool SipDialogFileTransferServer::a0_start_trying_receiveINVITE(const SipSMCommand &command){
	if (transitionMatch("INVITE", command, SipSMCommand::transaction_layer, SipSMCommand::dialog_layer)){
		cerr << "MAFE: GOT INVITE"<<endl;

		// ---------------------------------------------------------------
		// Ask user for file(s) transfer
		// ---------------------------------------------------------------
		
		int incomingfiles=0;
		
		lastInvite = dynamic_cast<SipRequest*>( *command.getCommandPacket() ) ;

		MRef<SipMessageContent*> content = lastInvite->getContent();
		MRef<SdpPacket *> sdp = dynamic_cast<SdpPacket*>(*content);

		CommandString ask_User(dialogState.callId, "incoming_filetransfer_accept");
		
		if (!sdp){
			//NOT SDP
		}else{
			sendTrying();

			std::vector<MRef<SdpHeader*> > header = sdp->getHeaders();
			for(int i=0; i<(int)header.size(); i++){
				
				MRef<SdpHeader*> one = header[i];
				if(one->getType() == SDP_HEADER_TYPE_M){
					MRef<SdpHeaderM*> headerM = dynamic_cast<SdpHeaderM*>(*one);
					
					int32_t fileport = headerM->getPort();
					incomingfiles++;
	
					std::list<MRef<SdpHeaderA*> > headersA = headerM->getAttributes();
					list<MRef<SdpHeaderA*> >::iterator h = headersA.begin();
					
					while(h != headersA.end()){
						if((*h)->getAttributeType()=="file-selector"){
							(*h)->getAttFromFileSelector();
							cerr<< "MAFE: Waiting for user response "<<(*h)->filename<<endl;

							ask_User["filename"+itoa(incomingfiles)]= (*h)->filename;

						}
						h++;
					}
				}
			}			
		
		}

		getSipStack()->getCallback()->handleCommand("gui", ask_User);
//		CommandString accept (dialogState.callId, SipCommandString::accept_invite);
//		SipSMCommand sipcmd (accept, SipSMCommand::dialog_layer, SipSMCommand::dialog_layer);
//		getSipStack()->enqueueCommand(sipcmd,HIGH_PRIO_QUEUE);
	
		return true;
	}else{
		return false;
	}
}

bool SipDialogFileTransferServer::a1_trying_receiving_accept(const SipSMCommand &command){
	if (transitionMatch(command,
				"accept_file_transfer",
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer)){

		MRef<SipResponse*> ok = new SipResponse(200, "OK", lastInvite);
		ok->getHeaderValueTo()-> setParameter("tag", dialogState.localTag);

		SimpleIpProvider ipp(phoneConf);
		string myIp = ipp.getLocalIp();
		cerr <<"MAFE: local ip: "<< myIp<<endl;

		string fileNameRecv;
		int fileSizeRecv;

		MRef<SdpPacket *> sdpAux = new SdpPacket("v=0\n"
				"o=bob 2890844526 2890844526 IN IP4 bobpc.example.com\n"
				"s=\n"
				"t= 0 0\n");

		MRef<SdpHeaderC *> headerC = new SdpHeaderC ("IN", "IP4", myIp);
		MRef<SdpHeader*> headerNew = dynamic_cast<SdpHeader*>(*headerC);
		sdpAux->addHeader(headerNew);

		MRef<SipMessageContent*> contentInv = lastInvite->getContent();
		MRef<SdpPacket *> lastSdp = dynamic_cast<SdpPacket*>(*contentInv);

		std::vector<MRef<SdpHeader*> > headers = lastSdp->getHeaders();
		for(int i=0; i<(int)headers.size(); i++){

			MRef<SdpHeader*> oneHeader = headers[i];
			
			if(oneHeader->getType() == SDP_HEADER_TYPE_M){
				
					sdpAux->addHeader(oneHeader);
					MRef<SdpHeaderM*> headerM = dynamic_cast<SdpHeaderM*>(*oneHeader);

					std::list<MRef<SdpHeaderA*> > headersA = headerM->getAttributes();
					list<MRef<SdpHeaderA*> >::iterator h = headersA.begin();

					while(h != headersA.end()){
						if((*h)->getAttributeType()=="file-selector"){
							(*h)->getAttFromFileSelector();
							fileNameRecv = (*h)->filename;
							fileSizeRecv =atoi(((*h)->filesizes.c_str()));
						} h++;
					}
				}
			}	
		ok->setContent(*sdpAux);

		MRef<SipMessage*>pref(*ok);
		SipSMCommand cmd(pref, SipSMCommand::dialog_layer, SipSMCommand::transaction_layer);
		getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE);	

		MRef<Thread*> rcvThread = new Thread(new MSRPReceiver(fileNameRecv, fileSizeRecv));
		return true;
	}else{
		return false;
	}
}

bool SipDialogFileTransferServer::a2_trying_termwait_rejecttransfer(const SipSMCommand &command){
	if (transitionMatch(command, 
				SipCommandString::reject_invite,
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer)){

	sendReject("");
		return true;
	}else{
		return false;
	}
}

bool SipDialogFileTransferServer::a3_receiving_termwait_BYE(const SipSMCommand &command){
	if (transitionMatch("BYE",
				command,
				SipSMCommand::transaction_layer,
				SipSMCommand::dialog_layer)){

		MRef<SipRequest*>bye = (SipRequest*) *command.getCommandPacket();
		sendByeOk(bye, "");
		return true;
	}else{
		return false;
	}

}

bool SipDialogFileTransferServer::a4_receiving_termwait_failedtransfer(const SipSMCommand &command){
	if(transitionMatch("Failed_Transfer",
				command,
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer)){
		
		sendFailureReport("");

		return true;
	}else{
		return false;
	}

}

bool SipDialogFileTransferServer::a5_termwait_terminated_notransactions(const SipSMCommand &command){
	if (transitionMatch(command, 
				SipCommandString::no_transactions,
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer) ){

		dialogState.isTerminated=true;
		
		SipSMCommand cmd( CommandString( dialogState.callId, SipCommandString::call_terminated), //FIXME: callId is ""
				  SipSMCommand::dialog_layer,
				  SipSMCommand::dispatcher);
		getSipStack()->enqueueCommand( cmd, HIGH_PRIO_QUEUE);
		return true;
	}else{
		return false;
	}
}


void SipDialogFileTransferServer::setUpStateMachine(){

	State<SipSMCommand,string> *s_start = new State<SipSMCommand,string>(this,"start");
	addState(s_start);

	State<SipSMCommand,string> *s_trying = new State<SipSMCommand,string>(this,"trying");
	addState(s_trying);
	
	State<SipSMCommand,string> *s_receiving = new State<SipSMCommand,string>(this,"receiving");
	addState(s_receiving);
		
	State<SipSMCommand,string> *s_termwait = new State<SipSMCommand,string>(this,"termwait");
	addState(s_termwait);
	
	State<SipSMCommand,string> *s_terminated = new State<SipSMCommand,string>(this,"terminated");
	addState(s_terminated);

	
	new StateTransition<SipSMCommand,string>(this, "transition_start_trying_receiveINVITE",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogFileTransferServer::a0_start_trying_receiveINVITE,
		s_start, s_trying);
	
 	new StateTransition<SipSMCommand,string>(this, "transition_trying_receiving_accept",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogFileTransferServer::a1_trying_receiving_accept,
		s_trying, s_receiving);
       
 	new StateTransition<SipSMCommand,string>(this, "transition_trying_termwait_rejecttransfer",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogFileTransferServer::a2_trying_termwait_rejecttransfer,
		s_trying, s_termwait);
 
 	new StateTransition<SipSMCommand,string>(this, "transition_receiving_termwait_BYE",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogFileTransferServer::a3_receiving_termwait_BYE,
		s_receiving, s_termwait);
	
 	new StateTransition<SipSMCommand,string>(this, "transition_receiving_termwait_failedtransfer",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogFileTransferServer::a4_receiving_termwait_failedtransfer,
		s_receiving, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_termwait_terminated_notransactions",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogFileTransferServer::a5_termwait_terminated_notransactions,
		s_termwait, s_terminated);
	
	setCurrentState(s_start);
}


SipDialogFileTransferServer::SipDialogFileTransferServer(MRef<SipStack*> stack, 
		MRef<SipSoftPhoneConfiguration* >pconf,
		MRef<SipIdentity*> ident,
		bool use_stun,
		string callId) : 
                	SipDialog(stack,ident,callId),
			phoneConf(pconf)
{
	setUpStateMachine();
}

SipDialogFileTransferServer::~SipDialogFileTransferServer(){	
}

void SipDialogFileTransferServer::sendTrying(){
	
	MRef<SipResponse*> trying = createSipResponse (lastInvite, 180, "Trying");

	sendSipMessage (*trying);

}

void SipDialogFileTransferServer::sendReject(const string &branch){
	
	MRef<SipResponse*> rejecting = new SipResponse(415, "File Transfer not accepted", lastInvite);
	rejecting-> getHeaderValueTo()-> setParameter("tag", dialogState.localTag);
	MRef<SipMessage*> pref(*rejecting);
	SipSMCommand cmd(pref, SipSMCommand::dialog_layer, SipSMCommand::transaction_layer);
	getSipStack()-> enqueueCommand(cmd, HIGH_PRIO_QUEUE);
}

void SipDialogFileTransferServer::sendFailureReport(const string &branch){

	//---------------------------------
	//Send MSRP Failure Report
	//-------------------------------

}

void SipDialogFileTransferServer::sendByeOk(MRef<SipRequest*> bye, const string &branch){
	
	MRef<SipResponse*> byeOk = new SipResponse(200, "OK", bye);
	byeOk->getHeaderValueTo()->setParameter("tag",dialogState.localTag);

	MRef<SipMessage*> pref(*byeOk);
	SipSMCommand cmd(pref, SipSMCommand::dialog_layer, SipSMCommand::transaction_layer);
	getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE);
}

