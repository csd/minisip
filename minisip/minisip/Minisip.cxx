#include<config.h>

#ifndef _MSC_VER
#include<unistd.h>
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
#include"ConsoleDebugger.h"
#include"../sip/Sip.h"
#include"LogEntry.h"
#include"contactdb/ContactDb.h"
#include"../mediahandler/MediaHandler.h"
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

#ifndef WIN32
#ifdef DEBUG_OUTPUT
static void signal_handler( int signal ){
        if( signal == SIGUSR1 ){
                merr << "Minisip was stopped" << end;
                ts.print();
        }
	exit( 1 );
}
#endif
#endif


static void *tcp_server_thread(void *arg){
        assert( arg != NULL );
        MRef<SipMessageTransport*> transport((SipMessageTransport *)arg);
	try{
        	IP4ServerSocket server(transport->getLocalTCPPort());
        	while(true){
                	transport->addSocket(server.accept());
        	}
	}
	catch( NetworkException * exc ){
		cerr << "Exception caught when creating TCP server." << endl;
		cerr << exc->errorDescription() << endl;
		return NULL;
	}
}

static void *tls_server_thread(void *arg){
        assert( arg != NULL );
        MRef<SipMessageTransport*> transport((SipMessageTransport *)arg);
	try{
        	TLSServerSocket server(transport->getLocalTLSPort(),transport->getMyCertificate());
        	while(true){
                	transport->addSocket(server.accept());
        	}
	}
	catch( NetworkException * exc ){
		cerr << "Exception caught when creating TLS server." << endl;
		cerr << exc->errorDescription() << endl;
		return NULL;
	}
		
}

Minisip::Minisip( int argc, char**argv ){

	if( argc == 2){
		conffile = argv[1];
	}
	else{
		char *home = getenv("HOME");
		if (home==NULL){
			merr << "WARNING: Could not determine home directory"<<end;

#ifdef WIN32
                        conffile=string("c:\\minisip.conf"); 
#else
                        conffile = string("/.minisip.conf");
#endif
                }else{
                        conffile = string(home)+ string("/.minisip.conf");
                }
        }

        srand(time(0));

#ifndef WIN32
#ifdef DEBUG_OUTPUT
	signal( SIGUSR1, signal_handler );
#endif
#endif
	

	mout << "Initializing NetUtil"<<end;

        if ( ! NetUtil::init()){
                printf("Could not initialize Netutil package\n");
                merr << "ERROR: Could not initialize NetUtil package"<<end;
                exit();
        }

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
	
	timeoutprovider = new TimeoutProvider<string,MRef<StateMachine<SipSMCommand,string>*> >;

	
        /* Create the global contacts database */
        ContactDb *contactDb = new ContactDb();
        ContactEntry::setDb(contactDb);
	
	//FIXME: move all this in a Gui::create()

#ifdef DEBUG_OUTPUT
        mout << BOLD << "init 2/9: Creating GUI" << PLAIN << end;
#endif

#ifdef TEXT_UI
        ///*gui = */debugtextui = new MinisipTextUI();
	gui = new MinisipTextUI();
//	gui = dynamic_cast<Gui*>(debugtextui);
//	assert(gui);
	//debugtextui = gui;
	merr.setExternalHandler( dynamic_cast<MinisipTextUI*>(gui) );
        LogEntry::handler = NULL;
#else //!TEXT_UI
#ifdef GTK_GUI

        gui = new MainWindow( argc, argv );
        LogEntry::handler = dynamic_cast<LogEntryHandler *>(gui);
	
	DbgHandler * dbgHandler = dynamic_cast<MainWindow *>( gui );
	fprintf( stderr, "dbgHandler: %x\n", dbgHandler );
	merr.setExternalHandler( dynamic_cast<MainWindow *>( gui ) );

#ifdef DEBUG_OUTPUT
	consoleDbg = MRef<ConsoleDebugger*>(new ConsoleDebugger(phoneConf));
	consoleDbg->startThread();
#endif

#else //!GTK_GUI
        gui= guiFactory(argc, argv, timeoutprovider);
	LogEntry::handler = NULL;
#endif //GTK_GUI
#endif //TEXT_UI

	gui->setContactDb(contactDb);
}

Minisip::~Minisip(){

}

