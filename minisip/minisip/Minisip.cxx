#include<config.h>

#ifndef _MSC_VER
#include<unistd.h>
#include<iostream>
using namespace std;
#endif

#include"Minisip.h"

#ifdef TEXT_UI
#include"gui/textui/MinisipTextUI.h"
#include<libmutil/TextUI.h>
#else //!TEXT_UI
#ifdef GTK_GUI
#include"gui/gtkgui/MainWindow.h"
#else //!GTK_GUI
#include"gui/qtgui/MinisipMainWindowWidget.h"
#include"gui/qtgui/qtguistarter.h"
#include<qmessagebox.h>
#endif //GTK_GUI
#endif //TEXT_UI

#include<libmutil/termmanip.h>
#include<libmsip/SipCallback.h>
#include<libmsip/SipCommandString.h>
#include<libmnetutil/IP4Address.h>
#include<libmnetutil/UDPSocket.h>
#include<libmnetutil/NetworkFunctions.h>
#include<libmnetutil/TLSServerSocket.h>
#include<libmnetutil/IP4ServerSocket.h>
#include<libmnetutil/NetUtil.h>
#include<libmnetutil/NetworkException.h>
#include<libmsip/SipMessageTransport.h>
#include<libmikey/keyagreement_dh.h>
#include"ConsoleDebugger.h"
#include"../sip/Sip.h"
#include"LogEntry.h"
#include"contactdb/ContactDb.h"
#include"../mediahandler/MediaHandler.h"
#include"../conf/ConferenceControl.h"
#include"../conf/ConfCallback.h"
#include"confbackend/ConfBackend.h"
#include"MessageRouter.h"

#include<libmsip/SipUtils.h>
#include<exception>
#include<libmutil/Timestamp.h>
#include<libmutil/TextUI.h>

//extern TextUI *debugtextui;

#ifndef WIN32
#ifdef DEBUG_OUTPUT
#include<signal.h>
#include<libmutil/Timestamp.h>
#endif
#endif

#ifdef OSSO_SUPPORT
#include<libosso.h>
#endif

extern Mutex global;

#ifndef WIN32
#ifdef DEBUG_OUTPUT
static void signal_handler( int signal ){
	if( signal == SIGUSR1 ){
		merr << "ERROR: Minisip was stopped (signal SIGUSR1 caught)" << end;
		ts.print();
		exit( 1 );
	} else {
		merr << "ERROR: Minisip was stopped (some signal caught)" << end;
	}
}
#endif
#endif

Minisip::Minisip( int argc, char**argv ):ehandler(NULL){

	srand(time(0));

#ifndef WIN32
#ifdef DEBUG_OUTPUT
	signal( SIGUSR1, signal_handler );
#endif
#endif
	

#ifdef DEBUG_OUTPUT
	mout << "Initializing NetUtil"<<end;
#endif

	if ( ! NetUtil::init()){
		//printf("ERROR: Could not initialize Netutil package\n");
		merr << "ERROR: Could not initialize NetUtil package"<<end;
		exit();
	}
#ifdef DEBUG_OUTPUT
	cerr << "Creating SipSoftPhoneConfiguration"<< endl;
#endif
	phoneConf =  new SipSoftPhoneConfiguration();
	phoneConf->sip=NULL;

#ifdef MINISIP_AUTOCALL
	if (argc==3){
		phoneConf->autoCall = string(argv[2]);
	}
#endif

#ifdef DEBUG_OUTPUT
	mout << BOLD << "init 1/9: Creating timeout provider" << PLAIN << end;
#endif
//	timeoutprovider = new TimeoutProvider<string,MRef<StateMachine<SipSMCommand,string>*> >;

	/* Create the global contacts database */
	ContactDb *contactDb = new ContactDb();
	ContactEntry::setDb(contactDb);
	
	//FIXME: move all this in a Gui::create()

#ifdef DEBUG_OUTPUT
	mout << BOLD << "init 2/9: Creating GUI" << PLAIN << end;
#endif

#ifdef TEXT_UI
	///*gui = */debugtextui = new MinisipTextUI();
	cerr << "Creating TextUI"<< endl;

	gui = new MinisipTextUI();
	
	merr.setExternalHandler( dynamic_cast<DbgHandler *>( *gui ) );
	LogEntry::handler = NULL;
#else //!TEXT_UI
#ifdef GTK_GUI

	cerr << "Creating GTK GUI"<< endl;
	gui = new MainWindow( argc, argv );
	LogEntry::handler = (MainWindow *)*gui;

#ifdef DEBUG_OUTPUT
	consoleDbg = MRef<ConsoleDebugger*>(new ConsoleDebugger(phoneConf));
	MRef<Thread *> consoleDbgThread = consoleDbg->start();
#else
	//in non-debug mode, send merr to the gui
	merr.setExternalHandler( dynamic_cast<MainWindow *>( *gui ) ); 
#endif

#else //!GTK_GUI
	gui= guiFactory(argc, argv, timeoutProvider);
	LogEntry::handler = NULL;
#endif //GTK_GUI
#endif //TEXT_UI

#ifdef OSSO_SUPPORT
	osso_context_t * ossoCtxt = NULL;
	ossoCtxt = osso_initialize( PACKAGE_NAME, PACKAGE_VERSION, TRUE, NULL );
	if( !ossoCtxt ){
		mdbg << "Could not initialize osso context" << end;
	}
#endif

#ifdef DEBUG_OUTPUT
	cerr << "Setting contact db"<< endl;
#endif
	gui->setContactDb(contactDb);
}

