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
 * 	RtcpTransactionReleaseFloor.cxx
 * Author
 * 	Florian Maurer, florian.maurer@floHweb.ch
 * Purpose
 * 
*/

#include <config.h>

#include<libminisip/signaling/p2t/RtcpTransactionReleaseFloor.h>

#include<assert.h>

#include<libminisip/signaling/p2t/P2T.h>
#include<libmsip/SipTransactionUtils.h>
#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipDialog.h>
#include<libmutil/stringutils.h>
//#include"DefaultCallHandler.h"


bool RtcpTransactionReleaseFloor::a0_start_relsent( const SipSMCommand &command)
{
	if (transitionMatch(command, "p2tReleaseFloor")){
		
		//RtcpTransactionReleaseFloor *gf = (RtcpTransactionReleaseFloor *)sipStateMachine;
		/*gf->*/sendRelease();
		
		//start timerIDLE
		/*gf->*/requestTimeout(/*gf->*/tIdle, "timerIDLE");
		/*gf->*/requestTimeout(/*gf->*/tRelTimeout,"timerRelTIMEOUT");
		/*gf->*/requestTimeout(/*gf->*/tRelFloorTerminate,"timerRelFloorTERMINATE");
				
		return true;
	}
	else{
		return false;
	}
	}

bool RtcpTransactionReleaseFloor::a1_relsent_relsent( const SipSMCommand &command)
{
	if (transitionMatch(command, "timerIDLE")){
		//RtcpTransactionReleaseFloor *gf = (RtcpTransactionReleaseFloor *)sipStateMachine;
		/*gf->*/sendRelease();
		
		if(/*gf->*/counter<=5)
			/*gf->*/tIdle=2* /*gf->*/tIdle;
	
		/*gf->*/requestTimeout(/*gf->*/tIdle, "timerIDLE");
		/*gf->*/counter++;
		
		return true;
	}else{
		return false;
	}
}

bool RtcpTransactionReleaseFloor::a2_relsent_completed_idle( const SipSMCommand &command)
{
	if (transitionMatch(command, "p2tIDLE")){
		//RtcpTransactionReleaseFloor *gf = (RtcpTransactionReleaseFloor *)sipStateMachine;		

		//cancel Timers
		/*gf->*/cancelTimeout("timerRelTIMEOUT");
		/*gf->*/cancelTimeout("timerIDLE");
		
		//set status
		/*gf->*/reportStatus(P2T::STATUS_RELEASED);
		
		//inform dialog about received IDLE message
		CommandString c(/*gf->*/ /*getDialog()->getCallId()*/ callId,"p2tFloorReleased");
		SipSMCommand cmd(c, SipSMCommand::transaction, SipSMCommand::TU);
		/*gf->*/ /*getDialog()*/ dialog->handleCommand(cmd);
		
		return true;
	}else{
		return false;
	}
}

bool RtcpTransactionReleaseFloor::a3_relsent_completed_timer( const SipSMCommand &command)
{
	if (transitionMatch(command, "timerRelTIMEOUT")){

		//RtcpTransactionReleaseFloor *gf = (RtcpTransactionReleaseFloor *)sipStateMachine;
		/*gf->*/cancelTimeout("timerIDLE");
		
		//set status
		/*gf->*/reportStatus(P2T::STATUS_NOTAVAILABLE);
		
		//inform dialog
		CommandString c(/*gf->*/ /*getDialog()->getCallId()*/ callId,"p2tFloorReleased");
		SipSMCommand cmd(c, SipSMCommand::transaction, SipSMCommand::TU);
		/*gf->getDialog()*/ dialog->handleCommand(cmd);
		
		
		return true;
	}else{
		return false;
	}
}

bool RtcpTransactionReleaseFloor::a4_completed_terminated( const SipSMCommand &command)
{
	if (transitionMatch(command, "timerRelFloorTERMINATE")){

		//RtcpTransactionReleaseFloor *gf = (RtcpTransactionReleaseFloor *)sipStateMachine;
		/*gf->*/cancelTimeout("timerIDLE");
		return true;
	}else{
		return false;
	}
}

bool RtcpTransactionReleaseFloor::a5_relsent_terminated( const SipSMCommand &command)
{
	if (transitionMatch(command, "timerRelFloorTERMINATE")){

		//RtcpTransactionReleaseFloor *gf = (RtcpTransactionReleaseFloor *)sipStateMachine;
		/*gf->*/cancelTimeout("timerIDLE");
		
		//set status
		/*gf->*/reportStatus(P2T::STATUS_NOTAVAILABLE);
		
		//inform dialog
		CommandString c(/*gf->getDialog()*/dialog->getCallId(),"p2tFloorReleased");
		SipSMCommand cmd(c, SipSMCommand::transaction, SipSMCommand::TU);
		/*gf->getDialog()*/dialog->handleCommand(cmd);
		
		return true;
	}else{
		return false;
	}
}

