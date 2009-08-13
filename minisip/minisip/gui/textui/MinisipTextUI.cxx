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

#include<config.h>

#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include"MinisipTextUI.h"
#include<libminisip/signaling/conference/ConferenceControl.h>
#include<libminisip/media/MediaCommandString.h>
#include<libmutil/MemObject.h>
#include<libmutil/stringutils.h>
#include<libmutil/termmanip.h>

#include<libmsip/SipCommandString.h>
#include<libminisip/signaling/sip/DefaultDialogHandler.h>

#include<libminisip/signaling/conference/ConfMessageRouter.h>

using namespace std;

MinisipTextUI::MinisipTextUI(): TextUI(), autoanswer(false){
	inCall=false;
	p2tmode=false;
	
	//#ifdef DEBUG_OUTPUT
	//    mdbg << "Setting global MinisipTextUI"<< end;
	//    debugtextui=this;
	//#endif
	
	addCommand("quit");
	addCommand("filter in");
	addCommand("filter out");
	addCommand("sleep");
	addCommand("answer");
	addCommand("disable autoanswer");
	addCommand("enable autoanswer");
	addCommand("enable debugmsgs");
	addCommand("disable debugmsgs");
	addCommand("send"); //sends a file. Give file name as argument
	addCommand("receive"); //accepts incoming files
	addCommand("hangup");
	addCommand("hide packets");
	addCommand("register");
#ifdef DEBUG_OUTPUT
	addCommand("show packets");
	addCommand("show state media");
	addCommand("show state memory");
	addCommand("show state sip");
#endif
	addCommand("cmd");
	addCommand("transfer");
	addCommand("refuse");
	addCommand("conf");
	addCommand("join");
	addCommand("accept"); //accept incoming P2T Session
	addCommand("deny");   //deny incoming P2T Session
	addCommand("im");   //deny incoming P2T Session
	//    addCommand("quit");
	addCompletionCallback("call", this);
	addCompletionCallback("im", this);
	
	state="IDLE";
	setPrompt("IDLE");
	semSipReady = new Semaphore();
}

void MinisipTextUI::run(){
	guimain();
}

void MinisipTextUI::displayErrorMessage(string msg){
	displayMessage(msg, red);
}


/**
 * Required by the TextUI superclass. This method enables "dynamic"
 * auto-completion where the set of string that are being auto-completed
 * are not fixed. An example is that this method has been used to implement
 * auto-completion of all addresses in the phonebook.
 */
minilist<std::string> MinisipTextUI::textuiCompletionSuggestion(string /*match*/){
	minilist<std::string> ret;

//TODO: implement it with the new phonebook.
/*
	if (!config.isNull()){
		list<string> phonebooks = config->phonebooks;
		for (list<string>::iterator i=phonebooks.begin(); i!=phonebooks.end(); i++){

			if ((*i).size()<=7)
				return ret;;
			XMLParser *parser;

			if ((*i).substr(0,7)=="file://"){
				//              cerr << "Creating XML file parser"<< endl;
				parser = new XMLFileParser((*i).substr(7));
			}else{
				continue;
			}

			string phonebook;
			int phonebooks=0;
			do{
				string q = "phonebook["+itoa(phonebooks)+"]/name";
				phonebook=parser->getValue(q,"");
				if (phonebook!=""){
					string contact;
					int contacts=0;
					do{
						string q = "phonebook["+itoa(phonebooks)+"]/contact["+itoa(contacts)+"]/name";
						contact = parser->getValue(q,"");
						if (contact!=""){
							int pops=0;
							string qbase;
							string desc;
							do{
								string qbase = "phonebook["+itoa(phonebooks)+"]/contact["+itoa(contacts)+"]/pop["+itoa(pops)+"]/";
								desc = parser->getValue(qbase+"desc","");
								if (desc!=""){
									string uri = parser->getValue(qbase+"uri","");
									ret.push_back("call "+uri);
								}
								pops++;
							}while(desc!="");
						}
						contacts++;
					}while(contact!="");
				}
				phonebooks++;
			}while(phonebook!="");
			delete parser;
		}
	}
*/
	return ret;
}

