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
#include<libmsip/SipStackInternal.h>

#include<libmsip/SipLayerTransport.h>
#include<libmsip/SipCommandDispatcher.h>
#include<libmsip/SipMessageContentIM.h>
#include<libmsip/SipMessageContentMime.h>
#include<libmutil/Timestamp.h>

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
#include<libmsip/SipHeaderSupported.h>
#include<libmsip/SipHeaderUnsupported.h>
#include<libmsip/SipHeaderCallID.h>
#include<libmsip/SipHeaderTo.h>
#include<libmsip/SipHeaderWWWAuthenticate.h>
#include<libmsip/SipCommandString.h>
#include<libmnetutil/UDPSocket.h>
#include<libmnetutil/TLSServerSocket.h>

#include<libmutil/massert.h>

#include<libmutil/dbg.h>
#include<libmcrypto/cert.h>

using namespace std;

SipStackInternal::SipStackInternal( MRef<SipStackConfig *> stackConfig )
{
	timers = new SipTimers;
	this->config = stackConfig;

	timeoutProvider = new TimeoutProvider<string, MRef<StateMachine<SipSMCommand,string>*> >;

	SipHeader::headerFactories.addFactory("Accept", sipHeaderAcceptFactory);
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
	SipHeader::headerFactories.addFactory("Subject", sipHeaderSubjectFactory);
	SipHeader::headerFactories.addFactory("s", sipHeaderSubjectFactory);
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

}

MRef<SipCommandDispatcher*> SipStackInternal::getDispatcher(){
	return dispatcher;
}

MRef<SipStackConfig*> SipStackInternal::getStackConfig(){
	return config;
}

void SipStackInternal::setDefaultDialogCommandHandler(MRef<SipSMCommandReceiver*> cb){
	dispatcher->getLayerDialog()->setDefaultDialogCommandHandler(cb);
}

void SipStackInternal::setTransactionHandlesAck(bool transHandleAck){
	dispatcher->getLayerTransaction()->doHandleAck(transHandleAck);
}


void SipStackInternal::setCallback(MRef<CommandReceiver*> callback){
	this->callback = callback;
	dispatcher->setCallback(callback);
}

MRef<CommandReceiver*> SipStackInternal::getCallback(){
	return callback;
}

void SipStackInternal::setConfCallback(MRef<CommandReceiver*> callback){
	this->confCallback = callback;
}

MRef<CommandReceiver*> SipStackInternal::getConfCallback(){
	return confCallback;
}

void SipStackInternal::run(){
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
			ret+= ind +"timeout: "
				+ (*jj).getCommand()
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
	ret+=getTimeoutDebugString(*t,torequests,2);
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
	for (list<MRef<SipTransaction*> >::iterator t = transactions.begin();
			t!=transactions.end(); t++){
		if ((*t)->getCallId()==did){
			ret+=getTransactionDebugString(*t, torequests, indent+1);
			n++;
			transactions.erase(t);
			t=transactions.begin();
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
	massert(torequests.size()==0);
	return ret;
}


MRef<SipSocketServer *> SipStackInternal::createUdpServer( bool ipv6, const string &ipString )
{
	int32_t port = config->localUdpPort;

	MRef<DatagramSocket *> sock = new UDPSocket( port, ipv6 );
	MRef<SipSocketServer *> server;

	server = new DatagramSocketServer( dispatcher->getLayerTransport(), sock );
	// IPv6 doesn't need different external udp port
	// since it never is NATed.
	if( !ipv6 && config->externalContactUdpPort ){
		server->setExternalPort( config->externalContactUdpPort );
	}
	server->setExternalIp( ipString );

	return server;
}

MRef<SipSocketServer *> SipStackInternal::createTcpServer( bool ipv6, const string &ipString )
{
	MRef<ServerSocket *> sock;
	MRef<SipSocketServer *> server;
	int32_t port = config->localTcpPort;

	sock = ServerSocket::create( port, ipv6 );
	server = new StreamSocketServer( dispatcher->getLayerTransport(), sock );
	server->setExternalIp( ipString );

	return server;
}

MRef<SipSocketServer *> SipStackInternal::createTlsServer( bool ipv6, const string &ipString )
{
	MRef<ServerSocket *> sock;
	MRef<SipSocketServer *> server;
	int32_t port = config->localTlsPort;

	sock = new TLSServerSocket( ipv6, port, config->cert->get_first(),
				    config->cert_db );
	server = new StreamSocketServer( dispatcher->getLayerTransport(), sock );
	server->setExternalIp( ipString );

	return server;
}

void SipStackInternal::startUdpServer()
{
	MRef<SipSocketServer *> server;
	string ipString;

	if( config->externalContactIP.size()>0 )
		ipString = config->externalContactIP;
	else
		ipString = config->localIpString;

	server = createUdpServer( false, ipString );
	dispatcher->getLayerTransport()->addServer( server );

	if( config->localIp6String != "" ){
		MRef<SipSocketServer *> server6;

		server6 = createUdpServer( true, config->localIp6String );
		dispatcher->getLayerTransport()->addServer( server6 );
	}
}


void SipStackInternal::startTcpServer()
{
	MRef<SipSocketServer *> server;

	server = createTcpServer( false, config->localIpString);
	dispatcher->getLayerTransport()->addServer( server );

	if( config->localIp6String != "" ){
		MRef<SipSocketServer *> server6;

		server6 = createTcpServer( true, config->localIp6String );
		dispatcher->getLayerTransport()->addServer( server6 );
	}
}

void SipStackInternal::startTlsServer(){
	MRef<SipSocketServer *> server;

	if( config->cert->get_first().isNull() ){
		merr << "You need a personal certificate to run "
			"a TLS server. Please specify one in "
			"the certificate settings. minisip will "
			"now disable the TLS server." << end;
		return;
	}

	server = createTlsServer( false, config->localIpString );
	dispatcher->getLayerTransport()->addServer( server );

	if( config->localIp6String != "" ){
		MRef<SipSocketServer *> server6;

		server6 = createTlsServer( true, config->localIp6String );
		dispatcher->getLayerTransport()->addServer( server6 );
	}
}
