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



#include<config.h>

#include<libmsip/SipCommandDispatcher.h>
#include<libmsip/SipTransaction.h>
#include<libmsip/SipDialog.h>
#include<libmsip/SipLayerDialog.h>
#include<libmsip/SipLayerTransaction.h>
#include<libmsip/SipCommandString.h>

using namespace std;

SipCommandDispatcher::SipCommandDispatcher(MRef<SipStack*> stack, MRef<SipLayerTransport*> transp):sipStack(stack),keepRunning(true){
	transportLayer = transp;
	transactionLayer = new SipLayerTransaction(this,transportLayer);
	dialogLayer = new SipLayerDialog(this);

	//transportLayer->setSipSMCommandReceiver(this);
	transportLayer->setDispatcher(this);


}

void SipCommandDispatcher::setDialogManagement(MRef<SipDialog*> mgmt){
	managementHandler = mgmt;
}
MRef<SipStack*> SipCommandDispatcher::getSipStack(){
	return sipStack;
}

list<MRef<SipDialog *> > SipCommandDispatcher::getDialogs(){
	return dialogLayer->getDialogs();
}

void SipCommandDispatcher::run(){
#ifdef DEBUG_OUTPUT
	static int runcount = 1;
#endif

	while (keepRunning){
#ifdef DEBUG_OUTPUT
		runcount--;
#endif
		mdbg << "DIALOG CONTAINER: waiting for command"<< end;
                semaphore.dec();

#ifdef DEBUG_OUTPUT
		runcount++;
		massert(runcount==1);
#endif
		
		struct queue_type item;
                
                mlock.lock();
		if (high_prio_command_q.size()>0)
			item = high_prio_command_q.pop_back();	
		else{
			massert(low_prio_command_q.size()>0);
			item = low_prio_command_q.pop_back();	
		}
                mlock.unlock();
#ifdef DEBUG_OUTPUT
		mdbg << "DISPATCHER: got command!"<<end;
		if (item.type==TYPE_COMMAND)
			mdbg << "command: "<< **item.command << end;
		else
			mdbg << "timeout: "<< **item.command << end;
#endif


		bool handled=false;
		
		// There are two types of "commands" - SipSMCommands and
		// timeouts. SipSMCommands are passed to the dispatcher
		// (and the defaultHandler if they are not handled).
		// Timeouts have a known receiver set in the queue item.
#ifdef DEBUG_OUTPUT
		if (item.type==TYPE_COMMAND){
			mdbg << "SipDialogContainer::run delivering command :: "<< **item.command << end;
		}else{
			mdbg << "SipDialogContainer::run delivering timeout :: "<< **item.command << end;
		}
#endif
		
		if (item.type == TYPE_COMMAND){
		
			handled=handleCommand(**(item.command));
			
		}else{  // item.type == TYPE_TIMEOUT
			if ( !item.transaction_receiver.isNull() ){
				handled = item.transaction_receiver->handleCommand(**item.command);
			}

			if ( !item.call_receiver.isNull() ){
				handled = item.call_receiver->handleCommand(**item.command);
			}
		}
		

#ifdef DEBUG_OUTPUT			
		if (handled){
			//mdbg<<"DISPATCHER: command handled"<<endl;
		}else{
			mdbg<<"DISPATCHER: command NOT handled"<<endl;
		}
#endif

		item.command=NULL;
                item.transaction_receiver=NULL;
                item.call_receiver=NULL;
	}
}

void SipCommandDispatcher::setCallback(MRef<CommandReceiver*> callback){
        this->callback = callback;
}


MRef<SipLayerTransport*> SipCommandDispatcher::getLayerTransport(){
	return transportLayer;
}

MRef<SipLayerTransaction*> SipCommandDispatcher::getLayerTransaction(){
	return transactionLayer;
}

MRef<SipLayerDialog*> SipCommandDispatcher::getLayerDialog(){
	return dialogLayer;
}

void SipCommandDispatcher::addDialog(MRef<SipDialog*> d){
	dialogLayer->addDialog(d);
}

void SipCommandDispatcher::enqueueCommand(const SipSMCommand &command, int queue){
#ifdef DEBUG_OUTPUT
	mdbg<<"Dispatcher: enqueue("<<command<<")"<<endl;
#endif
	struct queue_type item;
	item.type = TYPE_COMMAND;
	item.command = MRef<SipSMCommand*>(new SipSMCommand(command));

	mlock.lock();
	if (queue == HIGH_PRIO_QUEUE){
		high_prio_command_q.push_front(item);
	}else{
		low_prio_command_q.push_front(item);
	}
	mlock.unlock();
	semaphore.inc();
}


void SipCommandDispatcher::enqueueTimeout(MRef<SipTransaction*> receiver, const SipSMCommand &command){
#ifdef DEBUG_OUTPUT
	mdbg<<"Dispatcher: enqueue("<<command<<")"<<endl;
#endif

        struct queue_type item;
        item.type = TYPE_TIMEOUT;
        item.command = MRef<SipSMCommand*>( new SipSMCommand(command));
        item.transaction_receiver = receiver;
        item.call_receiver = NULL;

        mlock.lock();
        high_prio_command_q.push_front(item);
        mlock.unlock();

        semaphore.inc();
}

