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

#include"SipCommandDispatcher.h"
#include"transactions/SipTransaction.h"
#include<libmsip/SipDialog.h>
#include"SipLayerDialog.h"
#include"SipLayerTransaction.h"
#include<libmsip/SipCommandString.h>

using namespace std;

SipCommandDispatcher::SipCommandDispatcher(
		MRef<SipStackInternal*> stackInternal, 
		MRef<SipLayerTransport*> transp)
			:sipStackInternal(stackInternal),
			keepRunning(true),
			informTuOnTransactionTerminate(false)
{

	transportLayer = transp;
	transactionLayer = new SipLayerTransaction(this,transportLayer);
	dialogLayer = new SipLayerDialog(this);

	//transportLayer->setSipSMCommandReceiver(this);
	transportLayer->setDispatcher(this);

}

void SipCommandDispatcher::free(){
	sipStackInternal=NULL;
	callback=NULL;
	if (managementHandler){
		managementHandler->freeStateMachine();
		managementHandler=NULL;
	}
	transportLayer->setDispatcher(NULL);
	transportLayer=NULL;
	dialogLayer=NULL;
	transactionLayer=NULL;
}

void SipCommandDispatcher::setDialogManagement(MRef<SipDialog*> mgmt){
	managementHandler = mgmt;
}
MRef<SipStackInternal*> SipCommandDispatcher::getSipStackInternal(){
	return sipStackInternal;
}

MRef<CommandReceiver*> SipCommandDispatcher::getCallback(){
	return callback;
}

void SipCommandDispatcher::stopRunning(){
	keepRunning=false;

	transportLayer->stop();

	//The SIP stack is blocking on a semaphore waiting for
	//something to process. We wake it by sending it a
	//no-operation command. It will then check
	//the "keepRunning" flag and exit
	CommandString c("",SipCommandString::no_op);
	SipSMCommand sc(c, SipSMCommand::dispatcher, SipSMCommand::dispatcher);
	enqueueCommand(sc, LOW_PRIO_QUEUE);

}

list<MRef<SipDialog *> > SipCommandDispatcher::getDialogs(){
	return dialogLayer->getDialogs();
}

void SipCommandDispatcher::run(){

	while (keepRunning){
		mdbg("signaling/sip") << "DIALOG CONTAINER: waiting for command"<< endl;
                semaphore.dec();

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
		if (item.type==TYPE_COMMAND)
			mdbg("signaling/sip") << "DISPATCHER: got command: "<< **item.command << endl;
		else
			mdbg("signaling/sip") << "DISPATCHER: got timeout: "<< **item.command << endl;
#endif


		bool handled=false;
		
		// There are two types of "commands" - SipSMCommands and
		// timeouts. SipSMCommands are passed to the dispatcher
		// (and the defaultHandler if they are not handled).
		// Timeouts have a known receiver set in the queue item.
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
		if (!handled){
			merr << "DISPATCHER: command NOT handled:" << **(item.command) << endl;
		}
#endif

		item.command=NULL;
                item.transaction_receiver=NULL;
                item.call_receiver=NULL;
	}
}

