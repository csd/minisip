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


#include<libmsip/SipTransaction.h>
#include<libmsip/SipStack.h>
#include<libmsip/SipDialogConfig.h>
#include<libmsip/SipDialogRegister.h>
#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipTransactionUtils.h>
#include<libmsip/SipTransactionNonInviteClient.h>
#include<libmsip/SipRegister.h>
#include<libmsip/SipResponse.h>
#include<libmsip/SipCommandString.h>

#ifdef DEBUG_OUTPUT
#include<libmutil/Timestamp.h>
#include<libmutil/dbg.h>
#endif

/*               
 (a11 does not exist)
 
            +-------------+
	    |    S0       |
	    |    Start    |
	    |             |                           transport_error
	    +-------------+                            a10 GUI(failed)
                   |  +-----------------------------+---------------------------------+
cmdstr:proxy_regist|  |      401&&haspass    +------+--------+                        |
  a0 send_noauth   |  |      a2 send_auth    | S3            |---------------------+  |
                   |  | +--------------------| Trying_stored |                     |  |
                   V  | |                    |               |-+                   |  |
            +-------------+                  +---------------+ | 401               |  |
	    |S1           | 401&&nopass                        | a4 ask_dialog     |  |
	+-->|Trying_noauth| a3 ask_dialog    +---------------+ |                   |  |
	|   |             |------------------| S4            |<+                   |  |
	|   +-------------+                  | Ask password  |---+ cmdstr:setpass  |  |
        |          |                         |               |   | a5 send_auth    |  |
        |          |                         +---------------+<--+                 |  |
	|   2xx OK |                           | |      ^  |cancel +----------+    |  |
  	|   a1 -   |                           | +------+  +------>| S5       |<---)--+
	|	   V           200             |  401       a9     | Failed   |    |
        |   +-------------+    a6 -            |  a7 ask_dialog a8 |          |    |
	|   |  S2         |<-------------------+                   +----------+    |
	+---|  Registred  |                                              |         |
 register   |             |<---------------------------------------------^---------+
 send_noauth+-------------+                   2xx                        V     a13: notransactions
        |       |                             a8                   +----------+
        --------+                                                  |          |
      a12 cmdstr:register <proxy>	                           |terminated|
                                                                   |          |
                                                                   +----------+
*/  

bool SipDialogRegister::a0_start_tryingnoauth_register( const SipSMCommand &command){

    if (transitionMatch(command, SipCommandString::proxy_register)){

			if (command.getCommandString().getParam()!="" && command.getCommandString().getParam2()!=""){
				getDialogConfig()->inherited->sipIdentity->sipProxy.sipProxyUsername = command.getCommandString().getParam();
				getDialogConfig()->inherited->sipIdentity->sipProxy.sipProxyPassword = command.getCommandString().getParam2();
			}
			
			++dialogState.seqNo;

			MRef<SipTransaction*> trans = new SipTransactionNonInviteClient(sipStack, this, /*seqNo*/ dialogState.seqNo, dialogState.callId);
			registerTransaction(trans);
			send_noauth(trans->getBranch());
                        CommandString cmdstr( dialogState.callId, SipCommandString::register_sent);
			getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
		return true;
	}else{
		assert(1==0); // this should never happen - the first transaction MUST be register command
		return false;
	}
}

bool SipDialogRegister::a1_tryingnoauth_registred_2xx( const SipSMCommand &command){
	
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "2**")){  

///FIXME: XXXXXX Removed socket from packet - this functionality needs to be looked over
///		regcall->getDialogContainer()->getPhoneConfig()->proxyConnection = command.getCommandPacket()->getSocket();
		
		if (getGuiFeedback()){
                        CommandString cmdstr( dialogState.callId, 
                                    SipCommandString::register_ok, 
                                    getDialogConfig()->inherited->sipIdentity->sipProxy.sipProxyIpAddr->getString());
                        
			getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
			setGuiFeedback(false);
		}
		requestTimeout(1000*60*14,SipCommandString::proxy_register);
		return true;
	}else{
		return false;
	}
}

