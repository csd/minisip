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
#include<vector>
#include"MinisipTextUI.h"

#include<libmutil/MemObject.h>
#include<libmutil/trim.h>
#include<libmutil/termmanip.h>

#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipCommandString.h>
#include"../../../sip/DefaultDialogHandler.h"


extern TextUI *debugtextui;



#ifdef DEBUG_OUTPUT
extern bool sipdebug_print_packets;
#endif

MinisipTextUI::MinisipTextUI(): TextUI(), autoanswer(false){
    inCall=false;
    p2tmode=false;

#ifdef DEBUG_OUTPUT
    mdbg << "Setting global MinisipTextUI"<< end;
    debugtextui=this;
#endif

    addCommand("answer");
    addCommand("disable autoanswer");
    addCommand("enable autoanswer");
    addCommand("enable debugmsgs");
    addCommand("disable debugmsgs");
    addCommand("hangup");
    addCommand("hide packets");
    addCommand("register");
    addCommand("show all");
    addCommand("show calls");
    addCommand("show transactions");
    addCommand("show packets");
    addCommand("cmd");
    addCommand("accept"); //accept incoming P2T Session
    addCommand("deny");   //deny incoming P2T Session
    addCommand("im");   //deny incoming P2T Session
//    addCommand("quit");
    addCompletionCallback("call", this);
    addCompletionCallback("im", this);
    
    state="IDLE";
    setPrompt("IDLE");
    
}

void MinisipTextUI::run(){
	guimain();
}

void MinisipTextUI::displayErrorMessage(string msg){
	displayMessage(msg, red);
}

