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

/* Copyright (C) 2004-2006
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/

/* Name
 * 	SipDialogresenceClient.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/
#include<config.h>

#include"SipDialogFileTransferClient.h"

#include<libmutil/massert.h>
#include<libmsip/SipHeaderFrom.h>
#include<libmsip/SipHeaderTo.h>
#include<libmsip/SipResponse.h>
#include<libmsip/SipTransitionUtils.h>
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
#include<libminisip/media/MediaHandler.h>
#include<libmutil/MemObject.h>
#include<libmutil/Thread.h>
#include<libmsip/SipHeaderSubscriptionState.h>
#include<libmsip/SipMessageContent.h>
#include<libmsip/SipMessageContentMime.h>
#include<libminisip/ipprovider/SimpleIpProvider.h>

#include<libminisip/signaling/sdp/SdpPacket.h>

#include<libminisip/signaling/sdp/SdpHeaderA.h>
#include<libminisip/signaling/sdp/SdpHeaderM.h>

//#include"../../subsystem_media/msrp/MSRPSender.h"

#include"../../subsystem_media/msrp/MSRPMessage.h"

#ifdef _WIN32_WCE
#	include"../include/minisip_wce_extra_includes.h"
#endif

using namespace std;

/*
 Presence dialog for user "user@domain".

      +-------------+
      |   start     |
      +-------------+
             |
 	     | a0: CommandString "INVITE"
 	     |     new TransactionClient(INVITE)
a1:	     V
  100 +-------------+
 +----|   trying    |--------------+
 |    +-------------+              |
 |      ^    |                     |
 |      |    | a2: 200 OK          |  
 +------+    |                     |
             V                     | 
      +-------------+              |
 +----| transfering |		   | a4: 4XX message	
 |    +-------------+   	   |     sender rejects
 |           |          	   |     all files
 |a3: transp | a5: MSRPDONE        |
 |    error  |     "BYE req"       | 
 |           V 			   |
 |    +-------------+		   |
 +--->|  termwait   |<-------------+
      +-------------+		   
             |			   
	     | a6: no_transactions 
	     |			   
	     V			   
      +-------------+		  
      | terminated  |
      +-------------+
      
*/



bool SipDialogFileTransferClient::a0_start_trying_invite(const SipSMCommand &command){

	if (transitionMatch(command, 
				"start_filetransfer",
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer)){

		cerr << "MAFE: State machine got start command - will send INVITE"<<endl;
		dialogState.remoteUri = command.getCommandString().getParam();
	
		MRef<SipRequest*> inv;
		MRef<Session*> mediaSession;
		
		SimpleIpProvider ipp(phoneConf);
		string myIp = ipp.getLocalIp();
		cerr<<"MAFE: local Ip in client: "<<myIp<<endl;

		cerr <<"Remote uri: "<<dialogState.remoteUri<<endl;
		cerr <<"Contact uri: "<<getDialogConfig()->getContactUri(useSTUN)<<endl;
		
		inv = SipRequest::createSipMessageInvite(
				//branch,
				dialogState.callId,
				SipUri(dialogState.remoteUri),
				getDialogConfig()->sipIdentity->getSipUri(),
				getDialogConfig()->getContactUri(useSTUN),
				dialogState.seqNo,
				getSipStack() );
		
		inv->getHeaderValueFrom()->setParameter("tag",dialogState.localTag);
		
		MRef<SdpPacket *> sdp = new SdpPacket("v=0\n"
				"o=alice 2890844526 2890844526 IN IP4 host.atlanta.example.com\n");
				//"m=message 3333 TCP/MSRP *\n"
				//"a=sendonly\n"
				//"a=path:msrp://atlanta.example.com:7654/jshAwe:tcp\n");
				//"a=file-selector:name:\"biggerfile.txt\" type:doc/txt size:32349 hash:sha-1:722EDI65589GRF21\n");
				//"m=message 7655 TCP/MSRP *\n"
				//"a=path:msrp://atlanta.example.com:7655/pwnSwe:tcp\n"
				//"a=file-selector:name:\"My cool video.mpg\" type:video/mpg size:1232349 hash:sha-1:74IEJFRRI566591\n");

		cerr <<"MAFE: files: \n";
		string name;
		int i=0;
		
		//while( (name=command.getCommandString().get("filename"+itoa(i++))," ") != "" ){ *****MULTIPLE FILE TRANSFER*******
		
		name=command.getCommandString().get("filename"+itoa(i)," ");
		cerr << "    "<< name <<endl;

		MRef<SdpHeaderM *> headerM = new SdpHeaderM("message", 3333, 1, "TCP/MSRP *");
		MRef<SdpHeaderA *> headerA = new SdpHeaderA("a=file-selector:name:\""+name+"\" type:doc/txt size:3535 hash:sha-1:338448rfhd3\n");
		headerM->addAttribute(*headerA);
		MRef<SdpHeader *> headerMnew = dynamic_cast<SdpHeader*>(*headerM);
		sdp->addHeader(*headerMnew);

		cerr << "MAFE: end of file list"<<endl;
	
		MRef<SdpHeaderC *> headerC = new SdpHeaderC ("IN", "IP4", myIp);
		MRef<SdpHeader *> headernew = dynamic_cast<SdpHeader*>(*headerC);
		sdp->addHeader(headernew);		
			
		inv->setContent( *sdp );

		MRef<SipMessage*>pktr(*inv);
		
		cerr << "The packet is: "<<endl<<pktr->getString()<<endl;
		SipSMCommand scmd(
			pktr,
			SipSMCommand::dialog_layer,
			SipSMCommand::transaction_layer
			);

		getSipStack()->enqueueCommand(scmd, HIGH_PRIO_QUEUE);
		lastInvite=*inv;

		return true;
	}else{
		return false;
	}
}

