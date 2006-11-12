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

#include<config.h>

#include<libminisip/sip/SipSoftPhoneConfiguration.h>

#include<libminisip/soundcard/SoundIO.h>
#include<libminisip/mediahandler/MediaHandler.h>
#include<libminisip/contactdb/PhoneBook.h>
#include<libminisip/contactdb/MXmlPhoneBookIo.h>
#include<libminisip/contactdb/OnlineMXmlPhoneBookIo.h>
#include<libminisip/configbackend/ConfBackend.h>
#include<libminisip/configbackend/UserConfig.h>
#include<fstream>
#include<libminisip/soundcard/AudioMixer.h>
#include<libmsip/SipSimSoft.h>

#ifdef _WIN32_WCE
#	include<stdlib.h>
#	include"../include/minisip_wce_extra_includes.h"
#endif

#include<libmutil/dbg.h>
#include<libmutil/massert.h>

#include<libminisip/configbackend/OnlineConfBackend.h>

//update both!!!! the str define is to avoid including itoa.h
#define CONFIG_FILE_VERSION_REQUIRED 3
#define CONFIG_FILE_VERSION_REQUIRED_STR "3"

using namespace std;

SipSoftPhoneConfiguration::SipSoftPhoneConfiguration(): 
	//securityConfig(),
	sip(NULL),
	useSTUN(false),
	stunServerPort(0),
	findStunServerFromSipUri(false),
	findStunServerFromDomain(false),
	stunDomain(""),
	useUserDefinedStunServer(false),
	soundDeviceIn(""),
	soundDeviceOut(""),
	videoDevice(""),
	usePSTNProxy(false),
	tcp_server(false),
	tls_server(false),
	ringtone(""),
	p2tGroupListServerPort(0)
{
	inherited = new SipStackConfig;
}

SipSoftPhoneConfiguration::~SipSoftPhoneConfiguration(){
}

