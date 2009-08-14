/*
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2004-2006
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *	    Joachim Orrblad <joachim[at]orrblad.com>
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/
#include<config.h>
#include"SipStackInternal.h"

#include"SipLayerTransport.h"
#include"SipCommandDispatcher.h"
#include<libmnetutil/NetworkException.h>
#include<libmsip/SipMessageContentIM.h>
#include<libmsip/SipMessageContentMime.h>
#include<libmutil/Timestamp.h>

#include<libmsip/SipHeaderAllowEvents.h>
#include<libmsip/SipHeaderContact.h>
#include<libmsip/SipHeaderUnknown.h>
#include<libmsip/SipHeaderContentLength.h>
#include<libmsip/SipHeaderUserAgent.h>
#include<libmsip/SipHeaderContentType.h>         
#include<libmsip/SipHeaderVia.h>
#include<libmsip/SipHeaderCSeq.h>                
#include<libmsip/SipHeaderWarning.h>
#include<libmsip/SipHeaderEvent.h>            
#include<libmsip/SipHeaderExpires.h>             
#include<libmsip/SipHeaderFrom.h>            
#include<libmsip/SipHeader.h>               
#include<libmsip/SipHeaderMaxForwards.h>
#include<libmsip/SipHeaderProxyAuthenticate.h>
#include<libmsip/SipHeaderProxyAuthorization.h>
#include<libmsip/SipHeaderAcceptContact.h>
#include<libmsip/SipHeaderRecordRoute.h>
#include<libmsip/SipHeaderAccept.h>
#include<libmsip/SipHeaderRAck.h>
#include<libmsip/SipHeaderRSeq.h>
#include<libmsip/SipHeaderRoute.h>
#include<libmsip/SipHeaderReferTo.h>
#include<libmsip/SipHeaderAuthorization.h>
#include<libmsip/SipHeaderRequire.h>
#include<libmsip/SipHeaderSubject.h>
#include<libmsip/SipHeaderSnakeSM.h>
#include<libmsip/SipHeaderSubscriptionState.h>
#include<libmsip/SipHeaderSupported.h>
#include<libmsip/SipHeaderUnsupported.h>
#include<libmsip/SipHeaderCallID.h>
#include<libmsip/SipHeaderTo.h>
#include<libmsip/SipHeaderWWWAuthenticate.h>
#include<libmsip/SipCommandString.h>

#include<libmutil/massert.h>

#include<libmutil/dbg.h>
#include<libmcrypto/cert.h>

#include<libmsip/SipTransport.h>
#include<algorithm>

using namespace std;

SipStackInternal::SipStackInternal( MRef<SipStackConfig *> stackConfig )
{
	timers = new SipTimers;
	this->config = stackConfig;

	timeoutProvider = new TimeoutProvider<string, MRef<StateMachine<SipSMCommand,string>*> >;

	SipHeader::headerFactories.addFactory("Accept", sipHeaderAcceptFactory);
	SipHeader::headerFactories.addFactory("Allow-Events", sipHeaderAllowEventsFactory);
	SipHeader::headerFactories.addFactory("u", sipHeaderAllowEventsFactory);
	SipHeader::headerFactories.addFactory("Accept-Contact", sipHeaderAcceptContactFactory);
	SipHeader::headerFactories.addFactory("Authorization", sipHeaderAuthorizationFactory);
	SipHeader::headerFactories.addFactory("Call-ID", sipHeaderCallIdFactory);
	SipHeader::headerFactories.addFactory("i", sipHeaderCallIdFactory);
	SipHeader::headerFactories.addFactory("Contact", sipHeaderContactFactory);
	SipHeader::headerFactories.addFactory("m", sipHeaderContactFactory);
	SipHeader::headerFactories.addFactory("Content-Length", sipHeaderContentLengthFactory);
	SipHeader::headerFactories.addFactory("l", sipHeaderContentLengthFactory);
	SipHeader::headerFactories.addFactory("Content-Type", sipHeaderContentTypeFactory);
	SipHeader::headerFactories.addFactory("c", sipHeaderContentTypeFactory);
	SipHeader::headerFactories.addFactory("CSeq", sipHeaderCSeqFactory);
	SipHeader::headerFactories.addFactory("Event", sipHeaderEventFactory);
	SipHeader::headerFactories.addFactory("Expires", sipHeaderExpiresFactory);
	SipHeader::headerFactories.addFactory("From", sipHeaderFromFactory);
	SipHeader::headerFactories.addFactory("f", sipHeaderFromFactory);
	SipHeader::headerFactories.addFactory("Max-Forwards", sipHeaderMaxForwardsFactory);
	SipHeader::headerFactories.addFactory("Proxy-Authenticate", sipHeaderProxyAuthenticateFactory);
	SipHeader::headerFactories.addFactory("Proxy-Authorization", sipHeaderProxyAuthorizationFactory);
	SipHeader::headerFactories.addFactory("RAck", sipHeaderRAckFactory);
	SipHeader::headerFactories.addFactory("RSeq", sipHeaderRSeqFactory);
	SipHeader::headerFactories.addFactory("Record-Route", sipHeaderRecordRouteFactory);
	SipHeader::headerFactories.addFactory("Require", sipHeaderRequireFactory);
	SipHeader::headerFactories.addFactory("Refer-To", sipHeaderReferToFactory);
	SipHeader::headerFactories.addFactory("Route", sipHeaderRouteFactory);
	SipHeader::headerFactories.addFactory("Snake-SM", sipHeaderSnakeSMFactory);
	SipHeader::headerFactories.addFactory("Subject", sipHeaderSubjectFactory);
	SipHeader::headerFactories.addFactory("s", sipHeaderSubjectFactory);
	SipHeader::headerFactories.addFactory("Subscription-State", sipHeaderSubscriptionStateFactory);
	SipHeader::headerFactories.addFactory("Supported", sipHeaderSupportedFactory);
	SipHeader::headerFactories.addFactory("k", sipHeaderSupportedFactory);
	SipHeader::headerFactories.addFactory("To", sipHeaderToFactory);
	SipHeader::headerFactories.addFactory("t", sipHeaderToFactory);
	SipHeader::headerFactories.addFactory("Unsupported", sipHeaderUnsupportedFactory);
	SipHeader::headerFactories.addFactory("User-Agent", sipHeaderUserAgentFactory);
	SipHeader::headerFactories.addFactory("Via", sipHeaderViaFactory);
	SipHeader::headerFactories.addFactory("v", sipHeaderViaFactory);
	SipHeader::headerFactories.addFactory("Warning", sipHeaderWarningFactory);
	SipHeader::headerFactories.addFactory("WWW-Authenticate", sipHeaderWWWAuthenticateFactory);

	addSupportedExtension("100rel");
	addSupportedExtension("sdp-anat");

	MRef<SipLayerTransport*> transp = MRef<SipLayerTransport*>(new
			   SipLayerTransport(stackConfig->cert,
					     stackConfig->cert_db));

	// Here we need to really know what we are doing since
	// we are "breaking the law" of not passing this
	// as argument in the constructor.
	//
	// Here it's ok since the dispatcher will keep
	// a reference to the SipStackInternal thus we won't be
	// freed (crash) when this line executes.
	dispatcher = new SipCommandDispatcher(this,transp);

	SipMessage::contentFactories.addFactory("text/plain", sipIMMessageContentFactory);
	SipMessage::contentFactories.addFactory("multipart/mixed", SipMIMEContentFactory);
	SipMessage::contentFactories.addFactory("multipart/alternative", SipMIMEContentFactory);
	SipMessage::contentFactories.addFactory("multipart/parallel", SipMIMEContentFactory);
	SipMessage::contentFactories.addFactory("message/sipfrag", sipSipMessageContentFactory);

	// Instantiate transport registry
	SipTransportRegistry::getInstance();
}

MRef<SipCommandDispatcher*> SipStackInternal::getDispatcher(){
	return dispatcher;
}

void SipStackInternal::free(){
	timeoutProvider->stopThread();
	timeoutProvider=NULL;
	setCallback(NULL);
	setConfCallback(NULL);
	dispatcher->free();
	dispatcher=NULL;
}

MRef<SipStackConfig*> SipStackInternal::getStackConfig(){
	return config;
}

void SipStackInternal::setDefaultDialogCommandHandler(MRef<SipDefaultHandler*> cb){
	dispatcher->getLayerDialog()->setDefaultDialogCommandHandler(cb);
}

MRef<SipDefaultHandler*> SipStackInternal::getDefaultDialogCommandHandler(){
	return dispatcher->getLayerDialog()->getDefaultDialogCommandHandler();
}

void SipStackInternal::setTransactionHandlesAck(bool transHandleAck){
	dispatcher->getLayerTransaction()->doHandleAck(transHandleAck);
}


void SipStackInternal::setCallback(MRef<CommandReceiver*> cb){
	this->callback = cb;
	dispatcher->setCallback(callback);
}

MRef<CommandReceiver*> SipStackInternal::getCallback(){
	return callback;
}

void SipStackInternal::setConfCallback(MRef<CommandReceiver*> cb){
	this->confCallback = cb;
}

MRef<CommandReceiver*> SipStackInternal::getConfCallback(){
	return confCallback;
}

void SipStackInternal::run(){
#ifdef DEBUG_OUTPUT
	setThreadName("SipStack");
#endif
	dispatcher->run();
}

void SipStackInternal::stopRunning(){
	dispatcher->stopRunning();
}

bool SipStackInternal::handleCommand(const SipSMCommand &command){
	dispatcher->enqueueCommand(command, LOW_PRIO_QUEUE);
	return true;
}

bool SipStackInternal::handleCommand(const CommandString &cmd){
		//Commands from the gui etc is always sent to the
		//TU layer
	SipSMCommand c(cmd, SipSMCommand::dialog_layer, SipSMCommand::dialog_layer);
	return handleCommand(c);
}

void SipStackInternal::addDialog(MRef<SipDialog*> d){
	dispatcher->addDialog(d);
}


MRef<TimeoutProvider<string, MRef<StateMachine<SipSMCommand,string>*> > *> SipStackInternal::getTimeoutProvider(){
	return timeoutProvider;
}

MRef<SipTimers*> SipStackInternal::getTimers(){
	return timers;
}

void SipStackInternal::addSupportedExtension(string ext){
	sipExtensions.push_back(ext);
}

bool SipStackInternal::supports(string ext){
	list<string>::iterator i;
	for (i=sipExtensions.begin(); i!= sipExtensions.end(); i++){
		if (*i == ext)
			return true;
	}
	return false;
}

string SipStackInternal::getAllSupportedExtensionsStr(){
	string ret;
	bool first=true;
	list<string>::iterator i;
	for (i=sipExtensions.begin(); i!=sipExtensions.end();i++){
		if (!first){
			ret = ret+",";
		}
		else {
			first=false;
		}
		ret = ret+(*i);
	}
	return ret;
}


static std::string getTimeoutDebugString(MRef<StateMachine<SipSMCommand,std::string> *> sm, 
		list <TPRequest<string,MRef<StateMachine<SipSMCommand,string>*> > > &torequests,		
		int indent){
	string ret;
	string ind;
	for (int i=0; i<indent; i++)
		ind+="\t";

	int ntimeouts=0;
	std::list<TPRequest<string,MRef<StateMachine<SipSMCommand,string>*>  > >::iterator jj=torequests.begin();

	for (uint32_t j=0; jj!=torequests.end(); j++){
		if ( ((*sm)) == *((*jj).getSubscriber()) ){
			int ms= (*jj).getMsToTimeout();
			ret+= ind + (*jj).getCommand()
				+ " Time: " + itoa(ms/1000) + "." + itoa(ms%1000)+"\n";
			ntimeouts++;
			torequests.erase(jj);
			jj=torequests.begin();
		}else{
			jj++;
		}
	}
	if (ntimeouts==0)
		ret+= ind + "(no timeouts)"+"\n";
	return ret;
}

static std::string getTransactionDebugString(MRef<SipTransaction*> t, 
		list <TPRequest<string,MRef<StateMachine<SipSMCommand,string>*> > > &torequests, int indent){
	string ret;
	string ind;
	for (int i=0;i<indent; i++)
		ind+="\t";

	ret = ind+ (*t)->getName() + " State: " + (*t)->getCurrentStateName()+"\n";

	ret+= ind+"Timeouts:\n";
	ret+= ind+getTimeoutDebugString(*t,torequests,2);
	return ret;

}

static std::string getDialogDebugString(MRef<SipDialog*> d, 
		list<MRef<SipTransaction*> > &transactions, // a reference to the list so that transactions can be removed from it 
		list <TPRequest<string,MRef<StateMachine<SipSMCommand,string>*> > > &torequests, int indent)
{
	string ret;
	string ind;
	for (int i=0; i< indent ; i++)
		ind+="\t";

	ret+= d->getDialogDebugString(/*indent+1*/);
	
	ret+= ind + "Timeouts:\n";
	//cerr << "        Timeouts:"<< endl;
	ret+= getTimeoutDebugString(*d,torequests,indent+1);


	ret+= ind+"Transactions:\n";
	//cerr << "        Transactions:"<< endl;
	string did=d->getCallId();
	int n=0;
	bool restart;
	for (list<MRef<SipTransaction*> >::iterator t = transactions.begin();
			t!=transactions.end(); restart ? t=transactions.begin() : t++){
		restart=false;
		if ((*t)->getCallId()==did){
			ret+=getTransactionDebugString(*t, torequests, indent+1);
			n++;
			transactions.erase(t);
			restart=true; // t is invalidated, and we'll restart the loop
		}
	}

	return ret;
}