bool RtcpTransactionReleaseFloor::a6_completed_completed( const SipSMCommand &command)
{
	if (transitionMatch(command, "p2tIDLE")){

		//do nothing
		
		return true;
	}else{
		return false;
	}
}

void RtcpTransactionReleaseFloor::setUpStateMachine(){
	
	State<SipSMCommand,string> *s_start=new State<SipSMCommand,string>(this,"start");
	addState(s_start);

	State<SipSMCommand,string> *s_rel_sent=new State<SipSMCommand,string>(this,"rel_sent");
	addState(s_rel_sent);

	State<SipSMCommand,string> *s_completed=new State<SipSMCommand,string>(this,"completed");
	addState(s_completed);
	
	State<SipSMCommand,string> *s_terminated=new State<SipSMCommand,string>(this,"terminated");
	addState(s_terminated);


//	StateTransition<SipSMCommand,string> *transition_start_relsent=
		new StateTransition<SipSMCommand,string>(this,
				"transition_start_relsent",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &RtcpTransactionReleaseFloor::a0_start_relsent, 
				s_start, s_rel_sent
				);
	
//	StateTransition<SipSMCommand,string> *transition_relsent_relsent=
		new StateTransition<SipSMCommand,string>(this,
				"transition_reqsent_reqsent",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &RtcpTransactionReleaseFloor::a1_relsent_relsent, 
				s_rel_sent, s_rel_sent
				);

//	StateTransition<SipSMCommand,string> *transition_relsent_completed_idle=
		new StateTransition<SipSMCommand,string>(this,
				"transition_relsent_completed_idle",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &RtcpTransactionReleaseFloor::a2_relsent_completed_idle, 
				s_rel_sent, s_completed
				);

//	StateTransition<SipSMCommand,string> *transition_relsent_completed_timer=
		new StateTransition<SipSMCommand,string>(this,
				"transition_relsent_completed_timer",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &RtcpTransactionReleaseFloor::a3_relsent_completed_timer, 
				s_rel_sent, s_completed
				);
				
//	StateTransition<SipSMCommand,string> *transition_completed_terminated=
		new StateTransition<SipSMCommand,string>(this,
				"transition_completed_terminated",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &RtcpTransactionReleaseFloor::a4_completed_terminated, 
				s_completed, s_terminated
				);
				
//	StateTransition<SipSMCommand,string> *transition_relsent_terminated=
		new StateTransition<SipSMCommand,string>(this,
				"transition_relsent_terminated",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &RtcpTransactionReleaseFloor::a5_relsent_terminated, 
				s_rel_sent, s_terminated
				);
	
//	StateTransition<SipSMCommand,string> *transition_completed_completed=
		new StateTransition<SipSMCommand,string>(this,
				"transition_completed_completed",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &RtcpTransactionReleaseFloor::a6_completed_completed, 
				s_completed, s_completed
				);
	
	setCurrentState(s_start);
}



RtcpTransactionReleaseFloor::RtcpTransactionReleaseFloor(MRef<SipDialog*> call, int seqNo, IPAddress *toaddr, int32_t port, string callid, unsigned remoteSSRC) : 
		SipTransaction(/*"RtcpTransactionReleaseFloor",*/call->getSipStack(), call,seqNo,"", callid),
		seqNo(seqNo),
		remoteSSRC(remoteSSRC)
{
	//SipCallConfig * config = &getCall()->getCallConfig();
	//config->local_called=false;
	this->toaddr = toaddr;
	this->port = port;
	
	//initialize timers
	counter=1;
	tIdle=P2T::timerIDLE;
	tRelTimeout=P2T::timerRelTIMEOUT;
	tRelFloorTerminate=P2T::timerRelFloorTERMINATE;
	
	
	setUpStateMachine();
}

RtcpTransactionReleaseFloor::~RtcpTransactionReleaseFloor(){
#ifdef DEBUG_OUTPUT
	merr<< "RtcpTransactionReleaseFloor::~RtcpTransactionReleaseFloor invoked"<< end;
#endif
}

bool RtcpTransactionReleaseFloor::handleCommand(const SipSMCommand &c){
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

MRef<SipDialogP2T*> RtcpTransactionReleaseFloor::getDialogP2T() {
	MRef<SipDialog*> sipCall = /*getDialog()*/ dialog;
	MRef<SipDialogP2T*> sipCallP2T = MRef<SipDialogP2T*>((SipDialogP2T*)*sipCall);
	return sipCallP2T;
}

void RtcpTransactionReleaseFloor::sendRelease() {

		getDialogP2T()->getFloorControlSender()
			->send_APP_FC(P2T::APP_RELEASE, /*getDialog()*/ dialog->getDialogConfig()->local_ssrc, P2T::APP_NAME, toaddr, port,seqNo);
}

void RtcpTransactionReleaseFloor::reportStatus(int status){
	if(getDialogP2T()->getGroupList()->isParticipant(getSSRC()))
		getDialogP2T()->getGroupList()->getUser(getSSRC())->setStatus(status);
}

                
