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

#include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>

#include<libminisip/media/soundcard/SoundIO.h>
#include<libminisip/media/soundcard/SoundDriverRegistry.h>
#include<libminisip/media/SubsystemMedia.h>
#include<libminisip/media/codecs/Codec.h>
#include<libminisip/contacts/PhoneBook.h>
#include<libminisip/config/ConfBackend.h>
#include<libminisip/config/UserConfig.h>
#include<fstream>
#include<libminisip/media/soundcard/AudioMixer.h>
#include<libmcrypto/SipSimSoft.h>
#ifdef SCSIM_SUPPORT
#include<libmcrypto/SipSimSmartCardGD.h>
#endif
#include<libmcrypto/uuid.h>

#ifdef _WIN32_WCE
#	include<stdlib.h>
#	include"../include/minisip_wce_extra_includes.h"
#endif

#include<libmutil/dbg.h>
#include<libmutil/massert.h>

#include<libmsip/SipTransport.h>
#include<libminisip/config/OnlineConfBackend.h>

#include<algorithm>

//update both!!!! the str define is to avoid including itoa.h
#define CONFIG_FILE_VERSION_REQUIRED 3
#define CONFIG_FILE_VERSION_REQUIRED_STR "3"

using namespace std;

SipSoftPhoneConfiguration::SipSoftPhoneConfiguration():
	//securityConfig(),
	//sip(NULL),
	useSTUN(false),
	stunServerPort(0),
	findStunServerFromSipUri(false),
	findStunServerFromDomain(false),
	stunDomain(""),
	useUserDefinedStunServer(false),
	useAnat(false),
	useIpv6(false),
	soundDeviceIn(""),
	soundDeviceOut(""),
	videoDevice(""),
	videoDevice2(""),
	displayFrameSize(""),
	displayFrameRate(""),
	usePSTNProxy(false),
	ringtone(""),
	p2tGroupListServerPort(0)
{
	sipStackConfig = new SipStackConfig;
	sipStackConfig->transports =
		SipTransportRegistry::getInstance()->createDefaultConfig();
}

SipSoftPhoneConfiguration::~SipSoftPhoneConfiguration(){
	std::list<MRef<PhoneBook *> >::iterator i;
	for (i=phonebooks.begin(); i!=phonebooks.end() ; i++){
// 		(*i)->free();
	}
}

