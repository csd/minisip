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

#include"Sip.h"

#include"DefaultDialogHandler.h"
#include"../mediahandler/MediaHandler.h"
#include<libmsip/SipMessageTransport.h>
#include<libmsip/SipMessageContentIM.h>
#include<libmutil/Timestamp.h>
#include<libmutil/dbg.h>
#include<libmnetutil/IP4Address.h>

#include"SipSoftPhoneConfiguration.h"
#include"SipDialogVoip.h"
#include<libmsip/SipCommandString.h>
#include"../p2t/SipDialogP2T.h"
#include"../p2t/GroupListClient.h"
#include"../p2t/GroupListServer.h"
#include"../p2t/GroupList.h"
#include"../mediahandler/MediaHandler.h"
#include"PresenceMessageContent.h"

#include<assert.h>

#include<libmutil/dbg.h>

Sip::Sip(MRef<SipSoftPhoneConfiguration*> pconfig, MRef<MediaHandler*>mediaHandler){
	dialogContainer = MRef<SipDialogContainer*>(new SipDialogContainer());
	this->phoneconfig = pconfig;
	this->mediaHandler = mediaHandler;
	
#ifdef MINISIP_MEMDEBUG 
	phoneconfig.setUser("Sip/addr:phoneconfig");
#endif

	MRef<SipDialogConfig*> callconf = new SipDialogConfig(phoneconfig->inherited);
	MRef<DefaultDialogHandler*> defaultDialogHandler = 
			MRef<DefaultDialogHandler*>( new DefaultDialogHandler(dialogContainer,callconf,phoneconfig,mediaHandler) );
	
#ifdef MINISIP_MEMDEBUG
	defaultDialogHandler.setUser("Sip/attr:defaultDialogHandler");
#endif
	
	dialogContainer->setDefaultHandler(MRef<SipDialog*>(*defaultDialogHandler));

}

void Sip::init(){
	SipMessage::contentFactories.addFactory("text/plain", sipIMMessageContentFactory);
	SipMessage::contentFactories.addFactory("application/sdp", sdpSipMessageContentFactory);
	SipMessage::contentFactories.addFactory("application/xpidf+xml", presenceSipMessageContentFactory);
	phoneconfig->inherited.sipTransport->setSipSMCommandReceiver(this);
}

void Sip::setCallback(SipCallback *callback){
	this->callback = callback;
	dialogContainer->setCallback(callback);
}


SipCallback *Sip::getCallback(){
	return callback;
}

void Sip::registerMediaStream(MRef<SdpPacket*> sdppack){

}

//returns a the call id
string Sip::invite(string &user){
	SipDialogSecurityConfig securityConfig;
	ts.save( INVITE_START );
	MRef<SipDialogConfig*> callconf = MRef<SipDialogConfig*>(new SipDialogConfig(phoneconfig->inherited) );

	securityConfig = phoneconfig->securityConfig;
	
	int startAddr=0;
	if (user.substr(0,4)=="sip:")
		startAddr = 4;
	
	if (user.substr(0,4)=="sips:")
		startAddr = 5;

	bool onlydigits=true;
	for (unsigned i=0; i<user.length(); i++)
		if (user[i]<'0' || user[i]>'9')
			onlydigits=false;
	if (onlydigits && phoneconfig->usePSTNProxy){
		callconf->useIdentity( phoneconfig->pstnIdentity, false);
		securityConfig.useIdentity( phoneconfig->pstnIdentity );
	}
	else{
		securityConfig.useIdentity( phoneconfig->inherited.sipIdentity);
	}

	
	
	
	if (user.find(":", startAddr)!=string::npos){
		if (user.find("@", startAddr)==string::npos){
			return "malformed";
		}
		
		string proxy;
		string port;
		uint32_t i=startAddr;
		while (user[i]!='@')
			if (user[i]==':')
				return "malformed";
			else
				i++;
		i++;
		while (user[i]!=':')
			proxy = proxy + user[i++];
		i++;
		while (i<user.size())
			if (user[i]<'0' || user[i]>'9')
				return "malformed";
			else
				port = port + user[i++];
		
		//int iport = atoi(port.c_str());
				
//		merr << "IN URI PARSER: Parsed port=<"<< port <<"> and proxy=<"<< proxy<<">"<<end;
		
/*
		try{
			callconf->inherited.sipIdentity->sipProxy = SipProxy(proxy);
//			callconf->inherited.sipIdentity->sipProxyIpAddr = new IP4Address(proxy);
//			callconf->inherited.sipIdentity->sipProxyPort = iport;
		}catch(IPAddressHostNotFoundException *exc){
			merr << "Could not resolve PSTN proxy address:" << end;
			merr << exc->what();
			merr << "Will use default proxy instead" << end;
		}
*/
		
	}

	MRef<Session *> mediaSession = 
		mediaHandler->createSession( securityConfig );
	
	MRef<SipDialog*> voipCall( new SipDialogVoip(dialogContainer, callconf, phoneconfig, mediaSession)); 
#ifdef MINISIP_MEMDEBUG 
	voipCall.setUser("Sip");
#endif
	dialogContainer->addDialog(voipCall);
	
	CommandString inv(voipCall->getCallId(), SipCommandString::invite, user);
	ts.save( TMP );
	
        SipSMCommand cmd(SipSMCommand(inv, SipSMCommand::remote, SipSMCommand::TU));
	dialogContainer->enqueueCommand( cmd, LOW_PRIO_QUEUE, PRIO_LAST_IN_QUEUE );
	return voipCall->getCallId();
}


void Sip::run(){

	dialogContainer->run();
}


bool Sip::handleCommand(const SipSMCommand &command){
	dialogContainer->enqueueCommand(command, LOW_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
	return true;
}

void Sip::setMediaHandler( MRef<MediaHandler *> mediaHandler ){
	this->mediaHandler = mediaHandler;
}
