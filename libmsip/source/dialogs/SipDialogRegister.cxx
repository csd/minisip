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


#include<libmsip/SipStack.h>
#include<libmsip/SipDialogConfig.h>
#include<libmsip/SipDialogRegister.h>
#include"../SipCommandDispatcher.h"
#include<libmsip/SipTransitionUtils.h>
#include<libmsip/SipRequest.h>
#include<libmsip/SipResponse.h>
#include<libmsip/SipHeaderContact.h>
#include<libmsip/SipHeaderFrom.h>
#include<libmsip/SipHeaderSnakeSM.h>
#include<libmsip/SipHeaderSupported.h>
#include<libmsip/SipAuthenticationDigest.h>
#include<libmsip/SipCommandString.h>
#include<libmsip/SipSMCommand.h>
#include<libmutil/massert.h>

#ifdef DEBUG_OUTPUT
//#	include<libmutil/Timestamp.h>
#	include<libmutil/dbg.h>
#endif

/*               
 (a11 does not exist)
 
        +-------------+
        |    S0       |
        |    Start    |
        |             |                           transport_error
        +-------------+                            a10 GUI(failed)
                   |  +---------------------------------------------------------------+
cmdstr:proxy_regist|  | 401                                                           |
  a0 sendRegister  |  | a2 sendRegister                                               |
                   |  | +-+              cmdstr:setpass                               |
                   V  | | V              a5 sendRegister                              |
            +-------------+<-------------------------+                                |
            |   S1        | 401                      |                                |
        +-->|   Trying    | a3 ask_dialog    +---------------+                        |
        |   |             |----------------->| S4            |                        |
        |   +-------------+                  | Ask password  |                        |
        |          |  |  ^                   |               |                        |
        |          |  +--+                   +---------------+                        |
        |   2xx OK |    1xx                                |cancel +----------+       |
        |   a1 -   |    a14 -                              +------>| S5       |<------+
        |          V                                        a9     | Failed   |
        |   +-------------+                                        |          |
        |   |  S2         |                                        +----------+
        +---|  Registred  |                                              |
 register   |             |                                              |
 sendRegist +-------------+                                              V     a13: notransactions
        |       |                                                  +----------+
        --------+                                                  |          |
             a12 cmdstr:register <proxy>                           |terminated|
                                                                   |          |
                                                                   +----------+
  TODO: handle all possible responses							
*/  

using namespace std;

//a12 also deals with proxy_register ... but once it is already registered ...
bool SipDialogRegister::a0_start_trying_register( const SipSMCommand &command){

	if (transitionMatch(command, 
				SipCommandString::proxy_register,
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer)){

		//Set user and password for the register
		if (command.getCommandString().getParam()!="" && command.getCommandString().getParam2()!=""){
			MRef<SipCredential*> cred;
			cred = getDialogConfig()->sipIdentity->getCredential();

			cred->set( command.getCommandString().getParam(),
				   command.getCommandString().getParam2() );
		} 

		//Set expires param ... in seconds (in param3)
		if (command.getCommandString().getParam3()!="" ) {
			getDialogConfig()->sipIdentity->getSipRegistrar()->setRegisterExpires( command.getCommandString().getParam3() );
		}
		
		//if it comes with an identity ... use it to filter out commands not for this dialog ...
		if (command.getCommandString()["identityId"]!="" ) {
			string identity;
			identity = command.getCommandString()["identityId"];
			if( identity != getDialogConfig()->sipIdentity->getId() ) {
				//we got a proxy_register not for our identity ... 
				return false;
			}
		}
			
		++dialogState.seqNo;
		sendRegister();
		
		CommandString cmdstr( dialogState.callId, SipCommandString::register_sent);
		cmdstr["identityId"] = getDialogConfig()->sipIdentity->getId();
		getSipStack()->getCallback()->handleCommand("gui", cmdstr );

		return true;
	}else{
		massert(1==0); // this should never happen - the first transaction MUST be register command
		return false;
	}
}

