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
 * 	RtcpTransactionGetFloor.cxx
 * Author
 * 	Florian Maurer, florian.maurer@floHweb.ch
 * Purpose
 * 
*/

#include <config.h>

#include<assert.h>
#include<libminisip/signaling/p2t/RtcpTransactionGetFloor.h>
#include<libminisip/signaling/p2t/P2T.h>
#include<libmsip/SipTransactionUtils.h>
#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipDialog.h>
#include<libmutil/stringutils.h>
#include<libminisip/media/MediaHandler.h>


bool RtcpTransactionGetFloor::a0_start_reqsent( const SipSMCommand &command)
{
	if (transitionMatch(command, "p2tSendRequest")){
		
		//RtcpTransactionGetFloor *gf = (RtcpTransactionGetFloor *)sipStateMachine;
		/*gf->*/sendRequest();
		
		//start timerGRANT and timerRelTIMEOUT
		/*gf->*/requestTimeout(/*gf->*/tGrant, "timerGRANT");
		/*gf->*/requestTimeout(/*gf->*/tGetTimeout, "timerGetTIMEOUT");
		/*gf->*/requestTimeout(/*gf->*/tGetFloorTerminate, "timerGetFloorTERMINATE");
		
		return true;
	}
	else{
		return false;
	}
	}

bool RtcpTransactionGetFloor::a1_reqsent_reqsent( const SipSMCommand &command)
{
	if (transitionMatch(command, "timerGRANT")){
		//RtcpTransactionGetFloor *gf = (RtcpTransactionGetFloor *)sipStateMachine;
		
		//resend request
		/*gf->*/sendRequest();
		
		//set new timer value
		if(/*gf->*/counter<=5)
			/*gf->*/tGrant = 2 * /*gf->*/tGrant;
	
		/*gf->*/requestTimeout(/*gf->*/tGrant, "timerGRANT");
		/*gf->*/counter++;
		
		return true;
	}else{
		return false;
	}
}

bool RtcpTransactionGetFloor::a2_reqsent_completed_grant( const SipSMCommand &command)
{
	if (transitionMatch(command, "p2tGRANT")){
		
		//RtcpTransactionGetFloor *gf = (RtcpTransactionGetFloor *)sipStateMachine;
		
		merr<<"RtcpTransactionGetFloor:: a2_reqsent_completed_grant"<<end;
		merr<<"user="<</*gf->*/getUser()<<end;
		merr<<"p1="<<command.getCommandString().getParam()<<end;
		merr<<"p2="<<command.getCommandString().getParam2()<<end;
		merr<<"p3="<<command.getCommandString().getParam3()<<end;
			
		//set ssrc
		int ssrc=0;
		
		for(uint32_t k=0;k<command.getCommandString().getParam().size();k++) 
			ssrc = (ssrc*10) + (command.getCommandString().getParam()[k]-'0');
		
		if(/*gf->*/getDialogP2T()->getGroupList()->isParticipant(/*gf->*/getUser()))
			/*gf->*/getDialogP2T()->getGroupList()->getUser(/*gf->*/getUser())->setSSRC(ssrc);
		
		/*gf->*/setRemoteSSRC(ssrc);
		
		//stop timers
		/*gf->*/cancelTimeout("timerGRANT");	
		/*gf->*/cancelTimeout("timerGetTIMEOUT");
		
		//set status
		/*gf->*/reportStatus(P2T::STATUS_GRANT);
		
		//inform call that answer message was received
		string p3="";
		if(/*gf->*/getCollisionCounter()>0){
			p3=itoa(/*gf->*/getCollisionCounter());
		}
		else{
			p3=/*gf->*/getUser();
		}
		CommandString c(/*gf->*/ /*getDialog()->getCallId()*/ callId,"p2tRequestAnswered",
					itoa(/*gf->*/getRemoteSSRC()), 
					itoa(/*gf->*/getSeqNo()), 
					p3);
		SipSMCommand cmd(c, SipSMCommand::transaction, SipSMCommand::TU);
		/*gf->*/ /*getDialog()*/ dialog->getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
		
		return true;
	}else{
		return false;
	}
}

