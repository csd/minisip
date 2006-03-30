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


/* Name
 * 	SipTransaction.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>


#include<libmsip/SipTransaction.h>
#include<libmsip/SipStack.h>
#include<libmsip/SipDialog.h>
#include<libmsip/SipDialogConfig.h>
#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipHeaderVia.h>
#include<libmsip/SipMessageTransport.h>
#include<libmsip/SipCommandString.h>
#include<libmutil/dbg.h>

#include<libmsip/SipTransaction.h>
#include<libmsip/SipTransactionUtils.h>
#include<libmnetutil/IP4Address.h>
#include<libmnetutil/NetworkException.h>

SipTransaction::SipTransaction(MRef<SipStack*> stack, MRef<SipDialog*> d, int cseq, const string &cSeqMethod, const string &b, string callid): 
		StateMachine<SipSMCommand, string>(d->getTimeoutProvider() ), 
		sipStack(stack),
		dialog(d), 
		socket(NULL),
		cSeqNo(cseq),
		cSeqMethod(cSeqMethod),
		branch(b)
{
	callId = callid;
	if (b==""){
		branch = "z9hG4bK" + itoa(rand());		//magic cookie plus random number
	}
	
	MRef<SipCommonConfig *> conf;
	if (dialog){
		conf = dialog->getDialogConfig()->inherited;
	}else{
		conf = sipStack->getStackConfig();
	}

	MRef<SipProxy *> sipproxy = conf->sipIdentity->getSipProxy();
	port = sipproxy->sipProxyPort;
	transport = sipproxy->getTransport();
}

SipTransaction::~SipTransaction(){
}



bool SipTransaction::a1000_cancel_transaction(const SipSMCommand &command){
	if (transitionMatch(command, "cancel_transaction") && getCurrentStateName()!="terminated"){
		//Notify the TU that the transaction is terminated
		SipSMCommand cmdterminated(
			CommandString( callId, SipCommandString::transaction_terminated),
			SipSMCommand::transaction,
			SipSMCommand::TU);
		dialog->getDialogContainer()->enqueueCommand( cmdterminated, HIGH_PRIO_QUEUE, PRIO_FIRST_IN_QUEUE);
	
		return true;
	}else{
		return false;
	}
}

string SipTransaction::getBranch(){
	return branch;
}

void SipTransaction::setBranch(string b) {
	branch = b;
}

MRef<SipCommonConfig *> SipTransaction::getConfig(){
	MRef<SipCommonConfig *> conf;
	if (dialog){
		conf = dialog->getDialogConfig()->inherited;
	}else{
		conf = sipStack->getStackConfig();
	}
	return conf;
}

void SipTransaction::handleTimeout(const string &c){
        SipSMCommand cmd(CommandString(callId,c),SipSMCommand::transaction,SipSMCommand::transaction);
        dialog->getDialogContainer()->enqueueTimeout( this, cmd);
}


void SipTransaction::send(MRef<SipMessage*> pack, bool addVia, string br){
		if (br=="")
			br = branch;

		if( pack->getType() == SipResponse::type )
			pack->setSocket( getSocket() );

		dialog->getSipStack()->getSipTransportLayer()->sendMessage(pack, br, addVia);

		if( pack->getType() != SipResponse::type && pack->getSocket() )
			setSocket( *pack->getSocket() );

#ifdef DEBUG_OUTPUT
		mdbg<< "SipTransaction::send: WARNING: Ignoring created socket"<<end;
#endif
		
		return;
}

//FIXME: set the reliability ...
bool SipTransaction::isUnreliable() { 
	if( !socket ) {
		mdbg << "FIXME: SipTransaction::isUnrealiable: socket not initialized. Returning _unreliable_transport_ by default" << end;
		return true;
	}
	if( socket->getType() == SOCKET_TYPE_UDP )
		return true;
	else return false;
}


bool SipTransaction::handleCommand(const SipSMCommand &command){
        if (! (command.getDestination()==SipSMCommand::transaction 
				|| command.getDestination()==SipSMCommand::ANY)){
                return false;
	}

        if (command.getType()==SipSMCommand::COMMAND_PACKET 
				&& command.getCommandPacket()->getCSeq()!= getCSeqNo() 
				&& getCSeqNo()!=-1){
                return false;
        }

	if (command.getType()==SipSMCommand::COMMAND_PACKET &&
			command.getCommandPacket()->getCallId()!= callId){
		return false;
	}

	return StateMachine<SipSMCommand,string>::handleCommand(command);
}


SipTransactionClient::SipTransactionClient(MRef<SipStack*> stack, MRef<SipDialog*> d, int seq_no, const string &cSeqMethod, const string &branch, string callid):
		SipTransaction(stack, d,seq_no,cSeqMethod,branch,callid)
{
	
}

SipTransactionClient::~SipTransactionClient(){

}

SipTransactionServer::SipTransactionServer(MRef<SipStack*> stack, MRef<SipDialog*> d, int seq_no, const string &cSeqMethod, const string &branch, string callid):
		SipTransaction(stack,d,seq_no,cSeqMethod,branch,callid)
{
	
}

SipTransactionServer::~SipTransactionServer(){

}
