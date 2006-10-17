/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  Copyright (C) 2006 Mikael Magnusson
  
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
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/


#include<config.h>

#include<libmsip/SipHeaderContact.h>
#include<libmsip/SipHeaderFrom.h>
#include<libmsip/SipHeaderTo.h>
#include<libmsip/SipTransaction.h>
#include<libmsip/SipStack.h>
#include<libmsip/SipStackInternal.h>
#include<libmsip/SipDialogConfig.h>
#include<libmsip/SipDialog.h>
#include<libmsip/SipAuthenticationDigest.h>
#include<libmutil/dbg.h>
#include<libmsip/SipSMCommand.h>
#include<libmsip/SipCommandString.h>
#include<libmsip/SipHeaderMaxForwards.h>
#include<libmsip/SipHeaderCSeq.h>
#include<libmsip/SipHeaderCallID.h>
#include<libmsip/SipHeaderReferTo.h>
#include<libmsip/SipHeaderRoute.h>
#include<libmsip/SipHeaderRAck.h>
#include<libmsip/SipHeaderRSeq.h>
#include<libmsip/SipHeaderProxyAuthenticate.h>
#include<libmsip/SipHeaderProxyAuthorization.h>
#include<libmsip/SipHeaderWWWAuthenticate.h>
#include<libmsip/SipCommandDispatcher.h>
#include<libmutil/CommandString.h>
#include<libmutil/termmanip.h>

using namespace std;

SipDialog::SipDialog(MRef<SipStack*> stack, MRef<SipDialogConfig*> callconf):
                StateMachine<SipSMCommand,string>(stack->getTimeoutProvider()), 
                sipStack(stack), 
                callConfig(callconf)
{
	//dispatcher = stack->getDispatcher();
	dialogState.seqNo=100 * (rand()%9+1);
	dialogState.remoteSeqNo=-1;
	dialogState.secure=false;	//TODO: this variable is not maintained 
	 				//properly, right?! (this at least does not leave it uninitialized) -EE 
					
	dialogState.isEarly=false;	//same as for "secure"?! -EE
	dialogState.isEstablished = false;
	dialogState.rseqNo = (uint32_t)-1;
}

SipDialog::~SipDialog(){

}

MRef<SipDialogConfig*> SipDialog::getDialogConfig(){
	return callConfig;
}

void SipDialog::handleTimeout(const string &c){
	SipSMCommand cmd( 
			CommandString(dialogState.callId, c), 
			SipSMCommand::dialog_layer, 
			SipSMCommand::dialog_layer );

	//sipStack->sipStackInternal->getDispatcher()->enqueueTimeout( this, cmd);
	(*(MRef<SipStackInternal*> *) sipStack->sipStackInternal)->getDispatcher()->enqueueTimeout( this, cmd);
}

void SipDialog::signalIfNoTransactions(){

	if (getCurrentStateName()=="termwait"){

		list<MRef<SipTransaction*> > t = getTransactions();

		if (t.size()==0){
			SipSMCommand cmd(
					CommandString(dialogState.callId, SipCommandString::no_transactions), 
					SipSMCommand::dialog_layer, 
					SipSMCommand::dialog_layer 
					);

			// Dialogs does not need to be deleted immediately (unlike
			// transactions), and can be placed in the end of the queue. 
			// It is placed in the high prio queue so that it is guaranteed 
			// to be deleted even under high load.
			//dispatcher->enqueueCommand(cmd, HIGH_PRIO_QUEUE); 
			(*(MRef<SipStackInternal*> *) sipStack->sipStackInternal)->getDispatcher()->enqueueCommand(cmd);
		}
	}
}

void SipDialog::addRoute( MRef<SipRequest *> req ){
	if( !dialogState.isEstablished || req->getType() == "CANCEL" ){
		// Use proxy route for requests outside of the dialog
		// and for CANCEL requests
		MRef<SipProxy *> proxy = getDialogConfig()->inherited->sipIdentity->getSipProxy();

		if( !proxy.isNull() ){
			req->addRoute( proxy->sipProxyAddressString, proxy->sipProxyPort, proxy->getTransport() );
		}
	}
	else if( dialogState.routeSet.size() > 0 ) {
		//add route headers, if needed
		MRef<SipHeaderValueRoute *> rset = new SipHeaderValueRoute (dialogState.routeSet);
		req->addHeader(new SipHeader(*rset) );
	}
	else {
		//merr << "SipDialog:addRoute : dialog route set is EMPTY!!! " << end;
	}
}

