/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/


/* 
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef SIPDIALOGCONTAINER_H
#define SIPDIALOGCONTAINER_H

#ifdef _MSC_VER
#ifdef LIBMSIP_EXPORTS
#define LIBMSIP_API __declspec(dllexport)
#else
#define LIBMSIP_API __declspec(dllimport)
#endif
#else
#define LIBMSIP_API
#endif

#include<libmutil/minilist.h>
#include<libmutil/Semaphore.h>
#include<libmutil/MemObject.h>
#include<libmsip/SipCallback.h>
#include<libmsip/SipMessageDispatcher.h>
#include<libmsip/SipSMCommand.h>


using namespace std;

#define TYPE_COMMAND 2
#define TYPE_TIMEOUT 3

#define HIGH_PRIO_QUEUE 2
#define LOW_PRIO_QUEUE 4

//Enqueue priorities
#define PRIO_LAST_IN_QUEUE 1
#define PRIO_FIRST_IN_QUEUE 2

class SipTransaction;
class SipDialog;

/**
 * queue_type: For internal use only!
 *
 */
typedef struct queue_type{
	int type;
	MRef<SipSMCommand*> command;
	MRef<SipTransaction*> transaction_receiver;
	MRef<SipDialog*> call_receiver;
} queue_type;


/**
 * SIP applications usually contains a number of <i>SIP dialogs</i>. In libmsip, 
 * a </i>SIP dialog</i> is a state machine that represents the local application
 * state and zero or more transactions that controls a message transfer
 * between two <i>SIP dialogs</i> in two different User Agents. The
 * SipDialogContainer is a collection of <i>SIP dialogs</i> and logic to
 * deliver an incoming SIP message to the right dialog or transaction.
 *
 * An application keeps all its dialogs in the dialog container
 * and when a message is received it sends it to the container
 * via the <b>enqueueCommand</b> method.
 *
 * Commands (SIP messages, command strings and timeouts) can arrive before
 * the processing of an earlier command is finished (the processing of a
 * command can also generate new commands). The dialog container maintains
 * a list (or two to be precise) of incoming commands to be processed.
 * Commands can be enqueued with different priorities (in the front or
 * end of the two queues), but an application using libmsip should always
 * use the default values when a new message arrives or when a string
 * command is sent to the SIP stack.
 * 
 * If no dialog handled in incoming command (command string or SIP message)
 * it is sent to the <i>default handler<i> that can be set by the 
 * application. 
 *
 * 
 * 
 * @author Erik Eliasson, eliasson@it.kth.se
 */
class LIBMSIP_API SipDialogContainer : public MObject{

	public:
		SipDialogContainer();

		virtual std::string getMemObjectType() {return "SipDialogContainer";}

		/**
		 * When no dialog or transaction handles a packet it is
		 * sent to the <i>default handler</i>. A typical use for
		 * this is to create a new VoIP dialog when an <i>SIP
		 * INVITE</i> message is received.
		 * @param defaultHandler	<i>SipDialog</i> that
		 * 				will receive all SIP
		 * 				messages and command
		 * 				strings that are not
		 * 				handled by any other
		 * 				dialogor transaction.
		 * 
		 */
		void setDefaultHandler(MRef<SipDialog*> defaultHandler);

		MRef<SipDialog *> getDefaultHandler();
		
		/**
		 * SIP messages and CommandStrings are passed to the SIP
		 * dialogs and transactions via the enqueueCommand method.
		 * 
		 * The message will be passed to the default dialog if no
		 * dialog or transaction handles the message.
		 *
		 * The enqueueCommand will not wait until the command has
		 * been processed, and will return immediately once the
		 * command has been enqueued.
		 * 
		 * @param cmd	command that will be passed to the SIP
		 * 		dilalogs/transaction.
		 * @param queue	Two queues are maintained. One high and one
		 * 		low priority. Messages are only taken from
		 * 		the low priority queue if the high priority
		 * 		one is empty. The low priority queue should
		 * 		always be used unless in certain
		 * 		circumstances (transaction removal messages
		 * 		etc).
		 * @param priority	Messages may be places either first
		 * 			or last in a queue. Use the default
		 * 			(last in queue).
		 * @see SipSMCommand
		 */
		void enqueueCommand(const SipSMCommand &cmd, int queue=LOW_PRIO_QUEUE, int priority=PRIO_LAST_IN_QUEUE);


		void enqueueTimeout(MRef<SipTransaction*> receiver, const SipSMCommand &);
		void enqueueTimeout(MRef<SipDialog*> receiver, const SipSMCommand &);

		/**
		 * The dialog container uses one thread to do the command
		 * processing. The thread is running (blocking) in the
		 * <i>run</i> method, and will never return. The thread
		 * running the run method will wait for commands and
		 * timeouts put into the queues using the enquque* methods.
		 */
                virtual void run();
		
		MRef<SipMessageDispatcher*> getDispatcher() const;

		void addDialog(MRef<SipDialog*> call);

		void setCallback(SipCallback* callback);
		SipCallback* getCallback() const;
                
	private:
		MRef<SipMessageDispatcher*> dispatcher;
		
		minilist<queue_type> high_prio_command_q;
		minilist<queue_type> low_prio_command_q;
		
		MRef<SipDialog*> defaultHandler;
		SipCallback* callback;

                Semaphore semaphore;
                Mutex mlock;
                
};

#include<libmsip/SipTransaction.h>
#include<libmsip/SipDialog.h>

#endif
