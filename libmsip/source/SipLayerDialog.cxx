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
#include<libmsip/SipLayerDialog.h>
#include<libmsip/SipTransaction.h>
#include<libmsip/SipDialog.h>
#include<libmsip/SipSMCommand.h>
#include<libmsip/SipCommandString.h>
#include<libmsip/SipCommandDispatcher.h>


SipLayerDialog::SipLayerDialog(MRef<SipCommandDispatcher*> d):dispatcher(d){
	
}

void SipLayerDialog::removeTerminatedDialogs(){

	for (int i=0; i< dialogs.size(); i++){
		if (dialogs[i]->getCurrentStateName()=="terminated"){
			MRef<SipDialog *> dlg = dialogs[i];
			dialogs.remove(i);
			//merr << "CESC: SipMsgDispatcher::hdleCmd : breaking the dialog vicious circle" << endl;
			dlg->freeStateMachine();
			i=0;
		}
	}
}

/*
void SipLayerDialog::removeTransaction(MRef<SipTransaction*> t){

	for (int i=0; i< transactions.size(); i++){
		if (transactions[i]==t){
			transactions.remove(i);
			i=0;
			return;
		}
	}
	mdbg << "WARNING: BUG? Cound not remove transaction from SipLayerDialog!!!!!"<< end;
}
*/

void SipLayerDialog::addDialog(MRef<SipDialog*> d){
	dialogListLock.lock();
	dialogs.push_front(d);
	dialogListLock.unlock();
}

//TODO: Optimize how transactions are found based on branch parameter.
bool SipLayerDialog::handleCommand(const SipSMCommand &c){
	assert(c.getDestination()==SipSMCommand::dialog_layer);

#ifdef DEBUG_OUTPUT
	mdbg<< "SipLayerDialog: got command: "<< c <<end;
#endif

	dialogListLock.lock();
	// 2. If not any branch parameter or the transaction was not found, try with each dialog
	//int j=0; //unused??
        MRef<SipDialog *> dialog;
	int i;
	try{
		for (i=0; i<dialogs.size(); i++){
			dialog = dialogs[i];
			dialogListLock.unlock();

			if ( dialog->handleCommand(c) ){
				//dialogListLock.unlock();
				return true;
			}
			dialogListLock.lock();
		}
	}catch(exception &e){
		cerr << "SipLayerDialog: caught exception i="<< i<<" what: "<< e.what() << endl;
	}

	dialogListLock.unlock();

	if (defaultHandler){
		//cerr << "SipLayerDialog: No dialog handled the message - sending to default handler"<<endl;
		return defaultHandler->handleCommand(c);
	}else{
		cerr << "ERROR: libmsip: SipLayerDialog::handleCommand: No default handler for dialog commands set!"<<endl;
		return false;
	}
}