bool RtcpTransactionGetFloor::a3_reqsent_completed_request( const SipSMCommand &command)
{
	if (transitionMatch(command, "p2tREQUEST")){
		
		//RtcpTransactionGetFloor *gf = (RtcpTransactionGetFloor *)sipStateMachine;
				
		merr<<"RtcpTransactionGetFloor:: a3_reqsent_completed_request"<<end;
		merr<<"user="<</*gf->*/getUser()<<end;
		merr<<"p1="<<command.getCommandString().getParam()<<end;
		merr<<"p2="<<command.getCommandString().getParam2()<<end;
		merr<<"p3="<<command.getCommandString().getParam3()<<end;
			
		
		
		//stop timers
		/*gf->*/cancelTimeout("timerGRANT");	
		/*gf->*/cancelTimeout("timerGetTIMEOUT");	
		
		//set ssrc and store his sequence number
		//maybe we have to grant him later the floor.
		int ssrc=0;
		for(uint32_t k=0;k<command.getCommandString().getParam().size();k++) 
			ssrc = (ssrc*10) + (command.getCommandString().getParam()[k]-'0');
			
		int sNo=0;
		for(uint32_t x=0;x<command.getCommandString().getParam2().size();x++) 
			sNo = (sNo*10) + (command.getCommandString().getParam2()[x]-'0');
		
		
			
		if(/*gf->*/getDialogP2T()->getGroupList()->isParticipant(/*gf->*/getUser())){
			/*gf->*/getDialogP2T()->getGroupList()->getUser(/*gf->*/getUser())->setSSRC(ssrc);
			/*gf->*/getDialogP2T()->getGroupList()->getUser(/*gf->*/getUser())->setSeqNo(sNo);
			/*gf->*/setRemoteSSRC(ssrc);
		}
		else{
			merr<<"RtcpTransactionGetFloor:: a3"<<end;
			merr<<"User not found!"<<end;
		}
		

		//report collision
		/*gf->*/reportStatus(P2T::STATUS_COLLISION);
		
		//store his sequence number
		//maybe we have to grant him later the floor
		/*gf->*/getDialogP2T()->getGroupList()->getUser(/*gf->*/getRemoteSSRC())->setSeqNo(sNo);
		/*gf->*/setRemoteSeqNo(sNo);
		
		//inform call that answer message was received
		string p3="";
		if(/*gf->*/getCollisionCounter()>0){
			p3=itoa(/*gf->*/getCollisionCounter());
		}
		else{
			p3=/*gf->*/getUser();
		}
		CommandString c(/*gf->*/ /*getDialog()->getCallId()*/ callId,"p2tRequestAnswered",
					itoa(/*gf->*/getRemoteSSRC()), 
					itoa(/*gf->*/getSeqNo()), 
					p3);
		SipSMCommand cmd(c, SipSMCommand::transaction, SipSMCommand::TU);
		/*gf->*/ /*getDialog()*/ dialog->getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
		
		
		//for development reasons:
		//send again a request for ensure that the remote user
		//recognizes the collision
		/*gf->*/sendRequest();
		
		
		
		return true;
	}else{
		return false;
	}
} 

