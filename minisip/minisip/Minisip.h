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


class Minisip : public MObject{
	public:
		Minisip( int argc, char**argv );
		virtual ~Minisip();
		
		std::string getMemObjectType(){return "Minisip";}

		void exit();
		void startSip();
		void runGui();
	private:
		
		void initParseConfig();

		std::string conffile;
		MRef<MediaHandler *> mediaHandler;
		Gui * gui;
		MRef<SipSoftPhoneConfiguration *> phoneConf;
		MRef<Sip *> sip;
		
#ifdef GTK_GUI
	#ifdef DEBUG_OUTPUT
		MRef<ConsoleDebugger *> consoleDbg;
	#endif
#endif
};

#endif
