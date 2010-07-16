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

#ifndef _MSC_VER
#	include<unistd.h>
#	include<iostream>
#endif

#include<libminisip/Minisip.h>

#include<exception>

#include<libmutil/Timestamp.h>
#include<libmutil/TextUI.h>
#include<libmutil/Thread.h>
#include<libminisip/MinisipExceptions.h>

#ifdef _MSC_VER
#include<compilation_config_w32_wce.h>
#endif

#ifndef WIN32
#	ifdef DEBUG_OUTPUT
#		include<signal.h>
#		include<libmutil/Timestamp.h>
#	endif
#endif

#include<libmutil/termmanip.h>
#include<libmutil/MessageRouter.h>
#include<libmutil/MPlugin.h>
#include<libmutil/Library.h>

#include<libmnetutil/UDPSocket.h>
#include<libmnetutil/NetworkFunctions.h>
#include<libmnetutil/NetUtil.h>
#include<libmnetutil/NetworkException.h>

#include<libmcrypto/init.h>
#include<libmnetutil/init.h>

#include<libmikey/KeyAgreementDH.h>

#include<libmsip/SipUtils.h>
#include<libmsip/SipCommandString.h>

#include<libminisip/gui/Gui.h>
#include<libminisip/gui/ConsoleDebugger.h>
#include<libminisip/signaling/sip/Sip.h>
#include<libminisip/gui/LogEntry.h>
#include<libminisip/contacts/ContactDb.h>
#include<libminisip/media/SubsystemMedia.h>
#include<libminisip/media/RealtimeMedia.h>
#include<libminisip/ipprovider/IpProvider.h>
#include<libminisip/signaling/conference/ConferenceControl.h>
#include<libminisip/signaling/conference/ConfCallback.h>
#include<libminisip/config/ConfBackend.h>
#include<libminisip/contacts/PhoneBook.h>
#include<libminisip/signaling/conference/ConfMessageRouter.h>
#include<libminisip/media/soundcard/SoundDriverRegistry.h>
#include<libminisip/media/soundcard/Resampler.h>
#include<libminisip/media/codecs/Codec.h>

#ifdef ZRTP_SUPPORT
#include<libminisip/media/zrtp/ZrtpHostBridgeMinisip.h>
#endif

#ifdef SNAKE_SUPPORT
#include"subsystem_signaling/snakews/SnakeClient.h"
#endif

#include<stdlib.h>

#ifdef OSSO_SUPPORT
#include<libosso.h>
#endif

#ifdef SCSIM_SUPPORT
#include<libmcrypto/SmartCard.h>
#endif

using namespace std;

#ifndef WIN32
#ifdef DEBUG_OUTPUT
static void signal_handler( int signal ){
	if( signal == SIGUSR1 ){
		merr << "ERROR: Minisip was stopped (signal SIGUSR1 caught)" << endl;
		ts.print();
		exit( 1 );
	} else {
		merr << "ERROR: Minisip was stopped (some signal caught)" << endl;
	}
}
#endif
#endif

#ifdef WIN32
# define DIR_SEPARATOR "\\"
# define PATH_SEPARATOR ";"
#else
# define DIR_SEPARATOR "/"
# define PATH_SEPARATOR ":"
#endif

bool Minisip::pluginsLoaded=false;

static string buildPluginPath( const string &argv0 ){
	string pluginPath;
	size_t pos = argv0.find_last_of(DIR_SEPARATOR);
	string prefixDir;

	if( pos != string::npos )
		prefixDir = argv0.substr( 0, pos ) + DIR_SEPARATOR + "..";
	else{
		prefixDir = "..";
	}

	pluginPath += prefixDir + DIR_SEPARATOR + "lib" + DIR_SEPARATOR + PACKAGE + DIR_SEPARATOR + "plugins";
	return pluginPath;
}

static void loadPlugins(const string &argv0){
	SoundDriverRegistry::getInstance();
	AudioCodecRegistry::getInstance();
	ConfigRegistry::getInstance();
	MediaRegistry::getInstance();
	ResamplerRegistry::getInstance();
	PhoneBookIoRegistry::getInstance();

	MRef<MPluginManager *> pluginManager = MPluginManager::getInstance();

	string pluginPath;
	const char *path = getenv( "MINISIP_PLUGIN_PATH" );

	if( !path ){
		if( argv0 != "" )
			pluginPath += buildPluginPath( argv0 );

#ifdef MINISIP_PLUGINDIR
		if( pluginPath.size() > 0 )
			pluginPath += PATH_SEPARATOR;
		pluginPath += MINISIP_PLUGINDIR;
#endif

	}
	else
		pluginPath = path;


	// Load the video plugin first, if available, since it need to
	// initialize the grabber and display registries before those
	// plugins are loaded.
	pluginManager->setSearchPath( pluginPath );
	pluginManager->loadFromFile( "mvideo.la" );
	pluginManager->loadFromDirectory( pluginPath );
}