void MinisipTextUI::handleCommand(const CommandString &cmd){
#ifdef DEBUG_OUTPUT
	mdbg << FG_MAGENTA << "MinisipTextUI::handleCommand: Got "<<cmd.getString() << PLAIN <<endl;
#endif
	bool handled = false;

	if (cmd.getOp()==SipCommandString::ask_password){
		MRef<QuestionDialog*> d= new QuestionDialog;
		d->questionId = cmd.getDestinationId();
		d->questionId2 = cmd.get("realm");
		d->questions.push_back("Enter USER NAME for realm <"+d->questionId2+">");
		d->questions.push_back("Enter PASSWORD for realm <"+d->questionId2+">");
		showQuestionDialog(d);
	}

	if (cmd.getOp()=="register_ok"){
		handled=true;
		displayMessage("Register to proxy "+cmd.getParam()+" OK", green);
	}

	if (cmd.getOp()=="call_terminated"){
		handled=true;
		//we ignore this command
	}

	if (cmd.getOp()=="file_transfer_done"){
		handled=true;
		state="IDLE";
		setPrompt(state);
		displayMessage("File transfer done.",red);
		callId=""; //FIXME: should check the callId of cmd.
		inCall=false;
	}

	if (cmd.getOp()=="incoming_filetransfer_accept"){
		handled=true;
		if(state=="IDLE"){
			state="RECEIVE?";
			setPrompt(state);
			callId=cmd.getDestinationId();
			displayMessage("The file transfer from "+cmd.getParam(), blue);
		}
	}



	if (cmd.getOp()=="register_sent"){
		handled=true; // we don't need to inform the user
	}

		
	if (cmd.getOp()==SipCommandString::incoming_im){
		handled=true;
		displayMessage("IM to <"+cmd.getParam3()+"> from <"+cmd.getParam2()+">: "+cmd.getParam(), bold);
	}
	
	
	if (cmd.getOp()=="invite_ok"){
		handled=true;
		state="INCALL";
		inCall = true;
		setPrompt(state);
		displayMessage("PROGRESS: remote participant accepted the call...", blue);
		
		displayMessage("PROGRESS: Unmuting sending of sound.", blue);
		CommandString cmdstr( callId,
				MediaCommandString::set_session_sound_settings,
				"senders", "ON");
		sendCommand("media", cmdstr);
	
	}
	else if (cmd.getOp()=="security_failed"){
		handled=true;
		state="IDLE";
		setPrompt(state);
		displayMessage("Security is not handled by the receiver", red);
		inCall=false;
	}
	
	if (cmd.getOp()=="remote_ringing"){
		handled=true;
		state="REMOTE RINGING";
		setPrompt(state);
		displayMessage("PROGRESS: the remote UA is ringing...", blue);
	}
	
	if (cmd.getOp()==SipCommandString::remote_user_not_found && !p2tmode){
		handled=true;
		state="IDLE";
		setPrompt(state);
		displayMessage("User "+cmd.getParam()+" not found.",red);
		callId=""; //FIXME: should check the callId of cmd.
	}
	
	if (cmd.getOp()==SipCommandString::remote_hang_up){
		handled=true;
		state="IDLE";
		setPrompt(state);
		displayMessage("Remote user ended the call.",red);
		callId=""; //FIXME: should check the callId of cmd.
		inCall=false;
	}
	else if (cmd.getOp()==SipCommandString::remote_cancelled_invite){
		handled=true;
		state="IDLE";
		setPrompt(state);
		displayMessage("Remote user cancelled the call.",red);
		callId=""; //FIXME: should check the callId of cmd.
		inCall=false;
	}
	
	
	if (cmd.getOp()==SipCommandString::transport_error && !p2tmode){
		handled=true;
		state="IDLE";
		setPrompt(state);
		displayMessage("The call could not be completed because of a network error.", red);
		callId=""; //FIXME: should check the callId of cmd.
	}
	
	
	if (cmd.getOp()=="error_message"){
		handled=true;
		displayMessage("ERROR: "+cmd.getParam(), red);
	}
	
	if (cmd.getOp()=="remote_reject" && !p2tmode){
		handled=true;
		state="IDLE";
		setPrompt(state);
		callId="";
		displayMessage("The remote user rejected the call.", red);
	}
	
	if (cmd.getOp()==SipCommandString::incoming_available){
		handled=true;
		if(state=="IDLE"){
			state="ANSWER?";
			setPrompt(state);
			callId=cmd.getDestinationId();
			displayMessage("The incoming call from "+cmd.getParam(), blue);
		}
		else{
			displayMessage("You missed call from "+cmd.getParam(), red);
			CommandString hup(cmd.getDestinationId(), SipCommandString::reject_invite);
			sendCommand("sip",hup);
		
		}
		
	}
	if (cmd.getOp()=="conf_join_received"){
		handled=true;
		if(state=="IDLE"){
			state="ANSWER?";
			setPrompt(state);
			callId=cmd.getDestinationId();
			//addCommand("addc");
			
			
			currentcaller=cmd.getParam();
			
					//conf->setGui(this);
			string confid="";
			string users=cmd.getParam3();
			int i=0;	
			while (users[i]!=';'&&users.length()!=0 &&!(i>((int)users.length()-1))){
				confid=confid+users[i];
				i++;
			}
			string mysipuri = config->defaultIdentity->getSipUri().getUserIpString();
			users=trim(users.substr(i));
			currentconf=new ConferenceControl(mysipuri,confid,false);
			confCallback->setConferenceController(currentconf);
			displayMessage("The incoming conference call from "+cmd.getParam(), blue);
			displayMessage("The participants are "+cmd.getParam()+" "+users, blue);
			currentconfname=confid;
		}
		else{
			displayMessage("You missed call from "+cmd.getParam(), red);
			CommandString hup(cmd.getDestinationId(), SipCommandString::reject_invite);
			confCallback->guicb_handleCommand(hup);
			
		
		}
		
	}
	
	if (cmd.getOp()==SipCommandString::transfer_pending){
		handled=true;
		if(inCall){
			displayMessage( "Call transfer in progress..." );
		}
		
	}
	
	if (cmd.getOp()==SipCommandString::transfer_requested){
		handled=true;
		cerr << "TestUI got transfer_requested" << endl;
		if(inCall){
			state="TRANSFER?";
			setPrompt(state);
			displayMessage("Accept call transfer to "+cmd.getParam(), blue );
		}
		
	}
	
	if (cmd.getOp()==SipCommandString::call_transferred){
		handled=true;
		callId = cmd.getParam();
		state="INCALL";
		displayMessage("Call transferred ...");
		setPrompt(state);
	}
#ifdef P2T_SUPPORT
	
	//P2T commands
	if (cmd.getOp()=="p2tFloorGranted"){
		handled=true;
		displayMessage("Floor is granted!!!", blue);
	
		
	}else if (cmd.getOp()=="p2tFloorTaken"){
		handled=true;
		displayMessage("Floor is granted to "+cmd.getParam(),blue);
	
	}
	else if (cmd.getOp()=="p2tFloorReleased"){
		handled=true;
		displayMessage("Floor available!", blue);
		state="P2T CONNECTED";
		setPrompt(state);
	}
	else if (cmd.getOp()=="p2tFloorRevokeActive"){
		handled=true;
		displayMessage("Maximum Floortime reached. Revoking floor...", red);
		state="P2T CONNECTED";
		setPrompt(state);
	}
	
	/****
	* p2tFloorRevokePassiv
	* DestinationID: GroupIdentity
	* Param:	      user SIP URI
	* Param2:	      warning code
	* Description:   remote user revoked floor.
	****/
	else if (cmd.getOp()=="p2tFloorRevokePassiv"){
		handled=true;
		if(cmd.getParam2()=="1"){
			displayMessage("User " + cmd.getParam() + ":", bold);
			displayMessage("Please stop talking. Maximum floortime reached.", blue);
		}
		else if(cmd.getParam2()=="3"){
			displayMessage("User " + cmd.getParam() + " stopped listening!", red);
		}
	}
	
	/****
	* p2tAddUser
	* DestinationID: CallId (SipDialogP2Tuser) 
	* Param1:        SIP URI
	* Description:   a remote user wants to be added to a P2T Session. 
	*                This command is received when the SipDialogP2Tuser
	*                is in the RINGING state and waits for an accept.
	****/
	else if (cmd.getOp()=="p2tAddUser"){
		handled=true;
		if(inCall==true && p2tmode==true){
			//send automatically an accept back
			CommandString command(cmd.getDestinationId(),  SipCommandString::accept_invite);
			callback->guicb_handleCommand(command);
			
			if(grpList->isParticipant(cmd.getParam())==false){
				grpList->addUser(cmd.getParam());
				displayMessage("User " + cmd.getParam() + " added!", blue);
			}
			
		}
		else if(inCall==true && p2tmode==false){
			//do nothing, because
			//the user has first to accept or deny
			//a incoming P2T Session. The answer will
			//be sent there to this user
		}
	}
	
	/****
	* p2tRemoveUser
	* DestinationID: GroupId (SipDialogP2T) 
	* Param1:        SIP URI
	* Param2:        reason
	* Description:   a remote user wants to be added to a P2T Session. 
	*                This command is received when the SipDialogP2Tuser
	*                is in the RINGING state and waits for an accept.
	****/
	else if (cmd.getOp()=="p2tRemoveUser"){
		handled=true;
		if(inCall==true && p2tmode==true){
			displayMessage("User " + cmd.getParam() + " removed ("+ cmd.getParam2() +").", red);
			grpList->removeUser(cmd.getParam());
		}
	}
	
	/****
	* p2tModifyUser
	* DestinationID: GroupId (SipDialogP2T) 
	* Param1:        SIP URI
	* Param2:        status
	* Description:   information about a state (in the floor control)
	*                of a user.
	****/
	else if (cmd.getOp()=="p2tModifyUser"){
		handled=true;
		if(inCall==true && p2tmode==true){
			
			int status=0;
			for(uint32_t k=0;k<cmd.getParam2().size();k++) 
				status = (status*10) + (cmd.getParam2()[k]-'0');
			
			grpList->getUser(cmd.getParam())->setStatus(status);
		}
	}
	
	/****
	* p2tInvitation
	* DestinationID: GroupID (SipDialogP2T)
	* Param1:        Group Member List (XML-code)
	* Param2:        uri inviting user
	* 
	* Description:   an invitation to a P2T Session
	****/
	else if (cmd.getOp()=="p2tInvitation"){
		handled=true;
		//if already in a call, send DENY back
		if(inCall){
		
			//Close SipDialogP2T
			//CommandString cmd_term(cmd.getDestinationId(), "p2tTerminate");
			//callback->guicb_handleCommand(cmd_term);	
			
			//inform SipDialogP2Tuser
			//CommandString cmd_rej(cmd.getParam3(), SipCommandString::reject_invite);
			//callback->guicb_handleCommand(cmd_rej);
		
			//displayMessage("You missed P2T invitation from " + cmd.getParam2() +".", red);	    
		}
		//aks user to accept
		else{
			inCall=true;
			grpList = new GroupList(cmd.getParam());
			inviting_user=cmd.getParam2();
			//inviting_callId=cmd.getParam3();
			p2tGroupId=grpList->getGroupIdentity();   
			
			displayMessage(inviting_user + " invited you to a P2T Session:", blue);
			showGroupList();
			displayMessage("type 'accept' or 'deny'", blue);
			state="P2T ACCEPT?";
			setPrompt("P2T ACCEPT?");
		}
	
	}
	
	/****
	* p2tSessionCreated
	* DestinationID: 
	* Param1:        GroupId (SipDialogP2T)
	* Param2:
	* Param3:
	* Description:   the P2T Session is set up
	****/
	else if (cmd.getOp()=="p2tSessionCreated"){
		handled=true;
		p2tGroupId=cmd.getParam();
		displayMessage("P2T Session "+ p2tGroupId + " created", green);
		grpList->setGroupIdentity(p2tGroupId);
		state="P2T CONNECTED";
		setPrompt(state);
	}    
	
	else if (cmd.getOp().substr(0,3)=="p2t"){
		handled=true;
		displayMessage("Received: "+cmd.getOp(), blue);
	}
#endif

	if (autoanswer && state=="ANSWER?"){
		handled=true;
		CommandString command(callId, SipCommandString::accept_invite);
		sendCommand("sip",command);
		state="INCALL";
		setPrompt(state);
		displayMessage("Autoanswered call from "+ cmd.getParam());
		displayMessage("Unmuting sending of sound.", blue);
		CommandString cmdstr( callId,
				MediaCommandString::set_session_sound_settings,
				"senders", "ON");
		sendCommand("media",cmdstr);
	}

	if (!handled){
		displayMessage("WARNING: Gui did not handle command: "+ cmd.getString(), red );
	}
}

