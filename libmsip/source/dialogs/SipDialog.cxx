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
 *	    Cesc Santasusana, c e s c dot s a n t a A{T g m a i l dot co m; 2005
*/


#include<config.h>

#include<libmsip/SipHeaderContact.h>
#include<libmsip/SipHeaderFrom.h>
#include<libmsip/SipHeaderTo.h>
#include<libmsip/SipTransaction.h>
#include<libmsip/SipStack.h>
#include<libmsip/SipDialogConfig.h>
#include<libmsip/SipDialog.h>
#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipMessageTransport.h>
#include<libmutil/dbg.h>
#include<libmsip/SipSMCommand.h>
#include<libmsip/SipCommandString.h>
#include<libmutil/CommandString.h>


SipDialog::SipDialog(MRef<SipStack*> stack, MRef<SipDialogConfig*> callconf):
                StateMachine<SipSMCommand,string>(stack->getTimeoutProvider()), 
                sipStack(stack), 
                callConfig(callconf)
{
	dialogState.seqNo=100 * (rand()%9+1);
	dialogState.remoteSeqNo=-1;
	dialogState.secure=false;	//TODO: this variable is not maintained 
	 				//properly, right?! (this at least does not leave it uninitialized) -EE 
					
	dialogState.isEarly=false;	//same as for "secure"?! -EE
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

	sipStack->getDialogContainer()->enqueueTimeout(
			MRef<SipDialog*>(this),
			cmd
			);
}

MRef<SipDialogContainer*> SipDialog::getDialogContainer(){
	return sipStack->getDialogContainer();
}

void SipDialog::registerTransaction(MRef<SipTransaction*> trans){
	sipStack->getDialogContainer()->getDispatcher()->addTransaction(trans);
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
	
	//cerr<<"SD: "+command.getCommandString().getString()<<endl;
	if (command.getType()==SipSMCommand::COMMAND_STRING 
			&& command.getCommandString().getOp()==SipCommandString::transaction_terminated){
		
		bool handled =false;
		MRef<SipTransaction*> trans;
		bool done;
		do{
			done=true;
			for (list<MRef<SipTransaction*> >::iterator i=transactions.begin(); i!=transactions.end(); i++){
				if ((*i)->getCurrentStateName()=="terminated"){
					trans = (*i);
					sipStack->getDialogContainer()->getDispatcher()->removeTransaction(*i);
					transactions.erase(i);
					//i=transactions.begin();
					trans->freeStateMachine(); //let's break the cyclic references ...
					//merr << "CESC: SipDlg::hdlCmd : freeing sip transaction state machine ... breaking the vicious circle!" << end;
					handled=true;
					done=false;
					break;
				}
			}
		}while(!done);

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
	
	bool ret;
	ret=StateMachine<SipSMCommand,string>::handleCommand(command);
	
	mdbg << "SipDialog::handleCommand returning "<< ret << end;
	

	return ret;
}


MRef<SipStack*> SipDialog::getSipStack(){
	return sipStack;
}

/*
Establish a dialog acting as a UAS (receive a request)
- routeSet = Record-Route header list, direct order
- remote target = contact header from request
- remote uri = From uri
- local uri = To uri
- remote tag = From tag
- local tag = To tag
- remote seq = CSEQ from request
- local seq = empty
- call-id = call-id value from request
*/
bool SipDialogState::updateState( MRef<SipRequest*> inv ) {
	//merr << "SipDialogState: updating state ... " << end;
	if( routeSet.size() == 0 ) {
		//merr << "dialog state has NO routeset" << end;
		/*SipMessage::getRouteSet returns the top-to-bottom ordered 
		Record-Route headers. 
		As a server, we can use this directly as route set (no reversing).
		*/
		routeSet = inv->getRouteSet();
		for( list<string>::iterator iter = routeSet.begin(); 
					iter!=routeSet.end(); 
					iter++ ) {
			//merr << "SipDialogState:inv:  " << (*iter) << end;
		}
	} else {
		//merr << "CESC: dialog state has a routeset" << end;
	}

	isEarly = false;	
	MRef<SipHeaderValueContact *> c = inv->getHeaderValueContact();
	if( c ){
		remoteTarget = c->getUri().getString();
	}
	remoteUri = inv->getHeaderValueFrom()->getUri().getString();
	localUri = inv->getHeaderValueTo()->getUri().getString();
	
	remoteTag = inv->getHeaderValueFrom()->getParameter("tag");
	//The local tag needs to be computed locally ... for voip dialog,
	// done in the constructor
	
	remoteSeqNo = inv->getCSeq();
	//the callid already has a value, given at the constructor of 
	//the dialog ... we repeat, just in case
	callId = inv->getCallId(); 
	secure = false; //FIXME: check if secure call ...	
	return true;
}


/*
Establish a dialog acting as a UAC (send a request, create
state from the response)
- routeSet = Record-Route header list, inverse order
- remote target = contact header from response
- remote uri = To uri
- local uri = From uri
- remote tag = To tag
- local tag = From tag
- remote seq = empty
- local seq = CSEQ of req
- call-id = call-id value from request
*/
bool SipDialogState::updateState( MRef<SipResponse*> resp) {
	//Not all responses update the dialog state, only 
	// 101-199 (with a TO tag!) and 2xx
	if( resp->getStatusCode() < 101 
		|| resp->getStatusCode() >= 300 ) {
		return false;
	}
		
	remoteTag = resp->getHeaderValueTo()->getParameter("tag");
	
	if( resp->getStatusCode() < 200 ) {
		if( remoteTag == "" ) {
			return false;
		}
		isEarly = true;
	} else {
		isEarly = false;
	}
	
	//from here on, only 101-199 with to tag and 2xx responses
	//merr << "SipDialogState: updating state ... " << end;	
	if( routeSet.size() == 0 ) {
		//merr << "dialog state has NO routeset" << end;
		/*SipMessage::getRouteSet returns the top-to-bottom ordered 
		Record-Route headers. 
		As a client, we must reverse this directly as route set
		*/
		routeSet = resp->getRouteSet();
		routeSet.reverse();
		for( list<string>::iterator iter = routeSet.begin(); 
					iter!=routeSet.end(); 
					iter++ ) {
			//merr << "SipDialogState:resp:  " << (*iter) << end;
		}
	} else {
		//merr << "dialog state has a routeset" << end;
	}
	
	MRef<SipHeaderValueContact *> c = resp->getHeaderValueContact();
	if( c ){
		remoteTarget = c->getUri().getString();
	}
	remoteUri = resp->getHeaderValueTo()->getUri().getString();
	localUri = resp->getHeaderValueFrom()->getUri().getString();
	
	//remote tag ... updated at the top of this function ... 
	localTag = resp->getHeaderValueFrom()->getParameter("tag");
	
	seqNo = resp->getCSeq();
	//the callid already has a value, given at the constructor of 
	//the dialog ... we repeat, just in case
	callId = resp->getCallId(); 
	secure = false; //FIXME: check if secure call ... 
	return true;
}


string SipDialogState::getRemoteTarget() {
	if( remoteTarget != "" ) {
		return remoteTarget;
	} else {
		//merr << "SipDialogSTate::getRemoteTarget : remote target empty! returning remote uri .." << end;
		return remoteUri;
	}
}