std::string SipStackInternal::getStackStatusDebugString(){

	string ret;
	int indent =1;

	string ind;
	for (int i=0; i< indent; i++)
		ind+="\t";

	list<MRef<SipDialog*> > calls = getDispatcher()->getLayerDialog()->getDialogs();
	list<MRef<SipTransaction*> > transactions= getDispatcher()->getLayerTransaction()->getTransactions();
	list <TPRequest<string,MRef<StateMachine<SipSMCommand,string>*> > > torequests =
			getTimeoutProvider()->getTimeoutRequests();

	if (calls.size()==0)
		ret+=ind+"(no calls)\n";
	else{
		int ii=0;
		for (list<MRef<SipDialog*> >::iterator i=calls.begin(); i!= calls.end(); i++, ii++){
			ret+=ind+"("+itoa(ii)+")\n";
			//cerr << string("    (")+itoa(ii)+") " ;
			ret+= getDialogDebugString(*i, transactions, torequests, indent+1);
		}
		
	}

	if (transactions.size()==0){
		ret+=ind+"(no transactions outside dialogs)\n";
		//cerr << "    (no transactions outside dialogs)"<< endl;
	}else{
		// all transactions for the dialogs have been removed
		ret+=ind+"Transactions outside dialogs:\n";
		for (list<MRef<SipTransaction*> >::iterator ti=transactions.begin(); ti!=transactions.end(); ti++){
			ret+=getTransactionDebugString(*ti,torequests,indent+1);
		}
	}

	// If the following assertion fails it is likely to be because a 
	// timer was not cancelled correctly in a transaction.
	// (a timer exists for a transaction or dialog that is no longer
	// used by the stack)
	massert(torequests.size()==0);
	return ret;
}

