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
 *	    Joachim Orrblad <joachim[at]orrblad.com>
*/
#include<config.h>
#include<libmsip/SipStack.h>

#include<libmsip/SipMessageTransport.h>
#include<libmsip/SipMessageContentIM.h>
#include<libmsip/SipMIMEContent.h>
#include<libmutil/Timestamp.h>
#include<libmutil/dbg.h>
#include<libmnetutil/IP4Address.h>

#include<libmsip/SipHeaderAcceptContact.h>
#include<libmsip/SipHeaderContact.h>
#include<libmsip/SipHeaderUnsupported.h>
#include<libmsip/SipResponse.h>

#include<libmsip/SipHeaderContentLength.h>
#include<libmsip/SipHeaderUserAgent.h>
#include<libmsip/SipHeaderContentType.h>         
#include<libmsip/SipHeaderVia.h>
#include<libmsip/SipHeaderCSeq.h>                
#include<libmsip/SipHeaderWarning.h>
#include<libmsip/SipHeaderEvent.h>            
#include<libmsip/SipHeaderExpires.h>             
#include<libmsip/SipHeaderFrom.h>            
#include<libmsip/SipHeader.h>               
#include<libmsip/SipHeaderMaxForwards.h>
#include<libmsip/SipHeaderProxyAuthenticate.h>
#include<libmsip/SipHeaderProxyAuthorization.h>
#include<libmsip/SipHeaderAcceptContact.h>
#include<libmsip/SipHeaderRecordRoute.h>
#include<libmsip/SipHeaderAccept.h>
#include<libmsip/SipHeaderRoute.h>
#include<libmsip/SipHeaderReferTo.h>
#include<libmsip/SipHeaderAuthorization.h>
#include<libmsip/SipHeaderSubject.h>
#include<libmsip/SipHeaderCallID.h>
#include<libmsip/SipHeaderTo.h>
#include<libmsip/SipCommandString.h>
//#include"PresenceMessageContent.h"

#include<assert.h>

#include<libmutil/dbg.h>
#include<libmutil/cert.h>

SipStack::SipStack(MRef<SipDialog*> defaultDialog, 
		string localIpString, 
		string externalContactIP, 
		int32_t localUdpPort,
		int32_t localTcpPort,
		int32_t externalContactUdpPort,
		string defaultTransportProtocol
		,int32_t localTlsPort,
		MRef<certificate_chain *> cert_chain,
		MRef<ca_db *> cert_db
		)
{
	SipHeader::headerFactories.addFactory("Accept", sipHeaderAcceptContactFactory);
	SipHeader::headerFactories.addFactory("Accept-Contact", sipHeaderAcceptContactFactory);
	SipHeader::headerFactories.addFactory("Authorization", sipHeaderAuthorizationFactory);
	SipHeader::headerFactories.addFactory("Call-ID", sipHeaderCallIdFactory);
	SipHeader::headerFactories.addFactory("i", sipHeaderCallIdFactory);
	SipHeader::headerFactories.addFactory("Contact", sipHeaderContactFactory);
	SipHeader::headerFactories.addFactory("m", sipHeaderContactFactory);
	SipHeader::headerFactories.addFactory("Content-Length", sipHeaderContentLengthFactory);
	SipHeader::headerFactories.addFactory("l", sipHeaderContentLengthFactory);
	SipHeader::headerFactories.addFactory("Content-Type", sipHeaderContentTypeFactory);
	SipHeader::headerFactories.addFactory("c", sipHeaderContentTypeFactory);
	SipHeader::headerFactories.addFactory("CSeq", sipHeaderCSeqFactory);
	SipHeader::headerFactories.addFactory("Event", sipHeaderEventFactory);
	SipHeader::headerFactories.addFactory("Expires", sipHeaderEventFactory);
	SipHeader::headerFactories.addFactory("From", sipHeaderFromFactory);
	SipHeader::headerFactories.addFactory("f", sipHeaderFromFactory);
	SipHeader::headerFactories.addFactory("Max-Forwards", sipHeaderMaxForwardsFactory);
	SipHeader::headerFactories.addFactory("Proxy-Authenticate", sipHeaderProxyAuthenticateFactory);
	SipHeader::headerFactories.addFactory("Proxy-Authorization", sipHeaderProxyAuthorizationFactory);
	SipHeader::headerFactories.addFactory("Record-Route", sipHeaderRecordRouteFactory);
	SipHeader::headerFactories.addFactory("Refer-To", sipHeaderReferToFactory);
	SipHeader::headerFactories.addFactory("Route", sipHeaderRouteFactory);
	SipHeader::headerFactories.addFactory("Subject", sipHeaderSubjectFactory);
	SipHeader::headerFactories.addFactory("s", sipHeaderSubjectFactory);
	SipHeader::headerFactories.addFactory("To", sipHeaderToFactory);
	SipHeader::headerFactories.addFactory("t", sipHeaderToFactory);
	SipHeader::headerFactories.addFactory("User-Agent", sipHeaderUserAgentFactory);
	SipHeader::headerFactories.addFactory("Via", sipHeaderViaFactory);
	SipHeader::headerFactories.addFactory("v", sipHeaderViaFactory);
	SipHeader::headerFactories.addFactory("Warning", sipHeaderWarningFactory);
	

	 transportLayer = MRef<SipMessageTransport*>(new
			 SipMessageTransport(
				 localIpString,
				 externalContactIP,
				 defaultTransportProtocol,
				 externalContactUdpPort,
				 localUdpPort,
				 localTcpPort,
				 localTlsPort,
				 cert_chain,
				 cert_db
				 )
			 );
	
	dialogContainer = MRef<SipDialogContainer*>(new SipDialogContainer());
	
#ifdef MINISIP_MEMDEBUG 
	phoneconfig.setUser("Sip/addr:phoneconfig");
#endif

	if (defaultDialog){
		dialogContainer->setDefaultHandler(defaultDialog);
	}
}