void SipSoftPhoneConfiguration::save(){
	massert(backend); // This will happen if save() is done without first
			 // having done a load() (bug).

	//Set the version of the file ...
	backend->save( "version", CONFIG_FILE_VERSION_REQUIRED );

	backend->save( "local_udp_port", sipStackConfig->preferedLocalSipPort );
	backend->save( "local_tls_port", sipStackConfig->preferedLocalSipsPort );
	backend->saveBool( "auto_answer", sipStackConfig->autoAnswer );
	backend->save( "instance_id", sipStackConfig->instanceId);

	//securityConfig.save( backend );

	list< MRef<SipIdentity *> >::iterator iIdent;
	uint32_t ii = 0;

	string accountPath;

	for( iIdent = identities.begin(); iIdent != identities.end(); ii++, iIdent ++){
		//cerr << "Saving identity: " << (*iIdent)->getDebugString() << endl;
		accountPath = string("account[")+itoa(ii)+"]/";

		(*iIdent)->lock();

		backend->save( accountPath + "account_name", (*iIdent)->identityIdentifier );

		backend->save( accountPath + "sip_uri", (*iIdent)->getSipUri().getRequestUriString() );


/*From SipDialogSecurity below*/
		backend->saveBool(accountPath + "secured", (*iIdent)->securityEnabled);
		backend->saveBool(accountPath + "use_zrtp", /*use_zrtp*/ (*iIdent)->use_zrtp);
		backend->saveBool(accountPath + "psk_enabled", (*iIdent)->pskEnabled);
		backend->saveBool(accountPath + "dh_enabled", (*iIdent)->dhEnabled);
		backend->saveBool(accountPath + "check_cert", (*iIdent)->checkCert);

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
				break;
			case KEY_MGMT_METHOD_MIKEY_DHHMAC:
				kaTypeString = "dhhmac";
				break;
			case KEY_MGMT_METHOD_MIKEY_RSA_R:
				kaTypeString = "rsa-r";
				break;
			default:
				kaTypeString = "";
				break;
		}

		backend->save(accountPath + "ka_type", kaTypeString);


		/***********************************************************
		 * Certificate settings
		 ***********************************************************/

		MRef<CertificateChain*> cert;
		if ((*iIdent)->getSim()){
			cert = (*iIdent)->getSim()->getCertificateChain();
		}else{
			cert = CertificateChain::create(); //create an empty chain if no SIM to simplify code below
		}

		/* Update the Certificate part of the configuration file */
		cert->lock();
		cert->initIndex();
		MRef<Certificate *> certItem = cert->getNext();

		/* The first element is the personal Certificate, the next ones
		 * are saved as CertificateChain */
		if( !certItem.isNull() ){
			backend->save(accountPath + "certificate",certItem->getFile());
			backend->save(accountPath + "private_key",certItem->getPkFile());
			certItem = cert->getNext();
		}

		uint32_t i = 0;

		while( !certItem.isNull() ){
			backend->save(accountPath + "certificate_chain["+itoa(i)+"]",
					certItem->getFile() );
			i++;
			certItem = cert->getNext();
		}

		cert->unlock();

		/* CA database saved in the config file */
		uint32_t iFile = 0;
		uint32_t iDir  = 0;
		MRef<CertificateSet*> cert_db;
		if ((*iIdent)->getSim())
			cert_db = (*iIdent)->getSim()->getCAs();
		else
			cert_db = CertificateSet::create();

		cert_db->lock();
		cert_db->initIndex();
		MRef<CertificateSetItem*> caDbItem = cert_db->getNext();


		/*
		Since each item in caDbItem represents a single certificate it is important
		to keep track of which certificate directories have been saved, since it is
		otherwise very likely that the same directory name will be added multiple
		times (as it is very likely that several of the loaded certificates are
		stored in the same directory).
		*/
		std::set<std::string> processedCertDirs;

		while( !caDbItem.isNull() ){
			switch( caDbItem->getImportMethod() ){
				case CertificateSetItem::IMPORTMETHOD_FILE:
					backend->save(accountPath + "ca_file["+itoa(iFile)+"]",
							caDbItem->getImportParameter());
					iFile++;
					break;
				case CertificateSetItem::IMPORTMETHOD_DIRECTORY:
					if (processedCertDirs.find(caDbItem->getImportParameter()) != processedCertDirs.end()) {
						// The directory that the current certificate belongs to
						// has not been preivously saved.
						backend->save(accountPath + "ca_dir["+itoa(iDir)+"]",
								caDbItem->getImportParameter());
						processedCertDirs.insert(caDbItem->getImportParameter());
						iDir++;
					}
					break;
				default:
					merr<< "Warning: unknown Certificate object type"<<endl;
			}

			caDbItem = cert_db->getNext();
		}

		cert_db->unlock();

		// Remove old ca_file entries
		for( i = iFile; ; i++ ){
			string key = accountPath + "ca_file["+itoa(i)+"]";

			if( backend->loadString( key ) == "" ){
				break;
			}

			backend->reset( key );
		}

		// Remove old ca_dir entries
		for( i = iDir; ; i++ ){
			string key = accountPath + "ca_dir["+itoa(i)+"]";

			if( backend->loadString( key ) == "" ){
				break;
			}

			backend->reset( key );
		}



/*From SipDialogSecurity above*/



		backend->saveBool( accountPath + "auto_detect_proxy", false);

		const list<SipUri> &routeSet = (*iIdent)->getRouteSet();

		if( !routeSet.empty() ){
			SipUri proxyUri = *routeSet.begin();

			backend->save( accountPath + "proxy_addr",
				       proxyUri.getIp() );
			backend->save( accountPath + "proxy_port",
				       proxyUri.getPort() );

			string transportName = "";
			MRef<SipTransport*> transport =
				SipTransportRegistry::getInstance()->findTransport( proxyUri );

			if( transport )
				transportName = transport->getName();

			backend->save( accountPath + "transport", transportName );
		}
		else {
			backend->save( accountPath + "proxy_addr", "" );
			backend->save( accountPath + "proxy_port", 0 );
		}

		MRef<SipCredential*> cred = (*iIdent)->getCredential();

		string username;
		string password;

		if( cred ){
			username = cred->getUsername();
			password = cred->getPassword();
		}

		backend->save( accountPath + "proxy_username", username );
		backend->save( accountPath + "proxy_password", password );

		backend->saveBool( accountPath + "pstn_account",
			       (*iIdent) == pstnIdentity );

		backend->saveBool( accountPath + "default_account",
				   (*iIdent) == defaultIdentity );

		backend->saveBool( accountPath + "register",
				   (*iIdent)->registerToProxy );

		backend->save( accountPath + "register_expires", (*iIdent)->getSipRegistrar()->getDefaultExpires() );

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

// 	backend->saveBool( "mute_all_but_one", muteAllButOne ); //not used anymore

	backend->save( "mixer_type", soundIOmixerType );

	//Save the startup commands
	list<string>::iterator iter;
	int idx;
	for( idx=0,  iter = startupActions.begin();
			iter != startupActions.end();
			iter++, idx++ ) {
		size_t pos;
		string cmdActionsPath = string("startup_cmd[")+itoa(idx)+"]/";
		pos = (*iter).find(' ');
		string cmd = (*iter).substr( 0, pos );
		backend->save( cmdActionsPath + "command", cmd );
		pos ++; //advance to the start of the params ...
		string params = (*iter).substr( pos, (*iter).size() - pos );
		backend->save( cmdActionsPath + "params", params );
	}

	backend->save( "video_device", videoDevice );
////
	backend->save("video_device2", videoDevice2);
	backend->save("display_frame_size",displayFrameSize);
	backend->save("display_frame_rate",displayFrameRate);
	
	backend->save( "frame_width", frameWidth );
	backend->save( "frame_height", frameHeight );

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
	backend->saveBool("use_stun", useSTUN );
	backend->saveBool("stun_server_autodetect", findStunServerFromSipUri );
	if (findStunServerFromDomain){
		backend->save("stun_server_domain", stunDomain );
	}
	else{
		backend->save("stun_server_domain", "");
	}

	backend->save("stun_manual_server", userDefinedStunServer);

	/************************************************************
	 * SIP extensions
	 ************************************************************/
	backend->saveBool( "use_100rel", sipStackConfig->use100Rel );
	backend->saveBool( "use_anat", useAnat );

	/************************************************************
	 * Advanced settings
	 ************************************************************/
	backend->saveBool( "use_ipv6", useIpv6 );
	list< MRef<SipTransportConfig*> >::iterator j;
	for( j = sipStackConfig->transports.begin();
	     j != sipStackConfig->transports.end(); j++ ){
		MRef<SipTransportConfig*> transport = (*j);
		string name = transport->getName();

		transform( name.begin(), name.end(),
			   name.begin(), (int(*)(int))tolower );

		string label = name + "_server";
		bool enabled = transport->isEnabled();

		backend->saveBool( label, enabled );
	}

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
			mdbg("signaling/sip") << "SipSoftPhoneConfiguration: Add codec " << name << endl;
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

		ident->securityEnabled = backend->loadBool(accountPath + "secured");
		//ident->use_srtp = backend->loadBool(accountPath + "use_srtp");
		//ident->use_srtp = backend->loadBool(accountPath + "use_srtp");
		//if (use_srtp) {
		ident->use_zrtp = backend->loadBool(accountPath + "use_zrtp");
		//}
		ident->dhEnabled   = backend->loadBool(accountPath + "dh_enabled");
		ident->pskEnabled  = backend->loadBool(accountPath + "psk_enabled");
		ident->checkCert   = backend->loadBool(accountPath + "check_cert");


		if( backend->loadString(accountPath + "ka_type", "psk") == "psk" )
			ident->ka_type = KEY_MGMT_METHOD_MIKEY_PSK;

		else if( backend->loadString(accountPath + "ka_type", "psk") == "dh" )
			ident->ka_type = KEY_MGMT_METHOD_MIKEY_DH;
		else if( backend->loadString(accountPath + "ka_type", "psk") == "pk" )
			ident->ka_type = KEY_MGMT_METHOD_MIKEY_PK;
		else if( backend->loadString(accountPath + "ka_type", "psk") == "dhhmac" )
			ident->ka_type = KEY_MGMT_METHOD_MIKEY_DHHMAC;
		else if( backend->loadString(accountPath + "ka_type", "psk") == "rsa-r" )
			ident->ka_type = KEY_MGMT_METHOD_MIKEY_RSA_R;
		else{
			ident->ka_type = KEY_MGMT_METHOD_MIKEY_PSK;
#ifdef DEBUG_OUTPUT
			merr << "Invalid KA type in config file, default to PSK"<<endl;
#endif
		}

		string pskString = backend->loadString(accountPath + "psk","Unspecified PSK");
		ident->setPsk(pskString);



		/****************************************************************
		 * Certificate settings
		 ****************************************************************/
#ifdef SCSIM_SUPPORT
		string pin = backend->loadString(accountPath + "hwsim_pin","");
		if (pin.size()>0){
			MRef<SipSimSmartCardGD*> sim = new SipSimSmartCardGD;
			sim ->setPin(pin.c_str());

			assert(sim->verifyPin(0) /*TODO: FIXME: Today we quit if not correct PIN - very temp. solution*/);

			ident->setSim(*sim);

		}
#endif

		string certFile = backend->loadString(accountPath + "certificate","");
		string privateKeyFile = backend->loadString(accountPath + "private_key","");

		MRef<CertificateChain*> certchain = CertificateChain::create();

#ifdef ONLINECONF_SUPPORT
		if(certFile.substr(0,10)=="httpsrp://") {
			OnlineConfBack *conf;
			conf = backend->getConf();
			Certificate *cert=NULL;
			cert = conf->getOnlineCert();
			certchain->addCertificate( cert );
		} else
#endif

		if( certFile != "" ){
			Certificate * cert=NULL;

			try{
				cert = Certificate::load( certFile );
				certchain->addCertificate( cert );
			}
			catch( CertificateException & ){
				merr << "Could not open the given Certificate " << certFile <<endl;
			}

			if( privateKeyFile != "" ){

				try{
					cert->setPk( privateKeyFile );
				}
				catch( CertificateExceptionPkey & ){
					merr << "The given private key " << privateKeyFile << " does not match the certificate"<<endl;
				}

				catch( CertificateException &){
					merr << "Could not open the given private key "<< privateKeyFile << endl;
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
					Certificate *cert = Certificate::load((unsigned char *)res.at(i)->data,
							(size_t) res.at(i)->size,
							"httpsrp:///"+user + "/certificate_chain" );
					certchain->addCertificate( cert );
				} catch(CertificateException &) {
					merr << "Could not open the given Certificate" << endl;
				}
			}
		}

		else
#endif


		while( certFile != "" ){
			try{
				Certificate * cert = Certificate::load( certFile );
				massert(cert);
				certchain->addCertificate( cert );
			}
			catch( CertificateException &){
				merr << "Could not open the given Certificate" << endl;
			}
			iCertFile ++;
			certFile = backend->loadString(accountPath + "certificate_chain["+itoa(iCertFile)+"]","");

		}

		MRef<CertificateSet*> cert_db = CertificateSet::create();
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
					Certificate *cert = Certificate::load((unsigned char *)res.at(i)->data,
							(size_t) res.at(i)->size,
							"httpsrp:///"+user + "/root_cert" );
					cert_db->addCertificate( cert );
				}
				catch( CertificateException &){
					merr << "Could not open the CA Certificate" << endl;
				}
			}
		}

		else
