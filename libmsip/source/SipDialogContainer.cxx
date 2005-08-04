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
 * libmsip internal routing of messages and commands (timeouts are not
 * "routed" since they contain a fixed destination pointer). 
 *
 * - ALL messages, commands and timeouts MUST be enqueued to the queue
 *   of commands to be processed. This is done via the enqueue* methods
 *   in SipDialogContainer. They may not be sent to a dialog or transaction
 *   directly via the handleCommand method. Doing so may result in an
 *   unwanted behavour.
 * - Calls are routed using the following information:
 *    - The "branch" parameter in the top most via header. If a transaction
 *      with a branch value matching an incoming message will get the
 *      message.
 *    - Destination attribute in the SipSMCommand. A transaction can not
 *      handle a command with a destination of something else than
 *      "transaction" or "ANY" (i.e. will not get a command via the 
 *      handleCommand method). A dialog will can not handle a message with
 *      destination different from "dialog" or "ANY"
 *    - If no transaction with a matching "branch" parameter is
 *      found the following algorithm will be used to deliver the message:
 *        1. Try the command with all transactions (if dest=ANY or trans)
 *        2. Try the command with all dialogs (if dest=ANY or dialog)
 *        3. Try the command with the default handled (it is required to
 *           "handle" the command in some way - even if it means that it 
 *           ignoringes it).
 *  
 *
*/

#include<config.h>

#include<libmsip/SipDialogContainer.h>

#include<libmsip/SipMessageTransport.h>

#include<libmutil/itoa.h>
#include<libmutil/Timestamp.h>
#include<libmutil/dbg.h>
#include<libmutil/MemObject.h>
#include<libmnetutil/IP4Address.h>

#include<libmsip/SipInvite.h>
#include<libmsip/SipCommandString.h>

#include<assert.h>

#include<libmutil/dbg.h>

SipDialogContainer::SipDialogContainer() {
	dispatcher = new SipMessageDispatcher;
	keepRunning = true;
}

void SipDialogContainer::setDefaultHandler(MRef<SipDialog*> dflt){
	defaultHandler = dflt;
}

MRef<SipDialog*> SipDialogContainer::getDefaultHandler(){
	return defaultHandler;
}

void SipDialogContainer::setCallback(SipCallback *callback){
	this->callback = callback;
}

SipCallback * SipDialogContainer::getCallback() const{
	return callback;
}

void SipDialogContainer::enqueueCommand(const SipSMCommand &command, int queue,  int priority){
	struct queue_type item;
	item.type = TYPE_COMMAND;
	item.command = MRef<SipSMCommand*>(new SipSMCommand(command));
//	mdbg << "SipDialogContainer: enqueueing command "<< command << end;
       
        mlock.lock();
	if (queue == HIGH_PRIO_QUEUE){
		if (priority==PRIO_FIRST_IN_QUEUE)
			high_prio_command_q.push_back(item);	
		else
			high_prio_command_q.push_front(item);
	}else{
		if (priority==PRIO_FIRST_IN_QUEUE)
			low_prio_command_q.push_back(item);	
		else
			low_prio_command_q.push_front(item);
	}
		
        mlock.unlock();
        semaphore.inc();
}

void SipDialogContainer::enqueueTimeout(MRef<SipTransaction*> receiver, const SipSMCommand &command){
	struct queue_type item;
	item.type = TYPE_TIMEOUT;
	item.command = MRef<SipSMCommand*>( new SipSMCommand(command));
	item.transaction_receiver = receiver;
	item.call_receiver = NULL;

        mlock.lock();
	//high_prio_command_q.push_back(item);
	low_prio_command_q.push_front(item);
        mlock.unlock();
        
        semaphore.inc();
}

void SipDialogContainer::enqueueTimeout(MRef<SipDialog*> receiver, const SipSMCommand &command){
	struct queue_type item;
	item.type = TYPE_TIMEOUT;
	item.command = MRef<SipSMCommand*>( new SipSMCommand(command));
	
	item.call_receiver = receiver;
        
        mlock.lock();
	//high_prio_command_q.push_back(item);
	low_prio_command_q.push_front(item);
        mlock.unlock();

        semaphore.inc();
}

void SipDialogContainer::stopRunning() {
	//mdbg << "SipDialogContainer ... stopping!!!!!" << end;
	keepRunning = false;
}

void SipDialogContainer::run(){
#ifdef DEBUG_OUTPUT
	static int runcount = 1;
#endif

	while (keepRunning){
#ifdef DEBUG_OUTPUT
		runcount--;
#endif
//		mdbg << "SipDialogContainer::run semdec"<< end;
		mdbg << "DIALOG CONTAINER: waiting for command"<< end;
                semaphore.dec();
//		mdbg << "SipDialogContainer::run semdec done"<< end;

#ifdef DEBUG_OUTPUT
		runcount++;
		assert(runcount==1);
#endif
		
		struct queue_type item;
                
                mlock.lock();
		if (high_prio_command_q.size()>0)
			item = high_prio_command_q.pop_back();	
		else{
			assert(low_prio_command_q.size()>0);
			item = low_prio_command_q.pop_back();	
		}
                mlock.unlock();
		mdbg << "DIALOG CONTAINER: got command!"<<end;
		if (item.type==TYPE_COMMAND)
			mdbg << "command: "<< **item.command << end;
		else
			mdbg << "timeout: "<< **item.command << end;

		
		// There are two types of "commands" - SipSMCommands and
		// timeouts. SipSMCommands are passed to the dispatcher
		// (and the defaultHandler if they are not handled).
		// Timeouts have a known receiver set in the queue item.
		if (item.type==TYPE_COMMAND){
			mdbg << "SipDialogContainer::run delivering command :: "<< **item.command << end;
		}else{
			mdbg << "SipDialogContainer::run delivering timeout :: "<< **item.command << end;
		}
		if (item.type == TYPE_COMMAND){
			if (!dispatcher->handleCommand(**(item.command)) /*&& !item.command->getDispatched() */){
				item.command->incDispatchCount(); //Used to detect loops - the "dispatched" flag is set when everyone has had a chance to handle the command. (The default handler can put it back into the queue after creating some receiver.)
				defaultHandler->handleCommand(**(item.command));
//				item.command->setDispatched(true);
			}
		}else{  //item.type == TYPE_TIMEOUT
			if ( !item.transaction_receiver.isNull() ){
				item.transaction_receiver->handleCommand(**item.command);
			}

			if ( !item.call_receiver.isNull() )
				item.call_receiver->handleCommand(**item.command);
		}

		item.command=NULL;
                item.transaction_receiver=NULL;
                item.call_receiver=NULL;
	}
}

void SipDialogContainer::addDialog(MRef<SipDialog*> call){
	dispatcher->addDialog(call);
}

MRef<SipMessageDispatcher*> SipDialogContainer::getDispatcher() const {
	return dispatcher;
}