bool RtcpTransactionGetFloor::a4_reqsent_completed_timer( const SipSMCommand &command)
{ 
	if (transitionMatch(command, "timerGetTIMEOUT")){
		//RtcpTransactionGetFloor *gf = (RtcpTransactionGetFloor *)sipStateMachine;
		
		//stop timers
		/*gf->*/cancelTimeout("timerGRANT");
		
		//report 'not available'
		/*gf->*/reportStatus(P2T::STATUS_NOTAVAILABLE);
		
		//inform call that answer message was received
		string p3="";
		if(/*gf->*/getCollisionCounter()>0){
			p3=itoa(/*gf->*/getCollisionCounter());
		}
		else{
			p3=/*gf->*/getUser();
		}
		CommandString c(/*gf->*/ /*getDialog()->getCallId()*/ callId,"p2tRequestAnswered",
					itoa(/*gf->*/getRemoteSSRC()), 
					itoa(/*gf->*/getSeqNo()), 
					p3);
		SipSMCommand cmd(c, SipSMCommand::transaction, SipSMCommand::TU);
		/*gf->*/ /*getDialog()*/ dialog->getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
		
		return true;
	}else{
		return false;
	}
}

bool RtcpTransactionGetFloor::a5_completed_terminated( const SipSMCommand &command)
{
	if (transitionMatch(command, "timerGetFloorTERMINATE")){
		//RtcpTransactionGetFloor *gf = (RtcpTransactionGetFloor *)sipStateMachine;
		

		
		return true;
	}else{
		return false;
	}
}

