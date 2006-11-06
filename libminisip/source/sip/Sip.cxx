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
 *	    Joachim Orrblad <joachim[at]orrblad.com>
*/

#include<config.h>

#include<libminisip/sip/Sip.h>

#include<libmnetutil/NetworkException.h>
#include<libminisip/sip/DefaultDialogHandler.h>
#include<libmsip/SipMessageContentIM.h>
#include<libmsip/SipMessageContentMime.h>
#include<libmutil/Timestamp.h>
#include<libmutil/dbg.h>

#include<libminisip/sip/SipSoftPhoneConfiguration.h>
#include<libmsip/SipDialogManagement.h>
#include<libminisip/sip/SipDialogVoipClient.h>
#include<libminisip/sip/SipDialogConfVoip.h>
#include<libmsip/SipCommandString.h>
#include<libminisip/p2t/SipDialogP2T.h>
#include<libminisip/p2t/GroupListClient.h>
#include<libminisip/p2t/GroupListServer.h>
#include<libminisip/p2t/GroupList.h>
#include<libminisip/sip/PresenceMessageContent.h>
#include<libminisip/conference/ConfMessageRouter.h>

#include<libmutil/dbg.h>
#include<libmutil/termmanip.h>

#ifdef _WIN32_WCE
#	include"../include/minisip_wce_extra_includes.h"
#endif

using namespace std;

