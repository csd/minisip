#ifndef MINISIP_H
#define MINISIP_H

#include<libminisip/libminisip_config.h>

#include<libmutil/MemObject.h>
#include<libminisip/media/SubsystemMedia.h>

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

		void setConfigurationLocation(std::string c){
			confPath = c;
		}


		/**
		 * Tells minisip that it better prepare to be shut down.
		 */
		int stop();

#if 0
		/**
		 * Tell the internal worker threads to stop execution.
		 */
		int kill();
#endif 

		/**
		 * Wait for internal worker threads to exit (this is 
		 * normally called after "kill").
		 */
		int join();

		/**
		 * equal to stop() followed by kill and join()
		 */
		int exit();

		int startSip();
		int runGui();

		void startDebugger();
		void stopDebugger();

		static void doLoadPlugins(char **argv);

	private:
		static bool pluginsLoaded;
		
		int initParseConfig();

		std::string confPath;
		MRef<SubsystemMedia *> subsystemMedia;
		MRef<Gui *> gui;
		MRef<SipSoftPhoneConfiguration *> phoneConf;
		MRef<Sip *> sip;
		MRef<MessageRouter*> messageRouter;
		MRef<ConfMessageRouter*> confMessageRouter;
		MRef<ConsoleDebugger *> consoleDbg;
};

#endif
