#ifndef MINISIP_H
#define MINISIP_H

#include<config.h>

#include<libmutil/MemObject.h>
#include<libmutil/Thread.h>
#include<libmutil/TimeoutProvider.h>
#include<libmutil/StateMachine.h>
//#include"ConsoleDebugger.h"

class MediaHandler;
class Gui;
class SipSoftPhoneConfiguration;
class Sip;
class SipSMCommand;
class ConsoleDebugger;


class Minisip : public Runnable{
	public:
		Minisip( int32_t argc, char**argv );
		void exit();
		void startSip();
		void runGui();
	private:
		
		virtual void run();
		
		void initParseConfig();

		std::string conffile;
		MRef<MediaHandler *> mediaHandler;
		Gui * gui;
		MRef<SipSoftPhoneConfiguration *> phoneConf;
		MRef<Sip *> sip;
		TimeoutProvider<string, MRef<StateMachine<SipSMCommand,string>*> > * timeoutprovider;
#ifdef GTK_GUI
#ifdef DEBUG_OUTPUT
		MRef<ConsoleDebugger *> consoleDbg;
#endif
#endif
};

#endif
