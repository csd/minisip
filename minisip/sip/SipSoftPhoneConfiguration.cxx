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

/* Copyright (C) 2004, 2005
 *
 * Name
 * 	SipSoftPhoneConfiguration.cxx
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Cesc Santasusana < cesc Dot santa at@ gmail dOT com>
 * Purpose
 *          Read and write from the configuration file. 
 *
*/

/****************************************************
 * IF YOU MODIFY THE CONFIG FILE, UPDATE THE VERSION #define
 * (see below)
 *!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/

#include"SipSoftPhoneConfiguration.h"
#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipDialog.h>
#include<libmsip/SipMessageTransport.h>
#include"../soundcard/SoundIO.h"
#include"../mediahandler/MediaHandler.h"
#include"../minisip/contactdb/PhoneBook.h"
#include"../minisip/contactdb/MXmlPhoneBookIo.h"
#include"../minisip/confbackend/ConfBackend.h"
#include<fstream>
#include<libmnetutil/NetworkFunctions.h>
#include<libmnetutil/NetworkException.h>

#include"../soundcard/AudioMixer.h"

#include<libmutil/dbg.h>

//update both!!!! the str define is to avoid including itoa.h
#define CONFIG_FILE_VERSION_REQUIRED 2
#define CONFIG_FILE_VERSION_REQUIRED_STR "2"

SipSoftPhoneConfiguration::SipSoftPhoneConfiguration(): 
	securityConfig(),
//	dialogContainer(NULL),
	sip(NULL),
//	pstnProxy(NULL),
//	pstnProxyPort(0),
//	pstnNumber(string("")),
	useSTUN(false),
	stunServerPort(0),
	findStunServerFromSipUri(false),
	findStunServerFromDomain(false),
	stunDomain(""),
	useUserDefinedStunServer(false),
	proxyConnection(NULL),
//	use_gw_ip(false),
//	doRegister(false),
//	doRegisterPSTN(false),
	soundDevice(""),
	videoDevice(""),
	autodetectProxy(false),
	dynamicSipPort(false),
	usePSTNProxy(false),
	tcp_server(false),
	tls_server(false),
	ringtone(""),
	p2tGroupListServerPort(0)
{
	inherited = new SipCommonConfig;
}


void SipSoftPhoneConfiguration::save(){
	//Set the version of the file ... 
	backend->save( "version", CONFIG_FILE_VERSION_REQUIRED );
	
	backend->save( "local_udp_port", inherited->localUdpPort );
	backend->save( "local_tcp_port", inherited->localTcpPort );
	backend->save( "local_tls_port", inherited->localTlsPort );
	backend->save( "auto_answer", inherited->autoAnswer?"yes":"no");
	securityConfig.save( backend );
	
	list< MRef<SipIdentity *> >::iterator iIdent;
	uint32_t ii = 0;

	string accountPath;

	for( iIdent = identities.begin(); iIdent != identities.end(); ii++, iIdent ++){
		//cerr << "Saving identity: " << (*iIdent)->getDebugString() << endl;
		accountPath = string("account[")+itoa(ii)+"]/";
		
		(*iIdent)->lock();

		backend->save( accountPath + "account_name", (*iIdent)->identityIdentifier );
		
		backend->save( accountPath + "sip_uri", (*iIdent)->sipUsername + "@" + (*iIdent)->sipDomain );
		
		backend->save( accountPath + "proxy_addr", (*iIdent)->sipProxy.sipProxyAddressString );

		if( (*iIdent)->sipProxy.sipProxyUsername != "" ){
			backend->save( accountPath + "proxy_username", (*iIdent)->sipProxy.sipProxyUsername );
		
			backend->save( accountPath + "proxy_password", (*iIdent)->sipProxy.sipProxyPassword );
		}

		if( (*iIdent) == pstnIdentity ){
			backend->save( accountPath + "pstn_account", "yes" );
		}

		if( (*iIdent) == inherited->sipIdentity ){
			backend->save( accountPath + "default_account", "yes" );
		}
		
		if( (*iIdent)->registerToProxy ) {
			backend->save( accountPath + "register", "yes" );
			backend->save( accountPath + "register_expires", (*iIdent)->sipProxy.getDefaultExpires() );
		} else {
			backend->save( accountPath + "register", "no");
			backend->save( accountPath + "register_expires", (*iIdent)->sipProxy.getDefaultExpires() );
		}
		string transport = (*iIdent)->sipProxy.getTransport();
		
		if( transport == "TCP" ){
			backend->save( accountPath + "transport", "TCP" );
		}
		else if( transport == "TLS" ){
			backend->save( accountPath + "transport", "TLS" );
		}
		else{
			backend->save( accountPath + "transport", "UDP" );
		}

		(*iIdent)->unlock();

	}
	
	accountPath = "account[" + itoa( ii ) + "]/";
	/* Remove old identities remaining */
	while( backend->loadString( accountPath + "account_name" ) != "" ){
		backend->reset( accountPath + "account_name" );
		backend->reset( accountPath + "sip_uri" );
		backend->reset( accountPath + "proxy_addr" );
		backend->reset( accountPath + "proxy_username" );
		backend->reset( accountPath + "proxy_password" );
		backend->reset( accountPath + "pstn_account" );
		backend->reset( accountPath + "default_account" );
		backend->reset( accountPath + "register" );
		backend->reset( accountPath + "register_expires" );
		backend->reset( accountPath + "transport" );
		accountPath = "account[" + itoa( ++ii ) + "]/";
	}
	
	backend->save( "sound_device", soundDevice );
	
