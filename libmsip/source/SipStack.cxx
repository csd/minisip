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
#include<libmsip/SipStack.h>
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

#include<libmutil/massert.h>

#include<libmutil/dbg.h>
#include<libmcrypto/cert.h>

using namespace std;

#define STACK (*(MRef<SipStackInternal*> *) sipStackInternal)

SipStack::SipStack( MRef<SipStackConfig *> stackConfig,
		MRef<certificate_chain *> cert_chain,
		MRef<ca_db *> cert_db
		){
	
	SipStackInternal *istack = new SipStackInternal(stackConfig, cert_chain, cert_db);

	sipStackInternal = new MRef<SipStackInternal*>(istack);
}

SipStack::~SipStack(){
	MRef<SipStackInternal*> * mrefPtr = (MRef<SipStackInternal*> *) sipStackInternal;
	delete mrefPtr;
}

void SipStack::setTransactionHandlesAck(bool transHandlesAck){
	//sipStackInternal->setTransactionHandlesAck(transHandlesAck);
	STACK->setTransactionHandlesAck(transHandlesAck);

}

void SipStack::setDefaultDialogCommandHandler(MRef<SipSMCommandReceiver*> cb){
	STACK->setDefaultDialogCommandHandler(cb);
}

void SipStack::run(){
	STACK->run();
}

void SipStack::stopRunning(){
	STACK->stopRunning();
}

bool SipStack::handleCommand(const CommandString &cmd){
	return STACK->handleCommand(cmd);
}

void SipStack::setCallback(MRef<CommandReceiver*> callback){
	STACK->setCallback(callback);
}

MRef<CommandReceiver*> SipStack::getCallback(){
	return STACK->getCallback();
}

void SipStack::setConfCallback(MRef<CommandReceiver*> callback){
	STACK->setConfCallback(callback);
}

MRef<CommandReceiver *> SipStack::getConfCallback(){
	return STACK->getConfCallback();
}

void SipStack::addDialog(MRef<SipDialog*> d){
	STACK->addDialog(d);
}

MRef<SipTimers*> SipStack::getTimers(){
	return STACK->getTimers();
}	

MRef<SipStackConfig*> SipStack::getStackConfig(){
	return STACK->getStackConfig();
}

void SipStack::addSupportedExtension(std::string extension){
	STACK->addSupportedExtension(extension);
}

std::string SipStack::getAllSupportedExtensionsStr(){
	return STACK->getAllSupportedExtensionsStr();
}

bool SipStack::supports(std::string extension){
	return STACK->supports(extension);
}


MRef<TimeoutProvider<std::string,MRef<StateMachine<SipSMCommand,std::string>*> > *> 
SipStack::getTimeoutProvider(){
	return STACK->getTimeoutProvider();
}

void SipStack::enqueueTimeout(MRef<SipDialog*> receiver, const SipSMCommand &cmd){
	STACK->getDispatcher()->enqueueTimeout(receiver, cmd);
}

std::list<MRef<SipDialog *> > SipStack::getDialogs(){
	return STACK->getDispatcher()->getDialogs();
}

void SipStack::enqueueCommand(const SipSMCommand &cmd, int queue){
	STACK->getDispatcher()->enqueueCommand(cmd, queue);
}

bool SipStack::handleCommand(const SipSMCommand &cmd){
	return STACK->handleCommand(cmd);
}

void SipStack::setDialogManagement(MRef<SipDialog*> mgmt){
	STACK->getDispatcher()->setDialogManagement(mgmt);
}

void SipStack::startTcpServer(){
	STACK->getDispatcher()->getLayerTransport()->startTcpServer();
}

void SipStack::startTlsServer(){
	STACK->getDispatcher()->getLayerTransport()->startTlsServer();
}

void SipStack::setDebugPrintPackets(bool enable){
	set_debug_print_packets(enable);	
}

bool SipStack::getDebugPrintPackets(){
	return get_debug_print_packets();
}

std::string SipStack::getStackStatusDebugString(){
	return STACK->getStackStatusDebugString();
}



