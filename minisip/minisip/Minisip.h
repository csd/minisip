#ifndef MINISIP_H
#define MINISIP_H

#include<config.h>
#include"../conf/ConferenceControl.h"
#include<libmutil/MemObject.h>
#include<libmutil/Thread.h>
#include<libmutil/StateMachine.h>
//#include"ConsoleDebugger.h"

class MediaHandler;
class Gui;
class SipSoftPhoneConfiguration;
class Sip;
class SipSMCommand;
class ConsoleDebugger;
class ConferenceControl;


class Minisip : public Runnable{
	public:
		Minisip( int argc, char**argv );
		virtual ~Minisip();
		
		std::string getMemObjectType(){return "Minisip";}

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
		
		/**
		This thread object contains the running sip thread ...
		We need it to stop it when quitting minisip and waiting for
		it to be finished with the sip stack shutdown.
		*/
		MRef<Thread*>  sipThread;
		
		//TimeoutProvider<string, MRef<StateMachine<SipSMCommand,string>*> > * timeoutprovider;
#ifdef GTK_GUI
#ifdef DEBUG_OUTPUT
		MRef<ConsoleDebugger *> consoleDbg;
#endif
#endif
};

#endif