bool SipDialogRegister::a1_trying_registred_2xx( const SipSMCommand &command){
	
	if (transitionMatch(SipResponse::type,
				command, 
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer, 
				"2**")){  

///FIXME: XXXXXX Removed socket from packet - this functionality needs to be looked over
///		regcall->getDialogContainer()->getPhoneConfig()->proxyConnection = command.getCommandPacket()->getSocket();
		
		//Mark the identity as currently registered (or not, maybe we are unregistering)

		MRef<SipMessage*> pkt = command.getCommandPacket();
		MRef<SipHeaderValueContact *> c = pkt->getHeaderValueContact();
		list<SipUri> contacts;

		// TODO add all contacts from the response
		if( c ){
			contacts.push_back( c->getUri() );
		}
		getDialogConfig()->sipIdentity->setRegisteredContacts ( contacts );
		getDialogConfig()->sipIdentity->setIsRegistered ( true );
		
		CommandString cmdstr( 
			dialogState.callId, 
			SipCommandString::register_ok, 
			getDialogConfig()->sipIdentity->getSipRegistrar()->getUri().getIp());
		cmdstr["identityId"] = getDialogConfig()->sipIdentity->getId();
		if (getGuiFeedback()){
			getSipStack()->getCallback()->handleCommand("gui", cmdstr );
			setGuiFeedback(false);
		}
		
		//this is for the shutdown dialog 
		SipSMCommand cmd( cmdstr, SipSMCommand::dialog_layer, SipSMCommand::dispatcher );
		getSipStack()->enqueueCommand( cmd, HIGH_PRIO_QUEUE ); 

		MRef<SipHeaderValue*> snakehdr = pkt->getHeaderValueNo(SIP_HEADER_TYPE_SNAKESM, 0);
		if (snakehdr){
			MRef<SipHeaderValueSnakeSM*> h = dynamic_cast<SipHeaderValueSnakeSM*>(*snakehdr);
			string serviceManager=h->getString();
			CommandString cmdstr( dialogState.callId, "service_manager", serviceManager);
			getSipStack()->getCallback()->handleCommand("snake", cmdstr );
		}


		//request a timeout to retx a proxy_register only if we are registered ... 
		//otherwise we would just be unregistering every now and then ...
		if( getDialogConfig()->sipIdentity->isRegistered () ) {
			requestTimeout(
				getDialogConfig()->sipIdentity->getSipRegistrar()->getRegisterExpires_int() * 1000,
				SipCommandString::proxy_register);
		} 

		//Un-registering is done in the same dialog. With (a
		//configuration of?) opensips, we don't un-register
		//correctly by using the authentication state from
		//when we registred.
//		clearAuthentications();
		return true;
	}else{
		return false;
	}
}

bool SipDialogRegister::a2_trying_trying_40x( const SipSMCommand &command){
	if (transitionMatch(SipResponse::type, 
				command, 
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer, 
				"401\n407")){

		//extract authentication info from received response
		MRef<SipResponse*> resp( (SipResponse *)*command.getCommandPacket());

		if( !updateAuthentications( *resp ) ){
			// Fall through to a3
			return false;
		}

		++dialogState.seqNo;
		sendRegister();
		//TODO: inform GUI

		return true;
	}else{
		return false;
	}
}

bool SipDialogRegister::a3_trying_askpassword_40x( const SipSMCommand &command){
	if (transitionMatch(SipResponse::type, 
				command, 
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer, 
				"401\n407")){

		string realm = findUnauthenticatedRealm();
		
		//TODO: Ask password
		CommandString cmdstr( 
			dialogState.callId, 
			SipCommandString::ask_password, 
			getDialogConfig()->sipIdentity->getSipRegistrar()->getUri().getIp());
		cmdstr["identityId"] = getDialogConfig()->sipIdentity->getId();
		cmdstr["identityUri"] = getDialogConfig()->sipIdentity->getSipUri().getString();
		cmdstr["realm"] = realm;

		getSipStack()->getCallback()->handleCommand("gui", cmdstr );
		//authentication info from received response is extracted in a2
		return true;
	}else{
		return false;
	}
}