void Minisip::exit(){
	//TODO
	/* End on-going calls the best we can */

	/* Unregister */

	/* End threads */

	/* delete things */
}

void Minisip::run(){
	initParseConfig();

	try{
		MessageRouter *ehandler =  new MessageRouter();
		phoneConf->timeoutProvider = timeoutprovider;

#ifdef DEBUG_OUTPUT
                mout << BOLD << "init 4/9: Creating IP provider" << PLAIN << end;
#endif
                MRef<IpProvider *> ipProvider =
                        IpProvider::create( phoneConf, gui );
//#ifdef DEBUG_OUTPUT
//                mout << BOLD << "init 5/9: Creating SIP transport layer" << PLAIN << end;
//#endif
                string localIpString;
                string externalContactIP;

                // FIXME: This should be done more often
                localIpString = externalContactIP = ipProvider->getExternalIp();                
		UDPSocket udpSocket( false, phoneConf->inherited.localUdpPort );                
		phoneConf->inherited.localUdpPort =
                        ipProvider->getExternalPort( &udpSocket );
                phoneConf->inherited.localIpString = externalContactIP;
                phoneConf->inherited.externalContactIP = externalContactIP;
                udpSocket.close();
/*
                phoneConf->inherited.sipTransport= MRef<SipMessageTransport*>(new
                        SipMessageTransport(
                                        localIpString,
                                        externalContactIP,
					phoneConf->inherited.transport,
                                        phoneConf->inherited.externalContactUdpPort,
                                        phoneConf->inherited.localUdpPort,
                                        phoneConf->inherited.localTcpPort,
                                        phoneConf->inherited.localTlsPort,
                                        phoneConf->securityConfig.cert->get_first(),
                                        phoneConf->securityConfig.cert_db
                                )
                        );
*/
#ifdef DEBUG_OUTPUT
                mout << BOLD << "init 5/9: Creating MediaHandler" << PLAIN << end;
#endif
                MRef<MediaHandler *> mediaHandler = new MediaHandler( phoneConf, ipProvider );
		ehandler->setMediaHandler( mediaHandler );

#ifdef DEBUG_OUTPUT
                mout << BOLD << "init 6/9: Creating MSip SIP stack" << PLAIN << end;
#endif
		MRef<Sip*> sip=new Sip(phoneConf,mediaHandler,
					localIpString,
					externalContactIP,
					phoneConf->inherited.localUdpPort,
					phoneConf->inherited.localTcpPort,
					phoneConf->inherited.externalContactUdpPort,
					phoneConf->inherited.transport
#ifndef NO_SECURITY
					,phoneConf->inherited.localTlsPort,
					phoneConf->securityConfig.cert,    //The certificate chain is used by TLS
					//TODO: TLS should use the whole chain instead of only the f$                                MRef<ca_db *> cert_db = NULL
					phoneConf->securityConfig.cert_db
#endif
					);
		//sip->init();

                phoneConf->sip = sip;

                sip->getSipStack()->setCallback(ehandler);

                ehandler->setSip(sip);

#ifdef DEBUG_OUTPUT
                mout << BOLD << "init 7/9: Connecting GUI to SIP logic" << PLAIN << end;
#endif
                gui->setSipSoftPhoneConfiguration(phoneConf);
                ehandler->setGui(gui);

//                Thread t(*sip);

		try{
                if (phoneConf->tcp_server){
#ifdef DEBUG_OUTPUT
                        mout << BOLD << "init 8.2/9: Starting TCP transport worker thread" << PLAIN << end;
#endif
                        Thread::createThread(tcp_server_thread, *(/*phoneConf->inherited.sipTransport*/ sip->getSipStack()->getSipTransportLayer() ));

                }

                if (phoneConf->tls_server){
                        if( phoneConf->securityConfig.cert.isNull() ){
                                merr << "Certificate needed for TLS server" << end;
                        }
                        else{
#ifdef DEBUG_OUTPUT
                                mout << BOLD << "init 8.3/9: Starting TLS transport worker thread" << PLAIN << end;
#endif
                                Thread::createThread(tls_server_thread, *(/*phoneConf->inherited.sipTransport*/ sip->getSipStack()->getSipTransportLayer()));
                        }
                }
		}
		catch( NetworkException * exc ){
			cerr << "Exception thrown when creating TCP/TLS servers." << endl;
			cerr << exc->errorDescription() << endl;
			delete exc;
		}

#ifdef DEBUG_OUTPUT
                mout << BOLD << "init 9/9: Initiating register to proxy commands (if any)" << PLAIN << end;
#endif

                for (list<MRef<SipIdentity*> >::iterator i=phoneConf->identities.begin() ; i!=phoneConf->identities.end(); i++){
                        if ( (*i)->registerToProxy  ){
//                              cerr << "Registering user "<< (*i)->getSipUri() << " to proxy " << (*i)->sipProxy.sipProxyAddressString<< ", requesting domain " << (*i)->sipDomain << endl;
                                CommandString reg("",SipCommandString::proxy_register);
                                reg["proxy_domain"] = (*i)->sipDomain;
                                SipSMCommand sipcmd(reg, SipSMCommand::remote, SipSMCommand::TU);
                                sip->getSipStack()->handleCommand(sipcmd);

                        }
                }
/*
		mdbg << "Starting presence server"<< end;
		CommandString subscribeserver("", SipCommandString::start_presence_server);
		SipSMCommand sipcmdss(subscribeserver, SipSMCommand::remote, SipSMCommand::TU);
		sip->handleCommand(sipcmdss);
*/

/*		cerr << "Minisip: starting presence client for someone@ssvl.kth.se"<< endl;
		
		CommandString subscribe("", SipCommandString::start_presence_client,"erik_kphone@ssvl.kth.se");
		SipSMCommand sipcmd2(subscribe, SipSMCommand::remote, SipSMCommand::TU);
		sip->handleCommand(sipcmd2);
*/
		
                gui->setCallback(ehandler);
//		sleep(5);
		
//		CommandString pupd("", SipCommandString::remote_presence_update,"someone@ssvl.kth.se","online","Working hard");
//		gui->handleCommand(pupd);

#ifdef TEXT_UI
		MinisipTextUI * textui = dynamic_cast<MinisipTextUI *>(gui);
		if (textui){
			textui->displayMessage("");
			textui->displayMessage("To auto-complete, press <tab>. For a list of commands, press <tab>.", MinisipTextUI::bold);
		textui->displayMessage("");
		}
#endif
                sip->run();


        }catch(XMLElementNotFound *e){
                //FIXME: Display message in GUI
#ifdef DEBUG_OUTPUT
                merr << "Error: The following element could not be parsed: "<<e->what()<< "(corrupt config file?)"<< end;
#endif
	}
        catch(exception &exc){
                //FIXME: Display message in GUI
#ifdef DEBUG_OUTPUT
                merr << "Minisip caught an exception. Quitting."<< end;
		merr << exc.what() << end;
#endif
        }
        catch(...){
                //FIXME: Display message in GUI
#ifdef DEBUG_OUTPUT
                merr << "Minisip caught an unknown exception. Quitting."<< end;
#endif
        };

}

