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

#include<config.h>

#include<libminisip/signaling/conference/ConferenceControl.h>

#include<assert.h>
#include<stdio.h>
#include<vector>
#include<libminisip/signaling/conference/ConfMessageRouter.h>
#include<libminisip/gui/Gui.h>

#include<libmutil/MemObject.h>
#include<libmutil/stringutils.h>
#include<libmutil/termmanip.h>
#include<libmutil/CommandString.h>
#include<libmsip/SipCommandString.h>

#ifdef _WIN32_WCE
#	include<libminisip/minisip_wce_extra_includes.h>
#endif

using namespace std;

ConferenceControl::ConferenceControl(){
	numPending = 0;
}

ConferenceControl::ConferenceControl(string configUri, string cid, bool islocal){
	
	confId=cid;
	
	size_t i = (uint32_t)configUri.find("@");
	assert(i!=string::npos);
	myUri=configUri.substr(0,i);
	myDomain=trim(configUri.substr(i));
	//cerr<<"my Uri and domain: "+myUri+" "+myDomain<<endl;
	incoming=islocal;
	
	numPending = 0;
}

minilist<ConfMember> * ConferenceControl::getConnectedList()
{	
	return &connectedList;
}

void ConferenceControl::setCallback(ConfCallback *cb){
	this->callback=cb;
}

ConfCallback *ConferenceControl::getCallback(){
	return callback;
}

void ConferenceControl::setPendingList(string user)
{
	user=addDomainToPrefix(user);
	pendingList.push_back((ConfMember(user, "")));
	//pendingList[numPending]=user;
	numPending++;
}
/*string[10] ConferenceControl::getPendingList()
{
	return pendingList;
}*/
void ConferenceControl::setConnectedList(string user)
{	
	connectedList.push_back((ConfMember(user, "")));
	/*
	connectedList.uris[connectedList.numUser]=user;
	connectedList.numUser++;
	*/
}
/*string[10] ConferenceControl::getConnectedList()
{
	return connectedList.uris;
}*/


