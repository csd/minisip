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

#include"DefaultDialogHandler.h"
#include<libmnetutil/IP4Address.h>
#include<libmnetutil/NetworkException.h>
#include<libmsip/SipDialogRegister.h>
#include<libmsip/SipDialogContainer.h>
#include"SipDialogVoip.h"
#include<libmsip/SipMessage.h>
#include<libmsip/SipIMMessage.h>
#include<libmsip/SipMessageContentIM.h>
#include<libmsip/SipMessageTransport.h>
#include<libmsip/SipCommandString.h>
#include<libmsip/SipTransactionServer.h>
#include<libmsip/SipTransactionClient.h>
#include"../p2t/SipDialogP2T.h"
#include"../p2t/SipDialogP2Tuser.h"
#include"../mediahandler/MediaHandler.h"




#include<libmutil/dbg.h>


DefaultDialogHandler::DefaultDialogHandler(MRef<SipDialogContainer*> dContainer, 
            SipDialogConfig &conf,
	    MRef<SipSoftPhoneConfiguration*> pconf,
	    MRef<MediaHandler *>mediaHandler): 
                SipDialog(dContainer, conf, pconf->timeoutProvider),
		phoneconf(pconf),
		mediaHandler(mediaHandler)
{
	callId = string("DCH_")+itoa(rand())+"@"+getDialogConfig().inherited.externalContactIP;
	//Initialize GroupListServer
	grpListServer=NULL;
}

string DefaultDialogHandler::getName(){
	return "DefaultDialogHandler";
}

