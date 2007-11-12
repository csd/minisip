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

#include<libminisip/signaling/conference/ConfMessageRouter.h>

#include<iostream>

#ifdef _WIN32_WCE
#	include<libminisip/minisip_wce_extra_includes.h>
#endif

using namespace std;

ConfMessageRouter::ConfMessageRouter(){
}

ConfMessageRouter::~ConfMessageRouter(){
// 	cerr << "~ConfMessageRouter" << endl;
}

void ConfMessageRouter::setSip(MRef<Sip*> s){
	if (s.isNull()){
#ifdef DEBUG_OUTPUT		
		cerr << "ERROR: Sip is NULL in set_sip_state_machine in ConfMessageRouter"<< endl;
#endif
	}
	sip= s;
}

void ConfMessageRouter::confcb_handleGuiCommand(const CommandString &command){
	gui->handleCommand(command);
}	
void ConfMessageRouter::sipcb_handleCommand(const CommandString &command){
	gui->handleCommand(command);
}
void ConfMessageRouter::sipcb_handleConfCommand(const CommandString &command){
	bool done=false;
	int i;
	//cerr<<"command.getParam3 "+command.getParam3()<<endl;
	for (i=0;i < confrout.size();i++  ) {
		//cerr<<"From SCB=>MR with conf id: "+confrout[i]->confId<<endl;
		//cerr<<"confid "+confrout[i]->confId<<endl;
		if ((confrout[i]->confId) == command.getParam3()) 
		{
			confrout[i]->setCallback(this);
			done=true;
			break;
		}
	}
	if(done)
		confrout[i]->handleSipCommand(command);
	else
	{
		CommandString cmd("","error_message","no such conference with id "+command.getParam3());
		gui->handleCommand(cmd);
		
	}
}

void ConfMessageRouter::setConferenceController(ConferenceControl *conf)
{
	confrout.push_back(conf);

}
void ConfMessageRouter::removeConferenceController(ConferenceControl *conf)
{
	int i=0;
	while ((i < confrout.size() ) ) {
		
		if (confrout[i] == conf) 
			confrout.remove(conf);
		i++;
	}
	
}
void ConfMessageRouter::guicb_handleConfCommand(const string &){
	//confrout=conf;
	
	//confrout->setCallback(this);
	//cerr << "MR: from Gui -> CC: guicb_handleConfCommand"<< endl;
	//confrout->handleGuiCommand(conferencename);
	
}//bm
void ConfMessageRouter::guicb_handleConfCommand(const CommandString &command){
	bool done=false;
	int i;
	//cerr<<"command.getParam3 "+command.getParam3()<<endl;
	for (i=0;i < confrout.size();i++  ) {
		//cerr << "MR: from Gui -> CC: guicb_handleConfCommand with conf id"+confrout[i]->confId<< endl;
		//cerr<<"confrout[i]->confId "+confrout[i]->confId<<endl;
		if (confrout[i]->confId == command.getParam3()) 
		{
			confrout[i]->setCallback(this);
			done=true;
			break;
		}
	}
	if(done)
	{
		//cerr << "MR: from Gui -> CC: guicb_handleConfCommand"<< endl;
		confrout[i]->handleGuiCommand(command);
	}
	else
	{
		CommandString cmd("","error_message","no such conference with id "+command.getParam3());
		gui->handleCommand(cmd);
		
	}
	
	
}//bm
ConferenceControl* ConfMessageRouter::getConferenceController(string confid)
{
	bool done=false;
	int i;
	for (i=0;i < confrout.size();i++  ) {
		
		if (confrout[i]->confId == confid) 
		{
			done=true;
			break;
		}
	}
	if(done)
		return confrout[i];
	else
	{
		CommandString cmd("","error_message","no such conference with id "+confid);
		gui->handleCommand(cmd);
		return confrout[i]; //needs to be changed
	}
}

/*
string ConfMessageRouter::guicb_confDoInvite(string sip_url){
	//confrout=conf;
	//cerr << "MR: from Gui -> CC: guicb_confDoInvite"<< endl;
	//confrout->handleGuiDoInviteCommand(sip_url);
	//cerr << "ERROR: Sip is NULL in set_sip_state_machine in ConfMessageRouter"<< endl;
	return ""; //FIXME: Should this method be deleted??? 
}//bm
*/

void ConfMessageRouter::guicb_handleMediaCommand(const CommandString &cmd){
	subsystemMedia->handleCommand("media",cmd);
}

//string ConfMessageRouter::guicb_doInvite(string user){
////	cerr << "ERROR: INVITE USER UNIMPLEMENTED"<< endl;
//	return sip->invite(user);
//}

void ConfMessageRouter::guicb_handleCommand(const CommandString &cmd){
	//return sip_machine->enqueueCommand(cmd);
	SipSMCommand sipcmd(cmd, SipSMCommand::dialog_layer, SipSMCommand::dialog_layer);
	sip->getSipStack()->handleCommand(sipcmd);
}

string ConfMessageRouter::confcb_doJoin(string user, minilist<ConfMember> *conflist, string confId){
//	cerr << "ERROR: INVITE USER UNIMPLEMENTED"<< endl;
	//cerr << "MR: from CC -> MR: confcb_confDoInvite"<< endl;
	return sip->confjoin(user, conflist, confId);
	//return "12345";
}
string ConfMessageRouter::confcb_doConnect(string user, string confId){
//	cerr << "ERROR: INVITE USER UNIMPLEMENTED"<< endl;
	//cerr << "MR: from CC -> MR: confcb_confDoInvite"<< endl;
	return sip->confconnect(user, confId);
}
void ConfMessageRouter::confcb_handleSipCommand(const CommandString &command){
//	cerr << "ERROR: INVITE USER UNIMPLEMENTED"<< endl;
	//cerr << "MR: from CC -> MR: confcb_handleSipCommand"<< endl;
	SipSMCommand sipcmd(command, SipSMCommand::dialog_layer, SipSMCommand::dialog_layer);
	sip->getSipStack()->handleCommand(sipcmd);
	
}

