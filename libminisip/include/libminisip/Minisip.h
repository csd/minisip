#ifndef MINISIP_H
#define MINISIP_H

#include<libminisip/libminisip_config.h>

#include<libmutil/MemObject.h>

class MediaHandler;
class Gui;
class SipSoftPhoneConfiguration;
class Sip;
class SipSMCommand;
class ConferenceControl;
class ConsoleDebugger;
class MessageRouter;
class ConfMessageRouter;


class LIBMINISIP_API Minisip : public MObject{
	public:
		Minisip( MRef<Gui *> gui, int argc, char**argv );
		virtual ~Minisip();
		
		std::string getMemObjectType() const {return "Minisip";}

		int exit();
		int startSip();
		int runGui();

		void startDebugger();
		void stopDebugger();

	private:
		
		int initParseConfig();

		MRef<MediaHandler *> mediaHandler;
		MRef<Gui *> gui;
		MRef<SipSoftPhoneConfiguration *> phoneConf;
		MRef<Sip *> sip;
		MRef<MessageRouter*> messageRouter;
		MRef<ConfMessageRouter*> confMessageRouter;
		MRef<ConsoleDebugger *> consoleDbg;
};

#endif