void ConferenceControl::handleGuiCommand(string){

	
    	//cerr << "CC: from MR -> CC: handleGuiCommand"<< endl;
        //string uri = trim(cmd.substr(5));
	//displayMessage(cmd);    
}
void ConferenceControl::handleGuiCommand(const CommandString &command){

	
    	//cerr << "CC: from MR -> CC: handleGuiCommand command"<< endl;
	if(command.getOp()==SipCommandString::accept_invite)
	{
		//pendingList[numPending]=command.getParam();
		//pendingListCallIds[numPending]=command.getDestinationId();
		string remote=addDomainToPrefix(command.getParam());
		pendingList.push_back((ConfMember(remote, command.getDestinationId())));
		//cerr<<"call is accepted=>pending list: "<<endl;
		//printList(&pendingList);
		numPending++;
		string users;
		for(int t=0;t<connectedList.size();t++)

			users=users+ ((connectedList[t]).uri) + ";";       //was connectedList.uris[t]+";";
		//cerr<<"users "+users<<endl;
		CommandString cmd(command);
		cmd.setParam2(users);
		cmd.setParam3(confId);
		//command.setParam2((string) &connectedList);
		//cerr<<"(string) &connectedList************** "+(&connectedList)<<endl;
		callback->confcb_handleSipCommand(cmd);
	}
	if(command.getOp()==SipCommandString::hang_up)
	{
		int t;
		for(t=0;t<connectedList.size();t++)
		{
			CommandString hup(connectedList[t].callid, SipCommandString::hang_up);
			callback->confcb_handleSipCommand(hup);
		}
		for(t=0;t<pendingList.size();t++)
		{
			CommandString hup(pendingList[t].callid, SipCommandString::hang_up);
			callback->confcb_handleSipCommand(hup);
		}
		((ConfMessageRouter *)(callback))->removeConferenceController(this);
	}
	if(command.getOp()=="join")
	{
		bool done=false;
		string sip_url=command.getParam();

		sip_url=addDomainToPrefix(sip_url);
		int t;
		for(t=0;t<connectedList.size()&&!done;t++)

			if(connectedList[t].uri==sip_url)
			{
				done=true;
			}	
		for(t=0;t<pendingList.size()&&!done;t++)
			if(pendingList[t].uri==sip_url)
			{
				done=true;
			}
		if(done)
		{
			CommandString cmd("","error_message","user already added ");
			callback->confcb_handleGuiCommand(cmd);
		}
		else
		{
			callId = callback->confcb_doJoin(sip_url, &connectedList, confId);	
			
			pendingList.push_back((ConfMember(sip_url, callId)));
			sendUpdatesToGui();
		}

	//cerr <<"conf "+callId<< endl;
		
	}
		
        //string uri = trim(cmd.substr(5));
	//displayMessage(cmd);    
}
void ConferenceControl::handleGuiDoInviteCommand(string sip_url){

	//cerr << "CC: from MR -> CC: handleGuiDoInviteCommand"<< endl;
    	/*cerr <<"conf "+sip_url<< endl;
	cerr <<"conf "+confId<< endl;
	//BM pendingList[numPending]=sip_url;
	printList(&connectedList);*/
	
	//numPending++;



	callId = callback->confcb_doJoin(sip_url, &connectedList, confId);	

	
	//cerr <<"conf "+callId<< endl;
	sip_url=addDomainToPrefix(sip_url);
	pendingList.push_back((ConfMember(sip_url, callId)));
	sendUpdatesToGui();
	if (callId=="malformed"){
		//state="IDLE";
		//setPrompt(state);
		//displayMessage("The URI is not a valid SIP URI", red);
		//callId="";
	}else{
		//state="TRYING";
		//setPrompt(state);
		//cerr <<"Created call with id="+callId<<endl;    
	}
        //string uri = trim(cmd.substr(5));
	//displayMessage(cmd);    
}
void ConferenceControl::handleSipCommand( const CommandString &cmd){
    //cerr << "CC: from MR -> CC: handleSipCommand"<< endl;
	   
    if (cmd.getOp()=="invite_ok"){
	    //state="INCALL";
	    //gui->setPrompt(state);
	    cerr << "CC: PROGRESS: remote participant accepted to join the conference..."<< endl;
	    //cerr<<"print connected list-------------"<<endl;
	    //printList(&connectedList);
		//cerr<<"print pending list-------------"<<endl;
	    //printList(&pendingList);
	int i=0;
	string line="";
	
	string users=cmd.getParam();
	minilist<ConfMember> receivedList;
		
		while (users.length()!=0 &&!((uint32_t)i>(users.length()-1))){
			line+=users[i++];
			if(users[i]==';')
			{
				receivedList.push_back((ConfMember(line, "")));
				//connectedList[numConnected]=line;
				//cerr<< "CC------line: " + line << endl;
				line="";
				i++;
			}
		}
	    handleOkAck(cmd.getDestinationId() ,&receivedList);
	    //cerr<<"print connected list-------------"<<endl;
	    //printList(&connectedList);
		//cerr<<"print pending list-------------"<<endl;
	    //printList(&pendingList);
    }
    if (cmd.getOp()=="invite_ack"){
	    //state="INCALL";
	    //gui->setPrompt(state);
	    //cerr << "CC: PROGRESS: ack received..."<< endl;
	    //cerr<<"print connected list-------------"<<endl;
	    //printList(&connectedList);
		//cerr<<"print pending list-------------"<<endl;
	    //printList(&pendingList);
	int i=0;
	string line="";
	string users=cmd.getParam();
	//cerr<<"users-------------"+users<<endl;
	minilist<ConfMember> receivedList;
		while (users.length()!=0 &&!((uint32_t)i>(users.length()-1))){
			line+=users[i++];
			if(users[i]==';')
			{
				receivedList.push_back((ConfMember(line, "")));
				//connectedList[numConnected]=line;
				//cerr<< "CC------line: " + line << endl;
				line="";
				i++;
			}
		}
	    handleOkAck(cmd.getDestinationId() ,&receivedList);
	    //cerr<<"print connected list-------------"<<endl;
	    //printList(&connectedList);
		//cerr<<"print pending list-------------"<<endl;
	    //printList(&pendingList);
    }
    if (cmd.getOp()=="remote_ringing"){
	    //state="REMOTE RINGING";
	    //setPrompt(state);
	    cerr << "CC: PROGRESS: the remove UA is ringing..."<< endl;
	    //displayMessage("PROGRESS: the remove UA is ringing...", blue);
    }
	if (cmd.getOp()=="myuri"){
	    myUri=cmd.getParam();
	    //cerr << "my URI is "+myUri<< endl;
	    //displayMessage("PROGRESS: the remove UA is ringing...", blue);
    }



    /*if (autoanswer && cmd.getOp()==SipCommandString::incoming_available){
	    CommandString command(callId, SipCommandString::accept_invite);
	    callback->guicb_handleCommand(command);
	    state="INCALL";
	    setPrompt(state);
	    displayMessage("Autoanswered call from "+ cmd.getParam());
	    return;
    }*/

    if (cmd.getOp()==SipCommandString::remote_user_not_found){
        //state="IDLE";
	//setPrompt(state);
	cerr << "CC: User "+cmd.getDestinationId()+" not found."<< endl;
	removeMember(cmd.getDestinationId());
	sendUpdatesToGui();
        //displayMessage("User "+cmd.getParam()+" not found.",red);
        callId=""; //FIXME: should check the callId of cmd.
    }
    
    
    if (cmd.getOp()==SipCommandString::security_failed){
        //state="IDLE";
	//setPrompt(state);
	cerr << "CC: Security failed with user "+cmd.getDestinationId()<< endl;
	removeMember(cmd.getDestinationId());
	sendUpdatesToGui();
        //displayMessage("User "+cmd.getParam()+" not found.",red);
        callId=""; //FIXME: should check the callId of cmd.
    }
    if (cmd.getOp()==SipCommandString::remote_unacceptable){
        //state="IDLE";
	//setPrompt(state);
	cerr << "CC: User "+cmd.getDestinationId()+" unacceptable."<< endl;
	removeMember(cmd.getDestinationId());
	sendUpdatesToGui();
        //displayMessage("User "+cmd.getParam()+" not found.",red);
        callId=""; //FIXME: should check the callId of cmd.
    }

    
    if (cmd.getOp()==SipCommandString::remote_cancelled_invite){
        //state="IDLE";
	//setPrompt(state);
	cerr << "CC: User "+cmd.getDestinationId()+" cancelled invite"<< endl;
	removeMember(cmd.getDestinationId());
	sendUpdatesToGui();
        //displayMessage("User "+cmd.getParam()+" not found.",red);
        callId=""; //FIXME: should check the callId of cmd.
    }
    if (cmd.getOp()=="conf_connect_received"){
	    cerr << "CC: connect receieved: "+cmd.getParam()<< endl;
		string remote=addDomainToPrefix(cmd.getParam());
		pendingList.push_back((ConfMember(remote, cmd.getDestinationId())));
		//cerr<<"call is accepted=>pending list: "<<endl;
		//printList(&pendingList);
		sendUpdatesToGui();
		string users;
		for(int t=0;t<connectedList.size();t++)

			users=users+ ((connectedList[t]).uri) + ";";       //was connectedList.uris[t]+";";
		//cerr<<"users "+users<<endl;
		CommandString c(cmd);
		c.setParam2(users);
		
		c.setOp(SipCommandString::accept_invite);
		//command.setParam2((string) &connectedList);
		//cerr<<"(string) &connectedList************** "+(&connectedList)<<endl;
		callback->confcb_handleSipCommand(c);
	    //displayMessage("ERROR: "+cmd.getParam(), red);
    }
	
    if (cmd.getOp()==SipCommandString::remote_hang_up){
        //state="IDLE";
	//setPrompt(state);
	removeMember(cmd.getDestinationId());
	sendUpdatesToGui();
	cerr << "CC: Remote user ended the call."<< endl;
        //displayMessage("Remote user ended the call.",red);
        callId=""; //FIXME: should check the callId of cmd.
    }



    if (cmd.getOp()==SipCommandString::transport_error){
	    //state="IDLE";
	    //setPrompt(state);
	    removeMember(cmd.getDestinationId());
	    sendUpdatesToGui();
	    cerr << "CC: The call could not be completed because of a network error."<< endl;
	    //displayMessage("The call could not be completed because of a network error.", red);
	 
    }


    if (cmd.getOp()=="error_message"){
	    cerr << "CC: ERROR: "+cmd.getParam()<< endl;
	    //displayMessage("ERROR: "+cmd.getParam(), red);
    }

    if (cmd.getOp()=="remote_reject"){
	    removeMember(cmd.getDestinationId());
	    sendUpdatesToGui();
	    cerr << "CC: The remote user rejected the call."<< endl;
	    //displayMessage("The remote user rejected the call.", red);
    }
    
    
    /*if (cmd.getOp()==SipCommandString::incoming_available){
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
	    
    }*/

    
    
    /****
     * p2tAddUser
     * DestinationID: CallId (SipDialogP2Tuser) 
     * Param1:        SIP URI
     * Description:   a remote user wants to be added to a P2T Session. 
     *                This command is received when the SipDialogP2Tuser
     *                is in the RINGING state and waits for an accept.
     ****/
    /*else if (cmd.getOp()=="p2tAddUser"){
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
    }*/
    
    /****
     * p2tRemoveUser
     * DestinationID: GroupId (SipDialogP2T) 
     * Param1:        SIP URI
     * Param2:        reason
     * Description:   a remote user wants to be added to a P2T Session. 
     *                This command is received when the SipDialogP2Tuser
     *                is in the RINGING state and waits for an accept.
     ****/
    /*else if (cmd.getOp()=="p2tRemoveUser"){
	    if(inCall==true && p2tmode==true){
		displayMessage("User " + cmd.getParam() + " removed ("+ cmd.getParam2() +").", red);
		grpList->removeUser(cmd.getParam());
	    }
    }*/
    
     /****
     * p2tModifyUser
     * DestinationID: GroupId (SipDialogP2T) 
     * Param1:        SIP URI
     * Param2:        status
     * Description:   information about a state (in the floor control)
     *                of a user.
     ****/
    /*else if (cmd.getOp()=="p2tModifyUser"){
	    if(inCall==true && p2tmode==true){
	    	
	    	int status=0;
		for(uint32_t k=0;k<cmd.getParam2().size();k++) 
			status = (status*10) + (cmd.getParam2()[k]-'0');
	    	
		grpList->getUser(cmd.getParam())->setStatus(status);
	    }
    }*/
     
     /****
     * p2tInvitation
     * DestinationID: GroupID (SipDialogP2T)
     * Param1:        Group Member List (XML-code)
     * Param2:        uri inviting user
     * 
     * Description:   an invitation to a P2T Session
     ****/
    /*else if (cmd.getOp()=="p2tInvitation"){
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

    }*/
    
     /****
     * p2tSessionCreated
     * DestinationID: 
     * Param1:        GroupId (SipDialogP2T)
     * Param2:
     * Param3:
     * Description:   the P2T Session is set up
     ****/
    /*else if (cmd.getOp()=="p2tSessionCreated"){
	    p2tGroupId=cmd.getParam();
	    displayMessage("P2T Session "+ p2tGroupId + " created", green);
	    grpList->setGroupIdentity(p2tGroupId);
	    state="P2T CONNECTED";
	    setPrompt(state);
    }    
    
    else if (cmd.getOp().substr(0,3)=="p2t"){
	    displayMessage("Received: "+cmd.getOp(), blue);
    }*/
}

