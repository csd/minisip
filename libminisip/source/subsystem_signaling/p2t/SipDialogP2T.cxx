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

#include<assert.h>
#include<time.h>
#include<libminisip/signaling/p2t/SipDialogP2T.h>
#include<libmsip/SipDialogContainer.h>
//#include<libmsip/SipMessageTransport.h>
#include<libmsip/SipTransactionUtils.h>
#include<libmsip/SipCommandString.h>
#include<libminisip/signaling/p2t/RtcpTransactionGetFloor.h>
#include<libminisip/signaling/p2t/RtcpTransactionGrantFloor.h>
#include<libminisip/signaling/p2t/RtcpTransactionReleaseFloor.h>
#include<libminisip/signaling/p2t/RtcpTransactionIdleFloor.h>
#include<libminisip/signaling/p2t/RtcpTransactionTakenFloor.h>
#include<libmsip/SipDialog.h>
#include<libminisip/signaling/sip/DefaultDialogHandler.h>
#include<libmutil/stringutils.h>
#include<sstream>


#include<libmutil/Timestamp.h>
#include<libmutil/termmanip.h>
#include<libmutil/dbg.h>
#include<libmsip/SipSMCommand.h>


bool SipDialogP2T::a0_idle_talkreq( const SipSMCommand &command){
		
	//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;	
	
	if (transitionMatch(command, "p2tGetFloor")){
		
		//start RtcpTransactionGetFloor
		
		//time measurement
#ifndef _MSC_VER
		if(/*vc->*/p2tPerformance){
			ts.start();
		}
#endif
		//increment sequence number
		/*vc->*/dialogState.seqNo++;

		//start RtcpTransactionGetFloor
		for(uint32_t k=0;k</*vc->*/getGroupList()->getAllUser().size();k++){
			
			//filter out own user
			//if(vc->getGroupList()->getAllUser().at(k)->getUri()==vc->getDialogConfig().inherited->userUri)
			if(/*vc->*/getGroupList()->getAllUser()[k]->getUri()==/*vc->*/getDialogConfig()->inherited->sipIdentity->getSipUri())
				continue;
				
			//filter out NOTAVAILABLE users
			if(/*vc->*/getGroupList()->getAllUser()[k]->getStatus()==P2T::STATUS_NOTAVAILABLE)
				continue;
			
			//start transaction
			MRef<SipTransaction*> gf = new RtcpTransactionGetFloor(MRef<SipDialog *>(/* *vc*/ this), 
				/*vc->*/dialogState.seqNo, 
				/*vc->*/getGroupList()->getAllUser()[k]->getAddress(), 
				/*vc->*/getGroupList()->getAllUser()[k]->getRTCPport(), dialogState.callId);
			
			
			MRef<RtcpTransactionGetFloor*>gf_casted = MRef<RtcpTransactionGetFloor*>((RtcpTransactionGetFloor*)*gf);
			
			gf_casted->setUser(/*vc->*/getGroupList()->getAllUser()[k]->getUri());
			/*vc->*/registerTransaction(gf);
		
			CommandString cmd(/*vc->*/getCallId(),"p2tSendRequest",
				itoa(/*vc->*/getGroupList()->getAllUser()[k]->getSSRC()), 
				itoa(/*vc->*/dialogState.seqNo), 
				/*vc->*/getGroupList()->getAllUser()[k]->getUri());
			
			SipSMCommand scmd(cmd, SipSMCommand::TU, SipSMCommand::transaction);
			/*vc->*/getDialogContainer()->enqueueCommand(scmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);

			
		}
	
		return true;
	}else{
		return false;
	}
}

bool SipDialogP2T::a1_talkreq_talk( const SipSMCommand &command){
	
	//all users have granted OR not all users have granted but local user is the only with highest prio
	if (transitionMatch(command, "p2tRequestAnswered")){
	
		//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;
		vector<string> users;

		if(/*vc->*/allAnswered()==false)
			return false;
		
		//merr<<"SipDialogP2T::a1"<<end;
		//merr<<"checkStates="<<vc->checkStates(P2T::STATUS_GRANT)<<end;
		//merr<<"Prio="<<vc->highestPrio(P2T::STATUS_COLLISION, users)<<end;
		//merr<<"empty="<<users.empty()<<end;
		
		
		if(/*vc->*/checkStates(P2T::STATUS_GRANT) ==false){
			if(/*vc->*/highestPrio(P2T::STATUS_COLLISION, users)==false)
			 	return false;
			else if(users.empty()==false)
				return false;	
		}
		
		//time measurements
		if(/*vc->*/p2tPerformance){
#ifndef _MSC_VER
			ts.stop();
			merr<<"used time:"<<ts.writeElapsedTime("Push-2-Talk delay")<<end;
#endif
		}
		
		//send Floor TAKEN to all
		for(uint32_t k=0;k</*vc->*/getGroupList()->getAllUser().size();k++){
			//filter out own user
			//if(vc->getGroupList()->getAllUser().at(k)->getUri()==vc->getDialogConfig().inherited->userUri)
			if(/*vc->*/getGroupList()->getAllUser()[k]->getUri()==/*vc->*/getDialogConfig()->inherited->sipIdentity->getSipUri())
				continue;
				
			//filter out NOTAVAILABLE users
			if(/*vc->*/getGroupList()->getAllUser()[k]->getStatus()==P2T::STATUS_NOTAVAILABLE)
				continue;
			
			//start RtcpTransactionTakenFloor
			MRef<SipTransaction*> gf = new 
				RtcpTransactionTakenFloor(MRef<SipDialog*>(/* *vc */ this), 
					/*vc->*/dialogState.seqNo, 
					/*vc->*/getGroupList()->getAllUser()[k]->getAddress(), 
					/*vc->*/getGroupList()->getAllUser()[k]->getRTCPport(), 
					dialogState.callId,
					/*vc->*/getGroupList()->getAllUser()[k]->getSSRC());
			/*vc->*/registerTransaction(gf);

					
			CommandString cmd(/*vc->*/getCallId(), "p2tSendTaken", 
				itoa(/*vc->*/getGroupList()->getAllUser()[k]->getSSRC()),
				itoa(/*vc->*/dialogState.seqNo));
			SipSMCommand scmd(cmd, SipSMCommand::TU, SipSMCommand::transaction);
			/*vc->*/getDialogContainer()->enqueueCommand(scmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);

		}
				
		//inform GUI
		CommandString cmdstr(/*vc->*/getCallId(), "p2tFloorGranted");
		/*vc->*/getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
		 
		/******
		 Start SoundSender
		 *******/
	

		//vc->getSoundSender()->start();

		 
		return true;
	}else{
		return false;
	}
}

