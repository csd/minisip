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

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *	    Joachim Orrblad <joachim[at]orrblad.com>
*/
#include<config.h>
#include<libmsip/SipStackInternal.h>

#include<libmsip/SipLayerTransport.h>
#include<libmsip/SipCommandDispatcher.h>
#include<libmsip/SipMessageContentIM.h>
#include<libmsip/SipMIMEContent.h>
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

#include<libmutil/massert.h>

#include<libmutil/dbg.h>
#include<libmcrypto/cert.h>

using namespace std;

SipStackInternal::SipStackInternal( MRef<SipCommonConfig *> stackConfig,
		MRef<certificate_chain *> cert_chain,
		MRef<ca_db *> cert_db
		)
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

	MRef<SipLayerTransport*> transp = MRef<SipLayerTransport*>(new
			SipLayerTransport(
				stackConfig->localIpString,
				stackConfig->externalContactIP,
				stackConfig->externalContactUdpPort,
				stackConfig->localUdpPort,
				stackConfig->localTcpPort,
				stackConfig->localTlsPort,
				cert_chain,
				cert_db
				)
			);

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
			first=false;
		}
		ret = ret+(*i);
	}
	return ret;
}