void SipStack::init(){
	SipMessage::contentFactories.addFactory("text/plain", sipIMMessageContentFactory);
	SipMessage::contentFactories.addFactory("multipart/mixed", SipMIMEContentFactory);
	SipMessage::contentFactories.addFactory("multipart/alternative", SipMIMEContentFactory);
	SipMessage::contentFactories.addFactory("multipart/parallel", SipMIMEContentFactory);
	SipMessage::contentFactories.addFactory("message/sipfrag", sipSipMessageContentFactory);
	transportLayer->setSipSMCommandReceiver(this);
}

void SipStack::setCallback(SipCallback *callback){
	this->callback = callback;
	dialogContainer->setCallback(callback);
}

MRef<SipDialogContainer*> SipStack::getDialogContainer(){
	return dialogContainer;
}

SipCallback *SipStack::getCallback(){
	return callback;
}

#if 0
//returns a the call id
string SipStack::invite(string &user){
	SipDialogSecurityConfig securityConfig;
#ifndef _MSC_VER
	ts.save( INVITE_START );
#endif
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

#ifdef IPSEC_SUPPORT
	MRef<SipStackIpsecAPI *> ipsecSession = new MsipIpsecAPI(mediaHandler->getExtIP(), securityConfig);
	string callID = "";
	MRef<SipDialog*> voipCall( new SipDialogVoip(dialogContainer, callconf, phoneconfig, mediaSession, callID, ipsecSession )); 
	
#else	
	MRef<SipDialog*> voipCall( new SipDialogVoip(dialogContainer, callconf, phoneconfig, mediaSession)); 

#endif

#ifdef MINISIP_MEMDEBUG 
	voipCall.setUser("Sip");
#endif
	dialogContainer->addDialog(voipCall);
	
	CommandString inv(voipCall->getCallId(), SipCommandString::invite, user);
#ifndef _MSC_VER
	ts.save( TMP );
#endif
	
        SipSMCommand cmd(SipSMCommand(inv, SipSMCommand::remote, SipSMCommand::TU));
	dialogContainer->enqueueCommand( cmd, LOW_PRIO_QUEUE, PRIO_LAST_IN_QUEUE );
	return voipCall->getCallId();
}
#endif


void SipStack::run(){

	dialogContainer->run();
}


bool SipStack::handleCommand(const SipSMCommand &command){
	dialogContainer->enqueueCommand(command, LOW_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
	return true;
}


void SipStack::setDefaultHandler(MRef<SipDialog*> d){
	dialogContainer->setDefaultHandler(d);
}

void SipStack::addDialog(MRef<SipDialog*> d){
	assert(dialogContainer);
	dialogContainer->addDialog(d);
}