bool SipDialogP2T::a2_talk_releasepend( const SipSMCommand &command){

	if (transitionMatch(command, "p2tReleaseFloor")){
		//start RtcpTransactionReleaseFloor
		//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;
		
		//start RtcpTransactionReleaseFloor
		for(uint32_t k=0;k</*vc->*/getGroupList()->getAllUser().size();k++){
			//filter out own user
			//if(vc->getGroupList()->getAllUser().at(k)->getUri()==vc->getDialogConfig().inherited->userUri)
			if(/*vc->*/getGroupList()->getAllUser()[k]->getUri()==/*vc->*/getDialogConfig()->inherited->sipIdentity->getSipUri())
				continue;
				
			//filter out NOTAVAILABLE users
			if(/*vc->*/getGroupList()->getAllUser()[k]->getStatus()==P2T::STATUS_NOTAVAILABLE)
				continue;
			
			//set status back for being able to check if the user
			//answered the RELEASE message
			/*vc->*/getGroupList()->getAllUser()[k]->setStatus(P2T::STATUS_CONNECTED);
				
			//start transaction
			MRef<SipTransaction*> rf = new RtcpTransactionReleaseFloor(MRef<SipDialog *>(/* *vc */ this), 
				/*vc->*/dialogState.seqNo, 
				/*vc->*/getGroupList()->getAllUser()[k]->getAddress(), 
				/*vc->*/getGroupList()->getAllUser()[k]->getRTCPport(),
				dialogState.callId,
				/*vc->*/getGroupList()->getAllUser()[k]->getSSRC());
	
			/*vc->*/registerTransaction(rf);
		
			CommandString cmd(/*vc->*/getCallId(),"p2tReleaseFloor",
				itoa(/*vc->*/getGroupList()->getAllUser()[k]->getSSRC()), 
				itoa(/*vc->*/dialogState.seqNo), 
				/*vc->*/getGroupList()->getAllUser()[k]->getUri());
			
			SipSMCommand scmd(cmd, SipSMCommand::TU, SipSMCommand::transaction);
			/*vc->*/getDialogContainer()->enqueueCommand(scmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
		}
		
		//vc->getSoundSender()->stop();
		
		return true;
	}else{
		return false;
	}
}

bool SipDialogP2T::a3_releasepend_idle( const SipSMCommand &command){

	if (transitionMatch(command, "p2tFloorReleased")){
		
		//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;
		
		//check if all has answered RELEASE message
		if(/*vc->*/allAnswered()==false)
			return false;
		
		//inform GUI
		CommandString cmd(/*vc->*/getCallId(),"p2tFloorReleased", command.getCommandString().getParam());
		/*vc->*/getDialogContainer()->getCallback()->sipcb_handleCommand( cmd );
		
		//reset states
		/*vc->*/setStates(P2T::STATUS_CONNECTED);
		
		
		return true;
	}else{
		return false;
	}
}

bool SipDialogP2T::a4_idle_listenreq( const SipSMCommand &command){
	
	//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;
	
	if (transitionMatch(command, "p2tREQUEST") && /*vc->*/p2tCollisioner==false){
		//start RtcpTransactionGrantFloor
		


		//uri
		string uri = command.getCommandString().getParam3();
		//seqNo
		string p2 = command.getCommandString().getParam2();
		//ssrc
		string p = command.getCommandString().getParam();
		
		int sNo=0;
		int ssrc=0;
		
		for(uint32_t x=0;x<p2.size();x++) 
			sNo = (sNo*10) + (p2[x]-'0');
		
		for(uint32_t k=0;k<p.size();k++) 
			ssrc = (ssrc*10) + (p[k]-'0');
		
		//check if user is participating in the session
		if (/*vc->*/getGroupList()->isParticipant(uri)){
			MRef<GroupListUserElement*> ue = /*vc->*/getGroupList()->getUser(uri);
			ue->setSSRC(ssrc);
			ue->setSeqNo(sNo);
			ue->setStatus(P2T::STATUS_REQUESTING);
		
			MRef<SipTransaction*> gf = new 
				RtcpTransactionGrantFloor(MRef<SipDialog*>(/* *vc */ this), sNo, ue->getAddress(), ue->getRTCPport(), dialogState.callId, ssrc);				
			/*vc->*/registerTransaction(gf);
		
			CommandString cmd(/*vc->*/getCallId(), "p2tSendGrant", itoa(ssrc), itoa(sNo), uri);
			SipSMCommand scmd(cmd, SipSMCommand::TU, SipSMCommand::transaction);
			/*vc->*/getDialogContainer()->enqueueCommand(scmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
		}
		else{
			//unknown user
			//stay in IDLE
			return false;
		}
		return true;
	}else{
		return false;
	}
}

bool SipDialogP2T::a5_listenreq_listen( const SipSMCommand &command){

	if (transitionMatch(command, "p2tFloorTaken")){
		//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;
		
		//time measurements
#ifndef _MSC_VER
		if(/*vc->*/p2tPerformance){
			ts.stop();
			merr<<"Used time:"<<ts.writeElapsedTime("Push-2-Talk delay")<<end;
		}
#endif
		
		/******
		 inform GUI
		 *******/
		string user="";
		int ssrc=0;
		
		for(uint32_t k=0;k<command.getCommandString().getParam().size();k++) 
			ssrc = (ssrc*10) + (command.getCommandString().getParam()[k]-'0');
		
		//get user and set STATUS_TALKING in the Group Member List
		if(/*vc->*/getGroupList()->isParticipant(ssrc)){
			user = /*vc->*/getGroupList()->getUser(ssrc)->getUri();
			/*vc->*/getGroupList()->getUser(ssrc)->setStatus(P2T::STATUS_TALKING);
		
			//inform GUI
			CommandString cmd(/*vc->*/getCallId(),"p2tFloorTaken", user);
			/*vc->*/getDialogContainer()->getCallback()->sipcb_handleCommand( cmd );
			
			
			//start timerRevoke
			/*vc->*/counterRevoke=0;
			if(/*vc->*/getGroupList()->getMaxFloorTime()>0)
				/*vc->*/requestTimeout(/*vc->*/getGroupList()->getMaxFloorTime()*1000, "timerREVOKE");
				
		
			//start SoundReceiver
			//vc->getSoundReceiver()->initCrypto();
			//vc->getSoundReceiver()->start();
			//vc->getSoundReceiver()->flush();
			//vc->getSoundReceiver()->registerSoundSource(-2); //FIXME: 
		
			return true;
		}
		else {
			merr<<"SipDialogP2T:: Error in a5, user that sent TAKEN message was not in GroupList"<<end;
			return false;
		}
		 
		return true;
	}else{
		return false;
	}
}

bool SipDialogP2T::a6_listen_idle( const SipSMCommand &command){

	if (transitionMatch(command, "p2tRELEASE")){
		//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;
		
		//find talking user
		int talk_ssrc = 0;
		uint32_t k;
		for(k=0;k</*vc->*/getGroupList()->getAllUser().size();k++){
			if(/*vc->*/getGroupList()->getAllUser()[k]->getStatus()==P2T::STATUS_TALKING)
				talk_ssrc = /*vc->*/getGroupList()->getAllUser()[k]->getSSRC();
		}
		
		//start transaction
		string p = command.getCommandString().getParam();
		int ssrc=0;
		for(k=0;k<p.size();k++) 
			ssrc = (ssrc*10) + (p[k]-'0');		
		
		//int sNo=0;
		//string p2 = command.getCommandString().getParam2();
		//for(int x=0;x<p2.size();x++) 
		//	sNo = (sNo*10) + (p2[x]-'0');
			
		//check if user is participating in the session
		if (ssrc==talk_ssrc){
			MRef<GroupListUserElement*> ue = /*vc->*/getGroupList()->getUser(ssrc);
	
			MRef<SipTransaction*> gf = new 
				RtcpTransactionIdleFloor(MRef<SipDialog*>(/* *vc */ this), ue->getSeqNo(), ue->getAddress(), ue->getRTCPport(), dialogState.callId, ssrc);
			/*vc->*/registerTransaction(gf);
		

		
			CommandString cmd(/*vc->*/getCallId(), "p2tReleaseFloor", itoa(ssrc), itoa(ue->getSeqNo()));
			SipSMCommand scmd(cmd, SipSMCommand::TU, SipSMCommand::transaction);
			/*vc->*/getDialogContainer()->enqueueCommand(scmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
		}
		//unknown user.
		else{
			//Wrong user sent RELEASE
			return false;
		}
		
		//cancel REVOKE timer
		/*vc->*/cancelTimeout("timerREVOKE");
				
		//inform GUI
		CommandString cmd(/*vc->*/getCallId(),string("p2tFloorReleased"), command.getCommandString().getParam());
		/*vc->*/getDialogContainer()->getCallback()->sipcb_handleCommand( cmd );
		
		//reset states
		/*vc->*/setStates(P2T::STATUS_CONNECTED);
		
		//stop listening
		//vc->getSoundReceiver()->stop();
		
		
		
		
		return true;
	}else{
		return false;
	}
}

bool SipDialogP2T::a7_talkreq_listenreq( const SipSMCommand &command){
	

	
	//Not all users have granted and local user hasn't highest prio.
	if (transitionMatch(command, "p2tRequestAnswered")){
		
		//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;
		vector<string> users;	
		
		//mdbg<<"SipDialogP2T::a7"<<end;
		//mdbg<<"allAnswered="<<vc->allAnswered()<<end;

		
		
		if(/*vc->*/allAnswered()==false)
			return false;
	
		//mdbg<<"SipDialogP2T:: a7"<<end;
		//mdbg<<"checkStates="<<vc->checkStates(P2T::STATUS_GRANT)<<end;
		//mdbg<<"highestPrio="<<vc->highestPrio(P2T::STATUS_COLLISION, users)<<end;
		//mdbg<<"users.empty="<<users.empty()<<end;
			
			
		if(/*vc->*/checkStates(P2T::STATUS_GRANT)==true
			|| /*vc->*/highestPrio(P2T::STATUS_COLLISION, users)==true){
			return false;
		}
		
		
		//start for all users with highest priority a RtcpTransactionGrantFloor.
		//to one of them finally a TAKEN message will arrive.
		if(users.size()==0){
			merr<<"SipDialogP2T:: in a7: Collision but no user found!"<<end;
			return true;	
		
		}
		
		for(uint32_t k=0;k<users.size();k++){
				
		MRef<SipTransaction*> gf = new RtcpTransactionGrantFloor(MRef<SipDialog*>(/* *vc */ this), 
				/*vc->*/getGroupList()->getUser(users[k])->getSeqNo(), 
				/*vc->*/getGroupList()->getUser(users[k])->getAddress(), 
				/*vc->*/getGroupList()->getUser(users[k])->getRTCPport(), 
				dialogState.callId,
				/*vc->*/getGroupList()->getUser(users[k])->getSSRC());
			
			/*vc->*/registerTransaction(gf);
					
			CommandString cmd(/*vc->*/getCallId(), "p2tSendGrant", 
				itoa(/*vc->*/getGroupList()->getUser(users[k])->getSSRC()), 
				itoa(/*vc->*/getGroupList()->getUser(users[k])->getSeqNo()), 
				users[k]);
			SipSMCommand scmd(cmd, SipSMCommand::TU, SipSMCommand::transaction);
			/*vc->*/getDialogContainer()->enqueueCommand(scmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
		}
		
		
		return true;
	}else{
		return false;
	}
}


bool SipDialogP2T::a8_talkreq_collision( const SipSMCommand &command){
	
	//not all users have granted, there are other users with the same priority	
	if (transitionMatch(command, "p2tRequestAnswered")){
		//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;
		vector<string> users;	
		
		if(/*vc->*/allAnswered()==false)
			return false;
		
		/*mdbg<<"---------------------"<<end;
		mdbg<<"SipDialogP2T:: a8"<<end;
		mdbg<<"checkStates="<<vc->checkStates(P2T::STATUS_GRANT)<<end;
		mdbg<<"highestPrio="<<vc->highestPrio(P2T::STATUS_COLLISION, users)<<end;
		mdbg<<"users.empty="<<users.empty()<<end;	
		mdbg<<"ssrc="<<command.getCommandString().getParam()<<end;
		mdbg<<"seqno="<<command.getCommandString().getParam2()<<end;
		mdbg<<"user="<<command.getCommandString().getParam3()<<end;
		mdbg<<"--------------------"<<end;*/
			
		//check if it is a collision
		if(/*vc->*/checkStates(P2T::STATUS_GRANT)==true
			|| /*vc->*/highestPrio(P2T::STATUS_COLLISION, users)==false
			|| users.empty()==true){
			return false;
		}

		//set counterCollision
		/*vc->*/counterCollision=1;
				
		//set resend timer
		int timer_value = int(P2T::timerRESEND * rand()/(RAND_MAX+1.0));
		merr<<"---------------------"<<end;
		merr<<"SipDialogP2T:: a8 timer_value="<<itoa(timer_value)<<end;
		merr<<"---------------------"<<end;
		
		/*if(vc->getDialogConfig().inherited->userUri=="floh2@ssvl.kth.se")
			vc->requestTimeout(10000, "timerRESEND");
		else
			vc->requestTimeout(60000, "timerRESEND");*/
		/*vc->*/requestTimeout(timer_value, "timerRESEND");
			
	
		return true;
	}else{
		return false;
	}
}

bool SipDialogP2T::a9_collision_listenreq( const SipSMCommand &command){

	if (transitionMatch(command, "p2tREQUEST") ){
		
		//start RtcpTransactionGrantFloor
		//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;
		
		
			
		//check collision flag
		if (command.getCommandString().getParam3()!=itoa(/*vc->*/counterCollision))
			return false;
		
		
		//collision counter
		string p3 = command.getCommandString().getParam3();
		//seqNo
		string p2 = command.getCommandString().getParam2();
		//ssrc
		string p = command.getCommandString().getParam();
		
		int sNo=0;
		int ssrc=0;
		int cc=0;
		
		for(uint32_t k=0;k<p3.size();k++) 
			cc = (cc*10) + (p3[k]-'0');
			
		for(uint32_t x=0;x<p2.size();x++) 
			sNo = (sNo*10) + (p2[x]-'0');
		
		for(uint32_t l=0;l<p.size();l++) 
			ssrc = (ssrc*10) + (p[l]-'0');
		
		
		//check if user is participating in the session 
		if (/*vc->*/getGroupList()->isParticipant(ssrc)){

			
			MRef<GroupListUserElement*> ue = /*vc->*/getGroupList()->getUser(ssrc);

			
			//check seqNr and status. Should not be a user that has granted the floor
			//before the collision.
			if(ue->getSeqNo()==sNo && ue->getStatus()==P2T::STATUS_COLLISION){
			
				MRef<SipTransaction*> gf = new 
					RtcpTransactionGrantFloor(MRef<SipDialog*>(/* *vc */ this), sNo, ue->getAddress(), ue->getRTCPport(), dialogState.callId, ssrc);
				
				//set collision counter value for the Floor GRANT message
				MRef<RtcpTransactionGrantFloor*>gf_casted = MRef<RtcpTransactionGrantFloor*>((RtcpTransactionGrantFloor*)*gf);
				
				gf_casted->setCollisionCounter(cc);
				/*vc->*/registerTransaction(gf);
				
				ue->setStatus(P2T::STATUS_REQUESTING);
				
				CommandString cmd(/*vc->*/getCallId(), "p2tSendGrant", itoa(ssrc), itoa(sNo));
				SipSMCommand scmd(cmd, SipSMCommand::TU, SipSMCommand::transaction);
				/*vc->*/getDialogContainer()->enqueueCommand(scmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
			}
		}
		else{
			//not valid user
			//stay in collision
			return false;
		}
		
		return true;
	}else{
		return false;
	}
}

bool SipDialogP2T::a10_collision_resent_timer( const SipSMCommand &command){

	if (transitionMatch(command, "timerRESEND")){
		
		//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;
	
		//vc->getDialogConfig().seqNo++;
		merr<<"------------------------"<<end;
		merr<<"a10: send REQUEST "<<end;
		merr<<"----------------------"<<end;
		
		//start RtcpTransactionGetFloor
		for(uint32_t k=0;k</*vc->*/getGroupList()->getAllUser().size();k++){
				
			//take only COLLISIONED users
			if(/*vc->*/getGroupList()->getAllUser()[k]->getStatus()==P2T::STATUS_COLLISION){
				
				//reset the state
				/*vc->*/getGroupList()->getAllUser()[k]->setStatus(P2T::STATUS_CONNECTED);
				
				
				//start transaction
				MRef<SipTransaction*> gf = new RtcpTransactionGetFloor(MRef<SipDialog *>(/* *vc*/ this), 
					/*vc->*/dialogState.seqNo, 
					/*vc->*/getGroupList()->getAllUser()[k]->getAddress(), 
					/*vc->*/getGroupList()->getAllUser()[k]->getRTCPport(),
					dialogState.callId,
					/*vc->*/getGroupList()->getAllUser()[k]->getSSRC());
				
				merr<<"Collision with "<< /*vc->*/getGroupList()->getAllUser()[k]->getUri()<<end;
			
				MRef<RtcpTransactionGetFloor*>gf_casted = MRef<RtcpTransactionGetFloor*>((RtcpTransactionGetFloor*)*gf);
			
				gf_casted->setUser(/*vc->*/getGroupList()->getAllUser()[k]->getUri());
				gf_casted->setCollisionCounter(/*vc->*/counterCollision);

				/*vc->*/registerTransaction(gf);
		
				CommandString cmd(/*vc->*/getCallId(),"p2tSendRequest",
					itoa(/*vc->*/getGroupList()->getAllUser()[k]->getSSRC()), 
					itoa(/*vc->*/dialogState.seqNo), 
					/*vc->*/getGroupList()->getAllUser()[k]->getUri());
			
				SipSMCommand scmd(cmd, SipSMCommand::TU, SipSMCommand::transaction);
				/*vc->*/getDialogContainer()->enqueueCommand(scmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
			}
		}
	
		return true;
	}else{
		return false;
	}
}

bool SipDialogP2T::a11_resent_collision_collision( const SipSMCommand &command){
	
	//not all users have granted, there are other users with the same priority	
	if (transitionMatch(command, "p2tRequestAnswered")){
	
		//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;
		
		//check CollisionCounter
		if(command.getCommandString().getParam3()!=itoa(/*vc->*/counterCollision))
			return false;
		
		if(/*vc->*/allAnswered()==false)
			return false;
			
		//check if it is a collision
		if(/*vc->*/checkStates(P2T::STATUS_GRANT)==true){
			return false;
		}
		
		//increase counterCollison
		/*vc->*/counterCollision++;
		
		//set resend timer
		int timer_value = int(P2T::timerRESEND * rand()/(RAND_MAX+1.0));
		/*vc->*/requestTimeout(timer_value, "timerRESEND");
		
		merr<<"---------------------"<<end;
		merr<<"SipDialogP2T:: a11 COLLISION again timer_value="<<itoa(timer_value)<<end;
		//merr<<"ssrc="<<command.getCommandString().getParam()<<end;
		//merr<<"seqno="<<command.getCommandString().getParam2()<<end;
		//merr<<"user="<<command.getCommandString().getParam3()<<end;		
		merr<<"---------------------"<<end;
		
		return true;
	}else{
		return false;
	}
}

bool SipDialogP2T::a12_resent_talk( const SipSMCommand &command){

	
	if (transitionMatch(command, "p2tRequestAnswered")){
	
		//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;
		vector<string> users;
		
		if(/*vc->*/allAnswered()==false){
			return false;
		}
				
		if(/*vc->*/checkStates(P2T::STATUS_GRANT) ==false){
			return false;
		}
		
		/*merr<<"---------------------"<<end;
		merr<<"SipDialogP2T:: a12 talk"<<end;
		merr<<"ssrc="<<command.getCommandString().getParam()<<end;
		merr<<"seqno="<<command.getCommandString().getParam2()<<end;
		merr<<"user="<<command.getCommandString().getParam3()<<end;		
		merr<<"---------------------"<<end;*/
		
		
		
		//time measurements
#ifndef _MSC_VER
		if(/*vc->*/p2tPerformance){
			ts.stop();
			merr<<"used time:"<<ts.writeElapsedTime("Push-2-Talk delay")<<end;
		}
#endif
		
		
		//send Floor TAKEN to all
		for(uint32_t k=0;k</*vc->*/getGroupList()->getAllUser().size();k++){
			//filter out own user
			//if(vc->getGroupList()->getAllUser().at(k)->getUri()==vc->getDialogConfig().inherited->userUri)
			if(/*vc->*/getGroupList()->getAllUser()[k]->getUri()==/*vc->*/getDialogConfig()->inherited->sipIdentity->getSipUri())
				continue;
				
			//filter out NOTAVAILABLE users
			if(/*vc->*/getGroupList()->getAllUser()[k]->getStatus()==P2T::STATUS_NOTAVAILABLE)
				continue;
			
			//start RtcpTransactionTakenFloor
			MRef<SipTransaction*> gf = new 
				RtcpTransactionTakenFloor(MRef<SipDialog*>(/* *vc */ this), 
					/*vc->*/dialogState.seqNo, 
					/*vc->*/getGroupList()->getAllUser()[k]->getAddress(), 
					/*vc->*/getGroupList()->getAllUser()[k]->getRTCPport(), 
					dialogState.callId,
					/*vc->*/getGroupList()->getAllUser()[k]->getSSRC());
			/*vc->*/registerTransaction(gf);

					
			CommandString cmd(/*vc->*/getCallId(), "p2tSendTaken", 
				itoa(/*vc->*/getGroupList()->getAllUser()[k]->getSSRC()),
				itoa(/*vc->*/dialogState.seqNo));
			SipSMCommand scmd(cmd, SipSMCommand::TU, SipSMCommand::transaction);
			/*vc->*/getDialogContainer()->enqueueCommand(scmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
		}
				
		//inform GUI
		CommandString cmdstr(/*vc->*/getCallId(), "p2tFloorGranted");
		/*vc->*/getDialogContainer()->getCallback()->sipcb_handleCommand( cmdstr );
		 
		/******
		 Start SoundSender
		 *******/

		//vc->getSoundSender()->initCrypto();
		//vc->getSoundSender()->start();
		
		
		return true;
	}else{
		return false;
	}
}


bool SipDialogP2T::a13_resent_resent( const SipSMCommand &command){

	if (transitionMatch(command, "p2tRequestAnswered")){
		//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;

		if(/*vc->*/allAnswered()==true){
			return false;
		}
		
		
		merr<<"---------------------"<<end;
		merr<<"SipDialogP2T:: a13 wait until all users has sent their response"<<end;
		merr<<"ssrc="<<command.getCommandString().getParam()<<end;
		merr<<"seqno="<<command.getCommandString().getParam2()<<end;
		merr<<"cf="<<command.getCommandString().getParam3()<<end;		
		merr<<"---------------------"<<end;
		
		
		//wait until all users has sent their responses
	
		return true;
	}else{
		return false;
	}
}




bool SipDialogP2T::a14_listen_listen( const SipSMCommand &command){
	
	//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;
	if (transitionMatch(command, "timerREVOKE") && /*vc->*/counterRevoke<2 ){
		
		//find talking user
		string uri = "";
		for(uint32_t k=0;k</*vc->*/getGroupList()->getAllUser().size();k++){
			if(/*vc->*/getGroupList()->getAllUser()[k]->getStatus()==P2T::STATUS_TALKING)
				uri = /*vc->*/getGroupList()->getAllUser()[k]->getUri();
		}
		
		int warningCode=0;
		
		//send first warning
		if(/*vc->*/counterRevoke==0){
			warningCode=1;
			
			//inform GUI
			CommandString cmd2(/*vc->*/getCallId(),string("p2tFloorRevokeActive"));
			/*vc->*/getDialogContainer()->getCallback()->sipcb_handleCommand( cmd2 );
				
		}
		//send second warning
		else if(/*vc->*/counterRevoke==1){
			warningCode=2;
		}
		
		/*vc->*/counterRevoke++;
		/*vc->*/requestTimeout(P2T::timerREVOKE, "timerREVOKE");
		
		/*vc->*/getFloorControlSender()
			->send_APP_FC(P2T::APP_REVOKE, /*vc->*/getDialogConfig()->local_ssrc, 
				P2T::APP_NAME, 
				/*vc->*/getGroupList()->getUser(uri)->getAddress(),
				/*vc->*/getGroupList()->getUser(uri)->getRTCPport(), 
				/*vc->*/getGroupList()->getUser(uri)->getSeqNo(),
				warningCode);	
	
		return true;
	}else{
		return false;
	}
}

bool SipDialogP2T::a15_talkreq_talkreq( const SipSMCommand &command){

	
	if (transitionMatch(command, "p2tRequestAnswered")){
		//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;

		if(/*vc->*/allAnswered()==true){
			return false;
		}
		
		mdbg<<"---------------------"<<end;
		mdbg<<"SipDialogP2T:: a15 wait until all users has sent their response"<<end;
		mdbg<<"ssrc="<<command.getCommandString().getParam()<<end;
		mdbg<<"seqno="<<command.getCommandString().getParam2()<<end;
		mdbg<<"user="<<command.getCommandString().getParam3()<<end;		
		mdbg<<"---------------------"<<end;
		
		
		//wait until all users has sent their responses
	
		return true;
	}else{
		return false;
	}
}

bool SipDialogP2T::a16_releasepend_releasepend( const SipSMCommand &command){

	//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;
	if (transitionMatch(command, "p2tFloorReleased") && /*vc->*/checkStates(P2T::STATUS_RELEASED)==false ){
		
		//check if all has answered RELEASE message
		if(/*vc->*/allAnswered()==true)
			return false;
		
		//wait until all users has sent their responses
	
		return true;
	}else{
		return false;
	}
}

bool SipDialogP2T::a17_listen_idle_revoke( const SipSMCommand &command){

	//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;
	if (transitionMatch(command, "timerREVOKE") && /*vc->*/counterRevoke>=2 ){
	
		//find talking user
		string uri = "";
		for(uint32_t k=0;k</*vc->*/getGroupList()->getAllUser().size();k++){
			if(/*vc->*/getGroupList()->getAllUser()[k]->getStatus()==P2T::STATUS_TALKING)
				uri = /*vc->*/getGroupList()->getAllUser()[k]->getUri();
		}
		
		if(uri==""){
			merr<<"SipDialogP2T:: Error in a17. User didn't exist."<<end;
			return false;
		}
		
		//send Revoke message
		/*vc->*/getFloorControlSender()
			->send_APP_FC(P2T::APP_REVOKE, /*vc->*/getDialogConfig()->local_ssrc, 
				P2T::APP_NAME, 
				/*vc->*/getGroupList()->getUser(uri)->getAddress(),
				/*vc->*/getGroupList()->getUser(uri)->getRTCPport(), 
				/*vc->*/getGroupList()->getUser(uri)->getSeqNo(),
				3);	
		
		//start RtcpTransactionIdleFloor
		MRef<GroupListUserElement*> ue = /*vc->*/getGroupList()->getUser(uri);
	
		MRef<SipTransaction*> gf = new	RtcpTransactionIdleFloor(MRef<SipDialog*>(/* *vc */ this), 
				ue->getSeqNo(), 
				ue->getAddress(), 
				ue->getRTCPport(), 
				dialogState.callId,
				ue->getSSRC());
		/*vc->*/registerTransaction(gf);
		
		CommandString cmd(/*vc->*/getCallId(), "p2tReleaseFloor", itoa(ue->getSSRC()), itoa(ue->getSeqNo()));
		SipSMCommand scmd(cmd, SipSMCommand::TU, SipSMCommand::transaction);
		/*vc->*/getDialogContainer()->enqueueCommand(scmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);

		//inform GUI
		CommandString cmd2(/*vc->*/getCallId(),string("p2tFloorReleased"));
		/*vc->*/getDialogContainer()->getCallback()->sipcb_handleCommand( cmd2 );
		
		//reset states
		/*vc->*/setStates(P2T::STATUS_CONNECTED);
		
		//stop listening
		//vc->getSoundReceiver()->stop();

		
		return true;
	}else{
		return false;
	}
}

bool SipDialogP2T::a18_talk_talk_revoke( const SipSMCommand &command){

	if (transitionMatch(command, "p2tREVOKE")){
		
		//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;
		
		//check sequence number
		int sNo=0;
		uint32_t x;
		for(x=0;x<command.getCommandString().getParam2().size();x++) 
			sNo = (sNo*10) + (command.getCommandString().getParam2()[x]-'0');
		
		int ssrc=0;
		for(x=0;x<command.getCommandString().getParam().size();x++) 
			ssrc = (ssrc*10) + (command.getCommandString().getParam()[x]-'0');
			
		if(sNo==/*vc->*/dialogState.seqNo){
			//inform GUI
			CommandString cmd2(/*vc->*/getCallId(),string("p2tFloorRevokePassiv"),
			/*vc->*/getGroupList()->getUser(ssrc)->getUri(),
			command.getCommandString().getParam3());
			/*vc->*/getDialogContainer()->getCallback()->sipcb_handleCommand( cmd2 );
		}
		return true;
	}else{
		return false;
	}
}


bool SipDialogP2T::a19_listenreq_listenreq( const SipSMCommand &command){

	if (transitionMatch(command, "p2tREQUEST")){
		//start RtcpTransactionGrantFloor
		//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;

		//uri
		string uri = command.getCommandString().getParam3();
		//seqNo
		string p2 = command.getCommandString().getParam2();
		//ssrc
		string p = command.getCommandString().getParam();
		int ssrc=0;

		for(uint32_t k=0;k<p.size(); k++){
			ssrc = (ssrc*10)+(p[k]-'0');
		}
		
		mdbg<<"---------------------------------"<<end;
		mdbg<<"SipDialogP2T:: a19_listenreq_listenreq"<<end;
		mdbg<<"ssrc="<<itoa(ssrc)<<end;
		//mdbg<<"status="<<itoa(vc->getGroupList()->getUser(ssrc)->getStatus())<<end;
		mdbg<<"REQUESTING="<<P2T::STATUS_REQUESTING<<end;
		mdbg<<"----------------------------------"<<end;
		
		
		int sNo=0;
		
		
		for(uint32_t x=0;x<p2.size();x++) 
			sNo = (sNo*10) + (p2[x]-'0');
		
		//check if user is participating in the session
		if (/*vc->*/getGroupList()->isParticipant(uri)){
			
			//check if it is a new user that is requesting
			if(/*vc->*/getGroupList()->getUser(uri)->getStatus()==P2T::STATUS_REQUESTING)
				return false;
				
			MRef<GroupListUserElement*> ue = /*vc->*/getGroupList()->getUser(uri);
			ue->setSSRC(ssrc);
			ue->setSeqNo(sNo);
			ue->setStatus(P2T::STATUS_REQUESTING);
			
			MRef<SipTransaction*> gf = new 
				RtcpTransactionGrantFloor(MRef<SipDialog*>(/* *vc */), sNo, ue->getAddress(), ue->getRTCPport(), dialogState.callId, ssrc);
			/*vc->*/registerTransaction(gf);
		
			CommandString cmd(/*vc->*/getCallId(), "p2tSendGrant", itoa(ssrc), itoa(sNo), uri);
			SipSMCommand scmd(cmd, SipSMCommand::TU, SipSMCommand::transaction);
			/*vc->*/getDialogContainer()->enqueueCommand(scmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
			
	}
		else if (/*vc->*/getGroupList()->isParticipant(ssrc)){
			
			//check if it is a new user that is requesting
			if(/*vc->*/getGroupList()->getUser(ssrc)->getStatus()==P2T::STATUS_REQUESTING)
				return false;
			
			MRef<GroupListUserElement*> ue = /*vc->*/getGroupList()->getUser(ssrc);
			ue->setSSRC(ssrc);
			ue->setSeqNo(sNo);
			ue->setStatus(P2T::STATUS_REQUESTING);
			
			MRef<SipTransaction*> gf = new 
				RtcpTransactionGrantFloor(MRef<SipDialog*>(/* *vc */ this), sNo, ue->getAddress(), ue->getRTCPport(), dialogState.callId, ssrc);
			/*vc->*/registerTransaction(gf);
		
			CommandString cmd(/*vc->*/getCallId(), "p2tSendGrant", itoa(ssrc), itoa(sNo), uri);
			SipSMCommand scmd(cmd, SipSMCommand::TU, SipSMCommand::transaction);
			/*vc->*/getDialogContainer()->enqueueCommand(scmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
	

		}
		
		else{
			//unknown user
			//stay in IDLE
			return false;
		}
		return true;
	}else{
		return false;
	}
}

bool SipDialogP2T::a80_idle_terminated( const SipSMCommand &command){

	if (transitionMatch(command, "p2tTerminate")){
		//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;
		/*vc->*/terminateSession();
		return true;
	}else{
		return false;
	}
}

bool SipDialogP2T::a81_talkreq_terminated( const SipSMCommand &command){

	if (transitionMatch(command, "p2tTerminate")){
		//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;
		/*vc->*/terminateSession();
		return true;
	}else{
		return false;
	}
}

bool SipDialogP2T::a82_listenreq_terminated( const SipSMCommand &command){

	if (transitionMatch(command, "p2tTerminate")){
		//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;
		/*vc->*/terminateSession();
		return true;
	}else{
		return false;
	}
}

bool SipDialogP2T::a83_talk_terminated( const SipSMCommand &command){

	if (transitionMatch(command, "p2tTerminate")){

		//start RtcpTransactionReleaseFloor
		//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;
		
		//start RtcpTransactionReleaseFloor
		for(uint32_t k=0;k</*vc->*/getGroupList()->getAllUser().size();k++){
			//filter out own user
			//if(vc->getGroupList()->getAllUser().at(k)->getUri()==vc->getDialogConfig().inherited->userUri)
			if(/*vc->*/getGroupList()->getAllUser()[k]->getUri()==/*vc->*/getDialogConfig()->inherited->sipIdentity->getSipUri())
				continue;
				
			//filter out NOTAVAILABLE users
			if(/*vc->*/getGroupList()->getAllUser()[k]->getStatus()==P2T::STATUS_NOTAVAILABLE)
				continue;
			
			//start transaction
			MRef<SipTransaction*> rf = new RtcpTransactionReleaseFloor(MRef<SipDialog *>(/* *vc */ this), 
				/*vc->*/dialogState.seqNo, 
				/*vc->*/getGroupList()->getAllUser()[k]->getAddress(), 
				/*vc->*/getGroupList()->getAllUser()[k]->getRTCPport(),
				dialogState.callId,
				/*vc->*/getGroupList()->getAllUser()[k]->getSSRC());
	
			/*vc->*/registerTransaction(rf);
		
			CommandString cmd(/*vc->*/getCallId(),"p2tReleaseFloor",
				itoa(/*vc->*/getGroupList()->getAllUser()[k]->getSSRC()), 
				itoa(/*vc->*/dialogState.seqNo), 
				/*vc->*/getGroupList()->getAllUser()[k]->getUri());
			
			SipSMCommand scmd(cmd, SipSMCommand::TU, SipSMCommand::transaction);
			/*vc->*/getDialogContainer()->enqueueCommand(scmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
		}
		
		//terminate Session
		/*vc->*/terminateSession();
		return true;
	}else{
		return false;
	}
}

bool SipDialogP2T::a84_collision_terminated( const SipSMCommand &command){

	if (transitionMatch(command, "p2tTerminate")){
		//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;
		/*vc->*/terminateSession();
		return true;
	}else{
		return false;
	}
}

bool SipDialogP2T::a85_listen_terminated( const SipSMCommand &command){

	if (transitionMatch(command, "p2tTerminate")){
		//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;
		/*vc->*/terminateSession();
		return true;
	}else{
		return false;
	}
}

bool SipDialogP2T::a86_releasepend_terminated( const SipSMCommand &command){

	if (transitionMatch(command, "p2tTerminate")){
		//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;
		/*vc->*/terminateSession();
		return true;
	}else{
		return false;
	}
}

bool SipDialogP2T::a87_resent_terminated( const SipSMCommand &command){

	if (transitionMatch(command, "p2tTerminate")){
		//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;
		/*vc->*/terminateSession();
		return true;
	}else{
		return false;
	}
}

bool SipDialogP2T::a97_idle_talkreq_collisioner( const SipSMCommand &command){
		
	//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;	
	
	if (transitionMatch(command, "p2tREQUEST") && /*vc->*/p2tCollisioner){
		
		//start RtcpTransactionGetFloor
		

		//time measurement
#ifndef _MSC_VER
		if(/*vc->*/p2tPerformance){
			ts.start();
		}
#endif		

		//increment sequence number
		/*vc->*/dialogState.seqNo++;

		//start RtcpTransactionGetFloor
		for(uint32_t k=0;k</*vc->*/getGroupList()->getAllUser().size();k++){
			
			//filter out own user
			//if(vc->getGroupList()->getAllUser().at(k)->getUri()==vc->getDialogConfig().inherited->userUri)
			if(/*vc->*/getGroupList()->getAllUser()[k]->getUri()==/*vc->*/getDialogConfig()->inherited->sipIdentity->getSipUri())
				continue;
				
			//filter out NOTAVAILABLE users
			if(/*vc->*/getGroupList()->getAllUser()[k]->getStatus()==P2T::STATUS_NOTAVAILABLE)
				continue;
			
			//start transaction
			MRef<SipTransaction*> gf = new RtcpTransactionGetFloor(MRef<SipDialog *>(/* *vc */ this), 
				/*vc->*/dialogState.seqNo, 
				/*vc->*/getGroupList()->getAllUser()[k]->getAddress(), 
				/*vc->*/getGroupList()->getAllUser()[k]->getRTCPport(), dialogState.callId);
			
			
			MRef<RtcpTransactionGetFloor*>gf_casted = MRef<RtcpTransactionGetFloor*>((RtcpTransactionGetFloor*)*gf);
			
			gf_casted->setUser(/*vc->*/ getGroupList()->getAllUser()[k]->getUri());
			
			
			if(/*vc->*/getGroupList()->getAllUser()[k]->getUri()==command.getCommandString().getParam3()){
				gf_casted->first_invitation=true;
			}
						
			/*vc->*/registerTransaction(gf);
		
			CommandString cmd(/*vc->*/getCallId(),"p2tSendRequest",
				itoa(/*vc->*/getGroupList()->getAllUser()[k]->getSSRC()), 
				itoa(/*vc->*/dialogState.seqNo), 
				/*vc->*/getGroupList()->getAllUser()[k]->getUri());
			
			SipSMCommand scmd(cmd, SipSMCommand::TU, SipSMCommand::transaction);
			/*vc->*/getDialogContainer()->enqueueCommand(scmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);

			
		}

		//send received Floor REQUEST message to transaction
		/*vc->*/getDialogContainer()->enqueueCommand(command, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
		
		return true;
	}else{
		return false;
	}
}


bool SipDialogP2T::a98_idle_idle_performance( const SipSMCommand &command){

	if (transitionMatch(command, "p2tPerformance")){
		//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;
		/*vc->*/p2tPerformance=true;
		string init_data;
		init_data = "Timer Values[ms]\r\n";
		
		ostringstream Str;
		Str<<P2T::timerRESEND;
		
		init_data +=  "timerRESEND;" + Str.str() + "\r\n";
		init_data += "timerREVOKE;" + itoa(P2T::timerREVOKE) + "\r\n";
		init_data += "timerGRANT;" + itoa(P2T::timerGRANT) + "\r\n";
		init_data += "timerIDLE;" + itoa(P2T::timerIDLE) + "\r\n";
		init_data += "timerTAKEN;" + itoa(P2T::timerTAKEN) + "\r\n";
		init_data += "timerRelTIMEOUT;" + itoa(P2T::timerRelTIMEOUT) + "\r\n";
		init_data += "timerGetTIMEOUT;" + itoa(P2T::timerGetTIMEOUT) + "\r\n";
		init_data += "timerGetFloorTERMINATE;" + itoa(P2T::timerGetFloorTERMINATE) + "\r\n";
		init_data += "timerGrantFloorTERMINATE;" + itoa(P2T::timerGrantFloorTERMINATE) + "\r\n";
		init_data += "timerRelFloorTERMINATE;" + itoa(P2T::timerRelFloorTERMINATE)  + "\r\n";
		init_data += "timerIdleFloorTERMINATE;" + itoa(P2T::timerIdleFloorTERMINATE) + "\r\n";
#ifndef _MSC_VER
		ts.init(P2T::PERFORMANCE_FILE, init_data);
#endif
		return true;
	}else{
		return false;
	}
}

bool SipDialogP2T::a99_idle_idle_collisioner( const SipSMCommand &command){

	if (transitionMatch(command, "p2tCollisioner")){
		//MRef<SipDialogP2T *>vc= (SipDialogP2T *)sipStateMachine;
		/*vc->*/p2tCollisioner=true;
		return true;
	}else{
		return false;
	}
}


/**
 * SipDialogP2T::setUpStateMachine()
 * defines the states and transactions for the state machine.
 */

void SipDialogP2T::setUpStateMachine(){

	
	//states:
	State<SipSMCommand,string> *s_idle=new State<SipSMCommand,string>(this,"idle");
	addState(s_idle);

	State<SipSMCommand,string> *s_talk_req=new State<SipSMCommand,string>(this,"talk_req");
	addState(s_talk_req);

	State<SipSMCommand,string> *s_talk=new State<SipSMCommand,string>(this,"talk");
	addState(s_talk);

	State<SipSMCommand,string> *s_release_pend=new State<SipSMCommand,string>(this,"release_pend");
	addState(s_release_pend);

	State<SipSMCommand,string> *s_listen_req=new State<SipSMCommand,string>(this,"listen_req");
	addState(s_listen_req);
	
	State<SipSMCommand,string> *s_listen=new State<SipSMCommand,string>(this,"listen");
	addState(s_listen);

	State<SipSMCommand,string> *s_collision=new State<SipSMCommand,string>(this,"collision");
	addState(s_collision);
	
	State<SipSMCommand,string> *s_resent=new State<SipSMCommand,string>(this,"resent");
	addState(s_resent);
	
	State<SipSMCommand,string> *s_terminated=new State<SipSMCommand,string>(this,"terminated");
	addState(s_terminated);
	

	//transactions:
//	StateTransition<SipSMCommand,string> *transition_idle_talkreq=
		new StateTransition<SipSMCommand,string>(this,
				"transition_idle_talkreq",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a0_idle_talkreq, 
				s_idle, s_talk_req
				);

//	StateTransition<SipSMCommand,string> *transition_talkreq_talk=
		new StateTransition<SipSMCommand,string>(this,
				"transition_talkreq_talk",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a1_talkreq_talk, 
				s_talk_req, s_talk
				);

//	StateTransition<SipSMCommand,string> *transition_talk_releasepend=
		new StateTransition<SipSMCommand,string>(this,
				"transition_talk_releasepend",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a2_talk_releasepend, 
				s_talk, s_release_pend
				);

//	StateTransition<SipSMCommand,string> *transition_releasepend_idle=
		new StateTransition<SipSMCommand,string>(this,
				"transition_releasepend_idle",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a3_releasepend_idle, 
				s_release_pend, s_idle
				);


//	StateTransition<SipSMCommand,string> *transition_idle_listenreq=
		new StateTransition<SipSMCommand,string>(this,
				"transition_idle_listenreq",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a4_idle_listenreq,
				s_idle, s_listen_req
				);

//	StateTransition<SipSMCommand,string> *transition_listenreq_listen=
		new StateTransition<SipSMCommand,string>(this,
				"transition_listenreq_listen",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a5_listenreq_listen,
				s_listen_req, s_listen
				);

//	StateTransition<SipSMCommand,string> *transition_listen_idle=
		new StateTransition<SipSMCommand,string>(this,
				"transition_listen_idle",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a6_listen_idle,
				s_listen, s_idle
				);

//	StateTransition<SipSMCommand,string> *transition_talkreq_listenreq=
		new StateTransition<SipSMCommand,string>(this,
				"transition_talkreq_listenreq",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a7_talkreq_listenreq,
				s_talk_req, s_listen_req
				);
	
//	StateTransition<SipSMCommand,string> *transition_talkreq_collision=
		new StateTransition<SipSMCommand,string>(this,
				"transition_talkreq_collision",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a8_talkreq_collision,
				s_talk_req, s_collision
				);
	
//	StateTransition<SipSMCommand,string> *transition_collision_listenreq=
		new StateTransition<SipSMCommand,string>(this,
				"transition_collision_listenreq",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a9_collision_listenreq,
				s_collision, s_listen_req
				);
				
//	StateTransition<SipSMCommand,string> *transition_collision_resent_timer=
		new StateTransition<SipSMCommand,string>(this,
				"transition_collision_resent_timer",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a10_collision_resent_timer,
				s_collision, s_resent
				);
				
//	StateTransition<SipSMCommand,string> *transition_resent_collision_collision=
		new StateTransition<SipSMCommand,string>(this,
				"transition_resent_collision_collision",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a11_resent_collision_collision,
				s_resent, s_collision
				);
				
//	StateTransition<SipSMCommand,string> *transition_resent_talk=
		new StateTransition<SipSMCommand,string>(this,
				"transition_resent_talk",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a12_resent_talk,
				s_resent, s_talk
				);

//	StateTransition<SipSMCommand,string> *transition_resent_resent=
		new StateTransition<SipSMCommand,string>(this,
				"transition_resent_resent",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a13_resent_resent,
				s_resent, s_resent
				);	

	
//	StateTransition<SipSMCommand,string> *transition_listen_listen=
		new StateTransition<SipSMCommand,string>(this,
				"transition_listen_listen",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a14_listen_listen,
				s_listen, s_listen
				);
	
//	StateTransition<SipSMCommand,string> *transition_talkreq_talkreq=
		new StateTransition<SipSMCommand,string>(this,
				"transition_talkreq_talkreq",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a15_talkreq_talkreq,
				s_talk_req, s_talk_req
				);

//	StateTransition<SipSMCommand,string> *transition_releasepend_releasepend=
		new StateTransition<SipSMCommand,string>(this,
				"transition_releasepend_releasepend",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a16_releasepend_releasepend,
				s_release_pend, s_release_pend
				);
				
//	StateTransition<SipSMCommand,string> *transition_listen_idle_revoke=
		new StateTransition<SipSMCommand,string>(this,
				"transition_listen_idle_revoke",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a17_listen_idle_revoke,
				s_listen, s_idle
				);
				
//	StateTransition<SipSMCommand,string> *transition_talk_talk_revoke=
		new StateTransition<SipSMCommand,string>(this,
				"transition_talk_talk_revoke",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a18_talk_talk_revoke,
				s_talk, s_talk
				);
	
//	StateTransition<SipSMCommand,string> *transition_listenreq_listenreq=
		new StateTransition<SipSMCommand,string>(this,
				"transition_listenreq_listenreq",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a19_listenreq_listenreq,
				s_listen_req, s_listen_req
				);
				
//	StateTransition<SipSMCommand,string> *transition_idle_terminated=
		new StateTransition<SipSMCommand,string>(this,
				"transition_idle_terminated",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a80_idle_terminated,
				s_idle, s_terminated
				);						

//	StateTransition<SipSMCommand,string> *transition_talkreq_terminated=
		new StateTransition<SipSMCommand,string>(this,
				"transition_talkreq_terminated",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a81_talkreq_terminated,
				s_talk_req, s_terminated
				);
//	StateTransition<SipSMCommand,string> *transition_listenreq_terminated=
		new StateTransition<SipSMCommand,string>(this,
				"transition_listenreq_terminated",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a82_listenreq_terminated,
				s_listen_req, s_terminated
				);
				
//	StateTransition<SipSMCommand,string> *transition_talk_terminated=
		new StateTransition<SipSMCommand,string>(this,
				"transition_talk_terminated",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a83_talk_terminated,
				s_talk, s_terminated
				);
				
//	StateTransition<SipSMCommand,string> *transition_collision_terminated=
		new StateTransition<SipSMCommand,string>(this,
				"transition_collision_terminated",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a84_collision_terminated,
				s_collision, s_terminated
				);
				
//	StateTransition<SipSMCommand,string> *transition_listen_terminated=
		new StateTransition<SipSMCommand,string>(this,
				"transition_listen_terminated",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a85_listen_terminated,
				s_listen, s_terminated
				);
				
//	StateTransition<SipSMCommand,string> *transition_releasepend_terminated=
		new StateTransition<SipSMCommand,string>(this,
				"transition_releasepend_terminated",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a86_releasepend_terminated,
				s_release_pend, s_terminated
				);	
	
//	StateTransition<SipSMCommand,string> *transition_resent_terminated=
		new StateTransition<SipSMCommand,string>(this,
				"transition_resent_terminated",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a87_resent_terminated,
				s_resent, s_terminated
				);

	//used for development
	//forces the local UA to produce a collision, if the p2tCollisioner
	//variable is set.	
//	StateTransition<SipSMCommand,string> *transition_idle_talkreq_collisioner=
		new StateTransition<SipSMCommand,string>(this,
				"transition_idle_talkreq_collisioner",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a97_idle_talkreq_collisioner,
				s_idle, s_talk_req
				);
	
	//used for development. 
	//can be used to set the p2tPerformance variable			
//	StateTransition<SipSMCommand,string> *transition_idle_idle_performance=
		new StateTransition<SipSMCommand,string>(this,
				"transition_idle_idle_performance",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a98_idle_idle_performance,
				s_idle, s_idle
				);
	
	//used for development.
	//can be used to set the p2tCollisioner variable.
//	StateTransition<SipSMCommand,string> *transition_idle_idle_collisioner=
		new StateTransition<SipSMCommand,string>(this,
				"transition_idle_idle_collisioner",
				(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) &SipDialogP2T::a99_idle_idle_collisioner,
				s_idle, s_idle
				);	
	setCurrentState(s_idle);
}


SipDialogP2T::SipDialogP2T(MRef<SipStack*> stack, MRef<SipDialogConfig*> callconfig, MRef<SipSoftPhoneConfiguration*> pconf) : 
		SipDialog(stack,callconfig),
		phoneconf(pconf)
{
	
#ifdef OLD_MEDIA
	soundReceiver = new SoundReceiver(getPhoneConfig()->soundcard, NULL, getPhoneConfig());
	soundSender = new SoundSender(soundReceiver->getSocket(), 
			getPhoneConfig()->soundcard, 
			//getPhoneConfig()->inherited->codecs[0], //FIXME
			NULL);
	soundSender->setDialog(this);
	soundReceiver->setDialog(this);
#endif

	
	floorControlReceiver = new RtcpReceiver(getPhoneConfig(), 0/* soundReceiver->getContactPort()*/);
	floorControlSender = new RtcpSender(floorControlReceiver->getSocket());
	floorControlReceiver->start();
	floorControlSender->setCall(this);
	floorControlReceiver->setCall(this);
	//floorControlReceiver->flush();
					
	//generate GroupIdentity
	/*getDialogConfig().callId*/ dialogState.callId = itoa(rand())+"@"+getDialogConfig()->inherited->externalContactIP;
	
	//initialize sequence number
	dialogState.seqNo = 0;
	
	counterRevoke=0;
	counterCollision=0;
	
	p2tCollisioner=false;
	p2tPerformance=false;
		
	//start state machine
	setUpStateMachine();
}

SipDialogP2T::~SipDialogP2T(){	
#ifdef DEBUG_OUTPUT
	mdbg << "SipDialogP2T::~SipDialogP2T invoked"<< end;
#endif
}


bool SipDialogP2T::handleCommand(const SipSMCommand &c){
	
	//check CallId resp. GroupID
	
	
	
	return SipDialog::handleCommand(c);
}



bool SipDialogP2T::modifyUser(string uri, IPAddress *ip, int RTPport, int RTCPport, Codec *codec, string callId) {
	
	mdbg<<"-------------------------"<<end;
	mdbg<<"SipDialogP2T::modifyUser:"<<end;
	mdbg<<"User data arrived for "<<uri<<":"<<end;
	mdbg<<"RTPport="<<itoa(RTPport)<<end;
	mdbg<<"RTCPport="<<itoa(RTCPport)<<end;
	mdbg<<"-------------------------"<<end;
		
	
	if(grpList->isParticipant(uri)){
		MRef<GroupListUserElement *> ue = grpList->getUser(uri);
		
		//modify only if callIds are correct
		if(ue->getCallId()==callId){
			ue->setAddress(ip);
			ue->setRTPport(RTPport);
			ue->setRTCPport(RTCPport);
			ue->setCodec(codec);
			ue->setStatus(P2T::STATUS_CONNECTED);
			
			
			//check codec
			//getSoundSender()->setCodec(codec);
			
			//add to SoundSender
			//getSoundSender()->addRemoteAddress(uri, ip, RTPport);
			
		
			//inform GUI
			CommandString cmdstr(getCallId(), "p2tModifyUser", uri, itoa(ue->getStatus()) );
			getDialogContainer()->getCallback()->sipcb_handleCommand(cmdstr);
			return true;
		}
	}

	return false;	
}


bool SipDialogP2T::removeUser(string uri, string reason, string callId) {
	if(grpList->isParticipant(uri)){
		
		
	mdbg<<"-------------------------"<<end;
	mdbg<<"SipDialogP2T::removeUser:"<<end;
	mdbg<<"Remove user "<<uri<<":"<<end;
	mdbg<<"reason="<<reason<<end;
	mdbg<<"callID="<<callId<<"#"<<end;
	mdbg<<"GrplistCallID="<<grpList->getUser(uri)->getCallId()<<"#"<<end;
	mdbg<<"-------------------------"<<end;
		
		//remove only if callIds are correct
		if(grpList->getUser(uri)->getCallId()==callId){
			grpList->removeUser(uri);
			
			//remove user from SoundSender
			//not implemented
		
			//inform GUI
			CommandString cmdstr(getCallId(), "p2tRemoveUser", uri, reason);
			getDialogContainer()->getCallback()->sipcb_handleCommand(cmdstr);
			return true;
		}
	}
	
	return false;
}

void SipDialogP2T::setGroupList(MRef<GroupList*> GrpList){
	this->grpList = GrpList;
}

bool SipDialogP2T::checkStates(int status){
	bool ret=true;
	
	for(uint32_t k=0; k<grpList->getAllUser().size();k++){
		
		//filter out own user
		//if(grpList->getAllUser().at(k)->getUri()==getDialogConfig().inherited->userUri)
		if(grpList->getAllUser()[k]->getUri()==getDialogConfig()->inherited->sipIdentity->getSipUri())
				continue;
		
		if(grpList->getAllUser()[k]->getStatus()==status 
			|| grpList->getAllUser()[k]->getStatus()==P2T::STATUS_NOTAVAILABLE)
			continue;
		else
			ret=false;
	}
	
	return ret;
}

bool SipDialogP2T::allAnswered(){
	bool ret=true;
	
	//merr<<end<<"*****************************"<<end;
	//merr<<"SipDialogP2T::allAnswered()"<<end;
	
	for(uint32_t k=0; k<grpList->getAllUser().size();k++){
		
		//filter out own user
		//if(grpList->getAllUser().at(k)->getUri()==getDialogConfig().inherited->userUri)
		if(grpList->getAllUser()[k]->getUri()==getDialogConfig()->inherited->sipIdentity->getSipUri())
				continue;
		
		//merr<<grpList->getAllUser().at(k)->getUri()<<"="<<P2T::getStatus(grpList->getAllUser().at(k)->getStatus())<<end;
				
				
		if(grpList->getAllUser()[k]->getStatus()== P2T::STATUS_COLLISION
			|| grpList->getAllUser()[k]->getStatus()==P2T::STATUS_GRANT
			|| grpList->getAllUser()[k]->getStatus()==P2T::STATUS_RELEASED
			|| grpList->getAllUser()[k]->getStatus()==P2T::STATUS_NOTAVAILABLE){
			
			continue;
		}
		else {
			ret=false;
			break;
		}
	}
	//merr<<"return="<<itoa(ret)<<end;
	//merr<<"***************************"<<end;
	return ret;
}

void SipDialogP2T::setStates(int status){
	for(uint32_t k=0; k<grpList->getAllUser().size();k++)
		grpList->getAllUser()[k]->setStatus(status);
}

void SipDialogP2T::terminateSession(){


	for(uint32_t k=0; k<grpList->getAllUser().size();k++){
		
		
	
		//filter out own username
		//if(grpList->getAllUser().at(k)->getUri()==getDialogConfig().inherited->userUri)
		if(grpList->getAllUser()[k]->getUri()==getDialogConfig()->inherited->sipIdentity->getSipUri())
			continue;
		
		//send hang_up message to all SipDialogP2Tuser dialogs
		CommandString hup(grpList->getAllUser()[k]->getCallId(), SipCommandString::hang_up);
		SipSMCommand scmd(hup, SipSMCommand::remote, SipSMCommand::TU);
		getDialogContainer()->enqueueCommand(scmd, HIGH_PRIO_QUEUE, PRIO_LAST_IN_QUEUE);
		
		
	}
	

}

bool SipDialogP2T::highestPrio(int status,vector<string> &users){
	bool ret=false;
	int prio=0;
		
	//find my priority
	//int myPrio = getGroupList()->getUser(getDialogConfig().inherited->userUri)->getPriority();
	int myPrio = getGroupList()->getUser(getDialogConfig()->inherited->sipIdentity->getSipUri())->getPriority();
		
	//find highest priority from the other users
	for(uint32_t k=0; k<grpList->getAllUser().size();k++){
		//filter out own user 
		//if(grpList->getAllUser().at(k)->getUri()==getDialogConfig().inherited->userUri)
		if(grpList->getAllUser()[k]->getUri()==getDialogConfig()->inherited->sipIdentity->getSipUri())
			continue;
		
		if(grpList->getAllUser()[k]->getStatus()==status ){
			//higher prio found
			if(grpList->getAllUser()[k]->getPriority()>prio){
				//set new prio
				prio=grpList->getAllUser()[k]->getPriority();
				//clear vector
				users.clear();
				//add user
				users.push_back(grpList->getAllUser()[k]->getUri());
				
			}
			else if(grpList->getAllUser()[k]->getPriority()==prio){
				//add user to vector
				users.push_back(grpList->getAllUser()[k]->getUri());
			}
		}
	}
	
	if(myPrio>prio){
		ret=true;
		users.clear();
	}
	else if(myPrio==prio)
		ret=true;

	
	return ret;


}