bool SipDialogRegister::a5_askpassword_trying_setpassword( const SipSMCommand &command){
	
	if (transitionMatch(command, 
				SipCommandString::setpassword,
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer)){

		
		string realm = command.getCommandString()["realm"];
		
			//We store the new credentials for this dialogs
			//configuration. Note that it is not saved for the
			//next time minisip is started.
		const string &user = command.getCommandString().getParam();
		const string &pass = command.getCommandString().getParam2() ;

		MRef<SipCredential*> cred =
			new SipCredential( user, pass, realm );

		addCredential( cred );

		++dialogState.seqNo;

		sendRegister();
		return true;
	}else{
		return false;
	}
}

bool SipDialogRegister::a9_askpassword_failed_cancel( const SipSMCommand &command){
	
	//TODO: implement cancel functionality in GUI
	if (transitionMatch(command, 
				"cancel_register",
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer)){

		//Mark the identity as currently un-registered
		getDialogConfig()->sipIdentity->setIsRegistered ( false );
	
#ifdef DEBUG_OUTPUT
		mdbg("signaling/sip") << "WARNING: SipDialogRegister::a9: unimplemented section reached"<<endl;
#endif
		return true;
	}else{
		return false;
	}
}

bool SipDialogRegister::a10_trying_failed_transporterror( const SipSMCommand &command){
	
	if (transitionMatch(command, 
				SipCommandString::transport_error,
				IGN,
				SipSMCommand::dialog_layer)){
		
		//Mark the identity as currently un-registered
		getDialogConfig()->sipIdentity->setIsRegistered ( false );
		
		CommandString cmdstr( dialogState.callId, SipCommandString::transport_error);
		cmdstr["identityId"] = getDialogConfig()->sipIdentity->getId();
		cmdstr["identityUri"] = getDialogConfig()->sipIdentity->getSipUri().getString();
		getSipStack()->getCallback()->handleCommand("gui", cmdstr );
		return true;
	}else{
		return false;
	}
}

bool SipDialogRegister::a12_registred_trying_proxyregister( const SipSMCommand &command){

	if (transitionMatch(command, 
				SipCommandString::proxy_register,
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer)){

		cancelTimeout(SipCommandString::proxy_register);

		//Set proxy username and password
		if (command.getCommandString().getParam()!="" && command.getCommandString().getParam2()!=""){
			MRef<SipCredential*> cred;
			cred = getDialogConfig()->sipIdentity->getCredential();

			cred->set( command.getCommandString().getParam(),
				   command.getCommandString().getParam2() );
		}
		
		//Set expires param ... in seconds
		if (command.getCommandString().getParam3()!="" ) {
			getDialogConfig()->sipIdentity->getSipRegistrar()->setRegisterExpires( command.getCommandString().getParam3() );
		}
		
		//if it comes with an identity ... use it to filter out commands not for this dialog ...
		if (command.getCommandString()["identityId"]!="" ) {
			string identity;
			identity = command.getCommandString()["identityId"];
			if( identity != getDialogConfig()->sipIdentity->getId() ) {
				//we got a proxy_register not for our identity ... 
				return false;
			}
		}		
		
		++dialogState.seqNo;
		sendRegister();
		
		CommandString cmdstr(dialogState.callId, SipCommandString::register_sent);
		cmdstr["identityId"] = getDialogConfig()->sipIdentity->getId();
		cmdstr["identityUri"] = getDialogConfig()->sipIdentity->getSipUri().getString();
		getSipStack()->getCallback()->handleCommand("gui", cmdstr );
		return true;
	}else{
		return false;
	}
}