bool DefaultDialogHandler::handleCommand(const SipSMCommand &command){
	mdbg << "DefaultDialogHandler: got command "<< command << end;
	
	if (command.getType()==SipSMCommand::COMMAND_PACKET){
		if (command.getSource()==SipSMCommand::remote && command.getDispatchCount()>=2){ // this is the packets second run of handling.
			merr << "DefaultCallHandler::handleCommand: Detected dispatched already - sending 481"<< end;

			//FIXME: Check what branch parameter to send.
			MRef<SipResponse*> no_call= new SipResponse("nobranch", 481,"Call Leg/Transaction Does Not Exist", MRef<SipMessage*>(*command.getCommandPacket()));
			MRef<SipMessage*> pref(*no_call);

			Socket *snull=NULL;
			getDialogConfig().inherited.sipTransport->sendMessage(pref,
					*(getDialogConfig().inherited.sipIdentity->sipProxy.sipProxyIpAddr), //*toaddr,
					getDialogConfig().inherited.sipIdentity->sipProxy.sipProxyPort, //port,
					//                              sock, //(Socket *)NULL, //socket,
					snull,
					string(""), //branch
					getDialogConfig().inherited.transport
					);

			return true;
		}

		if (command.getSource()==SipSMCommand::remote && command.getDispatchCount()>=2){ // this is the packets second run of handling.
			cerr << "WARNING: INTERNAL ERROR: command was not handled (dispatched flag indication)"<<endl;
			return true;
		}
		
		if (command.getSource()==SipSMCommand::remote && command.getCommandPacket()->getType()==SipInvite::type){
			
//			mdbg << "DefaultDialogHandler:: creating new SipDialogVoip" << end;
//			MRef<SipCommonConfig*> ref(new SipCommonConfig(phoneconf->inherited));
//			SipDialogConfig callConf(ref);
			
			//type casting
			MRef<SipMessage*> pack = command.getCommandPacket();
			MRef<SipInvite*> inv = MRef<SipInvite*>((SipInvite*)*pack);
			 
			//check if it's a regular INVITE or a P2T INVITE
			if(inv->is_P2T()) {
				inviteP2Treceived(command);	
			}
			//start SipDialogVoIP
			else{
#ifdef DEBUG_OUTPUT			
				mdbg << "DefaultDialogHandler:: creating new SipDialogVoip" << end;
#endif			
				// get a session from the mediaHandler
				MRef<Session *> mediaSession = 
					mediaHandler->createSession(
					phoneconf->securityConfig );

				SipDialogConfig callConf(phoneconf->inherited);
				MRef<SipDialogVoip*> voipCall = new SipDialogVoip(getDialogContainer(), callConf, phoneconf, mediaSession);
#ifdef MINISIP_MEMDEBUG
				voipCall.setUser("DefaultDialogHandler");
#endif

				voipCall->setCallId(command.getCommandPacket()->getCallId());
				getDialogContainer()->addDialog(*voipCall);
			
				//voipCall->getDialogConfig().callId = command.getCommandPacket()->getCallId();
				

				SipSMCommand cmd(command);
				cmd.setSource(SipSMCommand::remote);
				cmd.setDestination(SipSMCommand::TU);
			
				//voipCall->handleCommand(command);
				getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
			}
			
			return true;
		
		}


		if (command.getSource()==SipSMCommand::remote && command.getCommandPacket()->getType()==SipIMMessage::type){
			
			MRef<SipMessage*> pack = command.getCommandPacket();
			MRef<SipIMMessage*> im = MRef<SipIMMessage*>((SipIMMessage*)*pack);
			 
#ifdef DEBUG_OUTPUT			
				mdbg << "DefaultDialogHandler:: creating new server transaction for incoming SipIMMessage" << end;
#endif			
				string branch = im->getDestinationBranch();
				
				MRef<SipTransaction*> trans = new SipTransactionServer(this, im->getCSeq(), branch, im->getCallId());
				registerTransaction(trans);
				
				SipSMCommand cmd(command);
				cmd.setSource(SipSMCommand::remote);
				cmd.setDestination(SipSMCommand::transaction);
			
				//voipCall->handleCommand(command);
				getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
			
				sendIMOk(im, trans->getBranch() );

				assert(dynamic_cast<SipMessageContentIM*>(*im->getContent())!=NULL);
				
				MRef<SipMessageContentIM*> imref = (SipMessageContentIM*)*im->getContent();

				string from =  im->getHeaderFrom()->getUri().getUserId()+"@"+ im->getHeaderFrom()->getUri().getIp();
				string to =  im->getHeaderTo()->getUri().getUserId()+"@"+ im->getHeaderTo()->getUri().getIp();

				CommandString cmdstr("", SipCommandString::incoming_im, imref->getString(), from, to );
				getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
			return true;
		
		}

		
		merr << "DefaultDialogHandler ignoring " << command.getCommandPacket()->getString() << end; 
	}
	
	if (command.getType()==SipSMCommand::COMMAND_STRING){
		if (command.getDispatchCount()>=2){
			merr << "WARNING: Command ["<< command <<"] ignored (dispatched flag indication)"<< end;
			return true;
		}


		if (command.getCommandString().getOp() == SipCommandString::outgoing_im){
			cerr << "DefaultDialogHandler: Creating SipTransactionClient for outgoing_im command"<< endl;
			int im_seq_no= requestSeqNo();
			MRef<SipTransaction*> trans = new SipTransactionClient(this, im_seq_no, callId);
			mdbg << "WWWWWWW: transaction created, branch id is <"<<trans->getBranch()<<">."<< end; 
			//cerr << "command standard arguments is <"<< command.getCommandString().getString() <<">"<< endl;
			registerTransaction(trans);
			sendIM( trans->getBranch(), command.getCommandString().getParam(), im_seq_no, command.getCommandString().getParam2() );
			return true;
		}
		
		if (command.getCommandString().getOp() == SipCommandString::proxy_register){
			
			mdbg << "DefaultCallhandler: got proxy_register: "<< command << end;
			
//			MRef<SipCommonConfig*> ref(new SipCommonConfig(phoneconf->inherited));
//			SipDialogConfig conf(ref);
			SipDialogConfig conf(phoneconf->inherited);
			cerr << "sipDomain="<< phoneconf -> inherited.sipIdentity->sipDomain<<endl;
			if (phoneconf->pstnIdentity){
				cerr << "pstnSipDomain="<< phoneconf -> pstnIdentity->sipDomain<<endl;
			}
			
			string proxyDomainArg = command.getCommandString()["proxy_domain"];
			
			if (phoneconf->pstnIdentity && (command.getCommandString().getDestinationId()=="pstn" 
					|| (proxyDomainArg!="" && proxyDomainArg==phoneconf->pstnIdentity->sipDomain))){
				conf.useIdentity( phoneconf->pstnIdentity, false);
	
			}
			MRef<SipDialogRegister*> reg(new SipDialogRegister(getDialogContainer(), conf, phoneconf->timeoutProvider ));
#ifdef MINISIP_MEMDEBUG
			reg.setUser("DefaultDialogHandler");
#endif

			getDialogContainer()->addDialog( MRef<SipDialog*>(*reg) );
			
//                        SipSMCommand cmd( CommandString(reg->getCallId(), CommandString::proxy_register),
                        SipSMCommand cmd( command.getCommandString(),
						SipSMCommand::remote, 
						SipSMCommand::TU);
			cmd.setDispatchCount(command.getDispatchCount());
			//reg->handleCommand( cmd );
			getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
			
			return true;
		}
		
		/*****
		 * P2T commands:
		 ****/

		 //start P2T Session 
		else if(command.getCommandString().getOp() == "p2tStartSession") {
				startP2TSession(command);
			return true;
		}
		 		 
		//start GroupListServer 
		else if(command.getCommandString().getOp() == "p2tStartGroupListServer") {
			grpListServer = new GroupListServer(phoneconf, 0);
			grpListServer->start();
			return true;
		}

		//stop GroupListServer 
		else if(command.getCommandString().getOp() == "p2tStopGroupListServer") {
			grpListServer->stop();
			grpListServer=NULL;
			return true;
		}
		
		//p2tSession is accepted
		else if(command.getCommandString().getOp() == "p2tSessionAccepted") {
			inviteP2Taccepted(command);
			return true;
		}
		
		//add user to a P2T Session
		else if(command.getCommandString().getOp() == "p2tAddUser") {
			
			//get SipDialogP2T			
			MRef<SipDialogP2T*>p2tDialog;
			getP2TDialog(command.getCommandString().getParam(), p2tDialog);
			
			//add user
			string user = command.getCommandString().getParam2();
			p2tDialog->getGroupList()->addUser(user);
			
			//create callConfig
			MRef<SipDialogConfig*> callConf = MRef<SipDialogConfig*>(new SipDialogConfig(phoneconf->inherited) );
			
					
			//check user uri and modify DialogConfig
			if(modifyDialogConfig(user, callConf)==false){
				p2tDialog->removeUser(user, "uri malformed", "");
				return true;
			}
		
			MRef<SipDialogP2Tuser*> p2tUserDialog = new SipDialogP2Tuser(getDialogContainer(), **callConf, phoneconf, p2tDialog);
#ifdef MINISIP_MEMDEBUG 
			p2tUserDialog.setUser("DefaultDialogHandler");
#endif
			dialogContainer->addDialog(*p2tUserDialog);
		
			//set CallId and localStarted in GroupMemberList
			p2tDialog->getGroupList()->getUser(user)->setCallId(p2tUserDialog->getCallId());
			p2tDialog->getGroupList()->getUser(user)->setLocalStarted(true);
			
			//send invite message
			CommandString inv(p2tUserDialog->getCallId(), SipCommandString::invite, user);
        		SipSMCommand cmd(SipSMCommand(inv, SipSMCommand::remote, SipSMCommand::TU));
			dialogContainer->enqueueCommand( cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE );
	
			return true;
		}
		
		merr << "DefaultDialogHandler ignoring command " << command.getCommandString().getString() << end; 
	}	
	
	return false;
}