bool SipDialogFileTransferClient::a1_trying_trying_100(const SipSMCommand &command){

	if(transitionMatchSipResponse("INVITE", command, SipSMCommand::transaction_layer, SipSMCommand::dialog_layer, "1**")){
		dialogState.updateState( MRef<SipResponse*>((SipResponse *)*command.getCommandPacket()) );

		MRef<SipResponse*> resp = (SipResponse*)*command.getCommandPacket();
		/*if ( resp->requires("100rel") && resp->getStatusCode() !=100){
			if( !handleRel1xx ( resp ) ){
				return true;
			}

			sendPrack(resp);
		}*/
		return true;
	}else{
		return false;
	}

}

bool SipDialogFileTransferClient::a2_trying_transfering_200OK(const SipSMCommand &command){

	if (transitionMatch(SipResponse::type, command, SipSMCommand::transaction_layer, SipSMCommand::dialog_layer, "2**")){
		MRef<SipResponse*> resp(  (SipResponse*)*command.getCommandPacket() );
		dialogState.remoteTag = command.getCommandPacket()->getHeaderValueTo()->getParameter("tag");

		int filesToSend=0;
		string serveraddr;
		//resp = command.getCommandPacket();

		MRef<SipMessageContent *> respContent = resp->getContent();
		MRef<SdpPacket *> respSdp = dynamic_cast<SdpPacket *>(*respContent);

		if(!respSdp){
			//NOT SDP
		}else{
			std::vector<MRef<SdpHeader *> > respHeader = respSdp->getHeaders();

			for(int i=0; i<(int)respHeader.size(); i++){
				
				MRef<SdpHeader*> one = respHeader[i];
				
				if(one->getType() == SDP_HEADER_TYPE_C){
					MRef<SdpHeaderC*> respHeaderC = dynamic_cast<SdpHeaderC*>(*one);
					serveraddr = respHeaderC->getAddr();
				}

				else if(one->getType() == SDP_HEADER_TYPE_M){
					MRef<SdpHeaderM*> respHeaderM = dynamic_cast<SdpHeaderM*>(*one);

					int32_t acceptedPort = respHeaderM->getPort();
											
					std::list<MRef<SdpHeaderA *> > respHeadersA = respHeaderM->getAttributes();
					list<MRef<SdpHeaderA *> >::iterator h = respHeadersA.begin();
								
					while(h != respHeadersA.end()){
						if((*h)->getAttributeType() == "file-selector"){
							(*h)->getAttFromFileSelector();
							if(acceptedPort == 0){
								//file rejected
								cerr<<"file: "<<(*h)->filename<<" was rejected for remote user"<<endl;
							}else{
								MRef<SipHeaderValueFrom*> from = resp->getHeaderValueFrom();
			
								//msrp = new MSRPSender...
								//MRef<Thread .... . .. new Thread(msrp);
								MRef<Thread*> rpThread = new Thread(new MSRPSender(serveraddr, acceptedPort, (*h)->filename,getSipStack(), dialogState.callId));
							}	
					}else;
					h++;
					}
				}else;	
			}
		}
		sendAck();
		return true;
	}else{
		return false;
	} 
}

bool SipDialogFileTransferClient::a3_transfering_termwait_transperror(const SipSMCommand &command){
	if (transitionMatch(command, 
				SipCommandString::transport_error,
				SipSMCommand::transaction_layer,
				SipSMCommand::dialog_layer )){
		
		CommandString cmdstr(dialogState.callId, SipCommandString::transport_error);
		getSipStack()->getCallback()->handleCommand("gui",cmdstr);

		return true;
	}else{
		return false;
	}

}

bool SipDialogFileTransferClient::a4_trying_termwait_4XX(const SipSMCommand &command){
	
	if (transitionMatch(SipResponse::type, command, SipSMCommand::transaction_layer, SipSMCommand::dialog_layer, "4**")){
	
	
		cerr<< "Remote user is rejecting whole transfer"<<endl;
		
	return true;
	}else{
		return false;
	}
}


