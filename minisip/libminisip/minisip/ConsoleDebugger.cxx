
#include"ConsoleDebugger.h"

#include<libmutil/termmanip.h>
#include<libmutil/merror.h>
//#include<libmutil/Thread.h>

#ifdef SM_DEBUG
#include<libmutil/StateMachine.h>
#endif
 
#include<libmsip/SipCommandString.h>
#include<libmsip/SipSMCommand.h>

#include"../mediahandler/MediaCommandString.h"

#include<iostream>

#ifdef HAVE_UNISTD_H
#	include<unistd.h>
#endif

#ifdef HAVE_TERMIOS_H
#	include<termios.h>
#endif

#ifdef WIN32
#	include<conio.h>
#	ifdef _WIN32_WCE
#		include<stdio.h>
#	endif
#endif

#ifdef _WIN32_WCE
#	include"../include/minisip_wce_extra_includes.h"
#endif

using namespace std;

ConsoleDebugger::ConsoleDebugger(MRef<SipSoftPhoneConfiguration *> conf): 
			config(conf), 
			mediaHandler(NULL),
			thread( NULL ) {
};

ConsoleDebugger::~ConsoleDebugger() {
#ifdef DEBUG_OUTPUT
	//cerr << "~ConsoleDebugger" << endl;
#endif
}

void ConsoleDebugger::showHelp() {
	cerr << "Welcome to the Console Debugger!" << endl;
	cerr << "   Available commands are:" << endl;
	cerr << "        - h/H : display this help message" << endl;
	cerr << endl;
	cerr << "        - r/R : register all identities" << endl;
	cerr << "        - u/U : de-register all identities" << endl;
	cerr << "        - t/T : terminate all ongoing calls" << endl;
	cerr << endl;
	cerr << "        - d/D : Turn on of the mdbg output stream (less verbosity)" << endl;
	cerr << "        - p/P : Print IN and OUT packets on the screen" << endl;
	cerr << endl;
	cerr << "        - * : List all MObjects in memory (needs libmutil to use be configured with --enable-memdebug)" << endl;
	cerr << "        - ; : More memory related debug output (object created or destroyed)" << endl;
	cerr << endl;
	cerr << "        - ( : Output StateMachine related debug messages (see StateMachine.h)" << endl;
	cerr << endl;
	cerr << "        - ) : Print a list of timers waiting to be fired" << endl;
	cerr << "        - + : Print info on all currently registered SipDialogs" << endl;
	cerr << endl;
	cerr << "        - m/M : Print info on MediaSessions currently running" << endl;
	cerr << endl;
	cerr << "   Note: only h/+/u/r/t commands are always available. The rest, only if --enable-debug is configured" << endl;
	cerr << endl;
	
}

void ConsoleDebugger::run(){
	bool tmpSet;
	
	keepRunning = true;
	while(keepRunning){
		char c;
#ifdef _MSC_VER
		int n=1;
#	ifdef _WIN32_WCE
		c= getchar();
#	else
		c= _getch();
#	endif
#else
		int n = read(STDIN_FILENO, &c, 1);
#endif
		if( !keepRunning ) {
			//cerr << "CDbg: run(): do not keep running" << endl;
			break;
		}
		if (n==1){
			switch (c){
			case ' ':
			case '\n':
				cerr << endl;
				break;
			case '+'://show sip stack state
				showStat();
				break;
			case 'h':
			case 'H': //list help
				showHelp();
				break;
			case 't':
			case 'T': //Terminate all ongoing calls
				sendManagementCommand( SipCommandString::terminate_all_calls );
				break;
			case 'u':
			case 'U': //Deregister all identities
				sendManagementCommand( SipCommandString::unregister_all_identities );
				break;
			case 'r':
			case 'R': //Deregister all identities
				sendManagementCommand( SipCommandString::register_all_identities );
				break;
	#ifdef DEBUG_OUTPUT
			case 'P':
			case 'p':
				set_debug_print_packets(!get_debug_print_packets());
				//sipdebug_print_packets= !sipdebug_print_packets;
				if (/*sipdebug_print_packets*/ get_debug_print_packets() )
					cerr << "Packets will be displayed to the screen"<< endl;
				else
					cerr << "Packets will NOT be displayed to the screen"<< endl;
				break;
				
			case 'd':
			case 'D':
				mdbg.setEnabled( ! mdbg.getEnabled() );
				if (mdbg.getEnabled()){
					cerr << "Debug information ON"<< endl;	
				}else{
					cerr << "Debug information OFF"<< endl;	
				}
				break;
	
			case '*':
				showMem();
				break;
			
			case ';': //output message when object is destroyed
				tmpSet = setDebugOutput(true);
				if( tmpSet )
					cerr << "MemObject debug info turned ON"<< endl;
				else 
					cerr << "MemObject debug info turned OFF"<< endl;
				break;
			
			case '(': //turn on/off state machine debug
		#ifdef SM_DEBUG
				outputStateMachineDebug = !outputStateMachineDebug;
				if( outputStateMachineDebug )
					cerr << "StateMachine debug info turned ON"<< endl;
				else 
					cerr << "StateMachine debug info turned OFF"<< endl;
		#else
				cerr << "StateMachine debug not usable: need to #define SM_DEBUG (see StateMachine.h)"<< endl;
		#endif
				break;
			case ')': //print all timers ... 
				cerr << "========= Timeouts still to fire : " << endl;
				cerr << config->sip->getSipStack()->getTimeoutProvider()->getTimeouts() << endl;
				cerr << "=========------------------ " << endl;
				break;
			
			case 'm': //print mediahandler session info
			case 'M':
				sendCommandToMediaHandler(MediaCommandString::session_debug);
				break;
	#endif
			default:
				cerr << "Unknown command: "<< c << endl;
			}
		}
	
	}
	
}

