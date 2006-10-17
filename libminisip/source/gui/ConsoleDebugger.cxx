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

#include<libminisip/mediahandler/MediaCommandString.h>

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

ConsoleDebugger::ConsoleDebugger(MRef<SipStack*> stack): 
			sipStack(stack), 
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
#ifdef DEBUG_OUTPUT
	bool tmpSet;
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

void ConsoleDebugger::showStat(){
	mout << sipStack->getStackStatusDebugString();
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