void SipSoftPhoneConfiguration::save(){
	massert(backend); // This will happen if save() is done without first 
			 // having done a load() (bug).
			 
	//Set the version of the file ... 
	backend->save( "version", CONFIG_FILE_VERSION_REQUIRED );
	
	backend->save( "local_udp_port", inherited->localUdpPort );
	backend->save( "local_tcp_port", inherited->localTcpPort );
	backend->save( "local_tls_port", inherited->localTlsPort );
	backend->save( "auto_answer", inherited->autoAnswer?"yes":"no");
	//securityConfig.save( backend );
	
	list< MRef<SipIdentity *> >::iterator iIdent;
	uint32_t ii = 0;

	string accountPath;

	for( iIdent = identities.begin(); iIdent != identities.end(); ii++, iIdent ++){
		//cerr << "Saving identity: " << (*iIdent)->getDebugString() << endl;
		accountPath = string("account[")+itoa(ii)+"]/";
		
		(*iIdent)->lock();

		backend->save( accountPath + "account_name", (*iIdent)->identityIdentifier );
		
		backend->save( accountPath + "sip_uri", (*iIdent)->getSipUri().getUserIpString() );
		

/*From SipDialogSecurity below*/
		backend->save(accountPath + "use_zrtp", /*use_zrtp*/ (*iIdent)->use_zrtp ? string("yes") : string("no"));
		backend->save(accountPath + "psk_enabled", (*iIdent)->pskEnabled ? string("yes") : string("no"));
		backend->save(accountPath + "dh_enabled", (*iIdent)->dhEnabled ? string("yes") : string("no"));

		backend->save(accountPath + "psk", (*iIdent)->getPsk() );


		string kaTypeString;
		switch( (*iIdent)->ka_type ){
		case KEY_MGMT_METHOD_MIKEY_DH:
			kaTypeString = "dh";
			break;
		case KEY_MGMT_METHOD_MIKEY_PSK:
			kaTypeString = "psk";
			break;
		case KEY_MGMT_METHOD_MIKEY_PK:
			kaTypeString = "pk";
		}

		backend->save(accountPath + "ka_type", kaTypeString);


		/***********************************************************
		 * Certificate settings
		 ***********************************************************/

		MRef<certificate_chain*> cert;
		if ((*iIdent)->getSim()){
			cert = (*iIdent)->getSim()->getCertificateChain();
		}else{
			cert = new certificate_chain(); //create an empty chain if no SIM to simplify code below
		}

		/* Update the certificate part of the configuration file */
		cert->lock();
		cert->init_index();
		MRef<certificate *> certItem = cert->get_next();

		/* The first element is the personal certificate, the next ones
		 * are saved as certificate_chain */
		if( !certItem.isNull() ){
			backend->save(accountPath + "certificate",certItem->get_file());
			backend->save(accountPath + "private_key",certItem->get_pk_file());
			certItem = cert->get_next();
		}

		uint32_t i = 0;

		while( !certItem.isNull() ){
			backend->save(accountPath + "certificate_chain["+itoa(i)+"]",
					certItem->get_file() );
			i++;
			certItem = cert->get_next();
		}

		cert->unlock();

		/* CA database saved in the config file */
		uint32_t iFile = 0;
		uint32_t iDir  = 0;
		MRef<ca_db*> cert_db;
		if ((*iIdent)->getSim())
			cert_db = (*iIdent)->getSim()->getCAs();
		else
			cert_db = new ca_db;

		cert_db->lock();
		cert_db->init_index();
		ca_db_item * caDbItem = cert_db->get_next();


		while( caDbItem != NULL ){
			switch( caDbItem->type ){
			case CERT_DB_ITEM_TYPE_FILE:
				backend->save(accountPath + "ca_file["+itoa(iFile)+"]",
						caDbItem->item);
				iFile ++;
				break;
			case CERT_DB_ITEM_TYPE_DIR:
				backend->save(accountPath + "ca_dir["+itoa(iDir)+"]",
						caDbItem->item);
				iDir ++;
				break;
			}

			caDbItem = cert_db->get_next();
		}

		cert_db->unlock();



/*From SipDialogSecurity above*/



		if( (*iIdent)->getSipProxy()->autodetectSettings ) {
			backend->save( accountPath + "auto_detect_proxy", "yes" );
		} else {
			backend->save( accountPath + "auto_detect_proxy", "no");		
			backend->save( accountPath + "proxy_addr", (*iIdent)->getSipProxy()->sipProxyAddressString );
			backend->save( accountPath + "proxy_port", (*iIdent)->getSipProxy()->sipProxyPort );
		}

		if( (*iIdent)->getSipProxy()->sipProxyUsername != "" ){
			backend->save( accountPath + "proxy_username", (*iIdent)->getSipProxy()->sipProxyUsername );
		
			backend->save( accountPath + "proxy_password", (*iIdent)->getSipProxy()->sipProxyPassword );
		}

		if( (*iIdent) == pstnIdentity ){
			backend->save( accountPath + "pstn_account", "yes" );
		}
		else
			backend->save( accountPath + "pstn_account", "no" );

		if( (*iIdent) == defaultIdentity ){
			backend->save( accountPath + "default_account", "yes" );
		}
		else
			backend->save( accountPath + "default_account", "no" );
		
		if( (*iIdent)->registerToProxy ) {
			backend->save( accountPath + "register", "yes" );
			backend->save( accountPath + "register_expires", (*iIdent)->getSipProxy()->getDefaultExpires() );
		} else {
			backend->save( accountPath + "register", "no");
			backend->save( accountPath + "register_expires", (*iIdent)->getSipProxy()->getDefaultExpires() );
		}
		string transport = (*iIdent)->getSipProxy()->getTransport();
		
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
		backend->reset( accountPath + "auto_detect_proxy" );
		backend->reset( accountPath + "proxy_username" );
		backend->reset( accountPath + "proxy_password" );
		backend->reset( accountPath + "pstn_account" );
		backend->reset( accountPath + "default_account" );
		backend->reset( accountPath + "register" );
		backend->reset( accountPath + "register_expires" );
		backend->reset( accountPath + "transport" );
		accountPath = "account[" + itoa( ++ii ) + "]/";
	}
	
	// Save soundDeviceIn in sound_device to be backward compatible.
	backend->save( "sound_device", soundDeviceIn );
	backend->save( "sound_device_in", soundDeviceIn );
	backend->save( "sound_device_out", soundDeviceOut );
	
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
	
	//add code to load the default network interface
	//<network_interface> into networkInterfaceName
	//We are not saving the interface name of the current localIP ...
	backend->save( "network_interface", networkInterfaceName );

	backend->commit();
}

void SipSoftPhoneConfiguration::addMissingAudioCodecs( MRef<ConfBackend *> be ){
	bool modified = false;

	// Add codecs missing in config.
	MRef<AudioCodecRegistry*> audioCodecReg = AudioCodecRegistry::getInstance();
	AudioCodecRegistry::const_iterator i;
	AudioCodecRegistry::const_iterator last;
	for( i = audioCodecReg->begin(); i != audioCodecReg->end(); i++ ){
		MRef<MPlugin *> plugin = *i;
		MRef<AudioCodec *> codec = dynamic_cast<AudioCodec*>(*plugin);

		if( !codec ){
			cerr << "SipSoftPhoneConfiguration: Not an AudioCodec: " << plugin->getName() << endl;			
			continue;
		}

		string name = codec->getCodecName();

		if( find( audioCodecs.begin(), audioCodecs.end(), name ) == audioCodecs.end() ){
			audioCodecs.push_back( name );
			mdbg << "SipSoftPhoneConfiguration: Add codec " << name << endl;
			modified = true;
		}
	}

	if( modified ){
		int iC = 0;
		list<string>::iterator iCodec;
	
		for( iCodec = audioCodecs.begin(); iCodec != audioCodecs.end(); iCodec ++, iC++ ){
			be->save( "codec[" + itoa( iC ) + "]", *iCodec );
		}
	}
}