struct transportConfigCmp {
		typedef MRef<SipTransportConfig*> first_argument_type;
		typedef const string second_argument_type;
		typedef bool  result_type;

		result_type operator()( first_argument_type config,
					second_argument_type name ) const {
			return config->getName() == name;
		}
};

MRef<SipTransportConfig*> SipStackInternal::findTransportConfig( const std::string &transportName ) const{
	list<MRef<SipTransportConfig*> >::const_iterator transp =
		find_if( config->transports.begin(), config->transports.end(),
			 bind2nd( transportConfigCmp(), transportName ) );

	if( transp == config->transports.end() ){
		return NULL;
	}

	return *transp;
}

void SipStackInternal::startServers(){
	startSipServers();
	startSipsServers();
}

void SipStackInternal::startSipServers(){
	int32_t port = config->preferedLocalSipPort;

	try {
		startServers( false, port );
	} catch ( const BindFailed &bf ){
		stopServers( false );
		// Retry with a dynamic port
		// choosen by the first started transport
		port = 0;
		startServers( false, port );
	}

	mdbg << "SIP servers started on port " << port << endl;
}

void SipStackInternal::startSipsServers(){
	int32_t port = config->preferedLocalSipsPort;

	try {
		startServers( true, port );
	} catch ( const BindFailed &bf ){
		stopServers( true );
		// Retry with a dynamic port
		// choosen by the first started transport
		port = 0;
		startServers( true, port );
	}

	mdbg << "SIPS servers started on port " << port << endl;
}

