#ifndef _CONSOLEDEBUGGER_H
#define _CONSOLEDEBUGGER_H

#include<config.h>

#include<libmutil/MemObject.h>
#include"../sip/SipSoftPhoneConfiguration.h"
#include<libmutil/Thread.h>
#include"../mediahandler/MediaHandler.h"

#include<string>

class ConsoleDebugger : public Runnable{
	public:
		ConsoleDebugger(MRef<SipSoftPhoneConfiguration *> conf);
		~ConsoleDebugger();
		
		std::string getMemObjectType(){return "ConsoleDebugger";}
		
		void showHelp();
		void showMem();
		void showStat();

		void sendManagementCommand( string str );
		void sendCommandToMediaHandler( string str );
		
		MRef<Thread *> start();
		
		virtual void run();
		
		void stop();
		
		void join();
		
		void setMediaHandler( MRef<MediaHandler *> r ) { mediaHandler = r;}

	private:
		void showDialogInfo(MRef<SipDialog *> d, bool usesSM);
		MRef<SipSoftPhoneConfiguration *> config;
		MRef<MediaHandler *> mediaHandler;
		
		MRef<Thread *> thread;
		bool keepRunning;
};

#endif
