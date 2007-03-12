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
 * 	RtcpTransactionGrantFloor.cxx
 * Author
 * 	Florian Maurer, florian.maurer@floHweb.ch
 * Purpose
 * 
*/

#include <config.h>

#include<libminisip/signaling/p2t/RtcpTransactionGrantFloor.h>

#include<assert.h>

#include<libminisip/signaling/p2t/P2T.h>
#include<libmsip/SipTransactionUtils.h>
#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipDialog.h>
#include<libmutil/stringutils.h>


bool RtcpTransactionGrantFloor::a0_start_grantsent( const SipSMCommand &command)
{
	if (transitionMatch(command, "p2tSendGrant")){
		//RtcpTransactionGrantFloor *gf = (RtcpTransactionGrantFloor *)sipStateMachine;
		
		//start timer
		/*gf->*/requestTimeout(/*gf->*/tGrantFloorTerminate, "timerGrantFloorTERMINATE");
		/*gf->*/requestTimeout(/*gf->*/tTaken, "timerTAKEN");
		
		//send GRANT message
		/*gf->*/sendGrant();
		
		return true;
	}
	else{
		return false;
	}
}

bool RtcpTransactionGrantFloor::a1_grantsent_completed( const SipSMCommand &command)
{
	if (transitionMatch(command, "p2tTAKEN")){
		//RtcpTransactionGrantFloor *gf = (RtcpTransactionGrantFloor *)sipStateMachine;
		
		//inform P2T call about TAKEN message
		//CommandString cmd (gf->getCall()->getCallId(), "p2tFloorTaken", "username (not implemented)");
		
		/*gf->*/cancelTimeout("timerTAKEN");
		
		/*gf->*/reportStatus(P2T::STATUS_TALKING);
		
		CommandString cmd = command.getCommandString();
		cmd.setOp("p2tFloorTaken");
		
		SipSMCommand smcmd(cmd, SipSMCommand::transaction, SipSMCommand::TU);
		/*gf->*/ /*getDialog()*/ dialog->handleCommand(smcmd);	
		return true;
	}
	else{
		return false;
	}
}

bool RtcpTransactionGrantFloor::a2_grantsent_grantsent( const SipSMCommand &command)
{
	if (transitionMatch(command, "p2tREQUEST")){
		//RtcpTransactionGrantFloor *gf = (RtcpTransactionGrantFloor *)sipStateMachine;
		/*gf->*/sendGrant();
		return true;
	}
	else{
		return false;
	}
}

bool RtcpTransactionGrantFloor::a3_completed_terminated( const SipSMCommand &command)
{
	if (transitionMatch(command, "timerGrantFloorTERMINATE")){

		return true;
	}
	else{
		return false;
	}
}

bool RtcpTransactionGrantFloor::a4_grantsent_terminated( const SipSMCommand &command)
{
	if (transitionMatch(command, "timerGrantFloorTERMINATE")){
		//RtcpTransactionGrantFloor *gf = (RtcpTransactionGrantFloor *)sipStateMachine;
		/*gf->*/cancelTimeout("timerTAKEN");
		return true;
	}
	else{
		return false;
	}
}

bool RtcpTransactionGrantFloor::a5_grantsent_grantsent_timer( const SipSMCommand &command)
{
	if (transitionMatch(command, "timerTAKEN")){
		//RtcpTransactionGrantFloor *gf = (RtcpTransactionGrantFloor *)sipStateMachine;
		/*gf->*/sendGrant();
		
		//set new timer value
		if(/*gf->*/counter<=5)
			/*gf->*/tTaken = 2* /*gf->*/tTaken;
		
		/*gf->*/requestTimeout(/*gf->*/tTaken, "timerTAKEN");
		/*gf->*/counter++;
		
		return true;
	}
	else{
		return false;
	}
}

bool RtcpTransactionGrantFloor::a6_completed_completed( const SipSMCommand &command)
{
	if (transitionMatch(command, "p2tTAKEN")){

		//do nothing
		
		return true;
	}
	else{
		return false;
	}
}