bool SipDialogRegister::a13_failed_terminated_notransactions( const SipSMCommand &command){

	if (transitionMatch(command, 
				SipCommandString::no_transactions,
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer)){

		dialogState.isTerminated=true;
		CommandString cmdstr ( dialogState.callId, 
				SipCommandString::call_terminated);
		cmdstr["identityId"] = getDialogConfig()->sipIdentity->getId();
		SipSMCommand cmd(cmdstr,
				SipSMCommand::dialog_layer,
				SipSMCommand::dispatcher);
		
		getSipStack()->enqueueCommand( cmd, HIGH_PRIO_QUEUE );

		return true;
	}else{
		return false;
	}
}

bool SipDialogRegister::a14_trying_trying_1xx( const SipSMCommand &command){
	
	if (transitionMatch(SipResponse::type, 
				command, 
				SipSMCommand::transaction_layer, 
				SipSMCommand::dialog_layer, 
				"1**")){
		return true;
	}else{
		return false;
	}
}

void SipDialogRegister::setUpStateMachine(){
	
	State<SipSMCommand, string> *s0_start = 
		new State<SipSMCommand,string>(this,"s0_start");
	addState(s0_start);
	
	State<SipSMCommand, string> *s1_trying = 
		new State<SipSMCommand,string>(this,"s1_trying");
	addState(s1_trying);
	
	State<SipSMCommand, string> *s2_registred = 
		new State<SipSMCommand,string>(this,"s2_registred");
	addState(s2_registred);
	
	State<SipSMCommand, string> *s4_askpassword= 
		new State<SipSMCommand,string>(this,"s4_askpassword");
	addState(s4_askpassword);
	
	State<SipSMCommand, string> *s5_failed= 
		new State<SipSMCommand,string>(this,"s5_failed");
	addState(s5_failed);

	State<SipSMCommand, string> *terminated= 
		new State<SipSMCommand,string>(this,"terminated");
	addState(terminated);
	

	new StateTransition<SipSMCommand,string>(this, "transition_start_trying_register",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogRegister::a0_start_trying_register,
		s0_start, s1_trying);

	new StateTransition<SipSMCommand,string>(this, "transition_trying_registred_2xx",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogRegister::a1_trying_registred_2xx,
		s1_trying, s2_registred);

	new StateTransition<SipSMCommand,string>(this, "transition_trying_trying_40x",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogRegister::a2_trying_trying_40x,
		s1_trying, s1_trying);

	new StateTransition<SipSMCommand,string>(this, "transition_trying_askpassword_40x",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogRegister::a3_trying_askpassword_40x,
		s1_trying, s4_askpassword);

	new StateTransition<SipSMCommand,string>(this, "transition_askpassword_askpassword_setpass",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogRegister::a5_askpassword_trying_setpassword,
		s4_askpassword, s1_trying);

	new StateTransition<SipSMCommand,string>(this, "transition_askpassword_failed_cancel",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogRegister::a9_askpassword_failed_cancel,
		s4_askpassword, s5_failed);

	new StateTransition<SipSMCommand,string>(this, "transition_trying_failed_transporterror",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogRegister::a10_trying_failed_transporterror,
		s1_trying, s5_failed);

	new StateTransition<SipSMCommand,string>(this, "transition_registred_trying_proxyregister",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogRegister::a12_registred_trying_proxyregister,
		s2_registred, s1_trying);

	new StateTransition<SipSMCommand,string>(this, "transition_failed_terminated_notransactions",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogRegister::a13_failed_terminated_notransactions,
		s5_failed, terminated );

	new StateTransition<SipSMCommand,string>(this, "transition_trying_trying",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogRegister::a14_trying_trying_1xx,
		s1_trying, s1_trying );

	setCurrentState(s0_start);
}

SipDialogRegister::SipDialogRegister(MRef<SipStack*> stack, MRef<SipIdentity*> ident)
		: SipDialog(stack, ident, ""),
			failCount(0),
			guiFeedback(true)
{
	setUpStateMachine();
	myDomain = getDialogConfig()->sipIdentity->getSipUri().getIp();
}