bool MinisipTextUI::configDialog( MRef<SipSoftPhoneConfiguration *> /*conf*/ ){
	cout << "ERROR: MinisipTextUI::configDialog is not implemented"<< endl;
	return false;
}

void MinisipTextUI::showMem(){
	string all;
	minilist<string> names = getMemObjectNames();
	for (int i=0; i<names.size();i++){
		all = all+names[i]+"\n";
	}
	displayMessage(all+itoa(getMemObjectCount())+" objects");
}

#ifdef P2T_SUPPORT

void MinisipTextUI::showGroupList(){
	if (grpList->getDescription().substr(0,5)=="ERROR"){
		displayMessage(grpList->getDescription(), red);
		return;
	}
	displayMessage("Session Parameters:", bold);
	
	if(grpList->getDescription()!="")
		displayMessage("Description:      " + grpList->getDescription());
	
	if(grpList->getGroupIdentity()!="")
		displayMessage("Group Id:         " + grpList->getGroupIdentity());
	
	if(grpList->getGroupOwner()!="")    
		displayMessage("Group Owner:      " + grpList->getGroupOwner());
		
	displayMessage("Session Type:     " + P2T::getSessionType(grpList->getSessionType()));
	displayMessage("Membership:       " + P2T::getMembership(grpList->getMembership()));   
	
	if(grpList->getMaxFloorTime()>0)
		displayMessage("Max Floor Time:   " + itoa(grpList->getMaxFloorTime()));
	
	if(grpList->getMaxParticipants()>0)
		displayMessage("Max Participants: " + itoa(grpList->getMaxParticipants()));
	
	if(grpList->getAllMember().size()>0){
		displayMessage("Members:", bold);
		for(uint32_t k=0;k<grpList->getAllMember().size();k++) {
			displayMessage(grpList->getAllMember().at(k));
		}
	}
	
	if(grpList->getAllUser().size()>0){
		displayMessage("Participants:", bold);
		for(uint32_t k=0;k<grpList->getAllUser().size();k++)
			displayMessage(grpList->getAllUser().at(k)->getUri() + " (" + 
				itoa(grpList->getAllUser().at(k)->getPriority()) + ") " +
				P2T::getStatus(grpList->getAllUser().at(k)->getStatus()));
	}
}