bool SipDialogRegister::a2_tryingnoauth_tryingstored_401haspass( const SipSMCommand &command){
	
	if ( hasPassword() && transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "401")){
		++dialogState.seqNo;

                MRef<SipTransaction*> trans( new SipTransactionNonInviteClient(sipStack, this, dialogState.seqNo, dialogState.callId));
                registerTransaction(trans);

                //extract authentication info from received response
                MRef<SipResponse*> resp( (SipResponse *)*command.getCommandPacket());
                setRealm( resp->getRealm() );
                setNonce( resp->getNonce() );

                send_auth(trans->getBranch());
                //TODO: inform GUI
                return true;
        }else{
            return false;
        }
}

bool SipDialogRegister::a3_tryingnoauth_askpassword_401nopass( const SipSMCommand &command){
	
	if ( hasPassword() && transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "401")){
		
		//TODO: Ask password
                CommandString cmdstr( dialogState.callId, SipCommandString::ask_password, getDialogConfig()->inherited->sipIdentity->sipProxy.sipProxyIpAddr->getString());
		getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
	//extract authentication info from received response
		MRef<SipResponse*> resp( (SipResponse *)*command.getCommandPacket());
		setRealm( resp->getRealm() );
		setNonce( resp->getNonce() );

		return true;
	}else{
		return false;
	}
}

bool SipDialogRegister::a4_tryingstored_askpassword_401( const SipSMCommand &command){
	
	if (transitionMatch(command, SipResponse::type, IGN, SipSMCommand::TU, "401")){

		//TODO: Ask password
                CommandString cmdstr( dialogState.callId, SipCommandString::ask_password, getDialogConfig()->inherited->sipIdentity->sipProxy.sipProxyIpAddr->getString());
		getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
		//extract authentication info from received response
		MRef<SipResponse*> resp( (SipResponse *)*command.getCommandPacket());
		setRealm( resp->getRealm() );
		setNonce( resp->getNonce() );

		return true;
	}else{
		return false;
	}
}

bool SipDialogRegister::a5_askpassword_askpassword_setpassword( const SipSMCommand &command){
	
	if (transitionMatch(command, SipCommandString::setpassword)){

	    //TODO: Ask password
		
		getDialogConfig()->inherited->sipIdentity->sipProxy.sipProxyUsername = command.getCommandString().getParam();
		getDialogConfig()->inherited->sipIdentity->sipProxy.sipProxyPassword= command.getCommandString().getParam2();
		
		++dialogState.seqNo;

		MRef<SipTransaction*> trans( new SipTransactionNonInviteClient(sipStack, this, dialogState.seqNo, dialogState.callId));
		registerTransaction(trans);

		send_auth(trans->getBranch());
		return true;
	}else{
		return false;
	}
}

bool SipDialogRegister::a6_askpassword_registred_2xx( const SipSMCommand &command){
	
	if (transitionMatch(command, SipResponse::type, SipSMCommand::transaction, IGN, "2**")){
 		
		//TODO: inform GUI
		if (getGuiFeedback()){
                        CommandString cmdstr( dialogState.callId, SipCommandString::register_ok, getDialogConfig()->inherited->sipIdentity->sipProxy.sipProxyIpAddr->getString());   
			getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
			setGuiFeedback(false);
		}
		requestTimeout(1000*60*14,SipCommandString::proxy_register);
		return true;
	}else{
		return false;
	}
}

bool SipDialogRegister::a7_askpassword_askpassword_401( const SipSMCommand &command){
	
	if (transitionMatch(command, SipResponse::type, SipSMCommand::transaction, IGN, "401")){

		//TODO: Ask password
                CommandString cmdstr( dialogState.callId, SipCommandString::ask_password, getDialogConfig()->inherited->sipIdentity->sipProxy.sipProxyIpAddr->getString());
		getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
	//extract authentication info from received response
		MRef<SipResponse*> resp = (SipResponse *)*command.getCommandPacket();
		setRealm( getRealm() );
		setNonce( getNonce() );

		return true;
	}else{
		return false;
	}
}