void SipStackInternal::startServers( bool secure, int32_t &prefPort ){
	list<MRef<SipTransportConfig*> >::const_iterator i;
	list<MRef<SipTransportConfig*> >::const_iterator last =
		config->transports.end();
	for( i = config->transports.begin(); i != last; i++ ){
		MRef<SipTransportConfig*> transportConfig = (*i);
		string name = transportConfig->getName();

		if( !transportConfig->isEnabled() )
			continue;

		MRef<SipTransport*> transport =
			SipTransportRegistry::getInstance()->findTransportByName( name );
		if( !transport ){
			merr << "Failed to start " << transport->getName() << " server, unsupported" << endl;
			continue;
		}

		if( secure != transport->isSecure() ){
			// Only start on type SIP or SIPS
			continue;
		}

		mdbg << "SipStack: Starting " << name << " transport worker thread" << endl;

// 			if( !phoneconfig->defaultIdentity->getSim() || phoneconfig->defaultIdentity->getSim()->getCertificateChain().isNull() ){
// 				merr << "Certificate needed for TLS server. You will not be able to receive incoming TLS connections." << endl;
// 			}

		startServer( transport, prefPort );
	}
}

void SipStackInternal::startServer( const string &transportName ){
	MRef<SipTransport*> transport =
		SipTransportRegistry::getInstance()->findTransportByName( transportName );

	if( !transport ){
		merr << "Failed to start " << transportName << " server, unsupported" << endl;
		return;
	}

	int32_t port = 0;

	if( transport->isSecure() )
		port = config->preferedLocalSipsPort;
	else
		port = config->preferedLocalSipPort;

	startServer( transport, port );
}


