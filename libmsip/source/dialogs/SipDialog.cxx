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
#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipMessageTransport.h>
#include<libmsip/SipInvite.h>
#include<libmutil/dbg.h>
#include<libmsip/SipSMCommand.h>
#include<libmsip/SipCommandString.h>
#include<libmutil/CommandString.h>


SipDialog::SipDialog(MRef<SipDialogContainer*> dContainer, MRef<SipDialogConfig*> callconf, MRef<TimeoutProvider<string, MRef<StateMachine<SipSMCommand,string>*> > *> tp):
                StateMachine<SipSMCommand,string>(tp), 
                dialogContainer(dContainer), 
                callConfig(callconf)
{
	dialogState.seqNo=100 * (rand()%9+1);
	dialogState.remoteSeqNo=-1;
}

SipDialog::~SipDialog(){

}

MRef<SipDialogConfig*> SipDialog::getDialogConfig(){
	return callConfig;
}

void SipDialog::handleTimeout(const string &c){
	SipSMCommand cmd( 
			CommandString(dialogState.callId, c), 
			SipSMCommand::TU, 
			SipSMCommand::TU );

	dialogContainer->enqueueTimeout(
			MRef<SipDialog*>(this),
			cmd
			);
}

MRef<SipDialogContainer*> SipDialog::getDialogContainer(){
	return dialogContainer;
}

void SipDialog::registerTransaction(MRef<SipTransaction*> trans){
	dialogContainer->getDispatcher()->addTransaction(trans);
	transactions.push_front(trans);
}

void SipDialog::signalIfNoTransactions(){
	if (transactions.size()==0){
		SipSMCommand cmd(
				CommandString(dialogState.callId, SipCommandString::no_transactions), 
				SipSMCommand::TU, 
				SipSMCommand::TU 
				);

				// Dialogs does not need to be deleted immediately (unlike
				// transactions), and can be placed in the end of the queue. 
				// It is placed in the high prio queue so that it is guaranteed 
				// to be deleted even under high load.
		getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE); 
	}
}



bool SipDialog::handleCommand(const SipSMCommand &command){

	mdbg << "SipDialog::handleCommand got command "<< command << "("<<getName()<<")"<<end;
	
	if (command.getType()==SipSMCommand::COMMAND_STRING 
			&& command.getCommandString().getOp()==SipCommandString::transaction_terminated){
		
		bool handled =false;
		for (list<MRef<SipTransaction*> >::iterator i=transactions.begin(); i!=transactions.end(); i++){
			if ((*i)->getCurrentStateName()=="terminated"){
				dialogContainer->getDispatcher()->removeTransaction(*i);
				transactions.erase(i);
				i=transactions.begin();
				handled=true;
			}
		}

		if (handled){
			signalIfNoTransactions();
			return true;
		}
	}
	
	if (! (command.getDestination()==SipSMCommand::TU||command.getDestination()==SipSMCommand::ANY) ){
		mdbg << "SipDialog::handleCommand: returning false based on command destination"<< end;
		return false;
	}

	if (command.getType()==SipSMCommand::COMMAND_PACKET && dialogState.callId!="" && dialogState.callId != command.getCommandPacket()->getCallId()){
		mdbg << "SipDialog: denying command based on destination id"<< end;
		return false;
	}

	mdbg << "SipDialog::handleCommand: sending command to state machine"<< end;
	bool ret=StateMachine<SipSMCommand,string>::handleCommand(command);
	mdbg << "SipDialog::handleCommand returning "<< ret << end;

	return ret;
}



