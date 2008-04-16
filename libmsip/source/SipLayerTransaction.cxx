/*
  Copyright (C) 2006 Erik Eliasson
  
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

#include"SipLayerTransaction.h"
#include"transactions/SipTransaction.h"
#include<libmsip/SipCommandString.h>
#include"SipCommandDispatcher.h"

using namespace std;

SipLayerTransaction::SipLayerTransaction(
		MRef<SipCommandDispatcher*> d,
		MRef<SipLayerTransport*> transp):
			handleAck(true),
			dispatcher(d),
			transportLayer(transp)
{
	
}

SipLayerTransaction::~SipLayerTransaction(){
	map<string, MRef<SipTransaction*> >::iterator i;
	for (i=transactions.begin(); i!=transactions.end(); i++)
		(*i).second->freeStateMachine();
}

string SipLayerTransaction::createClientTransaction( MRef<SipRequest*> req ){

	MRef<SipTransaction*> newTransaction;

	newTransaction = SipTransaction::create(dispatcher->getSipStackInternal(), 
			req, 
			true, 
			handleAck);

	if (newTransaction){
		addTransaction(newTransaction);
		bool handled = newTransaction->handleCommand( SipSMCommand(*req, SipSMCommand::dialog_layer, SipSMCommand::transaction_layer));
		if (!handled){
			cerr <<"ERROR: TransactionLayer::defaultCommandHandler: Transaction refused command: "<<req<<endl;
		}
		return newTransaction->getTransactionId();
	}
	return "";
}

bool SipLayerTransaction::defaultCommandHandler(const SipSMCommand &cmd){
	
	MRef<SipTransaction*> newTransaction;

	if (cmd.getType()==SipSMCommand::COMMAND_PACKET 
		&& cmd.getCommandPacket()->getType()!=SipResponse::type){ //If SIP request
		MRef<SipRequest*> req = dynamic_cast<SipRequest*>(*cmd.getCommandPacket());
		massert(req);

		if( !handleAck && req->getMethod() == "ACK" ){
			return false;
		}

		newTransaction = SipTransaction::create(dispatcher->getSipStackInternal(), 
				req, 
				cmd.getSource()==SipSMCommand::dialog_layer, 
				handleAck);
	}

	if (newTransaction){
		addTransaction(newTransaction);
		bool handled = newTransaction->handleCommand(cmd);
		if (!handled){
			cerr <<"ERROR: TransactionLayer::defaultCommandHandler: Transaction refused command: "<<cmd<<endl;
		}
		return handled;
	}else{
		cerr <<"ERROR: TransactionLayer::defaultCommandHandler: Could not handle: "<<cmd<<endl;
		return false;
	}
}

void SipLayerTransaction::doHandleAck(bool b){
	handleAck=b;
}

MRef<SipTransaction*> SipLayerTransaction::getTransaction(string tid){
	map<string, MRef<SipTransaction*> >::iterator i = transactions.find(tid);
	if (i==transactions.end()){
		MRef<SipTransaction*> null;
		return null;
	}else{
		return (*i).second;
	}
}

void SipLayerTransaction::addTransaction(MRef<SipTransaction*> t){
	massert(t->getBranch().size()>0);
	transactions[t->getTransactionId()]=t;
}

void SipLayerTransaction::removeTransaction(string tid){
	transactions[tid]->freeStateMachine();
	int n = (int)transactions.erase(tid);
	massert(n==1);
}

list<MRef<SipTransaction*> > SipLayerTransaction::getTransactions(){
	list<MRef<SipTransaction*> > ret;
	map<string, MRef<SipTransaction*> >::iterator i;
	for (i=transactions.begin(); i!=transactions.end(); i++){
		ret.push_back((*i).second);
	}
	return ret;
}




list<MRef<SipTransaction*> > SipLayerTransaction::getTransactionsWithCallId(string callid){
	list<MRef<SipTransaction*> > ret;
	map<string, MRef<SipTransaction*> >::iterator i;
	for (i=transactions.begin(); i!=transactions.end(); i++){
		if ((*i).second->getCallId() == callid)
			ret.push_back( (*i).second );
	}
	return ret;
}


//TODO: Optimize how transactions are found based on branch parameter.
bool SipLayerTransaction::handleCommand(const SipSMCommand &c){
	assert(c.getDestination()==SipSMCommand::transaction_layer);

#ifdef DEBUG_OUTPUT	
	mdbg("signaling/sip") << "SipLayerTransaction: handleCommand got: "<< c<<endl;
#endif
	string tid;
	if (c.getType()==SipSMCommand::COMMAND_STRING){
		tid = c.getCommandString().getDestinationId();
	}

	if (c.getType()==SipSMCommand::COMMAND_PACKET){
		string branch = c.getCommandPacket()->getBranch();
		if (branch.size()>0)
			tid = branch + c.getCommandPacket()->getCSeqMethod();
	}


	
	MRef<SipTransaction*> t;
	if (tid.size()>0)
		t = getTransaction(tid);

	if (t){ // This should be the normal way to handle a command
		bool ret = t->handleCommand(c);
		if (ret)
			return true;
	}else{
		// Fall back to try all transactions...
		//
		string branch;
		string seqMethod;
		if (c.getType()==SipSMCommand::COMMAND_PACKET){
			branch = c.getCommandPacket()->getBranch();
			seqMethod = c.getCommandPacket()->getCSeqMethod();
		}
		bool hasBranch = (branch!="");
		bool hasSeqMethod = (seqMethod!="");

		if (!hasBranch){
			mdbg("signaling/sip") <<  "WARNING: SipLayerTransaction::handleCommand could not find branch parameter from packet - trying all transactions"<<endl;
		}

		map<string, MRef<SipTransaction*> >::iterator i;
		for (i=transactions.begin(); i!=transactions.end(); i++){
			if ( (!hasBranch || (*i).second->getBranch()== branch || seqMethod=="ACK") &&
					(!hasSeqMethod || (*i).second->getCSeqMethod()==seqMethod || 
					 (c.getCommandPacket()->getType()!=SipResponse::type && seqMethod == "ACK" && (*i).second->getCSeqMethod() == "INVITE")) ){
				bool ret = (*i).second->handleCommand(c);
				if (ret){
					return true;
				}
			}
		}
	}

	//No transaction handled the command. It could be a message that
	//will trigger the creation of a transaction or it's a bad message..
	//The defaultCommandHandler creates new trasactions if needed.
	return defaultCommandHandler(c);
}