// 	backend->save( "mute_all_but_one", muteAllButOne? "yes":"no" ); //not used anymore
	
	backend->save( "mixer_type", soundIOmixerType );

	//Save the startup commands
	list<string>::iterator iter; 
	int idx;
	for( idx=0,  iter = startupActions.begin();
			iter != startupActions.end();
			iter++, idx++ ) {
		int pos; 
		string cmdActionsPath = string("startup_cmd[")+itoa(idx)+"]/";
		pos = (*iter).find(' ');
		string cmd = (*iter).substr( 0, pos );
		backend->save( cmdActionsPath + "command", cmd );
		pos ++; //advance to the start of the params ...
		string params = (*iter).substr( pos, (*iter).size() - pos );
		backend->save( cmdActionsPath + "params", params );
	}
		
#ifdef VIDEO_SUPPORT
	backend->save( "video_device", videoDevice );
	backend->save( "frame_width", frameWidth );
	backend->save( "frame_height", frameHeight );
#endif

	list<string>::iterator iCodec;
	uint8_t iC = 0;

	for( iCodec = audioCodecs.begin(); iCodec != audioCodecs.end(); iCodec ++, iC++ ){
		backend->save( "codec[" + itoa( iC ) + "]", *iCodec );
	}

	/************************************************************
	 * PhoneBooks
	 ************************************************************/	
	ii = 0;
	list< MRef<PhoneBook *> >::iterator iPb;
	for( iPb = phonebooks.begin(); iPb != phonebooks.end(); ii++, iPb ++ ){
		backend->save( "phonebook[" + itoa(ii) + "]", 
				     (*iPb)->getPhoneBookId() );
	}

	/************************************************************
	 * STUN settings
	 ************************************************************/
	backend->save("use_stun", (useSTUN ? "yes" : "no") );
	backend->save("stun_server_autodetect", findStunServerFromSipUri?"yes":"no");
	if (findStunServerFromDomain){
		backend->save("stun_server_domain", stunDomain );
	}
	else{
		backend->save("stun_server_domain", "");
	}

	backend->save("stun_manual_server", userDefinedStunServer);
	
	/************************************************************
	 * Advanced settings
	 ************************************************************/
	backend->save("tcp_server", tcp_server? "yes":"no");
	backend->save("tls_server", tls_server? "yes":"no");

	backend->save("ringtone", ringtone);

	backend->commit();