bool SipDialogFileTransferClient::a5_transfering_termwait_MSRPDONE(const SipSMCommand &command){
	if (transitionMatch(command, 
				"MSRP_DONE",
				SipSMCommand::dialog_layer, 
				SipSMCommand::dialog_layer)){

		sendBye("");
		CommandString transfer_done(dialogState.callId, "file_transfer_done");
		getSipStack()->getCallback()->handleCommand("gui", transfer_done);
/*

   When you create the sender/receiver, pass it the SipStack reference:

      new MSRPSender(...,sipStack, string callId);

   Then in the code you can send commands to the stack by doing
    CommandString cmd(callId, "MSRP_DONE");
    sipStack->handleCommand(cmd);

    To tell msrp, you can call a method in msrp from the sip code:
       

*/

		return true;
	}else{
		return false;
	}
}


bool SipDialogFileTransferClient::a6_termwait_terminated_notransactions(const SipSMCommand &command){
	if (transitionMatch(command, 
				SipCommandString::no_transactions,
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer) ){

		dialogState.isTerminated=true;

		SipSMCommand cmd( CommandString( dialogState.callId, SipCommandString::call_terminated),
				  SipSMCommand::dialog_layer,
				  SipSMCommand::dispatcher);
		getSipStack()->enqueueCommand( cmd, HIGH_PRIO_QUEUE );
		return true;
	}else{
		return false;
	}
}


void SipDialogFileTransferClient::setUpStateMachine(){

	State<SipSMCommand,string> *s_start = new State<SipSMCommand,string>(this,"start");
	addState(s_start);

	State<SipSMCommand,string> *s_trying = new State<SipSMCommand,string>(this,"trying");
	addState(s_trying);

	State<SipSMCommand,string> *s_transfering = new State<SipSMCommand,string>(this,"transfering");
	addState(s_transfering);

	State<SipSMCommand,string> *s_termwait=new State<SipSMCommand,string>(this,"termwait");
	addState(s_termwait);
	
	State<SipSMCommand,string> *s_terminated=new State<SipSMCommand,string>(this,"terminated");
	addState(s_terminated);

	
	new StateTransition<SipSMCommand,string>(this, "transition_start_trying_invite",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogFileTransferClient::a0_start_trying_invite,
		s_start, s_trying);
	 	
	new StateTransition<SipSMCommand,string>(this, "transition_trying_trying_100",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogFileTransferClient::a1_trying_trying_100,
		s_trying, s_trying);
	
	new StateTransition<SipSMCommand,string>(this, "transition_trying_transfering_200OK",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogFileTransferClient::a2_trying_transfering_200OK,
		s_trying, s_transfering);
       
 	new StateTransition<SipSMCommand,string>(this, "transition_transfering_termwait_transperror",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogFileTransferClient::a3_transfering_termwait_transperror,
		s_transfering, s_termwait);
 
 	new StateTransition<SipSMCommand,string>(this, "transition_trying_termwait_4XX",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogFileTransferClient::a4_trying_termwait_4XX,
		s_trying, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_transfering_termwait_MSRPDONE",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogFileTransferClient::a5_transfering_termwait_MSRPDONE,
		s_transfering, s_termwait);

	new StateTransition<SipSMCommand,string>(this, "transition_termwait_terminated_notransactions",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogFileTransferClient::a6_termwait_terminated_notransactions,
		s_termwait, s_terminated);

	setCurrentState(s_start);
}


SipDialogFileTransferClient::SipDialogFileTransferClient(MRef<SipStack*> stack, 
		MRef<SipSoftPhoneConfiguration* >pconf,
		MRef<SipIdentity*> ident,
		bool use_stun) : 
                	SipDialog(stack,ident,""),
			useSTUN(use_stun),
			phoneConf(pconf)
{
	setUpStateMachine();
}

SipDialogFileTransferClient::~SipDialogFileTransferClient(){	
}

void SipDialogFileTransferClient::sendAck(){

	MRef<SipRequest*> lastInvite1 = dynamic_cast<SipRequest*>(*lastInvite);
	MRef<SipRequest*> ack = createSipMessageAck(lastInvite1);

	SipSMCommand scmd(
			*ack,
			SipSMCommand::dialog_layer,
			SipSMCommand::transport_layer);

	getSipStack()->enqueueCommand(scmd, HIGH_PRIO_QUEUE);
}

void SipDialogFileTransferClient::sendBye(const string &branch){

	MRef<SipRequest*>bye=createSipMessageBye();
	MRef<SipMessage*>pref(*bye);

	SipSMCommand cmd(pref, SipSMCommand::dialog_layer, SipSMCommand::transaction_layer);
	getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE);
}