/**
* Moves a member from pending to connected and look for new members
*/
void ConferenceControl::sendUpdatesToGui()
{
	string connectedusers="";
	int t;
	for(t=0;t<connectedList.size();t++)
		connectedusers=connectedusers+ ((connectedList[t]).uri) + ", "; 
	//cerr<<"users "+connectedusers<<endl;
	string pendingusers="";
	for(t=0;t<pendingList.size();t++)
		pendingusers=pendingusers+ ((pendingList[t]).uri) + ", ";     
	//cerr<<"users "+pendingusers<<endl;
	CommandString cmd(confId,"list updated",connectedusers,pendingusers);
	callback->confcb_handleGuiCommand(cmd);
}
void ConferenceControl::handleOkAck(string callid, minilist<ConfMember> *conflist) {
	pendingToConnected(callid);
	updateLists(conflist);
	sendUpdatesToGui();
}
	
/**
* Print a list of conference members
*/
void ConferenceControl::printList(minilist<ConfMember> *conflist) {
	for (int i = 0; i < conflist->size(); i++ ) {
		cerr << "Member : " + ((*conflist)[i]).uri << endl;
		cerr << "CallId : " + ((*conflist)[i]).callid << endl;
	} 
}
        
	
/**
* Move a member from pending to connected status
*/
void ConferenceControl::pendingToConnected(string memberid) {

	//find member in the pending list and remove it
	int i = 0;
	bool done = false;
	
	while ((!done) && (i < pendingList.size() ) ) {
		
		if (pendingList[i].callid == memberid) {
			connectedList.push_back(pendingList[i]);
			pendingList.remove(i);
			done = true;
		}
		
		i++;
	}
	
	assert(done==true);
	
}
void ConferenceControl::removeMember(string memberid) {

	//find member in the pending list and remove it
	int i = 0;
	bool done = false;
	//printList(&pendingList);
	//cerr<<"callid  "+memberid<<endl;
	while ((!done) && (i < pendingList.size() ) ) {
		
		if (pendingList[i].callid == memberid) {
			pendingList.remove(i);
			done = true;
		}
		
		i++;
	}
	i=0;
	while ((!done) && (i < connectedList.size() ) ) {
		
		if (connectedList[i].callid == memberid) {
			connectedList.remove(i);
			done = true;
		}
		
		i++;
	}
	
	//assert(done==true);
	
}
/**
* Check for new members to connect to
*/
void ConferenceControl::updateLists(minilist<ConfMember> *conflist) {
	bool handled = false;
	
	
	for (int i = 0; i < conflist->size(); i++) {
		string current = (*conflist)[i].uri;
		
		//check against pending list
		for (int j = 0; j < pendingList.size(); j++ ) {
			if (current == pendingList[j].uri) {
				handled = true;
				break;
			}
		}	
	
		//check against connected list
		if (!handled) {
			for (int j = 0; j < connectedList.size(); j++ ) {
				if (current == connectedList[j].uri) {
					handled = true;
					break;
				}
			}
		
		}
		
		//if not found in pending or connected list then add to pending list
		if (!handled&&current!=(myUri+myDomain)) {
			//send a connect message to the newly discovered conference members
			callId = callback->confcb_doConnect(current,confId);
			current=addDomainToPrefix(current);
			pendingList.push_back(ConfMember(current, callId  )  );
			
			//cerr<<"update pending list=> "+current<<endl;
		}
	
	}
}
	
string ConferenceControl::addDomainToPrefix(string remoteUri)
{
	bool done=false;
	string result=remoteUri;
	for(unsigned int i=0;i<remoteUri.length();i++)
	{
		if(remoteUri[i]=='@'){
			done=true;
			break;	}		
	}
	if(!done)
		result=remoteUri+myDomain;
	return result;
}	
	