//	parser->saveToFile( configFileName );
	//delete( parser );

	//WARN about possible inconsistencies and the need to restart MiniSIP
	string warn;
	warn = "\n\nAttention! *********************************************\n";
	warn += "   MiniSIP needs to be restarted for changes to take effect\n\n";
	merr << warn << end;
}

string SipSoftPhoneConfiguration::load( MRef<ConfBackend *> be ){
	backend = be;

	//installConfigFile( this->configFileName );

	proxyConnection = NULL;
	usePSTNProxy = false;

	string ret = "";

	string account;
	int ii = 0;

	/* Check version first of all! */
	int32_t fileVersion;
	string fileVersion_str;
	fileVersion = backend->loadInt("version", 0);
		//get the string version also ... don't use the itoa.h
//	fileVersion_str = backend->loadString("version", "0");
	if( !checkVersion( fileVersion /*, fileVersion_str*/ ) ) {
		//check version prints a message ... 
		//here, deal with the error
//		ret = "ERROR";
		saveDefault( backend );
//		return ret;
	}
	
	do{


		string accountPath = string("account[")+itoa(ii)+"]/";
		account = backend->loadString(accountPath+"account_name");
		if( account == "" ){
			break;
		}
		MRef<SipIdentity*> ident= new SipIdentity();

		ident->setIdentityName(account);

		if( ii == 0 ){
			inherited->sipIdentity = ident;
		}

		ii++;

		string uri = backend->loadString(accountPath + "sip_uri");
		ident->setSipUri(uri);
		string proxy = backend->loadString(accountPath + "proxy_addr","");

		uint16_t proxyPort = backend->loadInt(accountPath +"proxy_port", 5060);

		ident->setDoRegister(backend->loadString(accountPath + "register","")=="yes");
		try{
			if (proxy!=""){
				ident->sipProxy = SipProxy(proxy);
			}
			else{
				autodetectProxy = true;
				proxy = SipProxy::findProxy(uri,proxyPort);
				if (proxy == "unknown"){
					ret += "Minisip could not guess your SIP proxy. Please check your settings and your network access.";
				}
				else{
					ident->sipProxy = SipProxy(proxy);
				}
			}
		}
		catch( NetworkException * exc ){
			ret +="Minisip could not resolve "
				"the SIP proxy of the account "
				+ ident->identityIdentifier + ".";
			ident->setDoRegister( false );
		}
		ident->sipProxy.sipProxyPort = proxyPort;
		string proxyUser = backend->loadString(accountPath +"proxy_username", "");

		ident->sipProxy.sipProxyUsername = proxyUser;
		string proxyPass = backend->loadString(accountPath +"proxy_password", "");
		ident->sipProxy.sipProxyPassword = proxyPass;

		ident->sipProxy.setTransport( backend->loadString(accountPath +"transport", "UDP") );

		string registerExpires = backend->loadString(accountPath +"register_expires", "");
		if (registerExpires != ""){
			ident->sipProxy.setRegisterExpires( registerExpires );
			//set the default value ... do not change this value anymore
			ident->sipProxy.setDefaultExpires( registerExpires ); 
		} 
#ifdef DEBUG_OUTPUT
		else {
			//cerr << "CESC: SipSoftPhoneConf::load : NO ident expires" << endl;
		}
		//cerr << "CESC: SipSoftPhoneConf::load : ident expires every (seconds) " << ident->sipProxy.getRegisterExpires() << endl;
		//cerr << "CESC: SipSoftPhoneConf::load : ident expires every (seconds) [default] " << ident->sipProxy.getDefaultExpires() << endl;
#endif

		if (backend->loadString(accountPath + "pstn_account","")=="yes"){
			pstnIdentity = ident;
			usePSTNProxy = true;
			ident->securitySupport = false;
		}

		if (backend->loadString(accountPath + "default_account","")=="yes"){
			inherited->sipIdentity = ident;
		}

		identities.push_back(ident);


	}while( true );

	tcp_server = backend->loadString("tcp_server", "yes") == "yes";
	tls_server = backend->loadString("tls_server", "no") == "yes";

	soundDevice =  backend->loadString("sound_device","");
	
	//not used anymore
// 	muteAllButOne = backend->loadString("mute_all_but_one", "yes") == "yes";

	soundIOmixerType = backend->loadString("mixer_type", "spatial");
// 	cerr << "sipconfigfile : soundiomixertype = " << soundIOmixerType << endl << endl;

	//Load the startup commands ... there may be more than one
	//cmd entry may not contain white spaces (anything after the space is considered
	// 	as a param
	//params list is a white space separated list of parameters:
	//Ex.
	//	call user@domain
	//	im user@domain message (the message is the last param, and may contain spaces)
	ii = 0;
	do{
		string cmdActionsPath = string("startup_cmd[")+itoa(ii)+"]/";
		string cmd = backend->loadString(cmdActionsPath + "command");
		if( cmd == "" ) {
			break;
		}
		string params = backend->loadString(cmdActionsPath + "params");
		startupActions.push_back( cmd + " " + params );
// 		cerr << "CONFIG: startup command: " << cmd << " " << params << endl;
		ii++;
	}while( true );
	
#ifdef VIDEO_SUPPORT
	videoDevice = backend->loadString( "video_device", "" );
	frameWidth = backend->loadInt( "frame_width", 176 );
	frameHeight = backend->loadInt( "frame_height", 144 );
#endif

	useSTUN = backend->loadString("use_stun","no")=="yes";
	findStunServerFromSipUri = backend->loadString("stun_server_autodetect","no")==string("yes");

	findStunServerFromDomain = backend->loadString("stun_server_domain","")!="";
	stunDomain = backend->loadString("stun_server_domain","");
	useUserDefinedStunServer = backend->loadString("stun_manual_server","")!="";
	userDefinedStunServer = backend->loadString("stun_manual_server","");
	phonebooks.clear();


	int i=0;
	string s;
	do{
		s = backend->loadString("phonebook["+itoa(i)+"]","");

		if (s!=""){
			MRef<PhoneBook *> pb;
			if (s.substr(0,7)=="file://"){
				pb = PhoneBook::create(new MXmlPhoneBookIo(s.substr(7)));
			}
			// FIXME http and other cases should go here
			if( !pb.isNull() ){
				phonebooks.push_back(pb);
			} else{
				merr << "Could not open the phonebook " << end;
			}
		}
		i++;
	}while(s!="");

	ringtone = backend->loadString("ringtone","");

	inherited->localUdpPort = backend->loadInt("local_udp_port",5060);
	inherited->externalContactUdpPort = inherited->localUdpPort; //?
	inherited->localTcpPort = backend->loadInt("local_tcp_port",5060);
	inherited->localTlsPort = backend->loadInt("local_tls_port",5061);
	inherited->autoAnswer = backend->loadString("auto_answer", "no") == "yes";

	securityConfig.load( backend );

	// FIXME: per identity security
	if( inherited->sipIdentity){
		inherited->sipIdentity->securitySupport = securityConfig.secured;
	}

	audioCodecs.clear();
	int iCodec = 0;
	string codec = backend->loadString("codec["+ itoa( iCodec ) + "]","");

	while( codec != "" && iCodec < 256 ){
		audioCodecs.push_back( codec );
		codec = backend->loadString("codec["+ itoa( ++iCodec ) +"]","");
	}
	if( audioCodecs.size() == 0 ) { //MEEC!! Error!
		//This is an error. It can happen, for example, if someone manually
		//edited the .minisip.conf file, or if it is using an older version
		mout << "ERROR! ********************************************" << end 
			<< "***  No codec was found in the .minisip.conf configuration file" << end 
			<< "***  Adding default G.711 ..." << end
			<< "ERROR! ********************************************" << end;
		audioCodecs.push_back( "G.711" );
		be->save( "codec[0]", "G.711" ); //save the changes ... 
	}
	
	return ret;

}

