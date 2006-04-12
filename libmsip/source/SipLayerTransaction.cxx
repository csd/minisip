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

#include<libmsip/SipLayerTransaction.h>
#include<libmsip/SipTransaction.h>
#include<libmsip/SipCommandString.h>
#include<libmsip/SipCommandDispatcher.h>
//#include<libmsip/SipTransactionInviteServer.h>
//#include<libmsip/SipTransactionInviteServerUA.h>
//#include<libmsip/SipTransactionNonInviteServer.h>

SipLayerTransaction::SipLayerTransaction(
		MRef<SipCommandDispatcher*> d,
		MRef<SipLayerTransport*> transp):
			dispatcher(d),
			transportLayer(transp)
{
	
}

bool SipLayerTransaction::defaultCommandHandler(const SipSMCommand &cmd){
	
	MRef<SipTransaction*> newTransaction;

	if (cmd.getType()==SipSMCommand::COMMAND_PACKET 
		&& cmd.getCommandPacket()->getType()!=SipResponse::type){ //If SIP request
		MRef<SipRequest*> req = dynamic_cast<SipRequest*>(*cmd.getCommandPacket());
		massert(req);

		newTransaction = SipTransaction::create(dispatcher->getSipStack(), 
				req, 
				cmd.getSource()==SipSMCommand::dialog_layer, 
				handleAck);
	}

	if (newTransaction){
		addTransaction(newTransaction);
//		cerr << "EEEE: TransactionLayer::defaultCommandHandler: forwarding cmd to transaction"<<endl;
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

MRef<SipTransaction*> SipLayerTransaction::findTransaction(std::string branch){
	for (int i=0; i<transactions.size();i++)
		if (transactions[i]->getBranch()==branch)
			return transactions[i];
	mdbg << "Warning: SipLayerTransaction::findTransaction: could not find transaction "<< branch<<endl;
	MRef<SipTransaction*> null;
	return null;
}

void SipLayerTransaction::addTransaction(MRef<SipTransaction*> t){
	//cerr << "EE: SipLayerTransaction::addTransaction: new transaction with branch id "<< t->getBranch()<<" added"<<endl;
	transactions.push_front(t);
}

void SipLayerTransaction::removeTransaction(MRef<SipTransaction*> t){

	for (int i=0; i< transactions.size(); i++){
		if (transactions[i]==t){
			transactions.remove(i);
			return;
		}
	}
	mdbg << "WARNING: BUG? Cound not remove transaction from SipLayerTransaction!!!!!"<< end;
}

void SipLayerTransaction::removeTerminatedTransactions(){
	MRef<SipTransaction*> trans;
	for (int i=0; i<transactions.size(); i++){
		if (transactions[i]->getCurrentStateName()=="terminated"){
			trans = transactions[i];
			transactions.remove(i);
			i--;
			trans->freeStateMachine(); //let's break the cyclic references ...
		}
	}
}

list<MRef<SipTransaction*> > SipLayerTransaction::getTransactionsWithCallId(string callid){
	list<MRef<SipTransaction*> > ret;
	for (int i=0; i< transactions.size(); i++){
		if (transactions[i]->getCallId()==callid){
			ret.push_back(transactions[i]);
		}
	}
	return ret;
}


//TODO: Optimize how transactions are found based on branch parameter.
bool SipLayerTransaction::handleCommand(const SipSMCommand &c){
	assert(c.getDestination()==SipSMCommand::transaction_layer);

#ifdef DEBUG_OUTPUT	
	mdbg << "SipLayerTransaction: handleCommand got: "<< c<<endl;
#endif
	
	// Find transaction based on branch parameter
	// 
	string branch;
	string seqMethod;
	if (c.getType()==SipSMCommand::COMMAND_PACKET){
		branch = c.getCommandPacket()->getDestinationBranch();
		seqMethod = c.getCommandPacket()->getCSeqMethod();
	}
	bool hasBranch = (branch!="");
	bool hasSeqMethod = (seqMethod!="");

	if (!hasBranch){
		mdbg <<  "WARNING: SipLayerTransaction::handleCommand could not find branch parameter from packet - trying all transactions"<<end;
	}

//	cerr << "SipLayerTransaction: trying "<<transactions.size()<<" transactions"<<endl;
	for (int i=0; i< transactions.size(); i++){
		if ( (!hasBranch || transactions[i]->getBranch()== branch || seqMethod=="ACK") &&
				(!hasSeqMethod || transactions[i]->getCSeqMethod()==seqMethod || 
				 (seqMethod == "ACK" && transactions[i]->getCSeqMethod() == "INVITE")) ){

//			cerr << "SipLayerTransaction: trying message with branch <"<<branch<<"> with transaction with branch <"<<transactions[i]->getBranch()<<">"<<endl;
			bool ret = transactions[i]->handleCommand(c);
//			cerr << "SipLayerTransaction: transaction returned "<<ret<<endl;
#ifdef DEBUG_OUTPUT
			if (!ret && hasBranch){
				mdbg << "WARNING: SipLayerTransaction: transaction did not handle message with matching branch id"<<end;
			}
#endif
			if (ret){
				return true;
			}
		}
	}

	//No transaction handled the command. It could be a message that
	//will trigger the creation of a transaction or it's a bad message..
	//The defaultCommandHandler creates new trasactions if needed.
	return defaultCommandHandler(c);
}