#endif


		while( certFile != ""){
			try{
				cert_db->addFile( certFile );
			}
			catch( CertificateException &e){
				merr << "Could not open the CA Certificate " << e.what() << endl;
			}
			iCertFile ++;
			certFile = backend->loadString(accountPath + "ca_file["+itoa(iCertFile)+"]","");

		}
		iCertFile = 0;

		certFile = backend->loadString(accountPath + "ca_dir[0]","");

		while( certFile != ""){
			try{
				cert_db->addDirectory( certFile );
			}
			catch( CertificateException &){
				merr << "Could not open the CA Certificate directory " << certFile << endl;
			}
			iCertFile ++;
			certFile = backend->loadString(accountPath + "ca_dir["+itoa(iCertFile)+"]","");
		}

		if (!ident->getSim()){
			ident->setSim(new SipSimSoft(certchain, cert_db));
		}else{
			ident->getSim()->setCertificateChain(certchain);	//TODO: certchain and cert_db should not be attributes in SipSoftPhoneConfig any more?!
			ident->getSim()->setCAs(cert_db);
		}

/*From SipDialogSecurity above*/

		//
		// Outbound proxy
		//
		bool autodetect = backend->loadBool(accountPath + "auto_detect_proxy");

		//these two values we collect them, but if autodetect is true, they are not used
		string proxy = backend->loadString(accountPath + "proxy_addr","");
		uint16_t proxyPort = (uint16_t)backend->loadInt(accountPath +"proxy_port", 5060);

		string preferredTransport = backend->loadString(accountPath +"transport", "UDP");

		ident->setSipProxy( autodetect, uri, preferredTransport, proxy, proxyPort );

		string proxyUser = backend->loadString(accountPath +"proxy_username", "");

		string proxyPass = backend->loadString(accountPath +"proxy_password", "");
		MRef<SipCredential*> cred;

		if( proxyUser != "" ){
			cred = new SipCredential( proxyUser, proxyPass );
		}

		ident->setCredential( cred );

		SipUri registrarUri;

		registrarUri.setProtocolId(ident->getSipUri().getProtocolId());
		registrarUri.setIp( ident->getSipUri().getIp() );
		registrarUri.makeValid( true );
		MRef<SipRegistrar*> registrar = new SipRegistrar( registrarUri );

		ident->setSipRegistrar( registrar );

		ident->setDoRegister(backend->loadBool(accountPath + "register"));
		string registerExpires = backend->loadString(accountPath +"register_expires", "");
		if (registerExpires != ""){
			ident->getSipRegistrar()->setRegisterExpires( registerExpires );
			//set the default value ... do not change this value anymore
			ident->getSipRegistrar()->setDefaultExpires( registerExpires );
		}