void RtcpTransactionGrantFloor::setUpStateMachine(){
	
	State<SipSMCommand,string> *s_start=new State<SipSMCommand,string>(this,"start");
	addState(s_start);

	State<SipSMCommand,string> *s_grant_sent=new State<SipSMCommand,string>(this,"grant_sent");
	addState(s_grant_sent);

	State<SipSMCommand,string> *s_completed=new State<SipSMCommand,string>(this,"completed");
	addState(s_completed);

	State<SipSMCommand,string> *s_terminated=new State<SipSMCommand,string>(this,"terminated");
	addState(s_terminated);


//	StateTransition<SipSMCommand,string> *transition_start_grantsent=
		new StateTransition<SipSMCommand,string>(this,
				"transition_start_grantsent",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &RtcpTransactionGrantFloor::a0_start_grantsent, 
				s_start, s_grant_sent
				);
	
//	StateTransition<SipSMCommand,string> *transition_grantsent_completed=
		new StateTransition<SipSMCommand,string>(this,
				"transition_grantsent_completed",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &RtcpTransactionGrantFloor::a1_grantsent_completed, 
				s_grant_sent, s_completed
				);

//	StateTransition<SipSMCommand,string> *transition_grantsent_grantsent=
		new StateTransition<SipSMCommand,string>(this,
				"transition_grantsent_grantsent",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &RtcpTransactionGrantFloor::a2_grantsent_grantsent, 
				s_grant_sent, s_grant_sent
				);
				
//	StateTransition<SipSMCommand,string> *transition_completed_terminated=
		new StateTransition<SipSMCommand,string>(this,
				"transition_completed_terminated",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &RtcpTransactionGrantFloor::a3_completed_terminated, 
				s_completed, s_terminated
				);
				
//	StateTransition<SipSMCommand,string> *transition_grantsent_terminated=
		new StateTransition<SipSMCommand,string>(this,
				"transition_grantsent_terminated",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &RtcpTransactionGrantFloor::a4_grantsent_terminated, 
				s_grant_sent, s_terminated
				);
//	StateTransition<SipSMCommand,string> *transition_grantsent_grantsent_timer=
		new StateTransition<SipSMCommand,string>(this,
				"transition_grantsent_grantsent_timer",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &RtcpTransactionGrantFloor::a5_grantsent_grantsent_timer, 
				s_grant_sent, s_grant_sent
				);
				
//	StateTransition<SipSMCommand,string> *transition_completed_completed=
		new StateTransition<SipSMCommand,string>(this,
				"transition_completed_completed",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &RtcpTransactionGrantFloor::a6_completed_completed, 
				s_completed, s_completed
				);
	setCurrentState(s_start);
}



RtcpTransactionGrantFloor::RtcpTransactionGrantFloor(MRef<SipDialog*> dialog, int seqNo, IPAddress *toaddr, int32_t port, string callId,unsigned remoteSSRC) : 
	SipTransaction(/*"RtcpTransactionGrantFloor",*/dialog->getSipStack(), dialog, seqNo, "", callId),
		seqNo(seqNo),
		remoteSSRC(remoteSSRC)
{
	//set address and port
	this->toaddr = toaddr;
	this->port = port;
	
	//set Timer
	counter=1;
	CollisionCounter=0;
	
	tGrantFloorTerminate=P2T::timerGrantFloorTERMINATE;
	tTaken = P2T::timerTAKEN;
	//start state machine
	setUpStateMachine();
}

RtcpTransactionGrantFloor::~RtcpTransactionGrantFloor(){
#ifdef DEBUG_OUTPUT
	merr<< "RtcpTransactionGrantFloor::~RtcpTransactionGrantFloor invoked"<< end;
#endif
}

bool RtcpTransactionGrantFloor::handleCommand(const SipSMCommand &c){
	if (c.getDestination()!=SipSMCommand::transaction && c.getDestination()!=SipSMCommand::ANY)
		return false;
	

	
	//check only sequencenr if it's not a 'timer'-Command
	if(c.getCommandString().getOp().substr(0,5) != "timer") {
		
		//check ssrc
		if(c.getCommandString().getParam()!=itoa(remoteSSRC))
			return false;
		
		//check sequencenr
		if(c.getCommandString().getParam2()!=itoa(seqNo))
			return false;
	}
	

	
			
	return StateMachine<SipSMCommand,string>::handleCommand(c);
}

MRef<SipDialogP2T*> RtcpTransactionGrantFloor::getDialogP2T() {
//	MRef<SipDialog*> sipCall = getDialog();
//	MRef<SipDialogP2T*> sipCallP2T = MRef<SipDialogP2T*>((SipDialogP2T*)*sipCall);
	MRef<SipDialogP2T*> sipCallP2T = MRef<SipDialogP2T*>((SipDialogP2T*)*dialog);
	return sipCallP2T;
}

void RtcpTransactionGrantFloor::sendGrant() {

		getDialogP2T()->getFloorControlSender()
			->send_APP_FC(P2T::APP_GRANT, /*getDialog()*/ dialog->getDialogConfig()->local_ssrc, 
				P2T::APP_NAME, toaddr, port, seqNo, CollisionCounter);

}

void RtcpTransactionGrantFloor::reportStatus(int status){
	if(getDialogP2T()->getGroupList()->isParticipant(remoteSSRC))
		getDialogP2T()->getGroupList()->getUser(remoteSSRC)->setStatus(status);
}

