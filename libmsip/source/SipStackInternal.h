/*
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2004, 2005, 2006
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/



#ifndef _SIPSTACKINTERNAL_H
#define _SIPSTACKINTERNAL_H


#include<libmutil/CommandString.h>
#include<libmsip/SipDialogConfig.h>
#include<libmsip/SipTimers.h>
#include<libmsip/SipTransport.h>

#include"SipLayerTransport.h"
#include"transactions/SipTransaction.h"
#include<libmcrypto/cert.h>
#include<libmutil/MessageRouter.h>
#include<libmutil/TimeoutProvider.h>
#include<libmutil/StateMachine.h>

class SipDialog;

class SipStackInternal : public SipSMCommandReceiver, public Runnable{

	public:
		SipStackInternal( MRef<SipStackConfig*> stackConfig );

		void setTransactionHandlesAck(bool transHandleAck);

		void setDefaultDialogCommandHandler(MRef<SipDefaultHandler*> cb);
		MRef<SipDefaultHandler*> getDefaultDialogCommandHandler();

		virtual std::string getMemObjectType() const {return "SipStackInternal";}
		
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
		MRef<SipStackConfig*> getStackConfig();

		void addSupportedExtension(std::string extension);
		std::string getAllSupportedExtensionsStr();
		bool supports(std::string extension);
                
		std::string getStackStatusDebugString();

		MRef<SipTransportConfig*> findTransportConfig( const std::string &transportName ) const;

		/** Start all enabled SIP(S) servers */
		void startServers();

		/** Start all enabled SIP(S) servers */
		void stopServers();

		void startServer( const std::string &transportName );

		void stopServer( const std::string &transportName );

		int32_t getLocalSipPort(bool usesStun, const std::string &transport);

		void free();

		std::string createClientTransaction(MRef<SipRequest*>);

		void setInformTransactionTerminate(bool doInform);

	protected:
		void startSipServers();
		void startSipsServers();
		void startServers( bool secure, int32_t &prefPort );
		void startServer( MRef<SipTransport*> transport,
				  int32_t &port );

		void stopServers( bool secure );
		void stopServer( MRef<SipTransport*> transport );

	private:

		MRef<SipTimers*> timers;
		MRef<SipStackConfig *> config;
		MRef<CommandReceiver*> callback;
		
		MRef<CommandReceiver*> confCallback;	//hack to make conference calling work until the ConfMessageRouter is removed
		
		//
		MRef<SipCommandDispatcher*> dispatcher;

		MRef<TimeoutProvider<std::string, MRef<StateMachine<SipSMCommand,std::string>*> > *> timeoutProvider;

		std::list<std::string> sipExtensions;
};




#endif