#ifdef DEBUG_OUTPUT
		else {
			//cerr << "CESC: SipSoftPhoneConf::load : NO ident expires" << endl;
		}
		//cerr << "CESC: SipSoftPhoneConf::load : ident expires every (seconds) " << ident->getSipRegistrar()->getRegisterExpires() << endl;
		//cerr << "CESC: SipSoftPhoneConf::load : ident expires every (seconds) [default] " << ident->getSipRegistrar()->getDefaultExpires() << endl;
#endif

		if (backend->loadBool(accountPath + "pstn_account")){
			pstnIdentity = ident;
			usePSTNProxy = true;
// 			ident->securityEnabled= false;
		}

		if (backend->loadBool(accountPath + "default_account")){
			//sipStackConfig->sipIdentity = ident;
			defaultIdentity=ident;
		}

		identities.push_back(ident);


	}while( true );

	useIpv6 = backend->loadBool( "use_ipv6", true );

	list< MRef<SipTransportConfig*> >::iterator j;
	for( j = sipStackConfig->transports.begin();
	     j != sipStackConfig->transports.end(); j++ ){
		MRef<SipTransportConfig*> config = (*j);
		string name = config->getName();

		MRef<SipTransport*> transport =
			SipTransportRegistry::getInstance()->findTransportByName( name );
		bool secure = transport->isSecure();

		transform( name.begin(), name.end(),
			   name.begin(), (int(*)(int))tolower );

		string label = name + "_server";

		config->setEnabled( backend->loadBool( label, !secure) );
	}

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

	videoDevice = backend->loadString( "video_device", "" );