minilist<std::string> MinisipTextUI::textuiCompletionSuggestion(string match){
	minilist<std::string> ret;

//TODO: implement it with the new phonebook.

/*	if (!config.isNull()){
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

void MinisipTextUI::handleCommand(CommandString cmd){
#ifdef DEBUG_OUTPUT
	mdbg << FG_MAGENTA << "MinisipTextUI::handleCommand: Got "<<cmd.getString() << PLAIN <<end;
#endif
	
    if (cmd.getOp()=="register_ok"){
        displayMessage("Register to proxy "+cmd.getParam()+" OK", green);
    }
	
    if (cmd.getOp()==SipCommandString::incoming_im){
        displayMessage("IM to <"+cmd.getParam3()+"> from <"+cmd.getParam2()+">: "+cmd.getParam(), bold);
    }
 
   
    if (cmd.getOp()=="invite_ok"){
	    state="INCALL";
	    setPrompt(state);
	    displayMessage("PROGRESS: remote participant accepted the call...", blue);
    }

    if (cmd.getOp()=="remote_ringing"){
	    state="REMOTE RINGING";
	    setPrompt(state);
	    displayMessage("PROGRESS: the remove UA is ringing...", blue);
    }



    if (autoanswer && cmd.getOp()==SipCommandString::incoming_available){
	    CommandString command(callId, SipCommandString::accept_invite);
	    callback->guicb_handleCommand(command);
	    state="INCALL";
	    setPrompt(state);
	    displayMessage("Autoanswered call from "+ cmd.getParam());
	    return;
    }

    if (cmd.getOp()==SipCommandString::remote_user_not_found && !p2tmode){
        state="IDLE";
	setPrompt(state);
        displayMessage("User "+cmd.getParam()+" not found.",red);
        callId=""; //FIXME: should check the callId of cmd.
    }

    if (cmd.getOp()==SipCommandString::remote_hang_up){
        state="IDLE";
	setPrompt(state);
        displayMessage("Remote user ended the call.",red);
        callId=""; //FIXME: should check the callId of cmd.
    }



    if (cmd.getOp()==SipCommandString::transport_error && !p2tmode){
	    state="IDLE";
	    setPrompt(state);
	    displayMessage("The call could not be completed because of a network error.", red);
	    callId=""; //FIXME: should check the callId of cmd.
    }


    if (cmd.getOp()=="error_message"){
	    displayMessage("ERROR: "+cmd.getParam(), red);
    }

    if (cmd.getOp()=="remote_reject" && !p2tmode){
	    state="IDLE";
	    setPrompt(state);
	    callId="";
	    displayMessage("The remote user rejected the call.", red);
    }

    if (cmd.getOp()==SipCommandString::incoming_available){
	    if(!inCall){
	    	state="ANSWER?";
	    	setPrompt(state);
	    	callId=cmd.getDestinationId();
	    	displayMessage("The incoming call from "+cmd.getParam(), blue);
	    }
	    else{
	    	displayMessage("You missed call from "+cmd.getParam(), red);
	    	CommandString hup(cmd.getDestinationId(), SipCommandString::reject_invite);
		callback->guicb_handleCommand(hup);
	    
	    }
	    
    }

    //P2T commands
    if (cmd.getOp()=="p2tFloorGranted"){
	    displayMessage("Floor is granted!!!", blue);

	    
    }else if (cmd.getOp()=="p2tFloorTaken"){
	    displayMessage("Floor is granted to "+cmd.getParam(),blue);

    }
    else if (cmd.getOp()=="p2tFloorReleased"){
	    displayMessage("Floor available!", blue);
	    state="P2T CONNECTED";
	    setPrompt(state);
    }
    else if (cmd.getOp()=="p2tFloorRevokeActive"){
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
	    if(inCall==true && p2tmode==true){
	    	
	    	int status=0;
		for(int k=0;k<cmd.getParam2().size();k++) 
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
	    p2tGroupId=cmd.getParam();
	    displayMessage("P2T Session "+ p2tGroupId + " created", green);
	    grpList->setGroupIdentity(p2tGroupId);
	    state="P2T CONNECTED";
	    setPrompt(state);
    }    
    
    else if (cmd.getOp().substr(0,3)=="p2t"){
	    displayMessage("Received: "+cmd.getOp(), blue);
    }
}

bool MinisipTextUI::configDialog( MRef<SipSoftPhoneConfiguration *> conf ){
	cout << "ERROR: MinisipTextUI::configDialog is not implemented"<< endl;
}

void MinisipTextUI::showCalls(string command){
	//list<MRef<SipDialog*> > *calls = config->sip->getDialogContainer()->getDispatcher()->getDialogs();
	list<MRef<SipDialog*> > calls = config->sip->getDialogContainer()->getDispatcher()->getDialogs();
	displayMessage(string("Calls:"));
	if (calls.size()==0)
		displayMessage("    (no calls)");
	else{
		int ii=0;	   
		for (list<MRef<SipDialog*> >::iterator i=calls.begin(); i!=calls.end(); i++,ii++){
			displayMessage(string("    (")+itoa(ii)+") "
					+ (*i)->getName() 
					+ "   State: "
					+ (*i)->getCurrentStateName());
			//                    + "  ObjectId: " + itoa(   (int) (*(((*calls)[i])))    )  );
		}
	}
}

void MinisipTextUI::showTransactions(string command){
	list<MRef<SipDialog*> > calls = config->sip->getDialogContainer()->getDispatcher()->getDialogs();

	string tno = trim(command).substr(17);
	int itno=0;
	if (trim(tno).size()==0){
		displayMessage("show transaction <transaction_nr>");
	}else{
		itno = atoi(tno.c_str());

		if (calls.size()-1>=itno){
			list<MRef<SipDialog*> >::iterator call = calls.begin();
			for (int j=0; j < itno; j++)
				call++;
			//            MRef<SipDialog*> call = (*calls)[itno];

			displayMessage(string("Transactions for call ")+(*call)->getName());
			list<MRef<SipTransaction*> > transactions = (*call)->getTransactions();
			if (transactions.size()==0)
				displayMessage("(no transactions)");
			else{
				int n=0;
				for (list<MRef<SipTransaction*> >::iterator i = transactions.begin();
						i!=transactions.end(); i++){
					displayMessage(string("    (")+itoa(n)+") "+
							(*i)->getName() 
							+ "   State: "
							+ (*i)->getCurrentStateName()
							+ "  ObjecdId: " + itoa((int)*(*i)));
					n++;
				}
			}
		}else{
			displayMessage("Call not found", red);
		}
	}

}

void MinisipTextUI::showTimeouts(string command){
	string to = config->timeoutProvider->getTimeouts();
	displayMessage(string("Timeouts: \n")+to);
}


void MinisipTextUI::showDialogInfo(MRef<SipDialog*> d, bool usesStateMachine){

	list <TPRequest<string,MRef<StateMachine<SipSMCommand,string>*> > > torequests = 
		config->timeoutProvider->getTimeoutRequests();

	if (usesStateMachine){
		displayMessage(/*string("    (")+itoa(ii)+") " +*/ d->getName() + "   State: " + d->getCurrentStateName());
	}else{
		displayMessage( d->getName());
	}
	displayMessage("        Timeouts:", bold);