Sip::Sip(MRef<SipSoftPhoneConfiguration*> pconfig, MRef<MediaHandler*>mediaHandler,
		string localIpString, 
		string externalContactIP, 
		int32_t localUdpPort,
		int32_t localTcpPort,
		int32_t externalContactUdpPort,
		int32_t localTlsPort,
		MRef<certificate_chain *> cert_chain,
		MRef<ca_db *> cert_db
		){

	this->phoneconfig = pconfig;
	this->mediaHandler = mediaHandler;

	MRef<SipStackConfig *> stackConfig = new SipStackConfig;
	stackConfig->localIpString = localIpString;
	stackConfig->externalContactIP= externalContactIP;
	stackConfig->localUdpPort=localUdpPort;
	stackConfig->localTcpPort=localTcpPort;
	stackConfig->externalContactUdpPort= externalContactUdpPort;
	stackConfig->localTlsPort= localTlsPort;
	
	sipstack = new SipStack(stackConfig, cert_chain,cert_db);

	MRef<DefaultDialogHandler*> defaultDialogHandler = 
			new DefaultDialogHandler(sipstack,
						phoneconfig,
						mediaHandler);
	
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

void Sip::handleCommand(string subsystem, const CommandString &cmd){
	assert(subsystem=="sip");
	sipstack->handleCommand(cmd);
}

CommandString Sip::handleCommandResp(string subsystem, const CommandString &cmd){
	assert(subsystem=="sip");
	assert(cmd.getOp()=="invite");//TODO: no assert, return error message instead
	
	string user = cmd.getParam();
	bool gotAtSign;
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
	MRef<SipIdentity *> id;
	
	for (unsigned i=0; i<user.length(); i++)
		if (user[i]<'0' || user[i]>'9')
			onlydigits=false;

	id = ( onlydigits && phoneconfig->usePSTNProxy )?
			phoneconfig->pstnIdentity:
			phoneconfig->defaultIdentity;

	if( !id ){
		merr << "ERROR: could not determine what local identity to use" << endl;
	}

//	securityConfig.useIdentity( id );

	gotAtSign = ( user.find("@", startAddr) != string::npos );

#if 0	
	// Uri check not compatible with IPv6
	if (user.find(":", startAddr)!=string::npos){
		string proxy;
		string port;
		uint32_t i=startAddr;
		while (user[i]!='@')
			if (user[i]==':'){
				//return "malformed";
				return CommandString("malformed","");;
			}else
				i++;
		i++;
		while (user[i]!=':')
			proxy = proxy + user[i++];
		i++;
		while (i<user.size())
			if (user[i]<'0' || user[i]>'9'){
				//return "malformed";
				return CommandString("malformed","");
	}else
				port = port + user[i++];
		
		
	}
#endif

	if( !gotAtSign && id ){
		id->lock();
		user += "@" + id->sipDomain;
		id->unlock();
	}

#ifdef DEBUG_OUTPUT
        cerr << "Before new mediaSession" << endl;
#endif
	MRef<Session *> mediaSession = 
		mediaHandler->createSession( /*securityConfig*/ id );
#ifdef DEBUG_OUTPUT
        cerr << "After new mediaSession" << endl;
#endif
	
	MRef<SipDialog*> voipCall( new SipDialogVoipClient(sipstack, id, phoneconfig, mediaSession)); 

#ifdef DEBUG_OUTPUT
	cerr << "Before addDialog" << endl;
#endif	
	/*dialogContainer*/sipstack->addDialog(voipCall);
#ifdef DEBUG_OUTPUT
	cerr << "After addDialog" << endl;
#endif
	CommandString inv(voipCall->getCallId(), SipCommandString::invite, user);
#ifdef ENABLE_TS
	ts.save( TMP );
#endif
	
        SipSMCommand c(SipSMCommand(inv, SipSMCommand::dialog_layer, SipSMCommand::dialog_layer)); //TODO: send directly to dialog instead
	
#ifdef DEBUG_OUTPUT
        cerr << "Before handleCommand" << endl;
#endif
	sipstack->handleCommand(c);
#ifdef DEBUG_OUTPUT
        cerr << "After handleCommand" << endl;
#endif
	
	mediaSession->setCallId( voipCall->getCallId() );

	string cid = voipCall->getCallId();

	CommandString ret(cid,"invite_started");
	return ret;
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

	if (user.substr(0,4)=="sips:")
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
		   callconf->inherited.sipIdentity->sipProxy = SipProxy(proxy);
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
		mediaHandler->createSession( /*securityConfig*/ identity );

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
	
	if (user.substr(0,4)=="sips:")
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
			callconf->inherited.sipIdentity->sipProxy = SipProxy(proxy);
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
		mediaHandler->createSession( /*securityConfig*/ identity );

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
		if (phoneconfig->tcp_server){
#ifdef DEBUG_OUTPUT
			mout << BOLD << "init 8.2/9: Starting TCP transport worker thread" << PLAIN << end;
#endif

			sipstack->startTcpServer();

		}

		if (phoneconfig->tls_server){
			//if( phoneconfig->securityConfig.cert.isNull() ){
			if( !phoneconfig->defaultIdentity->getSim() || phoneconfig->defaultIdentity->getSim()->getCertificateChain().isNull() ){
				merr << "Certificate needed for TLS server. You will not be able to receive incoming TLS connections." << end;
			}
			else{
#ifdef DEBUG_OUTPUT
				mout << BOLD << "init 8.3/9: Starting TLS transport worker thread" << PLAIN << end;
#endif
				sipstack->startTlsServer();
			}
		}
	}
	catch( NetworkException & exc ){
		cerr << "ERROR: Exception thrown when creating"
			"TCP/TLS servers." << endl;
		cerr << exc.what() << endl;
	}

#ifdef DEBUG_OUTPUT
	mout << BOLD << "init 9/9: Registering Identities to registrar server" << PLAIN << end;
#endif

	//We would like to use the SipSMCommand::register_all_identities, which is managed by the
	//SipDialogManagement. Unfortunately, this dialog only know about already existing dialogs ...
	cerr << endl;
	for (list<MRef<SipIdentity*> >::iterator i=phoneconfig->identities.begin() ; i!=phoneconfig->identities.end(); i++){
		if ( (*i)->registerToProxy  ){
			cerr << "Registering user "<< (*i)->getSipUri() << " to proxy " << (*i)->getSipProxy()->sipProxyAddressString<< ", requesting domain " << (*i)->sipDomain << endl;
			CommandString reg("",SipCommandString::proxy_register);
			reg["proxy_domain"] = (*i)->sipDomain;
			reg["identityId"] = (*i)->getId();
			SipSMCommand sipcmd(reg, SipSMCommand::dialog_layer, SipSMCommand::dialog_layer);
			sipstack->handleCommand(sipcmd);
		}
	}
	cerr << endl;

	sipstack->run();
}

void Sip::setMediaHandler( MRef<MediaHandler *> mediaHandler ){
	this->mediaHandler = mediaHandler;
}