void DefaultDialogHandler::inviteP2Treceived(const SipSMCommand &command){
	//type casting
	MRef<SipMessage*> pack = command.getCommandPacket();
	MRef<SipInvite*> inv = MRef<SipInvite*>((SipInvite*)*pack);
	
	//get the GroupList from the remote GroupListServer
	MRef<GroupList*>grpList;
	assert(dynamic_cast<SdpPacket*>(*inv->getContent())!=NULL);
	MRef<SdpPacket*> sdp = (SdpPacket*)*inv->getContent();
	string gServer = sdp->getSessionLevelAttribute("p2tGroupListServer");
	string gID = sdp->getSessionLevelAttribute("p2tGroupIdentity");
	string prot = sdp->getSessionLevelAttribute("p2tGroupListProt");
				
	//parse gServer
	string server="";
	int port=0;
	int k=0;
				
	for(k;k<gServer.size();k++){
		if(gServer[k]!=':')
			server+=gServer[k];
		else
			break;
	}
				
	for(++k;k<gServer.size();k++)
		port = (port*10) + (gServer[k]-'0');
				
	
	//download Group Member List if protocol is http/xml
	if(prot=="http/xml"){		
		MRef<GroupListClient*>client = new GroupListClient();
		grpList = client->getGroupList(gID,&server[0],port);
		client=NULL;
	}
	else{
#ifdef DEBUG_OUTPUT			
		merr << "DefaultDialogHandler:: Unknown GroupListProtocol "<<prot<< end;
#endif
		return;
	}
	
	
	//get inviting user name
	string inv_user = inv->getHeaderFrom()->getUri().getUserId()+"@"+ inv->getHeaderFrom()->getUri().getIp();	
	
	//start SipDialogP2T only, if there isn't already
	//one started with this GroupIdentity
	MRef<SipDialogP2T*>p2tDialog;
	MRef<SipDialogConfig*> callConf;
	
	if(getP2TDialog(gID, p2tDialog)==false){
		//start new SipDialogP2T
		callConf = MRef<SipDialogConfig*>(new SipDialogConfig(phoneconf->inherited) );
		p2tDialog = new SipDialogP2T(getDialogContainer(), **callConf, phoneconf); 
#ifdef MINISIP_MEMDEBUG 
		p2tDialog.setUser("DefaultDialogHandler");
#endif
		p2tDialog->setGroupList(grpList);
		//p2tDialog->getDialogConfig().callId = gID;
		p2tDialog->setCallId(gID);
		MRef<SipDialog*> dlg = *p2tDialog;
		getDialogContainer()->addDialog(dlg);
		
		//send invitation to the GUI
		CommandString cmdstr(gID, "p2tInvitation", grpList->print(), inv_user);
		getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
		
		
	}
	
	//set now correct GroupList
	grpList = p2tDialog->getGroupList();
	
	//start SipDialogP2Tuser for inviting user
	callConf = MRef<SipDialogConfig*>(new SipDialogConfig(phoneconf->inherited) );
	MRef<SipDialogP2Tuser*> p2tDialogUser = new SipDialogP2Tuser(getDialogContainer(), **callConf, phoneconf, p2tDialog);
#ifdef MINISIP_MEMDEBUG
	p2tDialogUser.setUser("DefaultDialogHandler");
#endif
	//p2tDialogUser->getDialogConfig().callId = command.getCommandPacket()->getCallId();
	p2tDialogUser->setCallId( command.getCommandPacket()->getCallId() );
	
	getDialogContainer()->addDialog(*p2tDialogUser);
	
	//user doesn't exist in grpList and will be added to it
	if(grpList->isParticipant(inv_user)==false){
		
		//check memberlist
		if(grpList->getMembership()==P2T::MEMBERSHIP_OPEN ||
			(grpList->getMembership()==P2T::MEMBERSHIP_RESTRICTED && grpList->isMember(inv_user))){
		
			//adduser to grpList
			grpList->addUser(inv_user);
			
			//set CallId in Group Member List
			grpList->getUser(inv_user)->setCallId(p2tDialogUser->getCallId());
			grpList->getUser(inv_user)->setStatus(P2T::STATUS_WAITACCEPT);
		
			SipSMCommand cmd(command);
			cmd.setSource(SipSMCommand::remote);
			cmd.setDestination(SipSMCommand::TU);
			getDialogContainer()->enqueueCommand(cmd, LOW_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
		
			//inform GUI. 
			CommandString cmds(p2tDialogUser->getCallId(), "p2tAddUser", inv_user);
			getDialogContainer()->getCallback()->sipcb_handleCommand( cmds );
		}
		//if user is not in GroupList deny it
		else {
			CommandString rej(/*p2tDialogUser->getDialogConfig().callId*/ p2tDialogUser->getCallId(), SipCommandString::reject_invite);
			SipSMCommand cmd(rej, SipSMCommand::remote, SipSMCommand::TU);
			getDialogContainer()->enqueueCommand(cmd, LOW_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
			 
		}
		
	
	}
	else{
		//there is already a call started for this user
		if(grpList->getUser(inv_user)->getCallId()!=""){
			
			//we have a collision
			if(grpList->getUser(inv_user)->getLocalStarted()==true){
			
			 //not implemented
			
			}
			
			SipSMCommand cmd(command);
			cmd.setSource(SipSMCommand::remote);
			cmd.setDestination(SipSMCommand::TU);
			getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
			
			//reject this call
			CommandString cmds(p2tDialogUser->getCallId(), SipCommandString::reject_invite);
			SipSMCommand scmd(cmds, SipSMCommand::remote, SipSMCommand::TU);
			getDialogContainer()->enqueueCommand(scmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
		}
		
		//there is no call started for this user
		else{
			
			//set CallId in Group Member List
			grpList->getUser(inv_user)->setCallId(p2tDialogUser->getCallId());
			grpList->getUser(inv_user)->setStatus(P2T::STATUS_WAITACCEPT);
			
			SipSMCommand cmd(command);
			cmd.setSource(SipSMCommand::remote);
			cmd.setDestination(SipSMCommand::TU);
			getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
			
			//user has to accept it
			CommandString cmds(p2tDialogUser->getCallId(), "p2tAddUser");
			getDialogContainer()->getCallback()->sipcb_handleCommand( cmds );

		}
	
	
	}
	
	

	


}

void DefaultDialogHandler::inviteP2Taccepted(const SipSMCommand &command){
	
	//get P2TDialog
	MRef<SipDialogP2T*> p2tDialog;
	if(getP2TDialog(command.getCommandString().getParam(), p2tDialog)==false){	
		merr<<"DefaultDialogHandler::Couldn't find SipDialogP2T!"<<end;
		return;
	}

	//start the Group List Server and add GroupList
	MRef<GroupList*> grpList = p2tDialog->getGroupList();
	grpListServer = new GroupListServer(phoneconf, 0);
	grpListServer->start();
	grpListServer->addGroupList(grpList);
	
	//send accept_invite to all waiting SipDialogP2Tuser dialogs for this session
	for(int l=0; l<grpList->getAllUser().size(); l++){
		if(grpList->getAllUser().at(l)->getStatus()==P2T::STATUS_WAITACCEPT){
			CommandString cmds(grpList->getAllUser().at(l)->getCallId(), SipCommandString::accept_invite);
			SipSMCommand scmd(cmds, SipSMCommand::remote, SipSMCommand::TU);
			getDialogContainer()->enqueueCommand(scmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
		}
	}

			
	//Start SipDialogP2Tuser for all remaining participants in the Group Member List
	MRef<SipDialogConfig*> callConf;
	string user="";
	for(int k=0; k<grpList->getAllUser().size(); k++){
		user=grpList->getAllUser().at(k)->getUri();
		
		//filter out own username
//		if(user==getDialogConfig().inherited.userUri)
		if(user==getDialogConfig().inherited.sipIdentity->getSipUri())
			continue;
			
		// filter out users that have already started
		// a dialog resp. has a callId in the grpList.
		if(grpList->getAllUser().at(k)->getCallId()!="")
			continue;
		
		//create callConfig
		callConf = MRef<SipDialogConfig*>(new SipDialogConfig(phoneconf->inherited) );	
		
		//check user uri and modify DialogConfig
		if(modifyDialogConfig(user, callConf)==false){
			p2tDialog->removeUser(user, "uri malformed", "");
			continue;
		}
		
			
				
		MRef<SipDialogP2Tuser*> p2tUserDialog = new SipDialogP2Tuser(getDialogContainer(), **callConf, phoneconf, p2tDialog);
#ifdef MINISIP_MEMDEBUG 
		p2tUserDialog.setUser("DefaultDialogHandler");
#endif
		dialogContainer->addDialog(*p2tUserDialog);
		
		//set CallId and localStarted in GroupMemberList
		grpList->getAllUser().at(k)->setCallId(p2tUserDialog->getCallId());
		grpList->getAllUser().at(k)->setLocalStarted(true);
		
		CommandString inv(p2tUserDialog->getCallId(), SipCommandString::invite, user);
        	SipSMCommand cmd(SipSMCommand(inv, SipSMCommand::remote, SipSMCommand::TU));
		dialogContainer->enqueueCommand( cmd, LOW_PRIO_QUEUE, PRIO_LAST_IN_QUEUE );
	}
	
	//Inform GUI
	CommandString cmdstr("", "p2tSessionCreated", p2tDialog->getCallId());
	getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );



}


void DefaultDialogHandler::startP2TSession(const SipSMCommand &command){
	string xml;

	//Start SipDialogP2T
	MRef<SipDialogConfig*> callConf = MRef<SipDialogConfig*>(new SipDialogConfig(phoneconf->inherited) );
	MRef<SipDialogP2T*> p2tDialog( new SipDialogP2T(getDialogContainer(), **callConf, phoneconf)); 
#ifdef MINISIP_MEMDEBUG 
	p2tDialog.setUser("DefaultDialogHandler");
#endif
		
	//Create Group Member List from the first parameter of the
	//CommandString and set the correct Group Identity
	MRef<GroupList*> grpList = new GroupList(command.getCommandString().getParam());
	grpList->setGroupIdentity(p2tDialog->getCallId());
	p2tDialog->setGroupList(grpList);
	
	//add the Group Member List to the GroupListServer
	grpListServer->addGroupList(grpList);
		
	//add SipDialogP2T to the DialogContainer
	MRef<SipDialog*> dialog = *p2tDialog;
	dialogContainer->addDialog(dialog);
		
		
	//Start SipDialogP2Tuser for all participants in the Group Member List
	string user="";
	for(int k=0; k<grpList->getAllUser().size(); k++){
		user=grpList->getAllUser().at(k)->getUri();
		
		//filter out own username
		//if(user==getDialogConfig().inherited.userUri)
		if(user==getDialogConfig().inherited.sipIdentity->getSipUri())
			continue;
		
		
		//create callConfig
		callConf = MRef<SipDialogConfig*>(new SipDialogConfig(phoneconf->inherited) );	
		
		//check user uri and modify DialogConfig
		if(modifyDialogConfig(user, callConf)==false){
			p2tDialog->removeUser(user, "uri malformed", "");
			continue;
		}
		
		MRef<SipDialogP2Tuser*> p2tUserDialog = new SipDialogP2Tuser(getDialogContainer(), **callConf, phoneconf, p2tDialog);
#ifdef MINISIP_MEMDEBUG 
		p2tUserDialog.setUser("DefaultDialogHandler");
#endif
		dialogContainer->addDialog(*p2tUserDialog);
		
		//set CallId and localStarted in GroupMemberList
		grpList->getAllUser().at(k)->setCallId(p2tUserDialog->getCallId());
		grpList->getAllUser().at(k)->setLocalStarted(true);
		
		CommandString inv(p2tUserDialog->getCallId(), SipCommandString::invite, user);
        	SipSMCommand cmd(SipSMCommand(inv, SipSMCommand::remote, SipSMCommand::TU));
		dialogContainer->enqueueCommand( cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE );
	}

	//send GUI the Group Identity
	CommandString cmdstr("", "p2tSessionCreated", p2tDialog->getCallId());
	getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
	
}

bool DefaultDialogHandler::getP2TDialog(string GroupId, MRef<SipDialogP2T*>&p2tDialog){
	bool match=false;
//	list<MRef<SipDialog*> > *dialogs = getDialogContainer()->getDispatcher()->getDialogs();
	list<MRef<SipDialog*> > dialogs = getDialogContainer()->getDispatcher()->getDialogs();
	
	for (list<MRef<SipDialog*> >::iterator i=dialogs.begin(); i!=dialogs.end(); i++){
		MRef<SipDialog*> tmpd= *i;
		if ( tmpd->getCallId()==GroupId){
			match=true;
			p2tDialog = MRef<SipDialogP2T*>((SipDialogP2T*)*tmpd);
			break;
		}
	}
	
	return match;
}

bool DefaultDialogHandler::modifyDialogConfig(string user, MRef<SipDialogConfig *> dialogConfig){
	int startAddr=0;
	if (user.substr(0,4)=="sip:")
		startAddr = 4;
	
	if (user.substr(0,4)=="sips:")
		startAddr = 5;

	bool onlydigits=true;
	for (unsigned i=0; i<user.length(); i++)
		if (user[i]<'0' || user[i]>'9')
			onlydigits=false;
	if (onlydigits && phoneconf->usePSTNProxy){
		dialogConfig->useIdentity( phoneconf->pstnIdentity, false);
	}
	
	if (user.find(":", startAddr)!=string::npos){
		if (user.find("@", startAddr)==string::npos){
			//malformed
			return false;
		}
		
		string proxy;
		string port;
		int i=startAddr;
		while (user[i]!='@')
			if (user[i]==':')
				return false; //malformed
			else
				i++;
		i++;
		while (user[i]!=':')
			proxy = proxy + user[i++];
		i++;
		while (i<user.size())
			if (user[i]<'0' || user[i]>'9')
				return false; //malformed
			else
				port = port + user[i++];
		
		int iport = atoi(port.c_str());
				
//		merr << "IN URI PARSER: Parsed port=<"<< port <<"> and proxy=<"<< proxy<<">"<<end;
		
		try{
			dialogConfig->inherited.sipIdentity->sipProxy.sipProxyIpAddr = new IP4Address(proxy);
			dialogConfig->inherited.sipIdentity->sipProxy.sipProxyPort = iport;
		}catch(HostNotFound *exc){
			merr << "Could not resolve PSTN proxy address:" << end;
			merr << exc->errorDescription();
			merr << "Will use default proxy instead" << end;
		}
	
	}
	
	return true;
}


void DefaultDialogHandler::sendIMOk(MRef<SipIMMessage*> bye, const string &branch){
        MRef<SipResponse*> ok= new SipResponse( branch, 200,"OK", MRef<SipMessage*>(*bye) );
        ok->getHeaderTo()->setTag(getDialogConfig().tag_local);

        MRef<SipMessage*> pref(*ok);
        SipSMCommand cmd( pref, SipSMCommand::TU, SipSMCommand::transaction);
        getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}


void DefaultDialogHandler::sendIM(const string &branch, string msg, int im_seq_no, string toUri){

        string tmp = getDialogConfig().inherited.sipIdentity->getSipUri();
        int i = tmp.find("@");
        assert(i!=string::npos);
        i++;
        string domain;
        for ( ; i < tmp.length() ; i++)
                domain = domain+tmp[i];

	MRef<SipIdentity *> toId = new SipIdentity(toUri);
	
//      mdbg << "///////////Creating bye with uri_foreign="<<getDialogConfig().uri_foreign << " and doman="<< domain<< end;
        MRef<SipIMMessage*> im = new SipIMMessage(
                        std::string(branch),
			std::string(callId),
			toId,
			getDialogConfig().inherited.sipIdentity,
			getDialogConfig().inherited.localUdpPort,
                        im_seq_no,
			msg
                        );

        im->getHeaderFrom()->setTag(getDialogConfig().tag_local);
        im->getHeaderTo()->setTag(getDialogConfig().tag_foreign);

        MRef<SipMessage*> pref(*im);
        SipSMCommand cmd( pref, SipSMCommand::TU, SipSMCommand::transaction);
//      handleCommand(cmd);
        getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}


