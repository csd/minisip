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

#include<libmsip/SipDialog.h>
#include<libmsip/SipMessageDispatcher.h>
#include<libmsip/SipTransaction.h>
#include<libmsip/SipCommandString.h>


void SipMessageDispatcher::addTransaction(MRef<SipTransaction*> t){
	transactions.push_front(t);
}

void SipMessageDispatcher::removeTransaction(MRef<SipTransaction*> t){

	for (int i=0; i< transactions.size(); i++){
		if (transactions[i]==t){
			transactions.remove(i);
			i=0;
			return;
		}
	}
	merr << "WARNING: BUG? Cound not remove transaction from SipMessageDispatcher!!!!!"<< end;
}


void SipMessageDispatcher::addDialog(MRef<SipDialog*> d){
#ifdef MINISIP_MEMDEBUG 
	d.setUser("SipMessageDispatcher");
#endif
	dialogListLock.lock();
	dialogs.push_front(d);
	dialogListLock.unlock();
}


//TODO: Optimize how transactions are found based on branch parameter.
bool SipMessageDispatcher::handleCommand(const SipSMCommand &c){

	// 0. Find transaction based on branch parameter
	// 
	
	
	string branch;
	if (c.getType()==SipSMCommand::COMMAND_PACKET){
		//branch = c.getCommandPacket()->getLastViaBranch();
		branch = c.getCommandPacket()->getDestinationBranch();
	}

#ifdef DEBUG_OUTPUT
	mdbg<< "Dispatcher got command: "<< c<< end;
#endif

	dialogListLock.lock();
	if ((c.getDestination()==SipSMCommand::DIALOGCONTAINER)
			&& c.getType()==SipSMCommand::COMMAND_STRING){	
		if (c.getCommandString().getOp()==SipCommandString::call_terminated){
			for (int i=0; i< dialogs.size(); i++){
				if (dialogs[i]->getCurrentStateName()=="terminated"){
					dialogs.remove(i);
					i=0;
				}
			}
			dialogListLock.unlock();
			return true;
		}else{
#ifdef DEBUG_OUTPUT
			merr << "SipMessageDispatcher: Error: did not understand command: "<< c << end;
#endif
			dialogListLock.unlock();
			return false;
		}
	}

	if ((c.getDestination()==SipSMCommand::ANY || c.getDestination()==SipSMCommand::transaction )
			&& c.getType()==SipSMCommand::COMMAND_PACKET){	

#ifdef DEBUG_OUTPUT
		mdbg<< "Dispatcher(0): Trying to find transaction"<<end;
#endif

		bool hasBranch = (branch!="");
		if (!hasBranch){
			mdbg <<  "WARNING: SipMessageDispatcher::handleCommand could not find branch parameter from packet - trying all transactions"<<end;
		}
		
		for (int i=0; i< transactions.size(); i++){
			if ( !hasBranch || transactions[i]->getBranch()== branch ){
				bool ret = transactions[i]->handleCommand(c);
#ifdef DEBUG_OUTPUT
				if (!ret && hasBranch)
					merr << "SipMessageDispatcher: transaction did not handle message with matching branch id"<<end;
#endif
				if (ret){
					dialogListLock.unlock();
					return ret;
				}
			}
		}

	}else{
#ifdef DEBUG_OUTPUT
		mdbg<< "Dispatcher(0): NOT trying to find transaction (destination is other than transaction)"<<end;
#endif
	}

/*
	// 1. If no branch parameter was set
	if (c.getDestination()==SipSMCommand::ANY 
			|| c.getDestination()==SipSMCommand::transaction 
			&& !(c.getType()==SipSMCommand::COMMAND_PACKET && branch!="")){
#ifdef DEBUG_OUTPUT
		mdbg<< "Dispatcher(1): trying all transactions (WARNING - SUB OPTIMAL INTERNAL MESSAGE ROUTING)"<<end;
#endif
		for (int i=0; i< transactions.size(); i++){
		//for (list<MRef<SipTransaction*> >::iterator i=transactions.begin(); i!=transactions.end(); i++){
			mdbg << "Trying transaction: "<< transactions[i]->getMemObjectType()<< end;
			if ( transactions[i]->handleCommand(c) ){
				dialogListLock.unlock();
				return true;
			}
		}
	}else{
#ifdef DEBUG_OUTPUT
		mdbg<< "Dispatcher(1): NOT trying all transactions"<<end;
#endif
	}
*/	
	// 2. If not any branch parameter or the transaction was not found, try with each dialog
	int j=0;
	if (c.getDestination()==SipSMCommand::ANY || c.getDestination()==SipSMCommand::TU){
#ifdef DEBUG_OUTPUT
		mdbg<< "Dispatcher(2): trying all dialogs"<<end;
#endif
/*		for (list<MRef<SipDialog*> >::iterator i=dialogs.begin(); i!=dialogs.end(); i++){
#ifdef DEBUG_OUTPUT
			mdbg << "SipMessageDispatcher: trying dialog with index "<< j++ << end;
#endif
			if ( (*i)->handleCommand(c) )
				return true;
		}*/
		
		for (int i=0; i<dialogs.size(); i++){
#ifdef DEBUG_OUTPUT
			mdbg << "SipMessageDispatcher: trying dialog with index "<< j++ << end;
#endif
			if ( dialogs[i]->handleCommand(c) ){
				dialogListLock.unlock();
				return true;
			}
		}


	}else{
#ifdef DEBUG_OUTPUT
		mdbg<< "Dispatcher(2): NOT trying all dialogs (destination other than TU)"<<end;
#endif
	}
	
	//No transaction or dialog handled the command. (SipDialogContainer
	//is responsible for sending the command to the default handler)
	mdbg<< "Dispatcher: not handled by transaction or dialog"<<end;
	dialogListLock.unlock();
	return false;

}

