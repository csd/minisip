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
#include"SipStackInternal.h"

#include"SipLayerTransport.h"
#include"SipCommandDispatcher.h"
#include<libmsip/SipTransport.h>
#include<libmutil/massert.h>

#include<libmutil/dbg.h>
#include<libmcrypto/cert.h>

using namespace std;

#define STACK (*(MRef<SipStackInternal*> *) sipStackInternal)

SipStack::SipStack( MRef<SipStackConfig *> stackConfig ){
	SipStackInternal *istack = new SipStackInternal(stackConfig);

	sipStackInternal = new MRef<SipStackInternal*>(istack);
}

SipStack::~SipStack(){
	free();
}

void SipStack::free(){
	if (sipStackInternal){
		MRef<SipStackInternal*> * mrefPtr = (MRef<SipStackInternal*> *) sipStackInternal;
		(*mrefPtr)->free();
		delete mrefPtr;
		sipStackInternal=NULL;
	}
}

void SipStack::setTransactionHandlesAck(bool transHandlesAck){
	//sipStackInternal->setTransactionHandlesAck(transHandlesAck);
	STACK->setTransactionHandlesAck(transHandlesAck);

}

void SipStack::setDefaultDialogCommandHandler(MRef<SipDefaultHandler*> cb){
	STACK->setDefaultDialogCommandHandler(cb);
}

void SipStack::run(){
	STACK->run();
}

void SipStack::stopRunning(){
	STACK->stopRunning();
}

void SipStack::handleCommand(std::string subsystem, const CommandString &cmd){
	massert(subsystem=="sip");
	if (!handleCommand(cmd))
		STACK->getDefaultDialogCommandHandler()->handleCommand(subsystem,cmd);
}

CommandString SipStack::handleCommandResp(std::string subsystem, const CommandString &cmd){
	return STACK->getDefaultDialogCommandHandler()->handleCommandResp(subsystem, cmd);
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

void SipStack::startServers(){
	STACK->startServers();
}

void SipStack::startUdpServer(){
	STACK->startServer( "UDP" );
}

void SipStack::startTcpServer(){
	STACK->startServer( "TCP" );
}

void SipStack::startTlsServer(){
	STACK->startServer( "TLS" );
}

void SipStack::startServer( const string &transportName ){
	STACK->startServer( transportName );
}

int32_t SipStack::getLocalSipPort(bool usesStun, const string &transport ) {
	return STACK->getLocalSipPort(usesStun, transport);
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

std::string SipStack::createClientTransaction(MRef<SipRequest*> req){
	return STACK->createClientTransaction(req);
}

void SipStack::setInformTransactionTerminate(bool doInform){
	return STACK->setInformTransactionTerminate(doInform);
}

