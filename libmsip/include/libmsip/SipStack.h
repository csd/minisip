/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/


/*

 +-SipStack-------------------------------------+
 |                                              |  invite(string)
 | +-SipDialogContainer---+                     |<----------------------------------
 | |                      | addDialog(d)        |
 | |                      |<----------------    | handleCommand(SipSMCommand)
 | |                      |                     |<----------------------------------
 | |                      | handleCommand(CS)   |
 | |                      |-------------------->| sipcb_handleCommand(CommandString)
 | |                      |                     |---------------------------------->
 | |                      | sipcb_handleCommand |
 | |                      |-------------------->|  setDefaultDialog()
 | |                      |                     |<----------------------------------
 | |                      | enqueuePacket()     |
 | |                      |<----------------    |  addDialog(d)
 | |                      |                     |<----------------------------------
 | |                      | enqueueCommand()    |
 | |                      |<----------------    |
 | |                      |                     |
 | | [call_list]          | handleSipMessage()  |
 | | [defaultHandler]     |<---+                |
 | +----------------------+    |                |
 |                             |                |
 | [SipLayerTransport]-------+                |
 | [SipSoftPhoneConfiguration]                  |
 |                                              |
 |                                              |
 +----------------------------------------------+


 SipStack object figure:

 +-----------------------------------------------------------------+
 |SipStack                                                         |
 |                                                                 |
 |                                                                 |              ...................
 | +--------------+------------------+ +----------+                |              .Callback
 | |dialogDefHandl|SipLayerDialog    | |Sip       |                |              .(SipMessageRouter)
 | |(3)           |                  | |Message   |                |       (5)    .
 | |              | (dialogs)        | |Dispatcher| enqueueCommand | handleCommand.
 | |              |                  | |(4)       |<---------------|<-------------.
 | +--------------+------------------+ |          |                |              .
 |                                     |          |                | handleCommand. 
 | +---------------------------------+ |          |                |------------->.
 | |SipLayerTransaction              | |          |                |              . 
 | |                                 | |          |                |              .
 | |                (transactions)   | |          |                |              .
 | |defCHandl. (2)                   | |          |                |              ...................
 | +---------------------------------+ |          |                |
 |                                     |          |                |
 | +---------------------------------+ |          |                |
 | |SipLayerTransport  (1)           | |          |                |
 | |                                 | |          |                |
 | |                                 | |          |                |
 | +---------------------------------+ +----------+                |
 +-----------------------------------------------------------------+

 Comments:
  (1) Incoming messages are sent to the transaction layer via the dispatcher (placed in 
      the low prio queue). 
  (2) If there are no transaction handling a SIP message passed to the transaction layer,
      it will be passed to the "defaultCommandHandler" that will create a transaction
      for any SIP request.
  (3) Any messages not handled by the existing dialogs in the dialog layer are passed
      to the "defaultHandler". It is application specific and is set using 
      SipStack::setDefaultDialogCommandHandler(handler). The handler implements
      SipSMCommand (i.e. handleCommand(SipSMCommand cmd) ).
  (4) The dispatcher maintains a priority queue of commands to process, and sends
      commands to the layer set in its destination field.
  (5) The SipStack sends commands to a "CommandReceiver" object. The commands
      sent and that are expected to be received are application specific (i.e.
      it's the interface to the dialogs created by the default dialog handler).
      In libminisip the CommandReceiver is the MessageRouter object.
 
*/

#ifndef LIBMSIP_SipStack_H
#define LIBMSIP_SipStack_H


#include<libmsip/libmsip_config.h>


#include<libmutil/minilist.h>
#include<libmutil/CommandString.h>
#include<libmsip/SipTransaction.h>
//#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipDialogConfig.h>
#include<libmsip/SipTimers.h>

#include<libmsip/SipLayerTransport.h>
#include<libmutil/cert.h>
#include<libmutil/MessageRouter.h>

class SipDialog;
class SipTransaction;

//TODO: Enable conference calling

class LIBMSIP_API SipStack: public SipSMCommandReceiver, public Runnable{

	public:
		SipStack( MRef<SipCommonConfig*> stackConfig,
				MRef<certificate_chain *> cert=NULL,	//The certificate chain is used by TLS 
								//TODO: TLS should use the whole chain instead of only the first certificate --EE
				MRef<ca_db *> cert_db = NULL,
				MRef<TimeoutProvider<std::string, MRef<StateMachine<SipSMCommand,std::string>*> > *> tp= NULL
			  );

		void setTransactionHandlesAck(bool transHandleAck);

		void setDefaultDialogCommandHandler(MRef<SipSMCommandReceiver*> cb);

		virtual std::string getMemObjectType(){return "SipStack";}
		
                virtual void run();

		MRef<SipCommandDispatcher*> getDispatcher();

		bool handleCommand(const CommandString &cmd){
			//Commands from the gui etc is always sent to the
			//TU layer
			SipSMCommand c(cmd, SipSMCommand::dialog_layer, SipSMCommand::dialog_layer);
			return handleCommand(c);
		}

		bool handleCommand(const SipSMCommand &command);
		
		void setCallback(MRef<CommandReceiver*> callback);	//Rename to setMessageRouterCallback?
		MRef<CommandReceiver *> getCallback();

		void setConfCallback(MRef<CommandReceiver*> callback); // Hack to make the conference calling work - should not be here FIXME
		MRef<CommandReceiver *> getConfCallback();
		
		//void setDefaultHandler(MRef<SipDialog*> d);

		void addDialog(MRef<SipDialog*> d);

//		MRef<SipLayerTransport *> getSipTransportLayer(){return transportLayer;}

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