void MinisipTextUI::showP2TInfo(){
	displayMessage("*********************************************", blue);
	displayMessage("*          You are now in P2T-Mode          *",blue);
	displayMessage("*********************************************",blue);				
	displayMessage("The following commands are possible:", blue);
	displayMessage("add <uri>      add a user to the session",blue);
	//displayMessage("edit           set the parameters",blue);
	displayMessage("download <url> load group member list from",blue);
	displayMessage("               http server, e.g.",blue);
	displayMessage("               http://1.2.3.4:3365/grouplist.xml",blue);
	//displayMessage("load <file>    load group member list form file",blue);
	//displayMessage("save <file>    save group member list to file",blue);
	displayMessage("show           shows the group member list",blue);
	displayMessage("connect        start p2t session",blue);
	displayMessage("talk           get the floor",blue);
	displayMessage("stop           release the floor",blue);
	displayMessage("exit           leave P2T Mode",blue);
}
#endif

void MinisipTextUI::keyPressed(int key){
	switch(key){
	case '*':
		showMem();
		break;
	case '+':
		displayMessage( config->sipStack->getStackStatusDebugString() );
		break;
	}
}

void MinisipTextUI::guiExecute(const MRef<QuestionDialog*> &d){
	CommandString command(d->questionId,  
			SipCommandString::setpassword, 
			d->answers[0], 	//user
			d->answers[1]); //pass
	command["realm"]=d->questionId2;
	sendCommand("sip",command);
}