///
	videoDevice2 = backend->loadString( "video_device2","");	
	displayFrameSize = backend->loadString("display_frame_size","");
	displayFrameRate = backend->loadString("display_frame_rate","");

	//Even if we can't send video, we might be able to display it.
	//Therefore this is not within the VIDEO_SUPPORT ifdef
	frameWidth = backend->loadInt( "frame_width", 176 );
	frameHeight = backend->loadInt( "frame_height", 144 );

	sipStackConfig->use100Rel = backend->loadBool("use_100rel");
	useAnat = backend->loadBool("use_anat");

	useSTUN = backend->loadBool("use_stun");
	findStunServerFromSipUri = backend->loadBool("stun_server_autodetect");

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
			pb = PhoneBookIoRegistry::getInstance()->createPhoneBook( s );

			// FIXME http and other cases should go here
			if( !pb.isNull() ){
				phonebooks.push_back(pb);
			} else{
				merr << "Could not open the phonebook " << endl;
			}
		}
		i++;
	}while(s!="");

	ringtone = backend->loadString("ringtone","");

	sipStackConfig->preferedLocalSipPort = backend->loadInt("local_udp_port",5060);
	sipStackConfig->externalContactUdpPort = 0; //sipStackConfig->preferedLocalUdpPort; //?
	sipStackConfig->preferedLocalSipsPort = backend->loadInt("local_tls_port",5061);
	sipStackConfig->autoAnswer = backend->loadBool("auto_answer");
	sipStackConfig->instanceId = backend->loadString("instance_id");

	if( sipStackConfig->instanceId.empty() ){
		MRef<Uuid*> uuid = Uuid::create();

		sipStackConfig->instanceId = "\"<urn:uuid:" + uuid->toString() + ">\"";
	}

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