SipDialogRegister::~SipDialogRegister(){
}

void SipDialogRegister::updateFailCount(){
	failCount++;
}

uint32_t SipDialogRegister::getFailCount(){
	return failCount;
}

bool SipDialogRegister::getGuiFeedback(){
	return guiFeedback;
}

void SipDialogRegister::setGuiFeedback(bool fb){
	guiFeedback=fb;
}

bool SipDialogRegister::handleCommand(const SipSMCommand &command){

	if (command.getType()==SipSMCommand::COMMAND_PACKET 
			&& !(command.getDestination()==SipSMCommand::dialog_layer 
			/*|| command.getDestination()==SipSMCommand::ANY*/)){
		merr << "WARNING: UNEXPECTED: received packet in SipDialogRegister: "<<command.getCommandPacket()->getDescription() << endl;
		return false;
	}

	if (command.getType()==SipSMCommand::COMMAND_STRING 
		&& (command.getDestination()==SipSMCommand::dialog_layer /*|| command.getDestination()==SipSMCommand::ANY*/)
		&& (command.getCommandString().getOp()==SipCommandString::proxy_register)
		&& (command.getCommandString()["identityId"] == getDialogConfig()->sipIdentity->getId() || (command.getCommandString()["identityId"] == "" && (command.getCommandString()["proxy_domain"]=="" 
			|| command.getCommandString()["proxy_domain"]== getDialogConfig()->sipIdentity->getSipUri().getIp())
			    ))){
		return SipDialog::handleCommand(command);
	}

	if (command.getType()==SipSMCommand::COMMAND_STRING
		&& (command.getDestination()==SipSMCommand::dialog_layer 
		/*|| command.getDestination()==SipSMCommand::ANY*/)){
		
		if (dialogState.callId == command.getCommandString().getDestinationId()){
//			getPhone()->log(LOG_INFO, "SipDialogRegister::handleCommand: found matching call id");
		}else{
//			getPhone()->log(LOG_INFO, "SipDialogRegister::handleCommand: not matching call id for this call");
			return false;
		}
	}	

	bool ret = SipDialog::handleCommand(command);

	//If the command is no_transactions, it is meant for us even if no
	//transition matched it - we return true. (the case when we are
	//registred)
	if (!ret 
		&& command.getType()==SipSMCommand::COMMAND_STRING 
		&& command.getCommandString().getOp()==SipCommandString::no_transactions){
		return true;
	}
	
	return ret;
}

void SipDialogRegister::sendRegister(){
	
	MRef<SipIdentity*> identity = getDialogConfig()->sipIdentity;

	const SipUri &contact = getDialogConfig()->getContactUri(true); //if udp, use stun
	int expires = identity->getSipRegistrar()->getRegisterExpires_int();
	
	MRef<SipHeaderValueContact *> contactHdr =
		new SipHeaderValueContact(contact, expires);
	const string &instanceId = getSipStack()->getStackConfig()->instanceId;

	if( !instanceId.empty() ){
		contactHdr->setParameter("+sip.instance", instanceId);
		contactHdr->setParameter("reg-id", identity->getId());
	}

	MRef<SipRequest*> reg= SipRequest::createSipMessageRegister(
		dialogState.callId,
		identity->getSipRegistrar()->getUri(),
		identity->getSipUri(),
		contactHdr,
		dialogState.seqNo
		);

	reg->getHeaderValueFrom()->setParameter( "tag", dialogState.localTag );

	addAuthorizations( reg );
	addRoute( reg );

	if( !instanceId.empty() ){
		// Draft-Outbound needs path support, and
		// Draft-GRUU needs gruu
		reg->addHeader(new SipHeader(new SipHeaderValueSupported("path, gruu")));
	}

	SipSMCommand cmd(*reg, SipSMCommand::dialog_layer, SipSMCommand::transaction_layer);
	getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE);
}