void SipSoftPhoneConfiguration::saveDefault( MRef<ConfBackend *> be ){
	//be->save( "version", CONFIG_FILE_VERSION_REQUIRED_STR );
	be->save( "version", CONFIG_FILE_VERSION_REQUIRED );
	
	be->save( "account[0]/account_name", "My account" );
	be->save( "account[0]/sip_uri", "username@domain.org" );
	be->save( "account[0]/proxy_addr", "sip.domain.org" );
	be->save( "account[0]/register", "yes" );
	be->save( "account[0]/proxy_port", 5060 );
	be->save( "account[0]/proxy_username", "user" );
	be->save( "account[0]/proxy_password", "password" );
	be->save( "account[0]/pstn_account", "no" );
	be->save( "account[0]/default_account", "yes" );
	
	be->save( "tcp_server", "yes" );
	be->save( "tls_server", "no" );

	be->save( "secured", "no" );
	be->save( "ka_type", "psk" );
	be->save( "psk", "Unspecified PSK" );
	be->save( "certificate", "" );
	be->save( "private_key", "" );
	be->save( "ca_certificate", "" );
	be->save( "dh_enabled", "no" );
	be->save( "psk_enabled", "no" );
	be->save( "check_cert", "yes" );
	be->save( "local_udp_port", 5060 );
	be->save( "local_tcp_port", 5060 );
	be->save( "local_tls_port", 5061 );

	be->save( "sound_device", "/dev/dsp" );
// 	be->save( "mute_all_but_one", "yes" ); //not used
	be->save( "mixer_type", "spatial" );
#if defined HAS_SPEEX && defined HAS_GSM
	be->save( "codec[0]", "speex" );
	be->save( "codec[1]", "G.711" );
	be->save( "codec[2]", "GSM" );
#elif defined HAS_SPEEX
	be->save( "codec[0]", "speex" );
	be->save( "codec[1]", "G.711" );
#elif defined HAS_GSM
	be->save( "codec[0]", "G.711" );
	be->save( "codec[1]", "GSM" );
#else
	be->save( "codec[0]", "G.711" );
#endif

	be->save( "phonebook[0]", "file://" + getDefaultPhoneBookFilename() );

//we can save startup commands ... but do nothing by default ...
//<startup_cmd><command>call</command><params>uri</params></startup_cmd>
	
	be->commit();
	
}