//	cerr << BOLD << "        Timeouts:"<< PLAIN << endl;
	int ntimeouts=0;
	std::list<TPRequest<string,MRef<StateMachine<SipSMCommand,string>*> > >::iterator jj=torequests.begin();
	for (int j=0; j< torequests.size(); j++,jj++){
		if ( *d == *((*jj).get_subscriber()) ){
			int ms= (*jj).get_ms_to_timeout();
			displayMessage("            timeout: "+ (*jj).get_command() + "  Time: " + itoa(ms/1000) + "." + itoa(ms%1000));
//			cerr << string("            timeout: ")+ (*jj).get_command()
//				+ "  Time: " + itoa(ms/1000) + "." + itoa(ms%1000) << endl;
			ntimeouts++;
		}
	}
	if (ntimeouts==0){
		displayMessage("            (no timeouts)");
	}


	displayMessage( "        Transactions:", bold);
	list<MRef<SipTransaction*> > transactions = d->getTransactions();
	if (transactions.size()==0)
		displayMessage("            (no transactions)");
	else{
		int n=0;
		for (list<MRef<SipTransaction*> >::iterator i = transactions.begin();
				i!=transactions.end(); i++){
			
			displayMessage( string("            (")+itoa(n)+") "+ (*i)->getName() + "   State: " + (*i)->getCurrentStateName());
			n++;

			displayMessage("                Timeouts:", bold);

			int ntimeouts=0;
			std::list<TPRequest<string,   MRef<StateMachine<SipSMCommand,string>*>  > >::iterator jj=torequests.begin();
			for (int j=0; j< torequests.size(); j++, jj++){
				if ( *((*i)) == *((*jj).get_subscriber()) ){
					int ms= (*jj).get_ms_to_timeout();
					displayMessage(string("                        timeout: ")
						+ (*jj).get_command()
						+ "  Time: " + itoa(ms/1000) + "." + itoa(ms%1000));
					ntimeouts++;
				}
			}
			if (ntimeouts==0)
				displayMessage("                        (no timeouts)");
		}
	}





}