void Minisip::initParseConfig(){

        bool done=false;
        do{
                try{
#ifdef DEBUG_OUTPUT
                        mout << BOLD << "init 3/9: Parsing configuration file ("<< conffile<<")" << PLAIN << end;
#endif
                        string ret = phoneConf->load( conffile );

                        cerr << "Identities: "<<endl;
                        for (list<MRef<SipIdentity*> >::iterator i=phoneConf->identities.begin() ; i!=phoneConf->identities.end(); i++){
                                cerr<< "\t"<< (*i)->getDebugString()<< endl;
                        }

                        if (ret.length()>0){
                                //bool ok;
                                merr << ret << end;
				/*
                                ok = gui->configDialog( phoneConf );
                                if( !ok ){
                                        exit();
                                }
                                done=false;
				*/
				done=true;
                        }else
                                done=true;
                }catch(XMLElementNotFound *enf){
#ifdef DEBUG_OUTPUT
                        merr << FG_ERROR << "Element not found: "<< enf->what()<< PLAIN << end;
#endif
                        merr << "Could not parse configuration item: "+enf->what() << end;
                        gui->configDialog( phoneConf );
                        done=false;
                }
        }while(!done);
}

void Minisip::startSip(){
	Thread( this );
}

void Minisip::runGui(){
	gui->run();
}

int main( int argc, char ** argv ){
	Minisip minisip( argc, argv );
	minisip.startSip();
	minisip.runGui();
	minisip.exit();
}