void SipCommandDispatcher::setCallback(MRef<CommandReceiver*> cb){
        this->callback = cb;
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
	mdbg("signaling/sip") << "Dispatcher: enqueue(" << command << ")" << endl;
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
	mdbg("signaling/sip") << "Dispatcher: enqueue(" << command << ")" << endl;
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
	mdbg("signaling/sip") << "Dispatcher: enqueue(" << command << ")" << endl;
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
	mdbg("signaling/sip") << "DISPATCHER: SipCommandDispatcher got command "<< c<<endl;
#endif
	
	int dst = c.getDestination();
	
	bool ret=false;
	if (dst==SipSMCommand::dialog_layer){
		if (c.getSource()!=SipSMCommand::dialog_layer &&
				c.getSource()!=SipSMCommand::transaction_layer){
			mdbg("signaling/sip") << "DISPATCHER: WARNING: Dialog layer is expected to receive commands only from dialog or trasaction"<<endl;
		}
		ret=dialogLayer->handleCommand(c);
	}else
	if (dst==SipSMCommand::transaction_layer){
		ret=transactionLayer->handleCommand(c);
	}else
	if (dst==SipSMCommand::transport_layer){
		if (c.getSource()!=SipSMCommand::transaction_layer
				&& c.getSource()!=SipSMCommand::transaction_layer
				&& !(c.getType()==SipSMCommand::COMMAND_PACKET
					&& c.getCommandPacket()->getType()=="ACK")){
			mdbg("signaling/sip") << "DISPATCHER: WARNING: Transport layer is expected to receive commands only from trasaction"<<endl;
		}
		ret=transportLayer->handleCommand(c);
	}else
	if (dst==SipSMCommand::dispatcher){
	
		ret = maintainenceHandleCommand(c);
	}else{
		cerr << "ERROR: SipCommandDispatcher::handleCommand: Unknown destination (layer)"<<endl;
	}

	// Retransmitted 2xx responses for INVITE are handled directly by
	// the dialog.
	if(!ret && c.getSource()==SipSMCommand::transport_layer){
		SipSMCommand cmd( c );
		cmd.setDestination( SipSMCommand::dialog_layer );
		ret=dialogLayer->handleCommand( cmd );
	}
	
	if (!ret){
#ifdef DEBUG_OUTPUT
		mdbg("signaling/sip") <<"WARNING SipCommandDispatcher: The destination layer did not handle the command!"<<endl;
#endif
	}

	return ret;
		
}

bool SipCommandDispatcher::maintainenceHandleCommand(const SipSMCommand &c){

	if (c.getType()==SipSMCommand::COMMAND_STRING){	
		if (c.getCommandString().getOp()==SipCommandString::no_op){
			return true;
		}

		if (c.getCommandString().getOp()==SipCommandString::transaction_terminated){
			string tid = c.getCommandString().getDestinationId();
			MRef<SipTransaction *> t = transactionLayer->getTransaction(tid);
			string cid = t->getCallId();
			t=NULL;
			transactionLayer->removeTransaction(tid);
			MRef<SipDialog*> d = dialogLayer->getDialog(cid);

			if (informTuOnTransactionTerminate){
				SipSMCommand tterm(
						CommandString( cid, SipCommandString::transaction_terminated, tid ),
						SipSMCommand::transaction_layer,
						SipSMCommand::dialog_layer 
						);

				enqueueCommand(tterm, HIGH_PRIO_QUEUE);
			}

			//It is ok to not find a dialog (transaction
			//without dialog).
			if (d){
				d->signalIfNoTransactions();
			}
			return true;
		}else if (c.getCommandString().getOp()==SipCommandString::call_terminated){
			return dialogLayer->removeDialog(c.getDestinationId());
		}else if (  managementHandler && (c.getCommandString().getOp() == SipCommandString::sip_stack_shutdown ||
				c.getCommandString().getOp() == SipCommandString::register_all_identities ||
				c.getCommandString().getOp() == SipCommandString::register_all_identities_done ||
				c.getCommandString().getOp() == SipCommandString::unregister_all_identities ||
				c.getCommandString().getOp() == SipCommandString::unregister_all_identities_done ||
				c.getCommandString().getOp() == SipCommandString::terminate_all_calls ||
				c.getCommandString().getOp() == SipCommandString::terminate_all_calls_done ||
				c.getCommandString().getOp() == SipCommandString::call_terminated_early ||
				c.getCommandString().getOp() == SipCommandString::register_ok ) ) { 
			//commands that are only interesting to the management dialog ...
			//Refurbish the command ... or the SipDialog::handleCmd won't let it through
			if (managementHandler){
				SipSMCommand cmd( c.getCommandString(),
						SipSMCommand::dispatcher,
						SipSMCommand::dialog_layer);	//It's a SipDialog sub-class
				managementHandler->handleCommand(cmd);
			}
			return true;
			
		}else if ( managementHandler && c.getCommandString().getOp() == SipCommandString::sip_stack_shutdown_done) { 
			
//			SipSMCommand cmd( c.getCommandString(),
//					SipSMCommand::dispatcher,
//					SipSMCommand::dispatcher);
// 			managementHandler->handleCommand(cmd); //process the command, so it moves to terminated state
			managementHandler->getSipStack()->stopRunning();
			return true;
		}else{
#ifdef DEBUG_OUTPUT
			mdbg("signaling/sip") << "SipCommandDispatcher: Error: maintainenceHandleCommand did not understand command: "<< c << endl;
#endif
			return false;
		}
	}

	return false;
}