static void getDefaultSoundDevices( string &soundDeviceIn,
				    string &soundDeviceOut ){
	MRef<SoundDriver*> defaultDriver =
		SoundDriverRegistry::getInstance()->getDefaultDriver();

	if( defaultDriver ){
		SoundDeviceName inputName;
		SoundDeviceName outputName;

		if( defaultDriver->getDefaultInputDeviceName( inputName ) ){
			soundDeviceIn = inputName.getName();
		}

		if( defaultDriver->getDefaultOutputDeviceName( outputName ) ){
			soundDeviceOut = outputName.getName();
		}
	}

	if( soundDeviceIn.empty() ){
		mdbg("signaling/sip") << "SipSoftPhoneConfiguration: Using fallback sound input driver" << endl;
#ifdef WIN32
		soundDeviceIn = "dsound:0";
#else
		soundDeviceIn = "/dev/dsp";
#endif
	}

	if( soundDeviceOut.empty() ){
		mdbg("signaling/sip") << "SipSoftPhoneConfiguration: Using fallback sound output driver" << endl;
#ifdef WIN32
		soundDeviceOut = "dsound:0";
#else
		soundDeviceOut = "/dev/dsp";
#endif
	}
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
	be->saveBool( "account[0]/register", true );
	be->save( "account[0]/proxy_port", 5060 );
	be->save( "account[0]/proxy_username", "user" );
	be->save( "account[0]/proxy_password", "password" );
	be->saveBool( "account[0]/pstn_account", false );
	be->saveBool( "account[0]/default_account", true );

	be->saveBool( "account[0]/secured", false );
	be->save( "account[0]/ka_type", "psk" );
	be->save( "account[0]/psk", "Unspecified PSK" );
	be->save( "account[0]/certificate", "" );
	be->save( "account[0]/private_key", "" );
	be->save( "account[0]/ca_file", "" );
	be->saveBool( "account[0]/dh_enabled", false );
	be->saveBool( "account[0]/psk_enabled", false );
	be->saveBool( "account[0]/check_cert", true );

	be->saveBool( "udp_server", true );
	be->saveBool( "tcp_server", true );
	be->saveBool( "tls_server", false );
	be->save( "local_udp_port", 5060 );
	be->save( "local_tcp_port", 5060 );
	be->save( "local_tls_port", 5061 );

	string soundDeviceIn;
	string soundDeviceOut;

	getDefaultSoundDevices( soundDeviceIn, soundDeviceOut );

	be->save( "sound_device", soundDeviceIn );
	be->save( "sound_device_in", soundDeviceIn );
	be->save( "sound_device_out", soundDeviceOut );

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

MRef<SipIdentity *> SipSoftPhoneConfiguration::getIdentity( const SipUri &uri ) {
	list< MRef<SipIdentity*> >::iterator it;
	SipUri tmpUri = uri;

	// Search registered contact addresses
	for( it = identities.begin(); it!=identities.end(); it++ ) {
		MRef<SipIdentity*> &identity =  *it;

		identity->lock();
		const list<SipUri> &contacts =
			identity->getRegisteredContacts();

		if( find( contacts.begin(), contacts.end(), tmpUri ) !=
		    contacts.end() ){
#ifdef DEBUG_OUTPUT
			cerr << "Found registered identity " << identity->identityIdentifier << endl;
#endif
			identity->unlock();
			return identity;
		}

		identity->unlock();
	}

	// Search identity AOR user names
	for( it = identities.begin(); it!=identities.end(); it++ ) {
		MRef<SipIdentity*> &identity =  *it;

		identity->lock();

		SipUri identityUri =
			identity->getSipUri();

		if( identityUri.getUserName() == tmpUri.getUserName() ){
#ifdef DEBUG_OUTPUT
			cerr << "Found AOR identity " << identity->identityIdentifier << endl;
#endif
			identity->unlock();
			return identity;
		}
		identity->unlock();
	}

	return NULL;
}