Minisip::~Minisip(){
}

int Minisip::exit(){
	int ret = 1;
	mout << BOLD << "Minisip is Shutting down!!!" << PLAIN << end;
	
	if( ! sip.isNull() ) { //it may not be initialized ... 
		//Send a shutdown command to the sip stack ... 
		//it will take care of de-registering and closing on-going calls
		CommandString cmdstr( "", SipCommandString::sip_stack_shutdown );
		SipSMCommand sipcmd(cmdstr, SipSMCommand::remote, SipSMCommand::DIALOGCONTAINER);
		sip->stop();
	
#ifdef DEBUG_OUTPUT
		mout << "Waiting for the SipStack to close ..." << end;
#endif
		sip->join();
	}
	
#ifdef GTK_GUI
#ifdef DEBUG_OUTPUT
	mout << end << "Stopping the Console Debugger thread" << end;
	if( ! consoleDbg.isNull() ) {
		consoleDbg->stop(); //uufff ... we are killing the thread, not nice ...
		consoleDbg->join();
	}
#endif	
#endif
	if( ehandler ){
		delete ehandler;
	}

	mout << end << end << BOLD << "Minisip can't wait to see you again! Bye!" << PLAIN << end << end << end;
	return ret;
}

int Minisip::startSip() {
	int ret = 1;
	
#ifdef DEBUG_OUTPUT
	cerr << "Thread 2 running - doing initParseConfig"<< endl;
#endif	
	
	if( initParseConfig() < 0 ){
#ifdef DEBUG_OUTPUT
		cerr << "Minisip::startSip::initParseConfig - fatal error" << endl;
#endif
		return -1;
	}

	try{
		ehandler =  new MessageRouter();
//		phoneConf->timeoutProvider = timeoutprovider;

#ifdef DEBUG_OUTPUT
		mout << BOLD << "init 4/9: Creating IP provider" << PLAIN << end;
#endif
		MRef<IpProvider *> ipProvider = IpProvider::create( phoneConf );
//#ifdef DEBUG_OUTPUT
//                mout << BOLD << "init 5/9: Creating SIP transport layer" << PLAIN << end;
//#endif
		string localIpString;
		string externalContactIP;

		// FIXME: This should be done more often
		localIpString = externalContactIP = ipProvider->getExternalIp();                
		
		MRef<UDPSocket*> udpSocket = new UDPSocket( false, phoneConf->inherited->localUdpPort );                
		
		phoneConf->inherited->localUdpPort = ipProvider->getExternalPort( udpSocket );
		phoneConf->inherited->localIpString = externalContactIP;
		phoneConf->inherited->externalContactIP = externalContactIP;
		udpSocket=NULL;

#ifdef DEBUG_OUTPUT
		mout << BOLD << "init 5/9: Creating MediaHandler" << PLAIN << end;
#endif
		MRef<MediaHandler *> mediaHandler = new MediaHandler( phoneConf, ipProvider );
		ehandler->setMediaHandler( mediaHandler );
#ifdef GTK_GUI
#ifdef DEBUG_OUTPUT
		consoleDbg->setMediaHandler( mediaHandler );
#endif
#endif
		Session::registry = *mediaHandler;
		/* Hack: precompute a KeyAgreementDH */
	//	Session::precomputedKa = new KeyAgreementDH( phoneConf->securityConfig.cert, phoneConf->securityConfig.cert_db, DH_GROUP_OAKLEY5 );

#ifdef DEBUG_OUTPUT
		mout << BOLD << "init 6/9: Creating MSip SIP stack" << PLAIN << end;
#endif
		//save Sip object in Minisip::sip ...
		this->sip=new Sip(phoneConf,mediaHandler,
				localIpString,
				externalContactIP,
				phoneConf->inherited->localUdpPort,
				phoneConf->inherited->localTcpPort,
				phoneConf->inherited->externalContactUdpPort,
				phoneConf->inherited->transport,
				phoneConf->inherited->localTlsPort,
				phoneConf->securityConfig.cert,    //The certificate chain is used by TLS
				//TODO: TLS should use the whole chain instead of only the f$                                MRef<ca_db *> cert_db = NULL
				phoneConf->securityConfig.cert_db
				);
		//sip->init();

		phoneConf->sip = sip;

		sip->getSipStack()->setCallback(ehandler);

		ehandler->setSip(sip);

		/* Load the plugins at this stage */
//		int32_t pluginCount = MPlugin::loadFromDirectory( PLUGINS_PATH );

//		cerr << "Loaded " << pluginCount << " plugins from " << PLUGINS_PATH << endl;

#ifdef DEBUG_OUTPUT
		mout << BOLD << "init 7/9: Connecting GUI to SIP logic" << PLAIN << end;
#endif
		gui->setSipSoftPhoneConfiguration(phoneConf);
		ehandler->setGui(gui);

	
/*
		mdbg << "Starting presence server"<< end;
		CommandString subscribeserver("", SipCommandString::start_presence_server);
		SipSMCommand sipcmdss(subscribeserver, SipSMCommand::remote, SipSMCommand::TU);
		sip->handleCommand(sipcmdss);
*/

/*
		cerr << "Minisip: starting presence client for johan@bilien.org"<< endl;
		
		CommandString subscribe("", SipCommandString::start_presence_client,"johan@bilien.org");
		SipSMCommand sipcmd2(subscribe, SipSMCommand::remote, SipSMCommand::TU);
		sip->getSipStack()->handleCommand(sipcmd2);
*/
		
		sip->start(); //run as a thread ...
		gui->setCallback(ehandler);
//		sleep(5);
		
//		CommandString pupd("", SipCommandString::remote_presence_update,"someone@ssvl.kth.se","online","Working hard");
//		gui->handleCommand(pupd);

#ifdef TEXT_UI
		MRef<MinisipTextUI *> textui = dynamic_cast<MinisipTextUI *>(*gui);
		if (textui){
			textui->displayMessage("");
			textui->displayMessage("To auto-complete, press <tab>. For a list of commands, press <tab>.", MinisipTextUI::bold);
		textui->displayMessage("");
		}
#endif
		//sip->run();
	}
		
	catch(exception &exc){
		//FIXME: Display message in GUI
#ifdef DEBUG_OUTPUT
		merr << "Minisip caught an exception. Quitting."<< end;
		merr << exc.what() << end;
#endif
		ret = -1;
	}
	catch(...){
		//FIXME: Display message in GUI
#ifdef DEBUG_OUTPUT
		merr << "Minisip caught an unknown exception (default). Quitting."<< end;
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
			mout << BOLD << "init 3/9: Parsing configuration file ("
					<< conffile<<")" << PLAIN << end;
#endif
			MRef<ConfBackend *> confBackend = ConfBackend::create();
			if( !confBackend ){
				merr << "Minisip could not load a configuration"
					"back end. The application will now"
					"exit." << end;
				::exit( 1 );
			}
			string ret = phoneConf->load( confBackend );


			done = true;
			retGlobal = 1; //for now, we finished ok ... check the return string
			if (ret.length()>0){
				if( ret == "ERROR" ) { //severe error
					retGlobal = -1;
				} else { //error, but not severe
					merr << ret << end;
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

		}catch(XMLElementNotFound *enf){
#ifdef DEBUG_OUTPUT
			merr << FG_ERROR << "Element not found: "<< enf->what()<< PLAIN << end;
#endif
			merr << "ERROR: Could not parse configuration item: "+enf->what() << end;
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

int main( int argc, char ** argv ){
	cerr << endl << "Starting MiniSIP ... welcome!" << endl << endl;
	
	Minisip minisip( argc, argv );
	if( minisip.startSip() > 0 ) {
		minisip.runGui();
	} else {
		cerr << endl << BOLD << "ERROR while starting SIP!" << PLAIN << endl << endl;
	}
	minisip.exit();
}