void SipCommandDispatcher::enqueueTimeout(MRef<SipDialog*> receiver, const SipSMCommand &command){
#ifdef DEBUG_OUTPUT
	mdbg<<"Dispatcher: enqueue("<<command<<")"<<endl;
#endif

        struct queue_type item;
        item.type = TYPE_TIMEOUT;
        item.command = MRef<SipSMCommand*>( new SipSMCommand(command));

        item.call_receiver = receiver;

        mlock.lock();
        high_prio_command_q.push_front(item);
        mlock.unlock();

        semaphore.inc();
}


//TODO: Optimize how transactions are found based on branch parameter.
bool SipCommandDispatcher::handleCommand(const SipSMCommand &c){

#ifdef DEBUG_OUTPUT
	mdbg << "DISPATCHER: SipCommandDispatcher got command "<< c<<endl;
#endif
	
	int dst = c.getDestination();
	
	bool ret=false;
	if (dst==SipSMCommand::dialog_layer){
		if (c.getSource()!=SipSMCommand::dialog_layer && c.getSource()!=SipSMCommand::transaction_layer){
			mdbg<< "DISPATCHER: WARNING: Dialog layer is expected to receive commands only from dialog or trasaction"<<endl;
		}
		ret=dialogLayer->handleCommand(c);
	}else
	if (dst==SipSMCommand::transaction_layer){
		ret=transactionLayer->handleCommand(c);
	}else
	if (dst==SipSMCommand::transport_layer){
		if (c.getSource()!=SipSMCommand::transaction_layer){
			mdbg<< "DISPATCHER: WARNING: Transport layer is expected to receive commands only from trasaction"<<endl;
		}
		ret=transportLayer->handleCommand(c);
	}else
	if (dst==SipSMCommand::dispatcher){
	
		ret = maintainenceHandleCommand(c);
	}else{
		cerr << "ERROR: SipCommandDispatcher::handleCommand: Unknown destination (layer)"<<endl;
	}
	
	if (!ret){
#ifdef DEBUG_OUTPUT
		mdbg <<"WARNING SipCommandDispatcher: The destination layer did not handle the command!"<<endl;
#endif
	}

	return ret;
		
}

bool SipCommandDispatcher::maintainenceHandleCommand(const SipSMCommand &c){

	if (c.getType()==SipSMCommand::COMMAND_STRING){	

		if (c.getCommandString().getOp()==SipCommandString::transaction_terminated){
			transactionLayer->removeTerminatedTransactions();

			// All transactions will be tried every time a transaction terminates
			// This can be improved, but is perfectly ok for an
			// UA. An application with a large number of
			// dialogs might want to optimize.
			list<MRef<SipDialog*> > dlgs = getDialogs();
			list<MRef<SipDialog*> >::iterator i;
			for ( i=dlgs.begin(); i!=dlgs.end(); i++){
				if ( (*i)->getCurrentStateName()=="termwait" ){
					(*i)->signalIfNoTransactions();
				}
			}

			return true;

		}else if (c.getCommandString().getOp()==SipCommandString::call_terminated){
			dialogLayer->removeTerminatedDialogs();
			return true;
			
		}else if ( 	c.getCommandString().getOp() == SipCommandString::sip_stack_shutdown ||
				c.getCommandString().getOp() == SipCommandString::register_all_identities ||
				c.getCommandString().getOp() == SipCommandString::register_all_identities_done ||
				c.getCommandString().getOp() == SipCommandString::unregister_all_identities ||
				c.getCommandString().getOp() == SipCommandString::unregister_all_identities_done ||
				c.getCommandString().getOp() == SipCommandString::terminate_all_calls ||
				c.getCommandString().getOp() == SipCommandString::terminate_all_calls_done ||
				c.getCommandString().getOp() == SipCommandString::call_terminated_early ||
				c.getCommandString().getOp() == SipCommandString::register_ok) { 
			//commands that are only interesting to the management dialog ...
			//Refurbish the command ... or the SipDialog::handleCmd won't let it through
			SipSMCommand cmd( c.getCommandString(),
					SipSMCommand::dispatcher,
					SipSMCommand::dialog_layer);	//It's a SipDialog sub-class
			managementHandler->handleCommand(cmd);
			return true;
			
		}else if ( c.getCommandString().getOp() == SipCommandString::sip_stack_shutdown_done) { 
			
//			SipSMCommand cmd( c.getCommandString(),
//					SipSMCommand::dispatcher,
//					SipSMCommand::dispatcher);
// 			managementHandler->handleCommand(cmd); //process the command, so it moves to terminated state
			managementHandler->getSipStack()->getDispatcher()->stopRunning();
			return true;
		}else{
#ifdef DEBUG_OUTPUT
			mdbg << "SipCommandDispatcher: Error: maintainenceHandleCommand did not understand command: "<< c << end;
#endif
			return false;
		}
	}

	return false;
}