list<MRef<SipTransaction*> > SipDialog::getTransactions(){
	return (*(MRef<SipStackInternal*> *) sipStack->sipStackInternal)->getDispatcher()->getLayerTransaction()->getTransactionsWithCallId(getCallId());
}

bool SipDialog::handleCommand(const SipSMCommand &command){

	mdbg << "SipDialog("<<getMemObjectType()<<")::handleCommand got command "<< command << "("<<getName()<<")"<<end;
	
	if (! (command.getDestination()==SipSMCommand::dialog_layer) ){
		mdbg << "SipDialog::handleCommand: returning false based on command destination"<< end;
		
		return false;
	}

	if (command.getType()==SipSMCommand::COMMAND_PACKET 
			&& dialogState.callId != "" 
			&& dialogState.callId != command.getCommandPacket()->getCallId()){
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

MRef<SipRequest*> SipDialog::createSipMessage( const std::string &method ){
	return createSipMessageSeq( method, dialogState.seqNo );
}

MRef<SipRequest*> SipDialog::createSipMessageSeq( const std::string &method, int seqNo ){
	MRef<SipRequest*> req = new SipRequest("", method);
	
	req->setUri( dialogState.getRemoteTarget() );
	
	req->addHeader(new SipHeader(new SipHeaderValueMaxForwards(70)));
	
	SipUri fromUri( dialogState.localUri );
	MRef<SipHeaderValueFrom*> from = new SipHeaderValueFrom( fromUri );
	from->setParameter( "tag", dialogState.localTag );
	req->addHeader(new SipHeader( *from ));

	SipUri toUri( dialogState.remoteUri );
	MRef<SipHeaderValueTo*> to = new SipHeaderValueTo( toUri );
	to->setParameter( "tag", dialogState.remoteTag );
	req->addHeader(new SipHeader( *to ));
	
	req->addHeader(new SipHeader(new SipHeaderValueCSeq( method, seqNo)));
	
	req->addHeader(new SipHeader(new SipHeaderValueCallID( dialogState.callId)));

	addRoute( req );

	// Add authorizations unless an ACK or CANCEL request
	// which should contain the same authorization headers as the INVITE
	if( method != "ACK" &&
	    method != "CANCEL" )
		addAuthorizations( req );

	return req;
}

MRef<SipRequest*> SipDialog::createSipMessageAck( MRef<SipRequest *> origReq )
{
	MRef<SipRequest*> ack = createSipMessageSeq( "ACK", origReq->getCSeq() );

	// Add all Authorization and Proxy-Authorization headers from
	// the original INVITE request. Refer to RFC 3261 section 22.1.
	// FIXME: deep copy?
	int noHeaders = origReq->getNoHeaders();
	for (int32_t i=0; i< noHeaders; i++){
		MRef<SipHeader *> header = origReq->getHeaderNo(i);
		int headerType = header->getType();
		switch (headerType){
			case SIP_HEADER_TYPE_AUTHORIZATION:
			case SIP_HEADER_TYPE_PROXYAUTHORIZATION:
				ack->addHeader(header);
				break;
		}
	}

	return ack;
}

MRef<SipRequest*> SipDialog::createSipMessageBye(){
	return createSipMessage("BYE");
}

MRef<SipRequest*> SipDialog::createSipMessagePrack( MRef<SipResponse*> resp ){
	MRef<SipHeaderValue *> value = resp->getHeaderValueNo( SIP_HEADER_TYPE_RSEQ, 0 );

	if( !value ){
		mdbg << "SipDialog: Missing RSeq in response" << end;
		return NULL;
	}

	MRef<SipRequest*> req = createSipMessage("PRACK");
	MRef<SipHeaderValueRSeq *> rseq = dynamic_cast<SipHeaderValueRSeq*>( *value );
	/* Add RAck header */
	req->addHeader( new SipHeader( new SipHeaderValueRAck( resp->getCSeqMethod(), rseq->getRSeq(), resp->getCSeq() )));

	return req;
}

MRef<SipRequest*> SipDialog::createSipMessageRefer( const string &referredUri ){
	MRef<SipRequest*> req = createSipMessage("REFER");

	/* Add the Refer-To: header */
	req->addHeader(new SipHeader(new SipHeaderValueReferTo(referredUri)));
	return req;
}

MRef<SipResponse*> SipDialog::createSipResponse( MRef<SipRequest*> req, int32_t status, const std::string &reason ){
	MRef<SipResponse*> resp = new SipResponse( req->getDestinationBranch(), status, reason, *req );
	// FIXME don't change the To tag if it's already present in the request.
	resp->getHeaderValueTo()->setParameter("tag",dialogState.localTag);
	return resp;
}

void SipDialog::sendSipMessage( MRef<SipMessage*> msg, int queue ){
	SipSMCommand cmd( *msg, SipSMCommand::dialog_layer, SipSMCommand::transaction_layer );
	(*(MRef<SipStackInternal*> *) sipStack->sipStackInternal)->getDispatcher()->enqueueCommand( cmd, queue );
}

bool SipDialog::updateAuthentication( MRef<SipResponse*> resp,
				      MRef<SipHeaderValueProxyAuthenticate*> auth){
	bool changed = false;

	MRef<SipAuthenticationDigest*> challenge;
	challenge = new SipAuthenticationDigest( auth );

	bool found = false;
	list<MRef<SipAuthenticationDigest*> >::iterator j;

	for ( j = dialogState.auths.begin();
	      j != dialogState.auths.end(); j++ ){
		MRef<SipAuthenticationDigest*> item = *j;
			
		if( item->getRealm() == challenge->getRealm() ){
			item->update( *auth );

			if( item->getStale() )
				changed = true;
			found = true;
		}
	}
		
	if( !found ){
		dialogState.auths.push_back( challenge );

		string username = getDialogConfig()->inherited->sipIdentity->getSipProxy()->sipProxyUsername;
		string password = getDialogConfig()->inherited->sipIdentity->getSipProxy()->sipProxyPassword;

		challenge->setCredential( username, password );
		changed = true;
	}

	return changed;
}

bool SipDialog::updateAuthentications( MRef<SipResponse*> resp ){
	bool changed = false;

	int i;
	for( i = 0;; i++ ){
		MRef<SipHeaderValueWWWAuthenticate*> auth;
		auth = resp->getHeaderValueWWWAuthenticate( i );

		if( !auth )
			break;

		changed |= updateAuthentication(resp, *auth);
	}

	for( i = 0;; i++ ){
		MRef<SipHeaderValueProxyAuthenticate*> auth;
		auth = resp->getHeaderValueProxyAuthenticate( i );

		if( !auth )
			break;

		changed |= updateAuthentication(resp, auth);
	}

	return changed;
}

void SipDialog::addAuthorizations( MRef<SipRequest*> req ){
	list<MRef<SipAuthenticationDigest*> >::iterator j;

	for ( j = dialogState.auths.begin();
	      j != dialogState.auths.end(); j++ ){
		MRef<SipAuthenticationDigest*> digest = *j;

		MRef<SipHeaderValueAuthorization*> authHeader = digest->createAuthorization( req );
		req->addHeader( new SipHeader( *authHeader ) );
	}
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
	isEstablished = true;
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
		
	const string toTag = resp->getHeaderValueTo()->getParameter("tag");
	
	if( resp->getStatusCode() < 200 ) {
		if( toTag == "" ) {
			return false;
		}
		if( isEstablished && toTag != remoteTag ){
			cerr << "SipDialogState: Multiple early dialogs unsupported" << endl;
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

	if( isEstablished && ( isEarly || toTag == remoteTag ) )
		// Update route set only for an existing dialog
		return true;
	
	MRef<SipHeaderValueContact *> c = resp->getHeaderValueContact();
	if( c ){
		remoteTarget = c->getUri().getString();
	}
	remoteUri = resp->getHeaderValueTo()->getUri().getString();
	localUri = resp->getHeaderValueFrom()->getUri().getString();
	
	remoteTag = toTag;
	localTag = resp->getHeaderValueFrom()->getParameter("tag");
	
	seqNo = resp->getCSeq();
	//the callid already has a value, given at the constructor of 
	//the dialog ... we repeat, just in case
	callId = resp->getCallId(); 
	secure = false; //FIXME: check if secure call ... 
	isEstablished = true;
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

std::string SipDialog::getDialogDebugString(){

	list <TPRequest<string,MRef<StateMachine<SipSMCommand,string>*> > > torequests = 
		sipStack->getTimeoutProvider()->getTimeoutRequests();

	cerr << (getName() + "   State: " + getCurrentStateName())<< endl;


	cerr << BOLD << "        SipDialogState: "<< PLAIN << endl;
	cerr <<         "            secure="<<dialogState.secure 
			<<"; localTag="<<dialogState.localTag
			<<"; remoteTag="<<dialogState.remoteTag 
			<<"; seqNo="<< dialogState.seqNo
			<<"; remoteSeqNo="<< dialogState.remoteSeqNo
			<<"; remoteUri="<< dialogState.remoteUri
			<<"; remoteTarget="<<dialogState.remoteTarget
			<<"; isEarly="<<dialogState.isEarly
			<< endl;
	cerr <<         "            route_set: ";
	
	list<string>::iterator i;
	for (i=dialogState.routeSet.begin(); i!= dialogState.routeSet.end(); i++){
		if (i!=dialogState.routeSet.begin())
			cerr << ",";
		cerr << *i;
	}
	cerr <<endl;
	
	cerr << BOLD << "        Identity: "<< PLAIN << endl;
	cerr <<         "            "<< getDialogConfig()->inherited->sipIdentity->getDebugString();
	cerr <<endl;
/*	
	cerr << BOLD << "        Timeouts:"<< PLAIN << endl;
	int ntimeouts=0;
	std::list<TPRequest<string,MRef<StateMachine<SipSMCommand,string>*> > >::iterator jj=torequests.begin();
	for (uint32_t j=0; j< torequests.size(); j++,jj++){
		if ( this == *((*jj).getSubscriber()) ){
			int ms= (*jj).getMsToTimeout();
			cerr << string("            timeout: ")+ (*jj).getCommand()
				+ "  Time: " + itoa(ms/1000) + "." + itoa(ms%1000) << endl;
			ntimeouts++;
		}
	}
	if (ntimeouts==0){
		cerr << "            (no timeouts)"<< endl;
	}


	cerr << BOLD << "        Transactions:"<< PLAIN << endl;
	list<MRef<SipTransaction*> > transactions = getTransactions();
	if (transactions.size()==0)
		cerr << "            (no transactions)"<< endl;
	else{
		int n=0;
		for (list<MRef<SipTransaction*> >::iterator i = transactions.begin();
				i!=transactions.end(); i++){
			cerr << string("            (")+itoa(n)+") "+
				(*i)->getName() 
				+ "   State: "
				+ (*i)->getCurrentStateName() << endl;
			n++;

			cerr << BOLD << "                Timeouts:" << PLAIN << endl;

			int ntimeouts=0;
			std::list<TPRequest<string,   MRef<StateMachine<SipSMCommand,string>*>  > >::iterator jj=torequests.begin();
			for (uint32_t j=0; j< torequests.size(); j++, jj++){
				if ( *((*i)) == *((*jj).getSubscriber()) ){
					int ms= (*jj).getMsToTimeout();
					cerr << string("                        timeout: ")
						+ (*jj).getCommand()
						+ "  Time: " + itoa(ms/1000) + "." + itoa(ms%1000)<< endl;
					ntimeouts++;
				}
			}
			if (ntimeouts==0)
				cerr << "                        (no timeouts)"<< endl;
		}
	}
*/

	return "";
}