void SipStackInternal::startServer( MRef<SipTransport*> transport,
				    int32_t &port ){
	int32_t externalUdpPort = 0;
	string ipString = config->localIpString;

	if( transport->getName() == "UDP" ){
		if( config->externalContactIP.size()>0 ){
			ipString = config->externalContactIP;
			externalUdpPort = config->externalContactUdpPort;
		}
	}

	bool done=false;
	int ntries=8;
	while (!done && ntries>0){

		try{
			dispatcher->getLayerTransport()->startServer( transport,
					ipString, 
					config->localIp6String, 
					port, 
					externalUdpPort,
					config->cert, 
					config->cert_db );
			done=true;
		}catch(Exception &e){
			port = config->externalContactUdpPort = rand()%32000+32000;
		}catch(...){
			throw;
		}
	}
}
	

void SipStackInternal::stopServers( bool secure ){
	list<MRef<SipTransportConfig*> >::const_iterator i;
	list<MRef<SipTransportConfig*> >::const_iterator last =
		config->transports.end();
	for( i = config->transports.begin(); i != last; i++ ){
		MRef<SipTransportConfig*> transportConfig = (*i);
		string name = transportConfig->getName();

		if( !transportConfig->isEnabled() )
			continue;

		MRef<SipTransport*> transport =
			SipTransportRegistry::getInstance()->findTransportByName( name );

		if( !transport ){
			mdbg << "Error: Failed to stop " << name << endl;
			continue;
		}

		if( secure != transport->isSecure() ){
			continue;
		}

		mdbg << "SipStack: Stopping " << name << " transport worker thread" << endl;

		stopServer( transport );
	}
}


void SipStackInternal::stopServer( const string &transportName ){
	MRef<SipTransport*> transport =
		SipTransportRegistry::getInstance()->findTransportByName( transportName );

	if( !transport ){
		mdbg << "Error: Failed to stop " << transportName << endl;
		return;
	}

	stopServer( transport );
}

void SipStackInternal::stopServer( MRef<SipTransport*> transport ){
	try{
		dispatcher->getLayerTransport()->stopServer( transport );
	}catch( NetworkException &e ){
		mdbg << "Error: Failed to stop " << transport->getName() << ": "<< e.what()<<endl;
	}
}

int32_t SipStackInternal::getLocalSipPort(bool usesStun, const string &transport ) {

	return dispatcher->getLayerTransport()->getLocalSipPort( transport );

}

std::string SipStackInternal::createClientTransaction(MRef<SipRequest*> req){
	return dispatcher->getLayerTransaction()->createClientTransaction(req);
}

void SipStackInternal::setInformTransactionTerminate(bool doInform){
	        return dispatcher->setInformTransactionTerminate(doInform);
}