void ConsoleDebugger::sendManagementCommand( string str ) {
	CommandString cmdstr ( "", str );
	SipSMCommand cmd( cmdstr, 
			SipSMCommand::remote,
			SipSMCommand::DIALOGCONTAINER);
	config->sip->getSipStack()->getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}

void ConsoleDebugger::sendCommandToMediaHandler( string str ) {
	CommandString cmdstr ("", str);
	cerr << "========= MediaHandler Debug info : " << endl;
	mediaHandler->handleCommand("media", cmdstr );
	cerr << "=========" << endl;
}

void ConsoleDebugger::showMem(){
	string all;
	minilist<string> names = getMemObjectNames();
	for (int i=0; i<names.size();i++){
		all = all+names[i]+"\n";
	}
	cerr << all << itoa(getMemObjectCount()) <<" objects"<< endl;
}

void ConsoleDebugger::showDialogInfo(MRef<SipDialog*> d, bool usesStateMachine){

	list <TPRequest<string,MRef<StateMachine<SipSMCommand,string>*> > > torequests = 
		config->sip->getSipStack()->getTimeoutProvider()->getTimeoutRequests();

	if (usesStateMachine){
		cerr << (/*string("    (")+itoa(ii)+") " +*/ d->getName() + "   State: " + d->getCurrentStateName())<< endl;
	}else{
		cerr << d->getName() << endl;
	}
	cerr << BOLD << "        SipDialogState: "<< PLAIN << endl;
	cerr <<         "            secure="<<d->dialogState.secure 
			<<"; localTag="<<d->dialogState.localTag
			<<"; remoteTag="<<d->dialogState.remoteTag 
			<<"; seqNo="<< d->dialogState.seqNo
			<<"; remoteSeqNo="<< d->dialogState.remoteSeqNo
			<<"; remoteUri="<< d->dialogState.remoteUri
			<<"; remoteTarget="<<d->dialogState.remoteTarget
			<<"; isEarly="<<d->dialogState.isEarly
			<< endl;
	cerr <<         "            route_set: ";
	
	list<string>::iterator i;
	for (i=d->dialogState.routeSet.begin(); i!= d->dialogState.routeSet.end(); i++){
		if (i!=d->dialogState.routeSet.begin())
			cerr << ",";
		cerr << *i;
	}
	cerr <<endl;
	
	cerr << BOLD << "        Identity: "<< PLAIN << endl;
	cerr <<         "            "<< d->getDialogConfig()->inherited->sipIdentity->getDebugString();
	cerr <<endl;
	
	cerr << BOLD << "        Timeouts:"<< PLAIN << endl;
	int ntimeouts=0;
	std::list<TPRequest<string,MRef<StateMachine<SipSMCommand,string>*> > >::iterator jj=torequests.begin();
	for (uint32_t j=0; j< torequests.size(); j++,jj++){
		if ( *d == *((*jj).get_subscriber()) ){
			int ms= (*jj).get_ms_to_timeout();
			cerr << string("            timeout: ")+ (*jj).get_command()
				+ "  Time: " + itoa(ms/1000) + "." + itoa(ms%1000) << endl;
			ntimeouts++;
		}
	}
	if (ntimeouts==0){
		cerr << "            (no timeouts)"<< endl;
	}


	cerr << BOLD << "        Transactions:"<< PLAIN << endl;
	list<MRef<SipTransaction*> > transactions = d->getTransactions();
	if (transactions.size()==0)
		cerr << "            (no transactions)"<< endl;
	else{
		int n=0;
		for (list<MRef<SipTransaction*> >::iterator i = transactions.begin();
				i!=transactions.end(); i++){
			cerr << string("            (")+itoa(n)+") "+
				(*i)->getName() 
				+ "   State: "
				+ (*i)->getCurrentStateName() << endl;
			n++;

			cerr << BOLD << "                Timeouts:" << PLAIN << endl;

			int ntimeouts=0;
			std::list<TPRequest<string,   MRef<StateMachine<SipSMCommand,string>*>  > >::iterator jj=torequests.begin();
			for (uint32_t j=0; j< torequests.size(); j++, jj++){
				if ( *((*i)) == *((*jj).get_subscriber()) ){
					int ms= (*jj).get_ms_to_timeout();
					cerr << string("                        timeout: ")
						+ (*jj).get_command()
						+ "  Time: " + itoa(ms/1000) + "." + itoa(ms%1000)<< endl;
					ntimeouts++;
				}
			}
			if (ntimeouts==0)
				cerr << "                        (no timeouts)"<< endl;
		}
	}





}

