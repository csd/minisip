
#include"ConsoleDebugger.h"
#include<libmutil/termmanip.h>
#include<libmutil/Thread.h>
#include<iostream>


#ifdef HAVE_TERMIOS_H
#include<termios.h>
#endif
#ifdef WIN32
#include<conio.h>
#endif

using namespace std;

void ConsoleDebugger::showMem(){
	cerr << "(disabled)"<< endl;
//    cerr << memhandler.listObjs() << endl;;
}

void ConsoleDebugger::run(){
	while(true){
		char c;
#ifdef _MSC_VER
		int n=1;
		c= _getch();
#else
		int n = read(STDIN_FILENO, &c, 1);
#endif
		if (n==1){
			switch (c){
			case ' ':
			case '\n':
				cerr << endl;
				break;
			case '+':
				showStat();
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
#endif
				
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
			default:
				cerr << "Unknown command: "<< c << endl;
			}
		}
	
	}
	
}

void ConsoleDebugger::showDialogInfo(MRef<SipDialog*> d, bool usesStateMachine){

	list <TPRequest<string,MRef<StateMachine<SipSMCommand,string>*> > > torequests = 
		config->timeoutProvider->getTimeoutRequests();

	if (usesStateMachine){
		cerr << (/*string("    (")+itoa(ii)+") " +*/ d->getName() + "   State: " + d->getCurrentStateName())<< endl;
	}else{
		cerr << d->getName() << endl;
	}
	cerr << BOLD << "        SipDialogState: "<< PLAIN;
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
		config->timeoutProvider->getTimeoutRequests();


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
        perror("tcgetattr:");
        return -1;
    }
    termattr.c_cc[VMIN]=1;
    termattr.c_cc[VTIME]=0;
    termattr.c_lflag &= ~(ICANON | ECHO | ECHONL);

    ret = tcsetattr (STDIN_FILENO, TCSANOW, &termattr);
    if (ret < 0) {
        perror("tcsetattr");
        return -1;
    }
#endif
//#ifdef WIN32
//#warning nonblockin_stdin unimplemented on Win32
//#endif
    return 0;
}



void ConsoleDebugger::startThread(){
	nonblockin_stdin();

	
	Thread t(this);
	

}


