#ifndef _CONSOLEDEBUGGER_H
#define _CONSOLEDEBUGGER_H

#include<libmutil/MemObject.h>
#include"../sip/SipSoftPhoneConfiguration.h"
#include<libmutil/Thread.h>
#include<string>

class ConsoleDebugger : public Runnable{
	public:
		ConsoleDebugger(MRef<SipSoftPhoneConfiguration *> conf):config(conf){};
		std::string getMemObjectType(){return "ConsoleDebugger";}
		void showMem();
		void showStat();
		void startThread();

		virtual void run();

	private:
		void showDialogInfo(MRef<SipDialog *> d, bool usesSM);
		MRef<SipSoftPhoneConfiguration *> config;
};

#endif