bool RtcpTransactionGetFloor::a6_reqsent_terminated( const SipSMCommand &command)
{
	if (transitionMatch(command, "timerGetFloorTERMINATE")){
		//RtcpTransactionGetFloor *gf = (RtcpTransactionGetFloor *)sipStateMachine;
		
		//stop timers
		/*gf->*/cancelTimeout("timerGRANT");
		/*gf->*/cancelTimeout("timerGetTIMEOUT");
		
		//set status
		/*gf->*/reportStatus(P2T::STATUS_NOTAVAILABLE);
		
		//inform call that answer message was received
		string p3="";
		if(/*gf->*/getCollisionCounter()>0){
			p3=itoa(/*gf->*/getCollisionCounter());
		}
		else{
			p3=/*gf->*/getUser();
		}
		CommandString c(/*gf->*/ /*getDialog()->getCallId()*/ callId, "p2tRequestAnswered",
					itoa(/*gf->*/getRemoteSSRC()), 
					itoa(/*gf->*/getSeqNo()), 
					p3);
		SipSMCommand cmd(c, SipSMCommand::transaction, SipSMCommand::TU);
		/*gf->*/ /*getDialog()*/ dialog->getDialogContainer()->enqueueCommand(cmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
		
		
		
		return true;
	}else{
		return false;
	}
}

bool RtcpTransactionGetFloor::a7_completed_completed_grant( const SipSMCommand &command)
{
	if (transitionMatch(command, "p2tGRANT")){
		
		//do nothing
		
		return true;
	}else{
		return false;
	}
}

bool RtcpTransactionGetFloor::a8_completed_completed_request( const SipSMCommand &command)
{
	if (transitionMatch(command, "p2tREQUEST")){
		
		//RtcpTransactionGetFloor *gf = (RtcpTransactionGetFloor *)sipStateMachine;
		
		//check remote sequence number
		if(command.getCommandString().getParam2()!=itoa(/*gf->*/getRemoteSeqNo()))
			return false;
		
		//resen Floor REQUEST
		//if this transaction interacts in a collision simulation
		//with the user that sent first a REQUEST message
		//used for development purposes.
		if(/*gf->*/first_invitation){
			if(command.getCommandString().getParam3()==/*gf->*/getUser()){
				/*gf->*/sendRequest();
			}
			return true;
		}
		
		/*merr<<"----------------------------------------"<<end;
		merr<<"RtcpTransactionGetFloor:::a8"<<end;
		merr<<"user="<<gf->getUser()<<end;
		merr<<"cmd="<<command.getCommandString().getOp()<<end;
		merr<<"p1="<<command.getCommandString().getParam()<<end;
		merr<<"p2="<<command.getCommandString().getParam2()<<end;
		merr<<"p3="<<command.getCommandString().getParam3()<<end;
		merr<<"cf="<<itoa(gf->CollisionCounter)<<end;
		merr<<"-------------------------------------------"<<end;*/
		
		return true;
	}else{
		return false;
	}
}



void RtcpTransactionGetFloor::setUpStateMachine(){
	
	State<SipSMCommand,string> *s_start=new State<SipSMCommand,string>(this,"start");
	addState(s_start);

	State<SipSMCommand,string> *s_req_sent=new State<SipSMCommand,string>(this,"req_sent");
	addState(s_req_sent);

	State<SipSMCommand,string> *s_completed=new State<SipSMCommand,string>(this,"completed");
	addState(s_completed);
	
	State<SipSMCommand,string> *s_terminated=new State<SipSMCommand,string>(this,"terminated");
	addState(s_terminated);


//	StateTransition<SipSMCommand,string> *transition_start_reqsent=
		new StateTransition<SipSMCommand,string>(this,
				"transition_start_reqsent",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &RtcpTransactionGetFloor::a0_start_reqsent, 
				s_start, s_req_sent
				);
	
//	StateTransition<SipSMCommand,string> *transition_reqsent_reqsent=
		new StateTransition<SipSMCommand,string>(this,
				"transition_reqsent_reqsent",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &RtcpTransactionGetFloor::a1_reqsent_reqsent, 
				s_req_sent, s_req_sent
				);

//	StateTransition<SipSMCommand,string> *transition_reqsent_completed_grant=
		new StateTransition<SipSMCommand,string>(this,
				"transition_reqsent_completed_grant",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &RtcpTransactionGetFloor::a2_reqsent_completed_grant, 
				s_req_sent, s_completed
				);

//	StateTransition<SipSMCommand,string> *transition_reqsent_completed_request=
		new StateTransition<SipSMCommand,string>(this,
				"transition_reqsent_completed_request",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &RtcpTransactionGetFloor::a3_reqsent_completed_request, 
				s_req_sent, s_completed
				);
	
//	StateTransition<SipSMCommand,string> *transition_reqsent_completed_timer=
		new StateTransition<SipSMCommand,string>(this,
				"transition_reqsent_completed_timer",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &RtcpTransactionGetFloor::a4_reqsent_completed_timer, 
				s_req_sent, s_completed
				);
	
//	StateTransition<SipSMCommand,string> *transition_completed_terminated=
		new StateTransition<SipSMCommand,string>(this,
				"transition_completed_terminated",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &RtcpTransactionGetFloor::a5_completed_terminated, 
				s_completed, s_terminated
				);
				
//	StateTransition<SipSMCommand,string> *transition_reqsent_terminated=
		new StateTransition<SipSMCommand,string>(this,
				"transition_reqsent_terminated",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &RtcpTransactionGetFloor::a6_reqsent_terminated, 
				s_req_sent, s_terminated
				);
				
/*	StateTransition<SipSMCommand,string> *transition_completed_completed_grant=
		new StateTransition<SipSMCommand,string>(this,
				"transition_completed_completed_grant",
				a7_completed_completed_grant, 
				s_completed, s_completed
				);*/
	
//	StateTransition<SipSMCommand,string> *transition_completed_completed_request=
		new StateTransition<SipSMCommand,string>(this,
				"transition_completed_completed_request",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &RtcpTransactionGetFloor::a8_completed_completed_request, 
				s_completed, s_completed
				);
	
	setCurrentState(s_start);
}



RtcpTransactionGetFloor::RtcpTransactionGetFloor(MRef<SipDialog*> dialog, int seqNo, IPAddress *toaddr, int32_t port, string callId, unsigned remoteSSRC) : 
	SipTransaction(/*"RtcpTransactionGetFloor",*/dialog->getSipStack(), dialog,seqNo,"", callId),
		seqNo(seqNo),
		remoteSSRC(remoteSSRC)
{
	//this->toaddr = toaddr;
	this->port = port;
	
	counter=1;
	tGrant=P2T::timerGRANT;
	tGetTimeout=P2T::timerGetTIMEOUT;
	tGetFloorTerminate=P2T::timerGetFloorTERMINATE;
	
	CollisionCounter=0;
	remoteSeqNo=-1;
	
	first_invitation=false;
	setUpStateMachine();
}

RtcpTransactionGetFloor::~RtcpTransactionGetFloor(){
#ifdef DEBUG_OUTPUT
	merr<< "RtcpTransactionGetFloor::~RtcpTransactionGetFloor invoked"<< end;
#endif
}

bool RtcpTransactionGetFloor::handleCommand(const SipSMCommand &c){
	
	mdbg<<"RtcpTransactionGetFloor::handleCommand"<<end;
	
	
	//wrong destination
	if (c.getDestination()!=SipSMCommand::transaction && c.getDestination()!=SipSMCommand::ANY)
		return false;
	
	//CommandPacket instead of CommandString
	if (c.getType()==SipSMCommand::COMMAND_PACKET)
		return false;
	
	//check ssrc only if it's not a 'timer'-Command	
	if(c.getCommandString().getOp().substr(0,5) != "timer"){

		
	
		if(CollisionCounter==0){
			
			//check SIP URI	
			if(c.getCommandString().getParam3()!=user)
				return false;
		
		}
		else if(CollisionCounter>0) {
			//check SSRC
			if(c.getCommandString().getParam()!=itoa(remoteSSRC))
				return false;
			
			//check CollisionCounter in REQUEST messages
			if(c.getCommandString().getOp()=="p2tREQUEST"){
				if(c.getCommandString().getParam3()!= itoa(CollisionCounter))
					return false;
			}
			
		}
			
/*
		//check ssrc or SIP URI
		if (getRemoteSSRC()==0){
			if(c.getCommandString().getParam3()!=user)
				return false;	
		}
		else{
			if(c.getCommandString().getParam()!=itoa(remoteSSRC))
				return false;
		
		}
	
		//check CollisionCounter in REQUEST message
		if(c.getCommandString().getOp()=="p2tREQUEST"){
			if (CollisionCounter>0){
				if(c.getCommandString().getParam3()!= itoa(CollisionCounter))
					return false;
			}
			else if(CollisionCounter==0)
				
		}
*/
		//check seqno only if its not another REQUEST message
		if(c.getCommandString().getOp()!= "p2tREQUEST"){
			if(c.getCommandString().getParam2()!=itoa(seqNo))
				return false;	
		}
	}
			
	return StateMachine<SipSMCommand,string>::handleCommand(c);
}

void RtcpTransactionGetFloor::setUser(string user) {
	this->user=user;
}

MRef<SipDialogP2T*> RtcpTransactionGetFloor::getDialogP2T() {
//	MRef<SipDialog*> sipCall = getDialog();
	//MRef<SipDialogP2T*> sipCallP2T = MRef<SipDialogP2T*>((SipDialogP2T*)*sipCall);
	MRef<SipDialogP2T*> sipCallP2T = MRef<SipDialogP2T*>((SipDialogP2T*)*dialog);
	return sipCallP2T;
}

void RtcpTransactionGetFloor::sendRequest() {
		getDialogP2T()->getFloorControlSender()
			->send_APP_FC(P2T::APP_REQUEST, /*getDialog()*/dialog->getDialogConfig()->local_ssrc, 
				P2T::APP_NAME, toaddr, port, seqNo, CollisionCounter);
}			

void RtcpTransactionGetFloor::reportStatus(int status){
	if(getDialogP2T()->getGroupList()->isParticipant(getUser()))
		getDialogP2T()->getGroupList()->getUser(this->getUser())->setStatus(status);
	/*	
	mdbg<<"¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤"<<end;
	mdbg<<"RtcpTransactionGetFloor::"<<end;
	mdbg<<getDialogP2T()->getGroupList()->print_debug()<<end;
	mdbg<<"¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤"<<end;*/
}



                