bool SipDialogRegister::a8_tryingstored_registred_2xx( const SipSMCommand &command){
	
	if (transitionMatch(command, SipResponse::type, SipSMCommand::transaction, IGN, "2**")){

		//TODO: inform GUI
		
		if (getGuiFeedback()){
                        CommandString cmdstr( dialogState.callId, SipCommandString::register_ok, getDialogConfig()->inherited->sipIdentity->sipProxy.sipProxyIpAddr->getString());
			getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
			setGuiFeedback(false);
		}
		requestTimeout(1000*60*14,SipCommandString::proxy_register);
		return true;
	}else{
		return false;
	}
}

bool SipDialogRegister::a9_askpassword_failed_cancel( const SipSMCommand &command){
	
	//TODO: implement cancel functionality in GUI
	if (transitionMatch(command, "cancel_register")){
#ifdef DEBUG_OUTPUT
		merr << "WARNING: SipDialogRegister::a9: unimplemented section reached"<<end;
#endif
		return true;
	}else{
		return false;
	}
}

bool SipDialogRegister::a10_tryingnoauth_failed_transporterror( const SipSMCommand &command){
	
	if (transitionMatch(command, SipCommandString::transport_error)){
		
                CommandString cmdstr( dialogState.callId, SipCommandString::transport_error);
		getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
		
		return true;
	}else{
		return false;
	}
}

bool SipDialogRegister::a12_registred_tryingnoauth_proxyregister( const SipSMCommand &command){

	if (transitionMatch(command, SipCommandString::proxy_register)){

			cancelTimeout(SipCommandString::proxy_register);

			if (command.getCommandString().getParam()!="" && command.getCommandString().getParam2()!=""){
				getDialogConfig()->inherited->sipIdentity->sipProxy.sipProxyUsername = command.getCommandString().getParam();
				getDialogConfig()->inherited->sipIdentity->sipProxy.sipProxyPassword = command.getCommandString().getParam2();
			}
			
			++dialogState.seqNo;
			MRef<SipTransaction*> trans = new SipTransactionNonInviteClient(sipStack, this, dialogState.seqNo, dialogState.callId);
			registerTransaction(trans);
			send_noauth(trans->getBranch());
			
                        CommandString cmdstr(dialogState.callId, SipCommandString::register_sent);
			getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
		return true;
	}else{
		return false;
	}
}


bool SipDialogRegister::a13_failed_terminated_notransactions( const SipSMCommand &command){

	if (transitionMatch(command, SipCommandString::no_transactions)){
		SipSMCommand cmd(
				CommandString( dialogState.callId, SipCommandString::call_terminated),
				SipSMCommand::TU,
				SipSMCommand::DIALOGCONTAINER);
		getDialogContainer()->enqueueCommand( cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE );
		return true;
	}else{
		return false;
	}
}