void Minisip::doLoadPlugins(char **argv){
	pluginsLoaded=true;

	string pluginPath = argv ? argv[0] : "";
	int i=0;
	char *a=NULL;
	do{	
		char *path;
		if (argv)
			a = argv[i++];
		if (a && a[0]=='-'){
			switch (a[1]){
				case 'p':
					path = argv[i++];
					if (path)
						pluginPath = path;
					else
						throw MinisipBadArgument("bad argument for -p");
					break;

				case 'c':
					i++;
					break;
				default:
					throw MinisipBadArgument((string("bad argument: ") + a[1]).c_str());
					break;
			}
		
		}

	}while(a);

	libmnetutilInit();
	libmcryptoInit();

	srand((unsigned int)time(0));


	#ifdef DEBUG_OUTPUT
	mdbg("init") << "Loading plugins"<<endl;
	#endif

	loadPlugins( pluginPath );
}

/**
 *
 * -p <plugin path>
 * -c <configuration file>
*/
Minisip::Minisip( MRef<Gui *> g, int /*argc*/, char **argv ) : gui(g){

	if (!pluginsLoaded)
		doLoadPlugins(argv);

	massert(g);

	int i=0;
	char *a=NULL;
	do{	
		char *path;
		if (argv)
			a = argv[i++];
		if (a && a[0]=='-'){
			switch (a[1]){
				case 'p': //handled by loadPlugins
					i++;
					break;

				case 'c':
					path = argv[i++];
					if (path)
						confPath = path;
					else
						throw MinisipBadArgument("bad argument for -c");
					break;
				default:
					throw MinisipBadArgument((string("bad argument: ") + a[1]).c_str());
					break;
			}
		
		}

	}while(a);


	#ifndef WIN32
		#ifdef DEBUG_OUTPUT
		signal( SIGUSR1, signal_handler );
	#endif
	#endif


	#ifdef DEBUG_OUTPUT
	mout << "Initializing NetUtil"<<endl;
	#endif

	if ( ! NetUtil::init()){
		//printf("ERROR: Could not initialize Netutil package\n");
		merr << "ERROR: Could not initialize NetUtil package"<<endl;
		exit();
	}

	#ifdef DEBUG_OUTPUT
	cerr << "Creating SipSoftPhoneConfiguration"<< endl;
	#endif
	phoneConf =  new SipSoftPhoneConfiguration();
	//phoneConf->sip=NULL;

	#ifdef DEBUG_OUTPUT
	mout << BOLD << "init 1/9: Creating contact database" << PLAIN << endl;
	#endif

	/* Create the global contacts database */
	ContactDb *contactDb = new ContactDb();
	ContactEntry::setDb(contactDb);
	
	#ifdef OSSO_SUPPORT
		osso_context_t * ossoCtxt = NULL;
		ossoCtxt = osso_initialize( PACKAGE_NAME, PACKAGE_VERSION, TRUE, NULL );
		if( !ossoCtxt ){
			mdbg("init") << "Could not initialize osso context" << endl;
		}
	#endif

	#ifdef DEBUG_OUTPUT
		cerr << "Setting contact db"<< endl;
	#endif
	gui->setContactDb(contactDb);
}

Minisip::~Minisip(){
}

int Minisip::stop(){
	if( ! sip.isNull() ) { //it may not be initialized ... 
		//Send a shutdown command to the sip stack ... 
		//it will take care of de-registering and closing on-going calls
		CommandString cmdstr( "", SipCommandString::sip_stack_shutdown );
		SipSMCommand sipcmd(cmdstr, SipSMCommand::dialog_layer, SipSMCommand::dispatcher);
		sip->getSipStack()->handleCommand(sipcmd);
		sip->stop();
	}
	return 1;
}

int Minisip::join(){
	if( ! sip.isNull() ) { //it may not be initialized ... 
		sip->join();
		sip->getSipStack()->free();
		sip = NULL;
	}
	gui->setCallback( NULL );
	gui->setSipSoftPhoneConfiguration( NULL );

	stopDebugger();

	messageRouter->clear();
	messageRouter=NULL;

	phoneConf = NULL;
	//mediaHandler = NULL;
	confMessageRouter->setGui(NULL);
	confMessageRouter = NULL;
	gui = NULL;

	return 1;
}


int Minisip::exit(){
	mout << BOLD << "Minisip is Shutting down!!!" << PLAIN << endl;

	stop();
#ifdef DEBUG_OUTPUT
		mout << "Waiting for the SipStack to close ..." << endl;
#endif
	join();
	
	mout << endl << endl << BOLD << "Minisip can't wait to see you again! Bye!" << PLAIN << endl << endl << endl;
	return 1;
}

