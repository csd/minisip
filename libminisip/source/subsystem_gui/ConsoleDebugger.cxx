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

#include<config.h>

#include<libminisip/gui/ConsoleDebugger.h>

#include<libmutil/termmanip.h>
#include<libmutil/merror.h>

#ifdef SM_DEBUG
#include<libmutil/StateMachine.h>
#endif
 
#include<libmsip/SipCommandString.h>
#include<libmsip/SipSMCommand.h>

#include<libminisip/media/MediaCommandString.h>

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
			mediaHandler(NULL),
			thread( NULL ),
			config(conf)
{
	this->sipStack = conf->sipStack;
}

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
	cerr << "        - a/A : List all thread ids currently active" << endl;

	cerr << "        - ; : More memory related debug output (object created or destroyed)" << endl;
	cerr << endl;
	cerr << "        - ( : Output StateMachine related debug messages (see StateMachine.h)" << endl;
	cerr << endl;
	cerr << "        - ) : Print a list of timers waiting to be fired" << endl;
	cerr << "        - + : Print info on all currently registered SipDialogs" << endl;
	cerr << endl;
	cerr << "        - m/M : Print info on MediaSessions currently running" << endl;
	cerr << endl;
	cerr << "        - c/C : Print info on internal configuration state" << endl;
	cerr << endl;
	cerr << "   Note: only h/+/u/r/t commands are always available. The rest, only if --enable-debug is configured" << endl;
	cerr << endl;
	
}

int globalBitRate=2816;

void ConsoleDebugger::run(){
#ifdef DEBUG_OUTPUT
	bool tmpSet;
	setThreadName("ConsoleDebugger");
#endif
	
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
			case 'c': //print mediahandler session info
			case 'C':
				showConfig();
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
			case '1':
				globalBitRate-=256;
				cerr <<"------->EEEE: bitrate="<<globalBitRate<<endl;
				break;
			case '2':
				globalBitRate+=256;
				cerr <<"------->EEEE: bitrate="<<globalBitRate<<endl;
				break;
			case 'P':
			case 'p':
				sipStack->setDebugPrintPackets(!sipStack->getDebugPrintPackets() );
				if (sipStack->getDebugPrintPackets() )
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
	
			case 'a':
			case 'A':
				printThreads();
				break;
			case '<':
			case '*':
				showMem();
				break;

			case '>':
				showMemSummary();
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
				cerr << sipStack->getTimeoutProvider()->getTimeouts() << endl;
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
			SipSMCommand::dialog_layer,
			SipSMCommand::dispatcher);
	sipStack->handleCommand(cmd);
}

