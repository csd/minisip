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

#include<assert.h>
#include"MessageRouter.h"
//#include<libmsip/CODECInterface.h>
//#include"../util/ConfigFile.h"
#include"../sip/SipSoftPhoneConfiguration.h"
#include<libmsip/SipDialogContainer.h>
#include"../sip/DefaultDialogHandler.h"
//#include"../conf/ConferenceControl.h" 


MessageRouter::MessageRouter(){
}

void MessageRouter::setSip(MRef<Sip*> s){
	if (s.isNull()){
#ifdef DEBUG_OUTPUT		
		cerr << "ERROR: Sip is NULL in set_sip_state_machine in MessageRouter"<< endl;
#endif
	}
	sip= s;
}

void MessageRouter::sipcb_handleCommand(CommandString &command){
	gui->handleCommand(command);
}
void MessageRouter::sipcb_handleConfCommand(CommandString &command){
	confrout->handleSipCommand(command);
}

/*
void MessageRouter::sipcb_handleConfCommand(CommandString &command){
	ConferenceControl->handleCommand(command);
}//bm*/

/*void MessageRouter::confcb_handleSipCommand(CommandString &cmd){
	SipSMCommand sipcmd(cmd, SipSMCommand::remote, SipSMCommand::TU);
	sip->getSipStack()->handleCommand(sipcmd);
}//bm*/
/*void MessageRouter::confcb_handleGuiCommand(CommandString &command){
	gui->handleCommand(command);
}//bm*/

void MessageRouter::guicb_handleConfCommand(ConferenceControl *conf,string &command){
	confrout=conf;
	confrout->setCallback(this);
	cerr << "MR: from Gui -> CC: guicb_handleConfCommand"<< endl;
	confrout->handleGuiCommand(command);
	
}//bm

string MessageRouter::guicb_confDoInvite(ConferenceControl *conf,string sip_url){
	confrout=conf;
	cerr << "MR: from Gui -> CC: guicb_confDoInvite"<< endl;
	confrout->handleGuiDoInviteCommand(sip_url);
	//cerr << "ERROR: Sip is NULL in set_sip_state_machine in MessageRouter"<< endl;
}//bm
void MessageRouter::guicb_handleMediaCommand(CommandString &cmd){
	mediaHandler->handleCommand(cmd);
}


string MessageRouter::guicb_doInvite(string user){
//	cerr << "ERROR: INVITE USER UNIMPLEMENTED"<< endl;
	return sip->invite(user);
}

void MessageRouter::guicb_handleCommand(CommandString &cmd){
	//return sip_machine->enqueueCommand(cmd);
	SipSMCommand sipcmd(cmd, SipSMCommand::remote, SipSMCommand::TU);
	sip->getSipStack()->handleCommand(sipcmd);
}

string MessageRouter::confcb_doJoin(string user, string list[10], int num){
//	cerr << "ERROR: INVITE USER UNIMPLEMENTED"<< endl;
	cerr << "MR: from CC -> MR: confcb_confDoInvite"<< endl;
	return sip->confjoin(user, list, num);
}
string MessageRouter::confcb_doConnect(string user){
//	cerr << "ERROR: INVITE USER UNIMPLEMENTED"<< endl;
	cerr << "MR: from CC -> MR: confcb_confDoInvite"<< endl;
	return sip->confconnect(user);
}