int Minisip::startSip() {
	int ret = 1;

#ifdef DEBUG_OUTPUT
	cerr << "Thread 2 running - doing initParseConfig"<< endl;
#endif	

	if( initParseConfig() < 0 ){
		merr << "Minisip::startSip::initParseConfig - fatal error" << endl;
		return -1;
	}

	try{
		messageRouter =  new MessageRouter();
		confMessageRouter =  new ConfMessageRouter();

#ifdef DEBUG_OUTPUT
		mout << BOLD << "init 4/9: Creating IP provider" << PLAIN << endl;
#endif
		MRef<IpProvider *> ipProvider = IpProvider::create( phoneConf );
		MRef<IpProvider *> ip6Provider;
		if( phoneConf->useIpv6 ){
			ip6Provider = IpProvider::create( phoneConf, true );
		}
		//#ifdef DEBUG_OUTPUT
		//                mout << BOLD << "init 5/9: Creating SIP transport layer" << PLAIN << endl;
		//#endif
		string localIpString;
		string externalContactIP;

		// FIXME: This should be done more often
		localIpString = externalContactIP = ipProvider->getExternalIp();                

		bool done;
		int port = phoneConf->sipStackConfig->preferedLocalSipPort;
		MRef<UDPSocket*> udpSocket;
		int ntries = 8;
		do{
			done=true;
			try{
				udpSocket = new UDPSocket( port );
			}catch(NetworkException &){
				phoneConf->sipStackConfig->preferedLocalSipPort = port = 32768+rand()%32000;
				done=false;
			
			}
			ntries--;
		}while (!done && ntries>0);

		// TODO call getExternalPort on the real UDPSocket:s
		// when they are created
		phoneConf->sipStackConfig->externalContactUdpPort = ipProvider->getExternalPort( udpSocket );
		phoneConf->sipStackConfig->localIpString = externalContactIP;
		phoneConf->sipStackConfig->externalContactIP = externalContactIP;
		if( ip6Provider )
			phoneConf->sipStackConfig->localIp6String = ip6Provider->getExternalIp();
		udpSocket=NULL;

#ifdef DEBUG_OUTPUT
		mout << BOLD << "init 5/9: Creating MediaHandler" << PLAIN << endl;
#endif
		subsystemMedia = new SubsystemMedia( phoneConf, ipProvider, ip6Provider );
		messageRouter->addSubsystem("media",*subsystemMedia);
		subsystemMedia->setMessageRouterCallback(*messageRouter);

		if( consoleDbg ){
			consoleDbg->setMediaHandler( /*mediaHandler*/ subsystemMedia );
		}

		//Session::registry = *mediaHandler; Moved to MediaHandler::MediaHandler

		/* Hack: precompute a KeyAgreementDH */
		//	Session::precomputedKa = new KeyAgreementDH( phoneConf->securityConfig.cert, 
		//                phoneConf->securityConfig.cert_db, DH_GROUP_OAKLEY5 );

#ifdef DEBUG_OUTPUT
		mout << BOLD << "init 6/9: Creating MSip SIP stack" << PLAIN << endl;
#endif

		MRef<SipSim*> sim = phoneConf->defaultIdentity->getSim();
		if (sim){
			phoneConf->sipStackConfig->cert = sim->getCertificateChain();
			phoneConf->sipStackConfig->cert_db = sim->getCAs();
		}

		//save Sip object in Minisip::sip ...
		this->sip=new Sip(phoneConf,/*mediaHandler*/ subsystemMedia);
		//sip->init();

//		phoneConf->sip = sip;
		phoneConf->sipStack = sip->getSipStack();

		sip->getSipStack()->setCallback(*messageRouter);
		//sip->getSipStack()->setConfCallback(*confMessageRouter);
		//TODO: Send the callback to the  conference dialog
		//instead.

		//messageRouter->addSubsystem("sip",*sip);
		messageRouter->addSubsystem("sip",*sip->getSipStack());

		confMessageRouter->setSip(sip);
#ifdef ZRTP_SUPPORT
		ZrtpHostBridgeMinisip::initialize(sip->getSipStack()->getTimeoutProvider());
#endif
		/* Load the plugins at this stage */
		//		int32_t pluginCount = MPlugin::loadFromDirectory( PLUGINS_PATH );

		//		cerr << "Loaded " << pluginCount << " plugins from " << PLUGINS_PATH << endl;

#ifdef DEBUG_OUTPUT
		mout << BOLD << "init 7/9: Connecting GUI to SIP logic" << PLAIN << endl;
#endif
		gui->setSipSoftPhoneConfiguration(phoneConf);
		//messageRouter->setGui(gui);
		messageRouter->addSubsystem("gui",*gui);
		confMessageRouter->setGui(gui);



#ifdef SCSIM_SUPPORT
//		MRef<SmartCardDetector*> scdetect = new SmartCardDetector(*messageRouter);
//		messageRouter->addSubsystem("smartcard",*scdetect);
//		scdetect->start();
#endif

		/*
		   mdbg("init") << "Starting presence server"<< endl;
		   CommandString subscribeserver("", SipCommandString::start_presence_server);
		   SipSMCommand sipcmdss(subscribeserver, SipSMCommand::remote, SipSMCommand::TU);
		   sip->getSipStack()->handleCommand(sipcmdss);
		   */

		/*
		   cerr << "Minisip: starting presence client for johan@bilien.org"<< endl;

		   CommandString subscribe("", SipCommandString::start_presence_client,"johan@bilien.org");
		   SipSMCommand sipcmd2(subscribe, SipSMCommand::remote, SipSMCommand::TU);
		   sip->getSipStack()->handleCommand(sipcmd2);
		   */
printf("---------------------- Minisip startSip Message Router setCallBack ");
		gui->setCallback(*messageRouter);
		gui->setConfCallback(*confMessageRouter);

		sip->start(); //run as a thread ...
		//		sleep(5);

		//		CommandString pupd("", SipCommandString::remote_presence_update,"someone@ssvl.kth.se","online","Working hard");
		//		gui->handleCommand(pupd);

		//sip->run();
#ifdef SNAKE_SUPPORT
		MRef<SnakeClient*> snake = new SnakeClient(phoneConf);

		messageRouter->addSubsystem("snake",*snake);
		snake->setCallback(*messageRouter);
#endif
	}

	catch(exception &exc){
		//FIXME: Display message in GUI
		merr << "Minisip caught an exception. Quitting."<< endl;
		merr << exc.what() << endl;
		ret = -1;
	}
	catch(...){
		//FIXME: Display message in GUI
#ifdef DEBUG_OUTPUT
		merr << "Minisip caught an unknown exception (default). Quitting."<< endl;
#endif
		ret = -1;
	};
	return ret;
}