string SipSoftPhoneConfiguration::load( MRef<ConfBackend *> be ){
	backend = be;

	//installConfigFile( this->configFileName );

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
			//inherited->sipIdentity = ident;
			defaultIdentity=ident;
		}

		ii++;

		string uri = backend->loadString(accountPath + "sip_uri");
		ident->setSipUri(uri);
		
		
/*From SipDialogSecurity below*/

		ident->securityEnabled = backend->loadString(accountPath + "secured","no")=="yes";
		//ident->use_srtp = backend->loadString(accountPath + "use_srtp","no")=="yes";
		//ident->use_srtp = backend->loadString(accountPath + "use_srtp","no")=="yes";
		//if (use_srtp) {
		ident->use_zrtp = backend->loadString(accountPath + "use_zrtp", "no") == "yes";
		//}
		ident->dhEnabled   = backend->loadString(accountPath + "dh_enabled","no")=="yes";
		ident->pskEnabled  = backend->loadString(accountPath + "psk_enabled","no")=="yes";
		ident->checkCert   = backend->loadString(accountPath + "check_cert","no")=="yes";


		if( backend->loadString(accountPath + "ka_type", "psk") == "psk" )
			ident->ka_type = KEY_MGMT_METHOD_MIKEY_PSK;

		else if( backend->loadString(accountPath + "ka_type", "psk") == "dh" )
			ident->ka_type = KEY_MGMT_METHOD_MIKEY_DH;
		else if( backend->loadString(accountPath + "ka_type", "psk") == "pk" )
			ident->ka_type = KEY_MGMT_METHOD_MIKEY_PK;
		else{
			ident->ka_type = KEY_MGMT_METHOD_MIKEY_PSK;
#ifdef DEBUG_OUTPUT
			merr << "Invalid KA type in config file, default to PSK"<<end;
#endif
		}

		string pskString = backend->loadString(accountPath + "psk","Unspecified PSK");
		ident->setPsk(pskString);



		/****************************************************************
		 * Certificate settings
		 ****************************************************************/

		string certFile = backend->loadString(accountPath + "certificate","");
		string privateKeyFile = backend->loadString(accountPath + "private_key","");

		MRef<certificate_chain*> certchain = new certificate_chain();

#ifdef ONLINECONF_SUPPORT
		if(certFile.substr(0,10)=="httpsrp://") {
			OnlineConfBack *conf;
			conf = backend->getConf();
			certificate *cert=NULL;
			cert = conf->getOnlineCert();
			certchain->add_certificate( cert );
		} else
#endif

		if( certFile != "" ){
			certificate * cert=NULL;

			try{
				cert = new certificate( certFile );
				certchain->add_certificate( cert );
			}
			catch( certificate_exception & ){
				merr << "Could not open the given certificate " << certFile <<end;
			}

			if( privateKeyFile != "" ){

				try{
					cert->set_pk( privateKeyFile );
				}
				catch( certificate_exception_pkey & ){
					merr << "The given private key " << privateKeyFile << " does not match the certificate"<<end;                        }

				catch( certificate_exception &){
					merr << "Could not open the given private key "<< privateKeyFile << end;
				}
			}
		}

		uint32_t iCertFile = 0;
		certFile = backend->loadString(accountPath + "certificate_chain[0]","");


#ifdef ONLINECONF_SUPPORT
		if(certFile.substr(0,10)=="httpsrp://") {
			OnlineConfBack *conf;
			conf = backend->getConf();
			vector<struct contdata*> res;
			string user = conf->getUser();
			conf->downloadReq(user, "certificate_chain",res);/*gets the whole chain*/
			for(int i=0;i<res.size();i++) {
				try {
					certificate *cert = new certificate((unsigned char *)res.at(i)->data,
							(size_t) res.at(i)->size,
							"httpsrp:///"+user + "/certificate_chain" );
					certchain->add_certificate( cert );
				} catch(certificate_exception &) {
					merr << "Could not open the given certificate" << end;
				}
			}
		}

		else