void MinisipTextUI::showStat(string command){
	list<MRef<SipDialog*> > calls = config->sip->getDialogContainer()->getDispatcher()->getDialogs();

	list <TPRequest<string, MRef<StateMachine<SipSMCommand,string>*> > > torequests = config->timeoutProvider->getTimeoutRequests();

	displayMessage(" Default dialog handler:", bold);
	showDialogInfo(config->sip->getDialogContainer()->getDefaultHandler(), false);
	
	displayMessage(" Calls:", bold);
	if (calls.size()==0)
		displayMessage("    (no calls)");
	else{
		//        for (int i=0; i<calls->getSize(); i++){
		int ii=0;
		for (list<MRef<SipDialog*> >::iterator i=calls.begin(); i!= calls.end(); i++, ii++){
			displayMessage(string("    (")+itoa(ii)+") ");
			showDialogInfo(*i,true);
#if 0
			displayMessage(string("    (")+itoa(ii)+") "
					//            displayMessage(string("    ")
				+ (*i)->getName() 
				+ "   State: "
				+ (*i)->getCurrentStateName());


					displayMessage("        Timeouts:", bold);
					int ntimeouts=0;
					std::list<TPRequest<string,MRef<StateMachine<SipSMCommand,string>*> > >::iterator jj=torequests.begin();
					for (int j=0; j< torequests.size(); j++,jj++){
					if ( *(*i) == *(*jj).get_subscriber() ){
					int ms= (*jj).get_ms_to_timeout();
					displayMessage(string("            timeout: ")+ (*jj).get_command()
						+ "  Time: " + itoa(ms/1000) + "." + itoa(ms%1000));
					ntimeouts++;
					}
					}
					if (ntimeouts==0){
					displayMessage(       "            (no timeouts)");
					}



					displayMessage("        Transactions:", bold);
					list<MRef<SipTransaction*> > transactions = (*i)->getTransactions();
					if (transactions.size()==0)
						displayMessage("            (no transactions)");
					else{
						int n=0;
                for (list<MRef<SipTransaction*> >::iterator i = transactions.begin();
                        i!=transactions.end(); i++){
                    displayMessage(string("            (")+itoa(n)+") "+
                            (*i)->getName() 
                            + "   State: "
                            + (*i)->getCurrentStateName());
                    n++;

                    displayMessage("                Timeouts:", bold);
                    
                    int ntimeouts=0;
                    std::list<TPRequest<string,MRef<StateMachine<SipSMCommand,string>*> > >::iterator jj=torequests.begin();
                    for (int j=0; j< torequests.size(); j++, jj++){
                        if ( (*(*i)) == *(*jj).get_subscriber() ){
                            int ms= (*jj).get_ms_to_timeout();
                            displayMessage(string("                        timeout: ")
                                        + (*jj).get_command()
                                        + "  Time: " + itoa(ms/1000) + "." + itoa(ms%1000) );
                            ntimeouts++;
                        }
                    }
                    if (ntimeouts==0)
                        displayMessage(       "                        (no timeouts)");
                }
            }

#endif
        }
	}

}