int Minisip::initParseConfig(){

	bool done=false;
	int retGlobal = -1;
	do{
		try{
#ifdef DEBUG_OUTPUT
			mout << BOLD << "init 3/9: Parsing configuration" << PLAIN << endl;
#endif
			MRef<ConfBackend *> confBackend =
			ConfigRegistry::getInstance()->createBackend( confPath);
			if( !confBackend ){
				merr << "Minisip could not load a configuration" << endl << 
					"back end. The application will now" << endl <<
					"exit." << endl;
				throw new MinisipBadArgument("The configured backend could not be loaded");
				//::exit( 1 );
			}
			string ret = phoneConf->load( confBackend );

			done = true;
			retGlobal = 1; //for now, we finished ok ... check the return string
			if (ret.length()>0){
				if( ret == "ERROR" ) { //severe error
					retGlobal = -1;
				} else { //error, but not severe
					merr << ret << endl;
				}
			}
#ifdef DEBUG_OUTPUT
			if( retGlobal > 0 ) {
				cerr << "Identities: "<<endl;
				for (list<MRef<SipIdentity*> >::iterator i=phoneConf->identities.begin(); 
						i!=phoneConf->identities.end(); i++){
					cerr<< "\t"<< (*i)->getDebugString()<< endl;
				}
			}
#endif

		}catch(XMLElementNotFound & enf){
#ifdef DEBUG_OUTPUT
			merr << FG_ERROR << "Element not found: "<< enf.what()<< PLAIN << endl;
#endif
			merr << string("ERROR: Could not parse configuration item: ")+enf.what() << endl;
			gui->configDialog( phoneConf );
			done=false;
		}
	}while(!done);
	return retGlobal;
}

int Minisip::runGui(){
	gui->run();
	return 1;
}

void Minisip::startDebugger(){
	cerr << "startDebugger" << endl;
	consoleDbg = MRef<ConsoleDebugger*>(new ConsoleDebugger(phoneConf));
	consoleDbg->setMediaHandler(subsystemMedia);
	MRef<Thread *> consoleDbgThread = consoleDbg->start();
}

void Minisip::stopDebugger(){
	if( ! consoleDbg.isNull() ) {
		mout << endl << "Stopping the Console Debugger thread" << endl;
		consoleDbg->stop(); //uufff ... we are killing the thread, not nice ...
		consoleDbg->join();
		consoleDbg->setMediaHandler( NULL );
		consoleDbg = NULL;
	}
}

 
