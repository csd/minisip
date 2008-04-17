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

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/


/*
 SipStack proxies requests to SipStackInternal. SipStack is part of 
 libmsip API while SipStackInternal is not.
 SipStack object figure:

 +-----------------------------------------------------------------+ +-+
 |SipStackInternal                                                 | | |
 |                                                                 | | |
 |                                                                 | | |               ...................
 | +--------------+------------------+ +----------+                | | |               .Callback
 | |dialogDefHandl|SipLayerDialog    | |Sip       |                | | |               .(SipMessageRouter)
 | |(3)           |                  | |Command   |                | |S|   (5)         .
 | |              | (dialogs)        | |Dispatcher| enqueueCommand | |i| handleCommand .
 | |              |                  | |(4)       |<---------------| |p| <-------------.
 | +--------------+------------------+ |          |                | |S|               .
 |                                     |          |                | |t| handleCommand . 
 | +---------------------------------+ |          |                | |a| ------------> .
 | |SipLayerTransaction              | |          |                | |c|               . 
 | |                                 | |          |                | |k|               .
 | |                (transactions)   | |          |                | | |               .
 | |defCHandl. (2)                   | |          |                | | |               ...................
 | +---------------------------------+ |          |                | | |
 |                                     |          |                | | |
 | +---------------------------------+ |          |                | | |
 | |SipLayerTransport  (1)           | |          |                | | |
 | |                                 | |          |                | | |
 | |                                 | |          |                | | |
 | +---------------------------------+ +----------+                | | |
 +-----------------------------------------------------------------+ +-+

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

class SipStackInternal;


#include<libmutil/CommandString.h>
#include<libmsip/SipDialogConfig.h>
#include<libmutil/Thread.h>
#include<libmsip/SipTimers.h>
#include<libmcrypto/cert.h>
#include<libmutil/MessageRouter.h>
#include<libmsip/SipSMCommand.h>
#include<libmutil/TimeoutProvider.h>
#include<libmutil/StateMachine.h>

class SipDialog;

#define HIGH_PRIO_QUEUE 2
#define LOW_PRIO_QUEUE 4
/**
 * \brief The SipDefaultHandler of an application is the class
 *        that decides how the application will behave when
 *        SIP messages are received or the GUI gives the "sip"
 *        subsystem a command. A typical task of the default
 *        handler is to add dialogs to the SIP stack as a 
 *        reaction to an incoming message or a command from
 *        the GUI.
 *
 * An application needs to "hook into" the SIP stack and decide
 * how it should handle messages from the network, GUI or commands
 * generated from within the SIP stack (such as transport error 
 * messages). The application creates a sub-class of SipDefaultHandler
 * and implements four handler methods.
 *
 * An example of what the default handler could do is:
 *  - Create a server voip dialog an INVITE message is receved
 *  - Create a client voip dialog if the GUI sends a command
 *    indicating that the user wants to make a call
 */
class LIBMSIP_API SipDefaultHandler : public SipSMCommandReceiver, public CommandReceiver{
	public:
		/**
		 * If the SIP stack does not handle a command sent to the
		 * SIP subsystem, then the default handler will receive it.
		 *
		 * This method is required to be a "CommandReceiver" that
		 * receives commands from the MessageRouter.
		 *
		 * @param subsystem	This argument will be "sip"
		 * @param cmd		
		 */
		virtual void handleCommand(std::string subsystem, const CommandString &cmd)=0;

		/**
		 * This method is similar to handleCommand(subsystem, cmd)
		 * with the difference that it returns a response to the
		 * caller.
		 */
		virtual CommandString handleCommandResp(std::string subsystem, const CommandString &cmd)=0;

		/**
		 * This method is what the SIP stack uses to communicate
		 * with the default handler. The handler receives
		 * SipSMCommand objects that contain either
		 *  - A SIP message received by the transport layer
		 *   or
		 *  - A CommandString object (a string such as
		 *  "transport_error")
		 *  plus additional information such as what layer sent
		 *  the message (transport, transaction or dialog).
		 */
		virtual bool handleCommand(const SipSMCommand &cmd)=0;
};


class LIBMSIP_API SipTransportConfig : public MObject{
	public:
		SipTransportConfig() {}

		SipTransportConfig( const std::string &name );

		/** Transport plugin name */
		std::string getName() const;

		bool isEnabled() const;

		void setEnabled( bool enabled );

		int operator==( const SipTransportConfig &dev ) const;

	private:
		std::string name;
		bool enabled;
};


class LIBMSIP_API SipStackConfig : public MObject{
	public:
		SipStackConfig();

		virtual std::string getMemObjectType() const {return "SipStackConfig";}
		
		//shared with Dialog config
		std::string localIpString; //GEneral->Network Interface
		std::string localIp6String;
		std::string externalContactIP;
		int32_t externalContactUdpPort;
                
		int32_t preferedLocalSipPort;
		int32_t preferedLocalSipsPort;

		bool autoAnswer;


                /**
                 * This is the setting telling a dialogwhether it should
                 * use the 100rel extension if the remote user supports it.
                 */
                bool use100Rel;

