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


#include"SipTransaction.h"
#include"../SipStackInternal.h"
#include"../SipCommandDispatcher.h"
#include<libmsip/SipHeaderVia.h>
#include"../SipLayerTransport.h"
#include<libmsip/SipCommandString.h>
#include<libmutil/dbg.h>

#include"SipTransactionInviteClient.h"
#include"SipTransactionInviteServer.h"
#include"SipTransactionInviteServerUA.h"
#include"SipTransactionNonInviteServer.h"
#include"SipTransactionNonInviteClient.h"
#include<libmsip/SipTransitionUtils.h>

#include<stdlib.h>

using namespace std;

SipTransaction::SipTransaction(MRef<SipStackInternal*> stackInternal, 
		int cseq, 
		const string &cseqm, 
		const string &b, 
		const string &callid): 
			StateMachine<SipSMCommand, string>(stackInternal->getTimeoutProvider() ), 
			sipStackInternal(stackInternal),
			cSeqNo(cseq),
			cSeqMethod(cseqm),
			branch(b)
{
	dispatcher = stackInternal->getDispatcher();
	assert(dispatcher);
	transportLayer = dispatcher->getLayerTransport();
	assert(transportLayer);
	
	callId = callid;
	if (b==""){
		branch = "z9hG4bK" + itoa(rand());		//magic cookie plus random number
	}
}

SipTransaction::~SipTransaction(){
}

MRef<SipTransaction*> SipTransaction::create(MRef<SipStackInternal*> stackInternal, 
		MRef<SipRequest*> req, 
		bool fromTU, 
		bool handleAck)
{
	int seqNo = req->getCSeq();
	string seqMethod = req->getCSeqMethod();
	string callId = req->getCallId();
	string branch= req->getBranch();
	
#ifdef DEBUG_OUTPUT
	mdbg("signaling/sip") << "TRANSACTION_CREATE: "<< seqMethod<<" "<<seqNo<<" branch="<<branch<<" callid=" << callId<<" client="<<fromTU<< endl;
#endif

	if (fromTU){ //client transaction
		if (req->getType()=="INVITE"){
			return new SipTransactionInviteClient(stackInternal,seqNo,seqMethod,callId);
		}else{
			MRef<SipTransaction *> res = new SipTransactionNonInviteClient(stackInternal,seqNo,seqMethod,callId);
			if( req->getType()=="CANCEL"){
				// A CANCEL constructed by a client
				// MUST have only a single Via header
				// field value matching the top Via
				// value in the request being
				// cancelled. (RFC 3261 section 9.1)
				res->setBranch( branch );
			}
			return res;
		}
	
	}else{	//server transaction
		if (req->getType()=="INVITE"){
			if (handleAck){	//UA-version
				return new SipTransactionInviteServerUA(stackInternal,seqNo,seqMethod,branch,callId);
			}else{
				return new SipTransactionInviteServer(stackInternal,seqNo,seqMethod,branch,callId);
			}
		}else{
			return new SipTransactionNonInviteServer(stackInternal,seqNo,seqMethod,branch,callId);
		}
	}
	
}

bool SipTransaction::a1000_anyState_terminated_canceltransaction(const SipSMCommand &command){
	if (transitionMatch(command, 
				"cancel_transaction", 
				SipSMCommand::dialog_layer, 
				SipSMCommand::transaction_layer) 
			&& getCurrentStateName()!="terminated")
{
		//Notify the TU that the transaction is terminated
		SipSMCommand cmdterminated(
			CommandString( getTransactionId(), SipCommandString::transaction_terminated),
			SipSMCommand::transaction_layer,
			SipSMCommand::transaction_layer);
		
		dispatcher->enqueueCommand( cmdterminated, HIGH_PRIO_QUEUE/*, PRIO_FIRST_IN_QUEUE*/);
	
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

void SipTransaction::handleTimeout(const string &c){
        SipSMCommand cmd(
			CommandString(getTransactionId(),c),
			SipSMCommand::transaction_layer,
			SipSMCommand::transaction_layer);
        dispatcher->enqueueTimeout( this, cmd);
}


void SipTransaction::setSocket(Socket * sock){
	socket=sock;
}

MRef<Socket *> SipTransaction::getSocket(){
	return socket;
}

std::string SipTransaction::getMemObjectType() const {
	return "SipTransaction";
}

void SipTransaction::setDebugTransType(std::string t){
	debugTransType = t;
}

std::string SipTransaction::getDebugTransType(){
	return debugTransType;
}

int SipTransaction::getCSeqNo(){
	return cSeqNo;
}

std::string SipTransaction::getCSeqMethod(){
	return cSeqMethod;
}

std::string SipTransaction::getCallId(){
	return callId;
}


void SipTransaction::send(MRef<SipMessage*> pack, bool addVia, string br){
		if (br=="")
			br = branch;

		if( pack->getType() == SipResponse::type )
			pack->setSocket( getSocket() );
		transportLayer->sendMessage(pack, br, addVia);

		if( pack->getType() != SipResponse::type && pack->getSocket() )
			setSocket( *pack->getSocket() );

#ifdef DEBUG_OUTPUT
		mdbg("signaling/sip") << "SipTransaction::send: WARNING: Ignoring created socket"<<endl;
#endif
		
		return;
}

//FIXME: set the reliability ...
bool SipTransaction::isUnreliable() { 
	if( !socket ) {
		mdbg("signaling/sip") << "FIXME: SipTransaction::isUnrealiable: socket not initialized. Returning _unreliable_transport_ by default" << endl;
		return true;
	}
	if( socket->getType() == MSOCKET_TYPE_UDP )
		return true;
	else return false;
}


bool SipTransaction::handleCommand(const SipSMCommand &command){
#ifdef DEBUG_OUTPUT
	mdbg("signaling/sip") << "SipTransaction:handleCommand: tid<"<< getTransactionId()<< "> got command "<<command<<endl;
#endif
        if (! (command.getDestination()==SipSMCommand::transaction_layer
				/*|| command.getDestination()==SipSMCommand::ANY*/)){
//		cerr << "Transaction: returning false based on destination"<<endl;
                return false;
	}

        if (command.getType()==SipSMCommand::COMMAND_PACKET 
				&& command.getCommandPacket()->getCSeq()!= getCSeqNo() 
				&& getCSeqNo()!=-1){
//		cerr << "Transaction: returning false based on cseq"<<endl;
                return false;
        }

	if (command.getType()==SipSMCommand::COMMAND_PACKET &&
			command.getCommandPacket()->getCallId()!= callId){
//		cerr << "Transaction: returning false based on callid"<<endl;
		return false;
	}

	//cerr << "Transaction: returning based on state machine"<<endl;
	return StateMachine<SipSMCommand,string>::handleCommand(command);
}


SipTransactionClient::SipTransactionClient(MRef<SipStackInternal*> stackInternal, 
		int seq_no, 
		const string &cseqm, 
		const string &branch_, 
		const string &callid):
			SipTransaction(stackInternal, seq_no, cseqm, branch_, callid)
{
	
}

SipTransactionClient::~SipTransactionClient(){

}

SipTransactionServer::SipTransactionServer(MRef<SipStackInternal*> stackInternal, 
		int seq_no, 
		const string &cseqm, 
		const string &branch_,
		const string &callid):
			SipTransaction(stackInternal,seq_no,cseqm,branch_,callid)
{
	
}

SipTransactionServer::~SipTransactionServer(){

}