void ConsoleDebugger::showStat(){
	list<MRef<SipDialog*> > calls = config->sip->getSipStack()->getDialogContainer()->getDispatcher()->getDialogs();

	list <TPRequest<string,MRef<StateMachine<SipSMCommand,string>*> > > torequests = 
		config->sip->getSipStack()->getTimeoutProvider()->getTimeoutRequests();


	cerr << "    (DefaultHandler) ";
	showDialogInfo(config->sip->getSipStack()->getDialogContainer()->getDefaultHandler(),false);
	
	cerr << BOLD << " Calls:" << PLAIN << endl;
	if (calls.size()==0)
		cerr << "    (no calls)"<< endl;
	else{
		int ii=0;
		for (list<MRef<SipDialog*> >::iterator i=calls.begin(); i!= calls.end(); i++, ii++){
			cerr << string("    (")+itoa(ii)+") " ;
			showDialogInfo(*i,true);

#if 0
			cerr << (string("    (")+itoa(ii)+") "
					//            displayMessage(string("    ")
				+ (*i)->getName() 
				+ "   State: "
				+ (*i)->getCurrentStateName())<< endl;


					cerr << BOLD << "        Timeouts:"<< PLAIN << endl;
					int ntimeouts=0;
					std::list<TPRequest<string,MRef<StateMachine<SipSMCommand,string>*> > >::iterator jj=torequests.begin();
					for (int j=0; j< torequests.size(); j++,jj++){
					if ( *(*i) == *((*jj).get_subscriber()) ){
					int ms= (*jj).get_ms_to_timeout();
					cerr << string("            timeout: ")+ (*jj).get_command()
						+ "  Time: " + itoa(ms/1000) + "." + itoa(ms%1000) << endl;
					ntimeouts++;
					}
					}
					if (ntimeouts==0){
					cerr << "            (no timeouts)"<< endl;
					}



					cerr << BOLD << "        Transactions:"<< PLAIN << endl;
					list<MRef<SipTransaction*> > transactions = (*i)->getTransactions();
					if (transactions.size()==0)
						cerr << "            (no transactions)"<< endl;
					else{
						int n=0;
                for (list<MRef<SipTransaction*> >::iterator i = transactions.begin();
                        i!=transactions.end(); i++){
                    cerr << string("            (")+itoa(n)+") "+
                            (*i)->getName() 
                            + "   State: "
                            + (*i)->getCurrentStateName() << endl;
                    n++;

                    cerr << BOLD << "                Timeouts:" << PLAIN << endl;
                    
                    int ntimeouts=0;
                    std::list<TPRequest<string,   MRef<StateMachine<SipSMCommand,string>*>  > >::iterator jj=torequests.begin();
                    for (int j=0; j< torequests.size(); j++, jj++){
                        if ( *((*i)) == *((*jj).get_subscriber()) ){
                            int ms= (*jj).get_ms_to_timeout();
                            cerr << string("                        timeout: ")
                                        + (*jj).get_command()
                                        + "  Time: " + itoa(ms/1000) + "." + itoa(ms%1000)<< endl;
                            ntimeouts++;
                        }
                    }
                    if (ntimeouts==0)
                        cerr << "                        (no timeouts)"<< endl;
                }
            }

#endif
        }
		
	}
}

static int nonblockin_stdin()
{
#ifdef HAVE_TERMIOS_H
    struct termios termattr;
    int ret=tcgetattr(STDIN_FILENO, &termattr);
    if (ret < 0) {
        merror("tcgetattr:");
        return -1;
    }
    termattr.c_cc[VMIN]=1;
    termattr.c_cc[VTIME]=0;
    termattr.c_lflag &= ~(ICANON | ECHO | ECHONL);

    ret = tcsetattr (STDIN_FILENO, TCSANOW, &termattr);
    if (ret < 0) {
        merror("tcsetattr");
        return -1;
    }
#endif
//#ifdef WIN32
//#warning nonblockin_stdin unimplemented on Win32
//#endif
    return 0;
}

MRef<Thread *> ConsoleDebugger::start(){
	nonblockin_stdin();
	
	thread = new Thread (this);
	return thread;
}

void ConsoleDebugger::stop() { 
	keepRunning = false; 

	//We need to kill the thread, as it gets blocked in the
	//read/getch operation. It is not a very nice thing to do,
	//but at this moment, the whole minisip is shutting down.
	thread->kill();
	
}

void ConsoleDebugger::join() {
	thread->join();
	mediaHandler = NULL;
	config = NULL;
	thread = NULL;
}