		/**
		 * The certificate chain is used by TLS
		 */
		MRef<CertificateChain *> cert;

		/**
		* TODO: TLS should use the whole chain instead of only the first certificate --EE
		*/
		MRef<CertificateSet *> cert_db;

		/**
		 * "sip.instance" media feature tag in the Contact
		 * header field.  This is a Uniform Resource Name (URN) that
		 * uniquely identifies this specific UA instance.
		 * It's used by draft-Outbound and draft-GRUU.
		 */
		std::string instanceId;

		/**
		 * Configuration for all transports, UDP, TCP etc.
		 */
		std::list<MRef<SipTransportConfig*> > transports;
};

//TODO: Enable conference calling


/**
 * \brief A SipStack object is the interface to the libmsip SIP
 *        stack. An application sends commands to
 *        the SipStack which sends commands (CommandString)
 *        to the application.
 *
 * Ideally, an application can create any number of SipStacks
 * that can be configured independently. This is true for
 * SIP timers, the transport layer, and all dialogs/transactions.
 * This is not true for what content types and SIP headers
 * are understood.
 *
 * The most commonly used methods of the SipStack API are:
 *  - handleCommand(SipSMCommand)
 *    Application to SIP stack command. For example, the following code
 *    will hang up a call with the matching call id.
 *      SipSMCommand cmd(callid,"hang_up");
 *      stack->handleCommand(cmd)
 *    Note that constants defined in SipCommandString.h should be used
 *    instead of locally declaring the command string (in this case
 *    SipCommandString::hang_up).
 *
 *  - The object passed to setCallback will receive commands from the
 *    SIP stack. The commands are passed as CommandString objects.
 *  - string SipStack::invite(string uri)
 *    This is the exception to the rule that all messages are passed
 *    using the handleCommand method and the similar one in the callback.
 *    This method is introduced so that the application knows the
 *    call id of the dialog created..
 *
 * See the main documentation document for how to implement application
 * layer support for new header and content types (@see Factories).
*/
class LIBMSIP_API SipStack : public CommandReceiver, public Runnable{
	public:
		SipStack( MRef<SipStackConfig*> stackConfig );

		~SipStack();

		void free();
		
		void setTransactionHandlesAck(bool transHandleAck);
		void setDefaultDialogCommandHandler(MRef<SipDefaultHandler*> cb);
                virtual void run();
		virtual void stopRunning();
		void handleCommand(std::string subsystem, const CommandString &cmd);

		CommandString handleCommandResp(std::string subsystem, const CommandString &cmd);

		bool handleCommand(const CommandString &cmd);
		bool handleCommand(const SipSMCommand &cmd);
		void enqueueTimeout(MRef<SipDialog*> receiver, const SipSMCommand &cmd);
		void enqueueCommand(const SipSMCommand &cmd, int queue=LOW_PRIO_QUEUE);

		/**
		 * Creates a client transaction, and lets it start handling
		 * a request.
		 *
		 * This allows the TU to know the branch parameter that 
		 * a request will have.
		 *
		 * NOTE: A UA should NOT need to use this method. Only in
		 * special cases, the UA (or dialog layer, to be more
		 * specific), needs to know about the transaction ID of a
		 * newly created transaction.
		 *
		 * @return Transaction id of newly created transaction.
		 */
		std::string createClientTransaction(MRef<SipRequest*>);

		void setCallback(MRef<CommandReceiver*> callback);	//Rename to setMessageRouterCallback?
		MRef<CommandReceiver*> getCallback();	
		void setConfCallback(MRef<CommandReceiver*> callback); // Hack to make the conference calling work - should not be here FIXME
		MRef<CommandReceiver *> getConfCallback();
		void addDialog(MRef<SipDialog*> d);
		MRef<SipTimers*> getTimers();
		MRef<SipStackConfig*> getStackConfig();
		void addSupportedExtension(std::string extension);
		std::string getAllSupportedExtensionsStr();
		bool supports(std::string extension);
		MRef<TimeoutProvider<std::string, MRef<StateMachine<SipSMCommand,std::string>*> > *> getTimeoutProvider();
		void setDialogManagement(MRef<SipDialog*> mgmt);

		std::list<MRef<SipDialog *> > getDialogs();

		/**
		 * Start SIP(S) servers enabled in stack config.
		 */
		void startServers();

		void startUdpServer();
		void startTcpServer();
		void startTlsServer();

		/**
		 * Start a SIP(S) server
		 * @param transportName Name of the transport plugin,
		 * one of UDP, TCP, TLS etc.
		 */
		void startServer( const std::string &transportName );

		/**
		@return the port in use, depending on the transport.
		@param usesStun (default false), found in SipSoftPhoneConfiguration::useSTUN
		*/
		int32_t getLocalSipPort(bool usesStun=false, const std::string &transport="UDP");


		void setDebugPrintPackets(bool enable);
		bool getDebugPrintPackets();

		std::string getStackStatusDebugString();

		void setInformTransactionTerminate(bool doInform);

	private:
		friend class SipDialog;
		void *sipStackInternal; // This is pointer to a MRef<SipStackInternal*> sipStackInternal

};


#endif
