#ifndef MINISIP_H
#define MINISIP_H

#include<config.h>
//#include"../conf/ConferenceControl.h"
//#include<libmutil/MemObject.h>
//#include<libmutil/Thread.h>
//#include<libmutil/StateMachine.h>
//#include"ConsoleDebugger.h"

class MediaHandler;
class Gui;
class SipSoftPhoneConfiguration;
class Sip;
class SipSMCommand;
class MessageRouter;
class ConferenceControl;
#ifdef GTK_GUI
	#ifdef DEBUG_OUTPUT
		class ConsoleDebugger;
	#endif
#endif


class Minisip : public MObject{
	public:
		Minisip( int argc, char**argv );
		virtual ~Minisip();
		
		std::string getMemObjectType(){return "Minisip";}

		int exit();
		int startSip();
		int runGui();
	private:
		
		int initParseConfig();

		std::string conffile;
		MRef<MediaHandler *> mediaHandler;
		MRef<Gui *> gui;
		MRef<SipSoftPhoneConfiguration *> phoneConf;
		MRef<Sip *> sip;
		MessageRouter * ehandler;
		
#ifdef GTK_GUI
	#ifdef DEBUG_OUTPUT
		MRef<ConsoleDebugger *> consoleDbg;
	#endif
#endif
};

#endif
