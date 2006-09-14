#ifndef _SIPSTACKINTERNAL_H
#define _SIPSTACKINTERNAL_H


#include<libmutil/CommandString.h>
#include<libmsip/SipTransaction.h>
#include<libmsip/SipDialogConfig.h>
#include<libmsip/SipTimers.h>

#include<libmsip/SipLayerTransport.h>
#include<libmcrypto/cert.h>
#include<libmutil/MessageRouter.h>

class SipDialog;
class SipTransaction;



class SipStackInternal : public SipSMCommandReceiver, public Runnable{

	public:
		SipStackInternal( MRef<SipCommonConfig*> stackConfig,
				MRef<certificate_chain *> cert=NULL,	//The certificate chain is used by TLS 
								//TODO: TLS should use the whole chain instead of only the first certificate --EE
				MRef<ca_db *> cert_db = NULL
			  );

		void setTransactionHandlesAck(bool transHandleAck);

		void setDefaultDialogCommandHandler(MRef<SipSMCommandReceiver*> cb);

		virtual std::string getMemObjectType(){return "SipStackInternal";}
		
                virtual void run();
		virtual void stopRunning();

		MRef<SipCommandDispatcher*> getDispatcher();

		bool handleCommand(const CommandString &cmd);

		bool handleCommand(const SipSMCommand &command);
		
		void setCallback(MRef<CommandReceiver*> callback);	//Rename to setMessageRouterCallback?
		MRef<CommandReceiver *> getCallback();

		void setConfCallback(MRef<CommandReceiver*> callback); // Hack to make the conference calling work - should not be here FIXME
		MRef<CommandReceiver *> getConfCallback();
		
		void addDialog(MRef<SipDialog*> d);

		/**
		 * Each SipStack object creates a TimeoutProvider that with
		 * a thread of it's own keeps track of timers waiting to
		 * fire. This method is used by the transactions and
		 * dialogs that wish to use timeouts to retrieve which
		 * timeout provider to use.
		 */
		MRef<TimeoutProvider<std::string, MRef<StateMachine<SipSMCommand,std::string>*> > *> getTimeoutProvider();

		MRef<SipTimers*> getTimers();
		MRef<SipCommonConfig*> getStackConfig(){return config;}

		void addSupportedExtension(std::string extension);
		std::string getAllSupportedExtensionsStr();
		bool supports(std::string extension);
                
	private:
		MRef<SipTimers*> timers;
		MRef<SipCommonConfig *> config;
		MRef<CommandReceiver*> callback;
		
		MRef<CommandReceiver*> confCallback;	//hack to make conference calling work until the ConfMessageRouter is removed
		
		//
		MRef<SipCommandDispatcher*> dispatcher;

		MRef<TimeoutProvider<std::string, MRef<StateMachine<SipSMCommand,std::string>*> > *> timeoutProvider;

		std::list<std::string> sipExtensions;
};




#endif