void MinisipTextUI::guiExecute(string cmd){
	string command = trim(cmd);
	string regproxy;
	bool handled = false;

/*	if (cmd=="quit"){
		exit(0);
	}
*/
	
	if (command.substr(0,8) == "register"){
		displayMessage("Registering to proxy", blue);
		regproxy = trim(command.substr(8));
		mdbg << "Regproxy=<"<<regproxy<<">"<< endl;
		CommandString command("",SipCommandString::proxy_register/*, regproxy*/);

//		if (regproxy=="pstn")
//			command.setDestinationId("pstn");

		command["proxy_domain"] = regproxy;
		sendCommand("sip",command);
		handled=true;
	}
	
	if (command == "quit"){
		running=false;
		displayMessage("Minisip is exiting...");
		handled=true;
	}

	if (command == "enable autoanswer"){
		autoanswer=true;
		displayMessage("Autoanswer is now enabled.");
		handled=true;
	}
	if (command == "disable autoanswer"){
		autoanswer=false;
		displayMessage("Autoanswer is now disabled.");
		handled=true;
	}
	if (command == "enable debugmsgs"){
		mdbg.setEnabled(true);
		displayMessage("Debug messages is now enabled.");
		handled=true;
	}
	if (command == "disable debugmsgs"){
		mdbg.setEnabled(false);
		displayMessage("Debug messages is now disabled.");
		handled=true;
	}


#ifdef DEBUG_OUTPUT
	if (command == "show packets"){
		config->sipStack->setDebugPrintPackets(true);
		displayMessage("SIP messages will be displayed on the screen", blue);
		handled=true;
	}

	if (command == "hide packets"){
		config->sipStack->setDebugPrintPackets(false);
		displayMessage("SIP messages will NOT be displayed on the screen", blue);
		handled=true;
	}

	if (command =="show state media"){
		handled=true;
		displayMessage("Media debug state:");
		CommandString print_debug("", MediaCommandString::session_debug);
		sendCommand("media",print_debug);
	}

	if (command=="show state sip"){
		handled=true;
		displayMessage("Signalling debug state:");
		displayMessage( config->sipStack->getStackStatusDebugString() );
	}

	if (command=="show state memory"){
		handled=true;
		displayMessage("Memory debug state:");
		showMem();
	}
#endif
	
	if (command.substr(0,5)=="sleep"){
		string time = trim(command.substr(5));
		int t = atoi(time.c_str());
		if (t>0)
			Thread::msleep(t*1000);
		handled=true;
	}

	if (command.substr(0,9)=="filter in"){
		string f = trim(command.substr(9));
		mdbg.include(f);
		handled=true;
	}

	if (command.substr(0,10)=="filter out"){
		string f = trim(command.substr(10));
		mdbg.exclude(f);
		handled=true;
	}

	if (command == "answer"){
		CommandString command(callId, SipCommandString::accept_invite);
		sendCommand("sip", command);
		displayMessage("A call with the most recent callId will be accepted");
		handled=true;
                inCall = true;
		displayMessage("Unmuting sending of sound.", blue);
		CommandString cmdstr( callId,
				MediaCommandString::set_session_sound_settings,
				"senders", "ON");
		sendCommand("media",cmdstr);
		

	}

	if (command == "receive"){
		CommandString c(callId, "accept_file_transfer");
		sendCommand("sip", c);
		displayMessage("Sent accept files command");
		handled=true;
                inCall = true;
		state="RECEIVEING";
	}



	if (command.substr(0,4) == "send"){
		string param = trim(command.substr(5));
		vector<string> params = split(param, true, ' ');
		if (params.size()<2){
			displayMessage("Syntax: send <uri> <file1> [<file2> ...] ");
		}else{
			string uri=params[0];
			CommandString command(callId, "start_filetransfer", uri);
			for (int f=1; f< params.size(); f++)
				command["filename"+itoa(f-1)]= params[f];
			sendCommand("sip", command);
			displayMessage("Starting file transfer...");
		}
		handled=true;
	}


	if (command == "join"){
		CommandString command(callId, SipCommandString::accept_invite, currentcaller);
		command.setParam3(currentconfname);
				//currentconf->setCallback(callback);
				//state="CONF";
				//displayMessage("	Conf. Name: "+currentconfname);
		//callback->guicb_handleConfCommand(cmd);
		confCallback->setConferenceController(currentconf);
		addCommand("addc");
		addCommand("hangupc");
		confCallback->guicb_handleConfCommand(command);
		displayMessage("A call with the most recent callId will be accepted");
		handled=true;
                inCall = true;
	}

	
	
	if (command == "accept" && state == "TRANSFER?"){
		CommandString command(callId, SipCommandString::user_transfer_accept);
		sendCommand("sip", command);
		handled=true;
	}
	
        if (command == "reject" && state == "TRANSFER?"){
		CommandString command(callId, SipCommandString::user_transfer_refuse);
		sendCommand("sip", command);
		handled=true;
	}

	if (trim(command) == "hangup"){
		CommandString hup(callId, SipCommandString::hang_up);
		sendCommand("sip", hup);
		
		state="IDLE";
		setPrompt(state);
		displayMessage("hangup");
		handled=true;
		inCall=false;
	}
	if (trim(command) == "hangupc"){
		CommandString hup("", SipCommandString::hang_up);
		hup.setParam3(currentconfname);
		confCallback->guicb_handleConfCommand(hup);
		state="IDLE";
		setPrompt(state);
		displayMessage("hangupc");
		handled=true;
		inCall=false;
		delete currentconf;
	}

	if ((command.size()>=4) && (command.substr(0,4) == "call")){
		if (command.size()>=6){
			if (state!="IDLE"){
				displayMessage("UNIMPLEMENTED - only one call at the time with this UI.", red);
			}else{
				string uri = trim(command.substr(5));
				displayMessage("Uri: "+uri);
				
				//CONTINUEHERE!!
				//callId = callback->guicb_doInvite(uri);
				CommandString invite("",SipCommandString::invite,uri);
				CommandString resp = callback->handleCommandResp("sip",invite);
				callId = resp.getDestinationId();
				
				if (callId=="malformed"){
					state="IDLE";
					setPrompt(state);
					displayMessage("The URI is not a valid SIP URI", red);
					callId="";

				}else{
					state="TRYING";
					setPrompt(state);
					displayMessage(string("Created call with id=")+callId);    
				}
			}
		}else{
			displayMessage("Usage: call <userid>");
//			displayHelp("call");
		}
		handled=true;
	}
	if ((command == "conf")){
		if (state!="IDLE"){
			displayMessage("UNIMPLEMENTED - only one call/conference at the time with this UI.", red);
		}else{
			currentconfname = itoa(rand());
			string mysipuri = config->defaultIdentity->getSipUri().getUserIpString();
			currentconf=new ConferenceControl(mysipuri,currentconfname, true);
			//conf->setGui(this);
			confCallback->setConferenceController(currentconf);
			//currentconf->setCallback(callback);
			//state="CONF";
			displayMessage("Conf. Name: "+currentconfname);
			confCallback->guicb_handleConfCommand(currentconfname);
			addCommand("addc");
			addCommand("hangupc");
			state="CONF";
			setPrompt(state);
		}
		handled=true;
	}
	
        if ((command.size()>=8) && (command.substr(0,8) == "transfer")){
		if (command.size()>=8){
			if (state!="INCALL"){
				displayMessage("Not in a call!", red);
			}else{
				string uri = trim(command.substr(9));
				CommandString transfer(callId, SipCommandString::user_transfer, uri);
				sendCommand("sip", transfer);
				
			}
		}else{
			displayMessage("Usage: transfer <userid>");
		}
		handled=true;
	}
        
	if ((command.size()>=4) && (command.substr(0,4) == "addc")){
		if (command.size()>=6){
			
				string uri = trim(command.substr(5));
				displayMessage("Conf.Uri: "+uri);
				CommandString cmd("","join",uri);
				cmd.setParam3(currentconfname);
				confCallback->guicb_handleConfCommand(cmd);
				/*callId = callback->guicb_doInvite(uri);
				if (callId=="malformed"){
					state="IDLE";
					setPrompt(state);
					displayMessage("The URI is not a valid SIP URI", red);
					callId="";

				}else{
					state="TRYING";
					setPrompt(state);
					displayMessage(string("Created call with id=")+callId);    
				}*/
			
		}else{
			displayMessage("Usage: call <userid>");
//			displayHelp("call");
		}
		handled=true;
	}
	/**
	 * send any command you want.
	 * Syntax: cmd <subsystem> <command> <param>
	 **/
	if ((command.size()>=3) && (command.substr(0,3) == "cmd")) {
		if(command.size()>=9) {

			string s_subsystem="";
			string s_cmd = "";
			string s_param = "";
			uint32_t x = 4;
			
			for ( ; x<command.size(); x++){
				if (command[x]!=' ')
					s_subsystem+=command[x];
				else
					break;
			}
			
			for( ;x<command.size();x++) {
				if (command[x]!=' ')
					s_cmd+=command[x];
				else
					break;
			}
			x++;
			for( ;x<command.size();x++)
				s_param+=command[x];

			CommandString command("",s_cmd, s_param);
			displayMessage("Created cmd=" + s_cmd + " param=" + s_param+" subsystem="+s_subsystem);
			sendCommand(s_subsystem, command);
		}
		else {
			displayMessage("HELP: cmd\nSyntax: cmd <subsystem> <command> <param>", bold);
		}

		handled=true;
	}

#ifdef P2T_SUPPORT

	/**
	 * Commands for Push-2-Talk
	 */
	if ((command.size()>=3) && (command.substr(0,4) == "add ") && inCall){
		if (command.size()>=5){
			string uri = trim(command.substr(4));
			grpList->addUser(uri);	
			
			//if session is already going on, establish SIP Session
			if(p2tmode==true) {
				CommandString command("", "p2tAddUser",p2tGroupId, uri);
				sendCommand("sip", command);
			}		
		
		}else{
//			displayHelp("call");
		}
		handled=true;
	}	
	
	//accept a p2t invitation
	if (command == "accept" /*FIXME*/&& state != "TRANSFER?" ){
		
		//add P2T commands
		addCommand("add");
    		addCommand("edit");
		addCommand("download");
		addCommand("load");
		addCommand("save");
		addCommand("show");
		addCommand("connect");
		addCommand("talk");
		addCommand("stop");
		addCommand("exit");
		addCompletionCallback("add", this);
	
		//inform DefaultDialogHandler
		CommandString cmd1("",  "p2tSessionAccepted", p2tGroupId, inviting_user);
		sendCommand("sip",cmd1);
		

		
		//Info Screen
		showP2TInfo();
		p2tmode=true;
		handled=true;
		state = "P2T IDLE";
		setPrompt(state);
	}
	
	//deny a p2t invitation
	if (command == "deny" ){
		
		//Close SipDialogP2T and he will terminate all
		//SipDialogP2Tuser Sessions.
		CommandString command(p2tGroupId, "p2tTerminate");
		sendCommand("sip", command);
			
		
		//reset states
		inCall=false;
		p2tmode=false;
		state = "IDLE";
		setPrompt("IDLE");
		handled=true;
	}
	
	//enter the P2T Mode
	if (command == "p2t" && p2tmode == false){
		//add possible p2t commands to the TextUI
		addCommand("add");
    		addCommand("edit");
		addCommand("download");
		addCommand("load");
		addCommand("save");
		addCommand("show");
		addCommand("connect");
		addCommand("talk");
		addCommand("stop");
		addCommand("exit");
		addCompletionCallback("add", this);
		
		//start p2tGroupListServer
		CommandString command("","p2tStartGroupListServer");
		sendCommand("sip",command);
		
		//initiate Group Member List and add
		//own username to it
		grpList = MRef<GroupList*>(new GroupList());
		//grpList->addUser(config->inherited.userUri);	
		grpList->addUser(config->inherited->sipIdentity->getSipUri());	
		
		//InfoScreen
		showP2TInfo();
		
		//set p2tmode variable and state
		inCall=true;
		//p2tmode = true;
		state = "P2T IDLE";
		setPrompt("P2T IDLE");
		handled=true;
	}

	if (command == "exit" && inCall == true){
		//remove p2t commands
		//TODO: add this function to <libmutil/TextUI>
		
		//delete Group Member List
		grpList=NULL;
		
		//inform user
		displayMessage("You left the P2T-Mode...", blue);
		p2tmode = false;
		
		//stop SipDialogP2T
		CommandString term(p2tGroupId, "p2tTerminate");
		sendCommand("sip",term);

		//stop p2tGroupListServer
		CommandString command("","p2tStopGroupListServer");
		sendCommand("sip",command);
		
				
		//set state
		inCall=false;
		state = "IDLE";
		setPrompt("IDLE");
		handled=true;
	}

	if (command == "connect" && inCall == true){
		CommandString cmd ("", "p2tStartSession", grpList->print());
		sendCommand("sip",cmd);
		p2tmode=true;
		handled=true;
	}
	
	if (command == "talk" && p2tmode == true){
		CommandString cmd (p2tGroupId, "p2tGetFloor");
		sendCommand("sip", cmd);
		
		handled=true;
	}

	if (command == "stop" && p2tmode == true){
		CommandString cmd (p2tGroupId, "p2tReleaseFloor");
		sendCommand("sip",cmd);
		handled=true;
	}

	if (command == "edit" && inCall == true){
		displayMessage("Not implemented!");
		handled=true;
	}

	if (command == "save" && inCall == true){
		displayMessage("Not implemented!");
		handled=true;
	}
	
	if (command == "load" && inCall == true){
		displayMessage("Not implemented!");
		handled=true;
	}
	
	if (command == "show" && inCall == true){
		showGroupList();
		handled=true;
	}
	
	//download Group Member List from a server
	if ((command.size()>=8) && (command.substr(0,8) == "download") && inCall==true){
		string file="";
		string server="";
		string s_port = "";
		int port=0;
		uint32_t x=0;
		
		displayMessage(command.substr(9,7));
		
		if(command.substr(9,7)=="http://" && command.size()>10)
			x=16;
		else
			x=9;
		
		//parse server
		for( ;x<command.size();x++){
			if(command[x]==':')
				break;
			else if(command[x]=='/') {
				s_port="80";
				break;
			}
			 
			server+=command[x];
		}
		
		//parse port
		for(++x;x<command.size();x++){
			if(s_port=="80"){
				--x;
				break;
			}
			
			if(command[x]=='/')
				break;
				
			s_port+=command[x];
		}
		
		//parse file
		for( ;x<command.size();x++){
			file+=command[x];
		}
		
		if(s_port==""){
			port=80;
		}
		else{
			for(uint32_t k=0;k<s_port.size();k++) {
				port = (port*10) + (s_port[k]-'0');
			}
		}

		displayMessage("Server: " + server);
		displayMessage("File: " + file);
		displayMessage("Port: " + s_port);
		//download GroupList		
		MRef<GroupListClient*>client = new GroupListClient();
		grpList = client->downloadGroupList(file, &server[0], port);
		
		displayMessage("Received GroupList:",bold);
		showGroupList();
		
		client=NULL;
		handled=true;
	}
#endif

	if (!handled && command.substr(0,2) == "im"){
		handled=true;
		string arg = command.substr(2);
		arg = trim(arg);
		size_t spos = arg.find(" ");
		if (spos==string::npos){
			displayMessage("im usage: im <user> <message>",red);
		}else{
			string addr =arg.substr(0,spos);
			addr = trim(addr);
			string msg = arg.substr(spos);
			msg = trim(msg);
			cerr << "Sending instant message to <"<< addr <<"> and message is <"<< msg<<">"<< endl;
			CommandString command("",SipCommandString::outgoing_im, msg, addr);

			//command["proxy_domain"] = regproxy;
			sendCommand("sip",command);
			
		}
	}

	if (!handled && command.size()>0){
		displayMessage("Unknown command: "+command, red);
	}

}



void MinisipTextUI::setSipSoftPhoneConfiguration(MRef<SipSoftPhoneConfiguration *>sipphoneconfig){
	config = sipphoneconfig;       
}

void MinisipTextUI::setCallback(MRef<CommandReceiver*> callback){
	Gui::setCallback(callback);
	MRef<Semaphore *> localSem = semSipReady;
	if( localSem ){
		localSem->inc();
	}
}

void MinisipTextUI::guimain(){
	semSipReady->dec();
	semSipReady = NULL;
	TextUI::guimain();
}

