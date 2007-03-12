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

/* Name
 * 	RtcpTransactionIdleFloor.cxx
 * Author
 * 	Florian Maurer, florian.maurer@floHweb.ch
 * Purpose
 * 
*/

#include <config.h>

#include<libminisip/signaling/p2t/RtcpTransactionIdleFloor.h>

#include<assert.h>

#include<libminisip/signaling/p2t/P2T.h>
#include<libmsip/SipTransactionUtils.h>
#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipDialog.h>
#include<libmutil/stringutils.h>


bool RtcpTransactionIdleFloor::a0_start_idlesent(
		const SipSMCommand &command)
{
	if (transitionMatch(command, "p2tReleaseFloor")){
		
		//RtcpTransactionIdleFloor *gf = (RtcpTransactionIdleFloor *)sipStateMachine;
		/*gf->*/sendIdle();
		
		//start timerIDLE
		/*gf->*/requestTimeout(/*gf->*/tIdleFloorTerminate,"timerIdleFloorTERMINATE");
				
		return true;
	}
	else{
		return false;
	}
	}

bool RtcpTransactionIdleFloor::a1_idlesent_idlesent(
		const SipSMCommand &command)
{
	if (transitionMatch(command, "p2tRELEASE")){
		//RtcpTransactionIdleFloor *gf = (RtcpTransactionIdleFloor *)sipStateMachine;
		/*gf->*/sendIdle();
	
		return true;
	}else{
		return false;
	}
}

bool RtcpTransactionIdleFloor::a2_idlesent_terminated( const SipSMCommand &command)
{
	if (transitionMatch(command, "timerIdleFloorTERMINATE")){
		//RtcpTransactionIdleFloor *gf = (RtcpTransactionIdleFloor *)sipStateMachine;		
		return true;
	}else{
		return false;
	}
}



void RtcpTransactionIdleFloor::setUpStateMachine(){
	
	State<SipSMCommand,string> *s_start=new State<SipSMCommand,string>(this,"start");
	addState(s_start);

	State<SipSMCommand,string> *s_idle_sent=new State<SipSMCommand,string>(this,"idle_sent");
	addState(s_idle_sent);
	
	State<SipSMCommand,string> *s_terminated=new State<SipSMCommand,string>(this,"terminated");
	addState(s_terminated);


//	StateTransition<SipSMCommand,string> *transition_start_idlesent=
		new StateTransition<SipSMCommand,string>(this,
				"transition_start_idlesent",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &RtcpTransactionIdleFloor::a0_start_idlesent, 
				s_start, s_idle_sent
				);
	
//	StateTransition<SipSMCommand,string> *transition_idlesent_idlesent=
		new StateTransition<SipSMCommand,string>(this,
				"transition_idlesent_idlesent",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &RtcpTransactionIdleFloor::a1_idlesent_idlesent, 
				s_idle_sent, s_idle_sent
				);

//	StateTransition<SipSMCommand,string> *transition_idlesent_terminated=
		new StateTransition<SipSMCommand,string>(this,
				"transition_idlesent_terminated",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &RtcpTransactionIdleFloor::a2_idlesent_terminated, 
				s_idle_sent, s_terminated
				);


	setCurrentState(s_start);
}



RtcpTransactionIdleFloor::RtcpTransactionIdleFloor(MRef<SipDialog*> call, int seqNo, IPAddress *toaddr, int32_t port, string callid, unsigned remoteSSRC) : 
		SipTransaction(/*"RtcpTransactionIdleFloor",*/call->getSipStack(), call,seqNo,"", callid),
		seqNo(seqNo),
		remoteSSRC(remoteSSRC)
{
	//SipCallConfig * config = &getCall()->getCallConfig();
	//config->local_called=false;
	this->toaddr = toaddr;
	this->port = port;
	
	//initialize timers
	tIdleFloorTerminate=P2T::timerIdleFloorTERMINATE;
	
	
	setUpStateMachine();
}

RtcpTransactionIdleFloor::~RtcpTransactionIdleFloor(){
#ifdef DEBUG_OUTPUT
	merr<< "RtcpTransactionIdleFloor::~RtcpTransactionIdleFloor invoked"<< end;
#endif
}

bool RtcpTransactionIdleFloor::handleCommand(const SipSMCommand &c){
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

MRef<SipDialogP2T*> RtcpTransactionIdleFloor::getDialogP2T() {
//	MRef<SipDialog*> sipCall = getDialog();
//	MRef<SipDialogP2T*> sipCallP2T = MRef<SipDialogP2T*>((SipDialogP2T*)*sipCall);
	MRef<SipDialogP2T*> sipCallP2T = MRef<SipDialogP2T*>((SipDialogP2T*)*dialog);
	return sipCallP2T;
}

void RtcpTransactionIdleFloor::sendIdle() {

		getDialogP2T()->getFloorControlSender()
			->send_APP_FC(P2T::APP_IDLE, /*getDialog()*/dialog->getDialogConfig()->local_ssrc, 
				P2T::APP_NAME, toaddr, port,seqNo);
}



