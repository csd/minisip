/*
 Copyright (C) 2004-2007 the Minisip Team
 
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
 *	    Joachim Orrblad <joachim[at]orrblad.com>
 *	    Mikael Magnusson <mikma@users.sourceforge.net>
*/

#include<config.h>

#include<libminisip/signaling/sip/Sip.h>

#include<libmnetutil/NetworkException.h>
#include<libminisip/signaling/sip/DefaultDialogHandler.h>
#include<libmsip/SipMessageContentIM.h>
#include<libmsip/SipMessageContentMime.h>
#include<libmutil/Timestamp.h>
#include<libmutil/dbg.h>

#include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>
#include<libmsip/SipDialogManagement.h>
#include<libminisip/signaling/sip/SipDialogVoipClient.h>
#include<libminisip/signaling/sip/SipDialogConfVoip.h>
#include<libmsip/SipCommandString.h>

#ifdef P2T_SUPPORT
#include<libminisip/signaling/p2t/SipDialogP2T.h>
#include<libminisip/signaling/p2t/GroupListClient.h>
#include<libminisip/signaling/p2t/GroupListServer.h>
#include<libminisip/signaling/p2t/GroupList.h>
#endif

#include<libminisip/signaling/sip/PresenceMessageContent.h>
#include<libminisip/signaling/conference/ConfMessageRouter.h>

#include<libmutil/dbg.h>
#include<libmutil/termmanip.h>

#ifdef _WIN32_WCE
#	include"../include/minisip_wce_extra_includes.h"
#endif

using namespace std;

Sip::Sip(MRef<SipSoftPhoneConfiguration*> pconfig, MRef<SubsystemMedia*> sm){

	this->phoneconfig = pconfig;
	this->subsystemMedia = sm;

	MRef<SipStackConfig *> stackConfig /*= new SipStackConfig()*/;
//	// Deep copy
//	**stackConfig = **(pconfig->inherited);
	stackConfig = pconfig->sipStackConfig;

	sipstack = new SipStack(stackConfig); //FIXME: stackConfig is not set yet!

	MRef<DefaultDialogHandler*> defaultDialogHandler = 
			new DefaultDialogHandler(sipstack,
						phoneconfig,
						subsystemMedia);
	
	sipstack->setDefaultDialogCommandHandler(*defaultDialogHandler);

	//CESC -- change to use set/get
	MRef<SipDialogManagement *> shutdown = new SipDialogManagement(sipstack);	
	sipstack->setDialogManagement(*shutdown);

	//Register content factories that are able to parse the content of
	//sip messages. text/plain, multipart/mixed, multipart/alternative
	//and multipart/parallel are implemented by libmsip
	SipMessage::contentFactories.addFactory("application/sdp", sdpSipMessageContentFactory);
	SipMessage::contentFactories.addFactory("application/mikey", SipMIMEContentFactory);
	SipMessage::contentFactories.addFactory("application/xpidf+xml", presenceSipMessageContentFactory);



}


Sip::~Sip(){
}

string Sip::confjoin(string &user, minilist<ConfMember> *conflist, string confId){
//	SipDialogSecurityConfig securityConfig;
#ifdef ENABLE_TS
	ts.save( INVITE_START );
#endif
//	securityConfig = phoneconfig->securityConfig;

	int startAddr=0;
	if (user.substr(0,4)=="sip:")
		startAddr = 4;

	if (user.substr(0,5)=="sips:")
		startAddr = 5;

	bool onlydigits=true;
	for (unsigned i=0; i<user.length(); i++)
		if (user[i]<'0' || user[i]>'9')
			onlydigits=false;

	MRef<SipIdentity*> identity=phoneconfig->defaultIdentity;
	if (onlydigits && phoneconfig->usePSTNProxy){
		identity = phoneconfig->pstnIdentity;
//		securityConfig.useIdentity( phoneconfig->pstnIdentity );
	}
	else{
//		securityConfig.useIdentity( phoneconfig->defaultIdentity);
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
		   callconf->inherited.sipIdentity->sipProxy = SipRegistrar(proxy);
		//			callconf->inherited.sipIdentity->sipProxyIpAddr = new IP4Address(proxy);
		//			callconf->inherited.sipIdentity->sipProxyPort = iport;
		}catch(IPAddressHostNotFoundException & exc){
		merr << "Could not resolve PSTN proxy address:" << end;
		merr << exc.what();
		merr << "Will use default proxy instead" << end;
		}
		*/

	}


	MRef<Session *> mediaSession = 
		subsystemMedia->createSession( /*securityConfig*/ identity, "" );

	MRef<SipDialog*> voipConfCall( new SipDialogConfVoip(dynamic_cast<ConfMessageRouter*>(*sipstack->getConfCallback()), sipstack, identity, phoneconfig, mediaSession, conflist, confId, "")); 

	/*dialogContainer*/sipstack->addDialog(voipConfCall);

	CommandString inv(voipConfCall->getCallId(), SipCommandString::invite, user);