string SipSoftPhoneConfiguration::getDefaultPhoneBookFilename() {
	string phonebookFileName;
	char *home = getenv("HOME");
	if (home==NULL){
			merr << "WARNING: Could not determine home directory"<<end;
#ifdef WIN32
	phonebookFileName = string("c:\\minisip.addr");
#else
	phonebookFileName = string("/.minisip.addr");
#endif
	}else{
			phonebookFileName = string(home)+ string("/.minisip.addr");
	}
	return phonebookFileName;
}

bool SipSoftPhoneConfiguration::checkVersion( uint32_t fileVersion/* , string fileVersion_str */) {
	string str="";
	bool ret = false;
	if( fileVersion != CONFIG_FILE_VERSION_REQUIRED ) {
		cerr << "ERROR? Your config file is an old version (some things may not work)" << endl
			<< "    If you delete it (or rename it), next time you open minisip" << endl
			<< "    a valid one will be created" << endl;
		ret = false;
	} else {
#ifdef DEBUG_OUTPUT
		str += "Config file version checked ok!\n";
#endif
		cerr << str;
		ret = true;
	}
	return ret;
}

MRef<SipIdentity *> SipSoftPhoneConfiguration::getIdentity( string id ) {
	list< MRef<SipIdentity*> >::iterator it;

	for( it = identities.begin(); it!=identities.end(); it++ ) {
		if( (*it)->getId() == id ) {
			return (*it);
		}
	}

	return NULL;
}

MRef<SipIdentity *> SipSoftPhoneConfiguration::getIdentity( SipURI &uri ) {
	list< MRef<SipIdentity*> >::iterator it;
	
	for( it = identities.begin(); it!=identities.end(); it++ ) {
		(*it)->lock();
		if( (*it)->sipUsername == uri.getUserName() && 
				(*it)->sipDomain == uri.getIp() ) {
			(*it)->unlock();
			return (*it);
		}
		(*it)->unlock();
	}

	return NULL;
}