void ConsoleDebugger::sendCommandToMediaHandler( string str ) {
	CommandString cmdstr ("", str);
	cerr << "========= MediaHandler Debug info : " << endl;
	if (mediaHandler)
		mediaHandler->handleCommand("media", cmdstr );
	else{
		cerr << "(no media handler registred)"<<endl;
	}
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

void ConsoleDebugger::showMemSummary(){
	string all;
	minilist<string> names = getMemObjectNamesSummary();
	int i;
	for (i=0; i<names.size();i++){
		all = all+names[i]+"\n";
	}
	cerr << all << i <<" types of objects"<< endl;
}

void ConsoleDebugger::showStat(){
	mout << sipStack->getStackStatusDebugString();
}

void ConsoleDebugger::showConfig(){
	cerr <<    "SipSoftPhoneConfiguration:"<<endl
		<< "  Minisip:" << endl
		<< "    useSTUN="<<config->useSTUN<<endl
		<< "    stunServerIpString="<<config->stunServerIpString<<endl
		<< "    stunServerPort="<<config->stunServerPort<<endl
		<< "    findStunServerFromSipUri="<<config->findStunServerFromSipUri<<endl
		<< "    findStunServerFromDomain="<<config->findStunServerFromDomain<<endl
		<< "    stunDomain="<<config->stunDomain<<endl
		<< "    useUserDefinedStunServer="<< config->useUserDefinedStunServer <<endl
		<< "    userDefinedStunServer=" << config->userDefinedStunServer << endl
		<< "    soundDeviceIn="<< config->soundDeviceIn  <<endl
		<< "    soundDeviceOut="<< config->soundDeviceOut<< endl
		<< "    videoDevice="<< config->videoDevice << endl
		<< "    frameWidth=" << config->frameWidth << endl
		<< "    frameHeight="<< config->frameHeight << endl
		<< "    usePSTNProxy="<< config->usePSTNProxy <<endl
// 		<< "    tcp_server="<< config->tcp_server<<  endl
// 		<< "    tls_server="<< config->tls_server << endl
		<< "    ringtone="<< config->ringtone << endl
		<< "    soundIOmixerType="<<config->soundIOmixerType <<endl
		<< "    networkInterfaceName="<< config->networkInterfaceName <<endl
		<< "  SipStackConfig:"<<endl
		<< "    localIpString="<<config->sipStackConfig->localIpString<<endl
		<< "    localIp6String="<< config->sipStackConfig->localIp6String << endl
		<< "    externalContactIP=" << config->sipStackConfig->externalContactIP << endl
		<< "    externalContactUdpPort=" << config->sipStackConfig->externalContactUdpPort << endl
// 		<< "    preferedLocalUdpPort="<< config->sipStackConfig->preferedLocalUdpPort << endl //TODO: output actual port in use as well?
// 		<< "    preferedLocalTcpPort="<< config->sipStackConfig->preferedLocalTcpPort << endl
// 		<< "    preferedLocalTlsPort="<< config->sipStackConfig->preferedLocalTlsPort << endl
		<< "    autoAnswer="<< config->sipStackConfig->autoAnswer<<endl
		<< "    use100Rel="<< config->sipStackConfig->use100Rel<< endl
		<< "    instanceId="<< config->sipStackConfig->instanceId<<endl
		<< "    Certificates:"<<endl;
		
	if (config->sipStackConfig->cert)
		config->sipStackConfig->cert->lock();
	if (config->sipStackConfig->cert && config->sipStackConfig->cert->length()>0){
		int n=1;
		MRef<Certificate *> crt=config->sipStackConfig->cert->getFirst();
		while (crt){
			cerr << "      certificate "<<n<<endl
			     << "        name="<<crt->getName()<<endl
			     << "        cn="<<crt->getCn()<<endl
			     << "        issuer="<<crt->getIssuer()<<endl
			     << "        issuer_cn="<< crt->getIssuerCn()<<endl
			     << "        has_pk="<< crt->hasPk()<<endl
			     << "        SubjectAltName,"<< endl;
#if 0
//Print all subjectAltName here - FIXME
			//Note: if the enum declaration in cert.h changes
			//we get a bug here.
			char *types[]={0,"SAN_DNSNAME", "SAN_RFC822NAME", "SAN_URI", "SAN_IPADDRESS",0};
			certificate::SubjectAltName san= certificate::SAN_DNSNAME;

			// there are four alt name types, and each can be
			// multiple values.
			for (int i=1; types[i]; i++){
				cerr << "          type "<< types[i]<<": ";
				vector<string> alt = crt->getAltName(san);
				vector<string>::iterator j;
				int n=0;
				for (j=alt.begin(); j!=alt.end(); j++,n++){
					if (n){
						cerr << ", "; // do not output before first element
					}
					cerr << *j;
				}
				cerr << endl;
				san=san+1;
			}
#endif
			crt = config->sipStackConfig->cert->getNext();
			n++;
		}
	}else{
		cerr    <<      "        (no certificate)"<<endl;
	}
	if (config->sipStackConfig->cert)
		config->sipStackConfig->cert->unlock();

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
	sipStack = NULL;
	thread = NULL;
}