#ifdef ENABLE_TS
	ts.save( TMP );
#endif

	SipSMCommand cmd(SipSMCommand(inv, SipSMCommand::dialog_layer, SipSMCommand::dialog_layer)); //FIXME: send directly to dialog instead

	sipstack->handleCommand(cmd);
	return voipConfCall->getCallId();
}

string Sip::confconnect(string &user, string confId){
//	SipDialogSecurityConfig securityConfig;
#ifdef ENABLE_TS
	ts.save( INVITE_START );
#endif
//	securityConfig = phoneconfig->securityConfig;
	
	int startAddr=0;
	if (user.substr(0,4)=="sip:")
		startAddr = 4;
	
	if (user.substr(0,5)=="sips:")
		startAddr = 5;

	bool onlydigits=true;
	MRef<SipIdentity*> identity=phoneconfig->defaultIdentity;
	for (unsigned i=0; i<user.length(); i++)
		if (user[i]<'0' || user[i]>'9')
			onlydigits=false;
	if (onlydigits && phoneconfig->usePSTNProxy){
		identity = phoneconfig->pstnIdentity;
//		securityConfig.useIdentity( phoneconfig->pstnIdentity );
	}else{
//		securityConfig.useIdentity( phoneconfig->defaultIdentity);
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
			callconf->inherited.sipIdentity->sipProxy = SipRegistrar(proxy);
//			callconf->inherited.sipIdentity->sipProxyIpAddr = new IP4Address(proxy);
//			callconf->inherited.sipIdentity->sipProxyPort = iport;
		}catch(IPAddressHostNotFoundException & exc){
			merr << "Could not resolve PSTN proxy address:" << end;
			merr << exc->what();
			merr << "Will use default proxy instead" << end;
		}
*/
		
	}


MRef<Session *> mediaSession = 
		subsystemMedia->createSession( /*securityConfig*/ identity, "" );

	MRef<SipDialog*> voipConfCall( new SipDialogConfVoip(dynamic_cast<ConfMessageRouter*>(*sipstack->getConfCallback()), sipstack, identity, phoneconfig, mediaSession, confId)); 

	/*dialogContainer*/sipstack->addDialog(voipConfCall);
	
	CommandString inv(voipConfCall->getCallId(), SipCommandString::invite, user);
#ifdef ENABLE_TS
	ts.save( TMP );
#endif
	
        SipSMCommand cmd(SipSMCommand(inv, SipSMCommand::dialog_layer, SipSMCommand::dialog_layer));
	
	sipstack->handleCommand(cmd);
	return voipConfCall->getCallId();
}

bool Sip::start() {
	thread = NULL;
	thread = new Thread( this );
	return !thread.isNull();
}

void Sip::stop() {
	CommandString cmdstr( "", SipCommandString::sip_stack_shutdown );
	SipSMCommand sipcmd(cmdstr, SipSMCommand::dialog_layer, SipSMCommand::dispatcher);
	
	getSipStack()->handleCommand(sipcmd);
}

void Sip::join() {
	if( thread.isNull() ) { //not started as a thread ... return immediately
		return ;
	}
	
	thread->join();
}

void Sip::run(){

	try{
		sipstack->startServers();
	}
	catch( Exception & exc ){
		cerr << "ERROR: Exception thrown when creating "
			"SIP transport servers." << endl;
		cerr << exc.what() << endl;
	}

#ifdef DEBUG_OUTPUT
	mout << BOLD << "init 9/9: Registering Identities to registrar server" << PLAIN << endl;
#endif

	//We would like to use the SipSMCommand::register_all_identities, which is managed by the
	//SipDialogManagement. Unfortunately, this dialog only know about already existing dialogs ...
	cerr << endl;
	for (list<MRef<SipIdentity*> >::iterator i=phoneconfig->identities.begin() ; i!=phoneconfig->identities.end(); i++){
		if ( (*i)->registerToProxy  ){
			cerr << "Registering user "<< (*i)->getSipUri().getString() << " to proxy " << (*i)->getSipRegistrar()->getUri().getIp()<< ", requesting domain " << (*i)->getSipUri().getIp() << endl;
			CommandString reg("",SipCommandString::proxy_register);
			reg["proxy_domain"] = (*i)->getSipUri().getIp();
			reg["identityId"] = (*i)->getId();
			SipSMCommand sipcmd(reg, SipSMCommand::dialog_layer, SipSMCommand::dialog_layer);
			sipstack->handleCommand(sipcmd);
		}
	}
	cerr << endl;

	sipstack->run();
}

void Sip::setMediaHandler( MRef<SubsystemMedia*> sm ){
	this->subsystemMedia = sm;
}

