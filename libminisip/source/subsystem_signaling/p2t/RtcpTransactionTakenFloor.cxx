/*
 Copyright (C) 2004-2006 the Minisip Team
 
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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include <config.h>

#include<libminisip/signaling/p2t/RtcpTransactionTakenFloor.h>

#include<assert.h>
#include<libminisip/signaling/p2t/P2T.h>
#include<libmsip/SipTransactionUtils.h>
#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipDialog.h>
#include<libmutil/stringutils.h>

bool RtcpTransactionTakenFloor::a0_start_takensent( const SipSMCommand &command)
{
	if (transitionMatch(command, "p2tSendTaken")){
		
		//RtcpTransactionTakenFloor *gf = (RtcpTransactionTakenFloor *)sipStateMachine;
		/*gf->*/sendTaken();
		
		//start timerIDLE
		/*gf->*/requestTimeout(/*gf->*/tTakenFloorTerminate,"timerTakenFloorTERMINATE");
				
		return true;
	}
	else{
		return false;
	}
	}

bool RtcpTransactionTakenFloor::a1_takensent_takensent(
		const SipSMCommand &command)
{
	if (transitionMatch(command, "p2tGRANT")){
		//RtcpTransactionTakenFloor *gf = (RtcpTransactionTakenFloor *)sipStateMachine;
		/*gf->*/sendTaken();
	
		return true;
	}else{
		return false;
	}
}

bool RtcpTransactionTakenFloor::a2_takensent_terminated(
		const SipSMCommand &command)
{
	if (transitionMatch(command, "timerTakenFloorTERMINATE")){
		//RtcpTransactionTakenFloor *gf = (RtcpTransactionTakenFloor *)sipStateMachine;		
		return true;
	}else{
		return false;
	}
}



void RtcpTransactionTakenFloor::setUpStateMachine(){
	
	State<SipSMCommand,string> *s_start=new State<SipSMCommand,string>(this,"start");
	addState(s_start);

	State<SipSMCommand,string> *s_taken_sent=new State<SipSMCommand,string>(this,"taken_sent");
	addState(s_taken_sent);
	
	State<SipSMCommand,string> *s_terminated=new State<SipSMCommand,string>(this,"terminated");
	addState(s_terminated);


//	StateTransition<SipSMCommand,string> *transition_start_takensent=
		new StateTransition<SipSMCommand,string>(this,
				"transition_start_takensent",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &RtcpTransactionTakenFloor::a0_start_takensent, 
				s_start, s_taken_sent
				);
	
//	StateTransition<SipSMCommand,string> *transition_takensent_takensent=
		new StateTransition<SipSMCommand,string>(this,
				"transition_takensent_takensent",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &RtcpTransactionTakenFloor::a1_takensent_takensent, 
				s_taken_sent, s_taken_sent
				);

//	StateTransition<SipSMCommand,string> *transition_takensent_terminated=
		new StateTransition<SipSMCommand,string>(this,
				"transition_takensent_terminated",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &RtcpTransactionTakenFloor::a2_takensent_terminated, 
				s_taken_sent, s_terminated
				);


	setCurrentState(s_start);
}



RtcpTransactionTakenFloor::RtcpTransactionTakenFloor(MRef<SipDialog*> call, int seqNo, IPAddress *toaddr, int32_t port, string callid, unsigned remoteSSRC) : 
		SipTransaction(/*"RtcpTransactionTakenFloor",*/call->getSipStack(), call,seqNo,"", callid),
		seqNo(seqNo),
		remoteSSRC(remoteSSRC)
{
	//SipCallConfig * config = &getCall()->getCallConfig();
	//config->local_called=false;
	this->toaddr = toaddr;
	this->port = port;
	
	//initialize timers
	tTakenFloorTerminate=P2T::timerTakenFloorTERMINATE;
	
	
	setUpStateMachine();
}

RtcpTransactionTakenFloor::~RtcpTransactionTakenFloor(){
#ifdef DEBUG_OUTPUT
	merr<< "RtcpTransactionTakenFloor::~RtcpTransactionTakenFloor invoked"<< end;
#endif
}

bool RtcpTransactionTakenFloor::handleCommand(const SipSMCommand &c){
	if (c.getDestination()!=SipSMCommand::transaction && c.getDestination()!=SipSMCommand::ANY)
		return false;
	
	

			
	//check only sequencenr if it's not a 'timer'-Command
	if(c.getCommandString().getOp().substr(0,5) != "timer") {
	
		if(c.getCommandString().getParam()!=itoa(remoteSSRC))
			return false;		
	
		//check sequencenr
		if(c.getCommandString().getParam2()!=itoa(seqNo))
			return false;
	}
			
	return StateMachine<SipSMCommand,string>::handleCommand(c);
}

MRef<SipDialogP2T*> RtcpTransactionTakenFloor::getDialogP2T() {
	MRef<SipDialog*> sipCall = /*getDialog()*/ dialog;
	MRef<SipDialogP2T*> sipCallP2T = MRef<SipDialogP2T*>((SipDialogP2T*)*sipCall);
	return sipCallP2T;
}

void RtcpTransactionTakenFloor::sendTaken() {

		getDialogP2T()->getFloorControlSender()
			->send_APP_FC(P2T::APP_TAKEN, /*getDialog()*/dialog->getDialogConfig()->local_ssrc, 
				P2T::APP_NAME, toaddr, port,seqNo);
}