void MinisipTextUI::showMem(){
	displayMessage("(not implemented any more)");
//    displayMessage(memhandler.listObjs());
}

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
    	for(int k=0;k<grpList->getAllMember().size();k++)
        	displayMessage(grpList->getAllMember().at(k));
    }

    if(grpList->getAllUser().size()>0){
    	displayMessage("Participants:", bold);
        for(int k=0;k<grpList->getAllUser().size();k++)
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

void MinisipTextUI::keyPressed(int key){
	switch(key){
	case '*':
		showMem();
		break;
	case '+':
		showStat("");
		break;

	}


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
		mdbg << "Regproxy=<"<<regproxy<<">"<< end;
		CommandString command("",SipCommandString::proxy_register/*, regproxy*/);

//		if (regproxy=="pstn")
//			command.setDestinationId("pstn");

		command["proxy_domain"] = regproxy;
		callback->guicb_handleCommand(command);
		handled=true;
	}

	if (command == "show calls"){
		showCalls(command);
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

	if (command == "show timeouts"){
		showTimeouts(command);
		handled=true;
	}

#ifdef DEBUG_OUTPUT
	if (command == "show packets"){
		sipdebug_print_packets=true;
		displayMessage("SIP messages will be displayed on the screen", blue);
		handled=true;
	}

	if (command == "hide packets"){
		sipdebug_print_packets=false;
		displayMessage("SIP messages will NOT be displayed on the screen", blue);
		handled=true;
	}
#endif

	if (command.substr(0,17) == "show transactions"){
		showTransactions(command);
		handled=true;
	}

	if (command == "answer"){
		CommandString command(callId, SipCommandString::accept_invite);
		callback->guicb_handleCommand(command);
		displayMessage("A call with the most recent callId will be accepted");
		handled=true;
	}

	if (command == "show all"){
		showCalls(command);

		list<MRef<SipDialog*> > calls = config->sip->getDialogContainer()->getDispatcher()->getDialogs();
		for (int i=0; i<calls.size(); i++){
			showTransactions("show transactions "+itoa(i));
		}
		showTimeouts(command);
		handled=true;
	}

	if (trim(command) == "hangup"){
		CommandString hup(callId, SipCommandString::hang_up);
		callback->guicb_handleCommand(hup);
		state="IDLE";
		setPrompt(state);
		displayMessage("hangup");
		handled=true;
	}

	if ((command.size()>=4) && (command.substr(0,4) == "call")){
		if (command.size()>=6){
			if (state!="IDLE"){
				displayMessage("UNIMPLEMENTED - only one call at the time with this UI.", red);
			}else{
				string uri = trim(command.substr(5));
				displayMessage("Uri: "+uri);
				callId = callback->guicb_doInvite(uri);
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


	/**
	 * send any command you want.
	 * Syntax: cmd <command> <param>
	 **/
	if ((command.size()>=3) && (command.substr(0,3) == "cmd")) {
		if(command.size()>=5) {

			string s_cmd = "";
			string s_param = "";
			int x = 4;
			for(x;x<command.size();x++) {
				if (command[x]!=' ')
					s_cmd+=command[x];
				else
					break;
			}
			x++;
			for(x;x<command.size();x++)
				s_param+=command[x];

			CommandString command("",s_cmd, s_param);
			displayMessage("Created cmd=" + s_cmd + " param=" + s_param);
			callback->guicb_handleCommand(command);
		}
		else {
			displayMessage("HELP: cmd\nSyntax: cmd <command> <param>", bold);
		}

		handled=true;
	}

	/**
	 * Commands for Push-2-Talk
	 */
	if ((command.size()>=3) && (command.substr(0,3) == "add") && inCall){
		if (command.size()>=5){
			string uri = trim(command.substr(4));
			grpList->addUser(uri);	
			
			//if session is already going on, establish SIP Session
			if(p2tmode==true) {
				CommandString command("", "p2tAddUser",p2tGroupId, uri);
				callback->guicb_handleCommand(command);
			}		
		
		}else{
//			displayHelp("call");
		}
		handled=true;
	}	
	
	//accept a p2t invitation
	if (command == "accept" ){
		
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
		callback->guicb_handleCommand(cmd1);
		

		
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
		callback->guicb_handleCommand(command);	
		
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
		callback->guicb_handleCommand(command);
		
		//initiate Group Member List and add
		//own username to it
		grpList = MRef<GroupList*>(new GroupList());
		//grpList->addUser(config->inherited.userUri);	
		grpList->addUser(config->inherited.sipIdentity->getSipUri());	
		
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
		callback->guicb_handleCommand(term);	

		//stop p2tGroupListServer
		CommandString command("","p2tStopGroupListServer");
		callback->guicb_handleCommand(command);
				
		//set state
		inCall=false;
		state = "IDLE";
		setPrompt("IDLE");
		handled=true;
	}

	if (command == "connect" && inCall == true){
		CommandString cmd ("", "p2tStartSession", grpList->print());
		callback->guicb_handleCommand(cmd);
		p2tmode=true;
		handled=true;
	}
	
	if (command == "talk" && p2tmode == true){
		CommandString cmd (p2tGroupId, "p2tGetFloor");
		callback->guicb_handleCommand(cmd);
		
		handled=true;
	}

	if (command == "stop" && p2tmode == true){
		CommandString cmd (p2tGroupId, "p2tReleaseFloor");
		callback->guicb_handleCommand(cmd);
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
		int x=0;
		
		displayMessage(command.substr(9,7));
		
		if(command.substr(9,7)=="http://" && command.size()>10)
			x=16;
		else
			x=9;
		
		//parse server
		for(x;x<command.size();x++){
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
		for(x;x<command.size();x++){
			file+=command[x];
		}
		
		if(s_port==""){
			port=80;
		}
		else{
			for(int k=0;k<s_port.size();k++) {
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

	if (!handled && command.substr(0,2) == "im"){
		handled=true;
		string arg = command.substr(2);
		arg = trim(arg);
		unsigned spos = arg.find(" ");
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
			callback->guicb_handleCommand(command);
		}
	}

	if (!handled && command.size()>0){
		displayMessage("Unknown command: "+command, red);
	}

}



void MinisipTextUI::setSipSoftPhoneConfiguration(MRef<SipSoftPhoneConfiguration *>sipphoneconfig){
	config = sipphoneconfig;       
}