#endif


		while( certFile != "" ){
			try{
				certificate * cert = new certificate( certFile );
				certchain->add_certificate( cert );
			}
			catch( certificate_exception &){
				merr << "Could not open the given certificate" << end;
			}
			iCertFile ++;
			certFile = backend->loadString(accountPath + "certificate_chain["+itoa(iCertFile)+"]","");

		}

		MRef<ca_db*> cert_db = new ca_db();
		iCertFile = 0;
		certFile = backend->loadString(accountPath + "ca_file[0]","");



#ifdef ONLINECONF_SUPPORT
		if(certFile.substr(0,10)=="httpsrp://")
		{
			OnlineConfBack *conf;
			conf = backend->getConf();
			vector<struct contdata*> res;
			string user = conf->getUser();
			conf->downloadReq(user, "certificate_chain",res);
			for(int i=0;i<res.size();i++)
			{
				try{
					certificate *cert = new certificate((unsigned char *)res.at(i)->data,
							(size_t) res.at(i)->size,
							"httpsrp:///"+user + "/root_cert" );
					cert_db->add_certificate( cert );
				}
				catch( certificate_exception &){
					merr << "Could not open the CA certificate" << end;
				}
			}
		}

		else
#endif


		while( certFile != ""){
			try{
				cert_db->add_file( certFile );
			}
			catch( certificate_exception &){
				merr << "Could not open the CA certificate" << end;
			}
			iCertFile ++;
			certFile = backend->loadString(accountPath + "ca_file["+itoa(iCertFile)+"]","");

		}
		iCertFile = 0;

		certFile = backend->loadString(accountPath + "ca_dir[0]","");

		while( certFile != ""){
			try{
				cert_db->add_directory( certFile );
			}
			catch( certificate_exception &){
				merr << "Could not open the CA certificate directory " << certFile << end;
			}
			iCertFile ++;
			certFile = backend->loadString(accountPath + "ca_dir["+itoa(iCertFile)+"]","");
		}

		ident->setSim(new SipSimSoft(certchain, cert_db));

/*From SipDialogSecurity above*/


		bool autodetect = ( backend->loadString(accountPath + "auto_detect_proxy","no") == "yes" );
		
		//these two values we collect them, but if autodetect is true, they are not used
		string proxy = backend->loadString(accountPath + "proxy_addr","");
		if( proxy == "" ) {
			#ifdef DEBUG_OUTPUT
			cerr << "SipSoftPhoneConfig::load - empty proxy address ... setting autodetect = true " << endl;
			#endif
			autodetect = true; //empty proxy .. then autodetect ... just doing some checks ...
		}
		uint16_t proxyPort = (uint16_t)backend->loadInt(accountPath +"proxy_port", 5060);

		ident->setDoRegister(backend->loadString(accountPath + "register","")=="yes");
		
		string preferredTransport = backend->loadString(accountPath +"transport", "UDP");
		
		string proxret = ident->setSipProxy( autodetect, uri,  preferredTransport, proxy, proxyPort );
		
		if( proxret != "" ) {
			ret += "\n" + proxret ;
			#ifdef DEBUG_OUTPUT
			cerr << "SipSoftConfig::load - Identity::setSipProxy return = " << proxret << endl;
			#endif
		}
		
		string proxyUser = backend->loadString(accountPath +"proxy_username", "");

		ident->getSipProxy()->sipProxyUsername = proxyUser;
		string proxyPass = backend->loadString(accountPath +"proxy_password", "");
		ident->getSipProxy()->sipProxyPassword = proxyPass;

		string registerExpires = backend->loadString(accountPath +"register_expires", "");
		if (registerExpires != ""){
			ident->getSipProxy()->setRegisterExpires( registerExpires );
			//set the default value ... do not change this value anymore
			ident->getSipProxy()->setDefaultExpires( registerExpires ); 
		} 
#ifdef DEBUG_OUTPUT
		else {
			//cerr << "CESC: SipSoftPhoneConf::load : NO ident expires" << endl;
		}
		//cerr << "CESC: SipSoftPhoneConf::load : ident expires every (seconds) " << ident->getSipProxy()->getRegisterExpires() << endl;
		//cerr << "CESC: SipSoftPhoneConf::load : ident expires every (seconds) [default] " << ident->getSipProxy()->getDefaultExpires() << endl;