void SipDialogRegister::setUpStateMachine(){
	
	State<SipSMCommand, string> *s0_start = new State<SipSMCommand,string>(this,"s0_start");
	addState(s0_start);
	
	State<SipSMCommand, string> *s1_tryingnoauth = new State<SipSMCommand,string>(this,"s1_tryingnoauth");
	addState(s1_tryingnoauth);
	
	State<SipSMCommand, string> *s2_registred = new State<SipSMCommand,string>(this,"s2_registred");
	addState(s2_registred);
	
	State<SipSMCommand, string> *s3_tryingstored = new State<SipSMCommand,string>(this,"s3_tryingstored");
	addState(s3_tryingstored);
	
	State<SipSMCommand, string> *s4_askpassword= new State<SipSMCommand,string>(this,"s4_askpassword");
	addState(s4_askpassword);
	
	State<SipSMCommand, string> *s5_failed= new State<SipSMCommand,string>(this,"s5_failed");
	addState(s5_failed);

	State<SipSMCommand, string> *terminated= new State<SipSMCommand,string>(this,"terminated");
	addState(terminated);
	

	new StateTransition<SipSMCommand,string>(this, "transition_start_tryingnoauth_register",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogRegister::a0_start_tryingnoauth_register,
			s0_start, s1_tryingnoauth);

	new StateTransition<SipSMCommand,string>(this, "transition_tryingnouath_registred_2xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogRegister::a1_tryingnoauth_registred_2xx,
			s1_tryingnoauth, s2_registred);

	new StateTransition<SipSMCommand,string>(this, "transition_tryingnouath_tryingstored_401haspass",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogRegister::a2_tryingnoauth_tryingstored_401haspass,
			s1_tryingnoauth, s3_tryingstored);

	new StateTransition<SipSMCommand,string>(this, "transition_tryingnouath_askpassword_401nopass",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogRegister::a3_tryingnoauth_askpassword_401nopass,
			s1_tryingnoauth, s4_askpassword);

	new StateTransition<SipSMCommand,string>(this, "transition_tryingstored_askpassword_401",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogRegister::a4_tryingstored_askpassword_401,
			s3_tryingstored, s4_askpassword);

	new StateTransition<SipSMCommand,string>(this, "transition_askpassword_askpassword_setpass",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogRegister::a5_askpassword_askpassword_setpassword,
			s4_askpassword, s4_askpassword);

	new StateTransition<SipSMCommand,string>(this, "transition_askpassword_registred_2xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogRegister::a6_askpassword_registred_2xx,
			s4_askpassword, s2_registred);

	new StateTransition<SipSMCommand,string>(this, "transition_askpassword_askpassword_401",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogRegister::a7_askpassword_askpassword_401,
			s4_askpassword, s4_askpassword);

	new StateTransition<SipSMCommand,string>(this, "transition_tryingstored_registred_2xx",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogRegister::a8_tryingstored_registred_2xx,
			s3_tryingstored, s2_registred);

	new StateTransition<SipSMCommand,string>(this, "transition_askpassword_failed_cancel",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogRegister::a9_askpassword_failed_cancel,
			s4_askpassword, s5_failed);

	new StateTransition<SipSMCommand,string>(this, "transition_tryingnoauth_failed_transporterror",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogRegister::a10_tryingnoauth_failed_transporterror,
			s1_tryingnoauth, s5_failed);

	new StateTransition<SipSMCommand,string>(this, "transition_tryingauth_failed_transporterror",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogRegister::a10_tryingnoauth_failed_transporterror,
			s3_tryingstored, s5_failed);

	new StateTransition<SipSMCommand,string>(this, "transition_registred_tryingnoauth_proxyregister",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogRegister::a12_registred_tryingnoauth_proxyregister,
			s2_registred, s1_tryingnoauth);

	new StateTransition<SipSMCommand,string>(this, "transition_failed_terminated_notransactions",
			(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogRegister::a13_failed_terminated_notransactions,
			s5_failed, terminated );

	setCurrentState(s0_start);
}

SipDialogRegister::SipDialogRegister(MRef<SipStack*> stack, MRef<SipDialogConfig*> callconf)
		: SipDialog(stack, callconf),
			realm(""),
			nonce(""),
			failCount(0),
			guiFeedback(true)
{
	setUpStateMachine();
	dialogState.callId = itoa(rand())+"@"+getDialogConfig()->inherited->localIpString;

	if (callconf->inherited->sipIdentity->sipDomain==""){

		uint32_t i=0;
		string uri = getDialogConfig()->inherited->sipIdentity->getSipUri();

		for ( ; uri[i]!='@' && i<uri.length(); i++)
			;
		assert(uri[i]=='@');
		i++;

		for ( ; i<uri.length(); i++)		//FIXME: Is this correct? (line below)
			callconf->inherited->sipIdentity->sipDomain=callconf->inherited->sipIdentity->sipDomain+uri[i];
	}
#ifdef DEBUG_OUTPUT
	mdbg << "SipDialogRegister::SipDialogRegister: DEBUG - domain set to "<< callconf->inherited->sipIdentity->sipDomain << end;
#endif

	myDomain = getDialogConfig()->inherited->sipIdentity->sipDomain;
}

SipDialogRegister::~SipDialogRegister(){
}

bool SipDialogRegister::hasPassword(){
	return getDialogConfig()->inherited->sipIdentity->sipProxy.sipProxyUsername!="" && getDialogConfig()->inherited->sipIdentity->sipProxy.sipProxyPassword!="";
}

bool SipDialogRegister::handleCommand(const SipSMCommand &command){

	if (command.getType()==SipSMCommand::COMMAND_PACKET 
			&& !(command.getDestination()==SipSMCommand::TU || command.getDestination()==SipSMCommand::ANY)){
		merr << "WARNING: UNEXPECTED: received packet in SipDialogRegister: "<<command.getCommandPacket()->getDescription() << end;
		return false;
	}

	if (command.getType()==SipSMCommand::COMMAND_STRING && 
			(command.getDestination()==SipSMCommand::TU || command.getDestination()==SipSMCommand::ANY) &&
			command.getCommandString().getOp()==SipCommandString::proxy_register &&
			(	   command.getCommandString()["proxy_domain"]=="" 
			 	|| command.getCommandString()["proxy_domain"]== getDialogConfig()->inherited->sipIdentity->sipDomain
			)){
		return SipDialog::handleCommand(command);
	}

	if (command.getType()==SipSMCommand::COMMAND_STRING
			&& (command.getDestination()==SipSMCommand::TU || command.getDestination()==SipSMCommand::ANY)){
		
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
	if (!ret && command.getType()==SipSMCommand::COMMAND_STRING && command.getCommandString().getOp()==SipCommandString::no_transactions){
		return true;
	}
	
	return ret;
}

void SipDialogRegister::send_noauth(string branch){
	
	int32_t localSipPort;

	if(getDialogConfig()->inherited->transport=="TCP")
		localSipPort = getDialogConfig()->inherited->localTcpPort;
	else if(getDialogConfig()->inherited->transport=="TLS")
		localSipPort = getDialogConfig()->inherited->localTlsPort;
	else{
//		localSipPort = getDialogConfig()->inherited.localUdpPort;
		localSipPort = getDialogConfig()->inherited->externalContactUdpPort;
        }

//	mdbg << "SipDialogRegister: domain is "<< proxy_domain<< end;
	MRef<SipRegister*> reg= new SipRegister(branch, /*getDialogConfig().callId*/ dialogState.callId,
			getDialogConfig()->inherited->sipIdentity->sipDomain, //proxy_domain,
//			getDialogConfig()->inherited.localIpString,
			getDialogConfig()->inherited->externalContactIP,
			localSipPort,
//			getDialogConfig()->inherited.userUri,
			getDialogConfig()->inherited->sipIdentity->getSipUri(),
			dialogState.seqNo,
			getDialogConfig()->inherited->transport);
        SipSMCommand cmd(MRef<SipMessage*>((SipMessage*)*reg), SipSMCommand::TU,SipSMCommand::transaction);
	sipStack->getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}

void SipDialogRegister::send_auth(string branch){
	int32_t localSipPort;

	if(getDialogConfig()->inherited->transport=="TCP")
		localSipPort = getDialogConfig()->inherited->localTcpPort;
	else if(getDialogConfig()->inherited->transport=="TLS")
		localSipPort = getDialogConfig()->inherited->localTlsPort;
	else{
		localSipPort = getDialogConfig()->inherited->externalContactUdpPort;
        }
	MRef<SipRegister*> reg=new SipRegister(branch, dialogState.callId, 
			getDialogConfig()->inherited->sipIdentity->sipDomain, //proxy_domain,
			getDialogConfig()->inherited->externalContactIP,
			localSipPort, 
			getDialogConfig()->inherited->sipIdentity->getSipUri(), 
			dialogState.seqNo, 
			getDialogConfig()->inherited->transport,
			getDialogConfig()->inherited->sipIdentity->sipProxy.sipProxyUsername, 
			realm, 
			nonce, 
			getDialogConfig()->inherited->sipIdentity->sipProxy.sipProxyPassword
		       );
        SipSMCommand cmd(MRef<SipMessage*>((SipMessage*)*reg), SipSMCommand::TU,SipSMCommand::transaction);
	sipStack->getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
}