#endif

		if (backend->loadString(accountPath + "pstn_account","")=="yes"){
			pstnIdentity = ident;
			usePSTNProxy = true;
			ident->securityEnabled= false;
		}

		if (backend->loadString(accountPath + "default_account","")=="yes"){
			//inherited->sipIdentity = ident;
			defaultIdentity=ident;
		}

		identities.push_back(ident);


	}while( true );

	tcp_server = backend->loadString("tcp_server", "yes") == "yes";
	tls_server = backend->loadString("tls_server", "no") == "yes";

	string soundDevice = backend->loadString("sound_device","");
	soundDeviceIn = backend->loadString("sound_device_in",soundDevice);
	soundDeviceOut = backend->loadString("sound_device_out",soundDeviceIn);
	
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
	cerr << "Loaded video_device" << videoDevice << endl;
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
#ifdef ONLINECONF_SUPPORT
		   if (s.substr(0,10)=="httpsrp://")
		     {
			OnlineConfBack *conf;
			conf = backend->getConf(); 
			pb = PhoneBook::create(new OnlineMXmlPhoneBookIo(conf));
			
		     }
#endif
		   
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

	//securityConfig.load( backend ); //TODO: EEEE Load security per identity

	// FIXME: per identity security
/*	if( inherited->sipIdentity){
		inherited->sipIdentity->securitySupport = securityConfig.secured;
	}
*/

//	if ( defaultIdentity){
//		defaultIdentity->securitySupport = securityConfig.secured;
//	}

	audioCodecs.clear();
	int iCodec = 0;
	string codec = backend->loadString("codec["+ itoa( iCodec ) + "]","");

	while( codec != "" && iCodec < 256 ){
		audioCodecs.push_back( codec );
		codec = backend->loadString("codec["+ itoa( ++iCodec ) +"]","");
	}

	addMissingAudioCodecs( backend );

	//add code to load the default network interface
	//<network_interface> into networkInterfaceName
	networkInterfaceName = backend->loadString("network_interface", "");
	
	return ret;

}

void SipSoftPhoneConfiguration::saveDefault( MRef<ConfBackend *> be ){
	//be->save( "version", CONFIG_FILE_VERSION_REQUIRED_STR );
	be->save( "version", CONFIG_FILE_VERSION_REQUIRED );
	
#ifdef WIN32
	be->save( "network_interface", "{12345678-1234-1234-12345678}" );
#else
	be->save( "network_interface", "eth0" );
#endif
	
	be->save( "account[0]/account_name", "My account" );
	be->save( "account[0]/sip_uri", "username@domain.example" );
	be->save( "account[0]/proxy_addr", "sip.domain.example" );
	be->save( "account[0]/register", "yes" );
	be->save( "account[0]/proxy_port", 5060 );
	be->save( "account[0]/proxy_username", "user" );
	be->save( "account[0]/proxy_password", "password" );
	be->save( "account[0]/pstn_account", "no" );
	be->save( "account[0]/default_account", "yes" );

	be->save( "account[0]/secured", "no" );
	be->save( "account[0]/ka_type", "psk" );
	be->save( "account[0]/psk", "Unspecified PSK" );
	be->save( "account[0]/certificate", "" );
	be->save( "account[0]/private_key", "" );
	be->save( "account[0]/ca_file", "" );
	be->save( "account[0]/dh_enable", "no" );
	be->save( "account[0]/psk_enable", "no" );
	be->save( "account[0]/check_cert", "yes" );
	
	be->save( "tcp_server", "yes" );
	be->save( "tls_server", "no" );
	be->save( "local_udp_port", 5060 );
	be->save( "local_tcp_port", 5060 );
	be->save( "local_tls_port", 5061 );

#ifdef WIN32
	be->save( "sound_device", "dsound:0" );
#else
	be->save( "sound_device", "/dev/dsp" );
#endif
	
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
	return UserConfig::getFileName( "minisip.addr" );
}

bool SipSoftPhoneConfiguration::checkVersion( uint32_t fileVersion/* , string fileVersion_str */) {
	string str="";
	bool ret = false;
	if( fileVersion != CONFIG_FILE_VERSION_REQUIRED ) {
		cerr << "ERROR? Your config file is an old version (some things may not work)" << endl
			<< "    If you delete it (or rename it), next time you open minisip" << endl
			<< "    a valid one will be created (you will have to reenter your settings" << endl;
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

MRef<SipIdentity *> SipSoftPhoneConfiguration::getIdentity( SipUri &uri ) {
	list< MRef<SipIdentity*> >::iterator it;
	
	for( it = identities.begin(); it!=identities.end(); it++ ) {
		(*it)->lock();
		if( (*it)->getSipUri().getUserName() == uri.getUserName() && 
		    (*it)->getSipUri().getIp() == uri.getIp() ) {
			(*it)->unlock();
			return (*it);
		}
		(*it)->unlock();
	}

	return NULL;
}

