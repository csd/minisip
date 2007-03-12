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


#ifndef SipDialogP2T_H
#define SipDialogP2T_H

#include<libminisip/libminisip_config.h>

#include<libmutil/StateMachine.h>

#include<libmnetutil/IPAddress.h>

#include<libmsip/SipResponse.h>
#include<libmsip/SipStack.h>

#include<libminisip/media/codecs/Codec.h>
#include<libminisip/signaling/p2t/RtcpReceiver.h>
#include<libminisip/signaling/p2t/RtcpSender.h>
#include<libminisip/signaling/p2t/GroupList.h>
#include<libminisip/signaling/p2t/GroupListUserElement.h>
#include<libminisip/signaling/p2t/GroupListClient.h>
#include<libminisip/signaling/p2t/P2T.h>
#include<libminisip/media/MediaHandler.h>
#include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>


/**
 * The main dialog for a P2T Session.
 * The <CODE>SipDialogP2T</CODE> dialog is mainly responsible for the Floor Control in a 
 * P2T Session.
 * 
 * <p><b>Floor Control:</b><br>
 * All participants in a P2T Session shall be informed when the floor is idle. A participant
 * who wishes to take the floor shall initiate a floor request by pressing the Push-2-Talk
 * button. If the user's request is granted, then the user is notified and all other users
 * should be notified that the floor has been taken and who has taken it. Only one
 * participant in a P2T Session can be granted the floor at one time. There exists four
 * different Floor Procedures:<br>
 * Floor Request Procedure, Floor Collision Procedure, Floor Release Procedure, and
 * Floor Revoke Procedure.
 *
 * <p><b>SipDialogP2T:</b><br>
 * The Floor Control is implemented in the <code>SipDialogP2T</code> dialog and uses five
 * different transactions: <code>RtcpTransactionGetFloor, RtcpTransactionReleaseFloor, 
 * RtcpTransactioinTakenFloor, RtcpTransactionGrantFloor, RtcpTransactionIdleFloor.</code>
 * 
 * <p><b>State Machine:</b><br>
 * The state machine implemented in <code>SipDialogP2T</code> is shown in the next three
 * figures (the illustration of the state machine is split up to avoid having a too crowded
 * figure). The notation for every transition is as follows: The upper part shows the
 * incoming command plus some additional conditions that have to be fullfilled and the 
 * lower part gives a short description about the action that takes place.
 * <p>
 *
 * <img src=material/SipDialogP2T-1.gif><br>
 * Figure 1 - The Floor Request, Floor Release and Floor Revoke Procedure
 * <p><p>
 *
 * <img src=material/SipDialogP2T-2.gif><br>
 * Figure 2 - The Floor Collision Procedure
 * <p><p>
 *
 * <img src=material/SipDialogP2T-3.gif><br>
 * Figure 3 - Terminating a P2T Session
 * <p><p>
 *
 * <b>Transitions:</b>
 * <table border="0" cellspacing="0" cellpadding="5">
 * <tr bgcolor="#999999">
 * <td><font color=white><b>&nbsp;<CODE>CommandString</CODE> Input:</b></font></td>
 * <td><font color=white><b>Action:</b></font></td>
 * <td><font color=white><b>Description:</b></font></td>
 * </tr>
 * 
 * <tr><td>&nbsp;p2tGetFloor</td><td>a0_idle_talkreq</td><td>
 * starts for every participating user a <code>RtcpTransactionGetFloor</code>
 * transaction that sends Floor REQUEST messages out.
 * </td></tr>
 * 
 * <tr bgcolor="#E6E6E6"><td>&nbsp;p2tRequestAnswered<br>
 * && allAnswered()=true <br> && checkStates(P2T::STATUS_GRANT)=true
 * </td><td>a1_talkreq_talk</td><td>
 * all other users has sent a Floor GRANT message back and thus the floor
 * is granted to the local user. Sends Floor TAKEN messages to all
 * participants.
 * </td></tr>
 *
 * <tr><td>&nbsp;p2tReleaseFloor</td><td>a2_talk_releasepend</td><td>
 * starts for every user a <code>RtcpTransactionReleaseFloor</code> that  
 * sends Floor RELEASE messages out.
 * </td></tr>
 *
 * <tr bgcolor="#E6E6E6"><td>&nbsp;p2tFloorReleased</td><td>a3_releasepend_idle</td><td>
 * all users have released the floor.
 * </td></tr>
 *
 * <tr><td>&nbsp;p2tREQUEST</td><td>a4_idle_listenreq</td><td>
 * Another user has sent a REQUEST message. Starts a 
 * <code>RtcpTransactionGrantFloor</code> transaction.
 * </td></tr>
 *
 * <tr bgcolor="#E6E6E6"><td>&nbsp;p2tSendRequest</td><td>a5_listenreq_listen</td><td>
 * A user that has requested the floor before has send a Floor
 * TAKEN message and the floor is granted to him.
 * </td></tr>
 *
 * <tr><td>&nbsp;p2tRELEASE</td><td>a6_listen_idle</td><td>
 * The user that had granted floor sent a Floor RELEASE message.
 * Starts a <code>RtcpTransactionIdleFloor</code> transaction.
 * </td></tr>
 *
 * <tr bgcolor="#E6E6E6"><td>&nbsp;p2tRequestAnswered <br>
 * && allAnswered()=true<br>
 * && checkStates(P2T::STATUS_GRANT)=false <br>
 * && highestPrio()=false</td><td>a7_talkreq_listenreq</td><td>
 * All users have answered the Floor REQUEST message, but not all
 * with a Floor GRANT message. There are collisions and one or more
 * of them have a higher priority. 
 * Starts for all collisioned users with higer priority a 
 * <code>RtcpTransactionGrantFloor</code> transaction.
 * </td></tr>
 *
 * <tr><td>&nbsp;p2tRequestAnswered <br>
 * && allAnswered()=true<br>
 * && checkStates(P2T::STATUS_GRANT)=false <br>
 * && highestPrio()=true <br>
 * && u</td><td>a8_talkreq_collision</td><td>
 * All users have answered the Floor REQUEST message, but there
 * are collisions and there are one or more users with the same
 * highest priority as the local user has. Starts the timerRESEND för
 * resending the Floor REQUEST message to the collisioned users.
 * </td></tr>
 *
 * <tr bgcolor="#E6E6E6"><td>&nbsp;p2tREQUEST</td><td>a9_collision_listenreq</td><td>
 * Another user was faster with resending the Floor REQUEST message after
 * the collision, thus I have to grant him the floor. Starts a <code>
 * RtcpTransactionGrantFloor</code> transaction for this user.
 * </td></tr>
 *
 * <tr><td>&nbsp;timerRESEND</td><td>a10_collision_resent_timer</td><td>
 * The timer for resending the Floor REQUEST message to all collisoned users
 * has expired. Starts a <code>RtcpTransactionGetFloor</code> transaction for 
 * all users that have collisioned.
 * </td></tr>
 *
 * <tr bgcolor="#E6E6E6"><td>&nbsp;p2tRequestAnswered</td><td>a11_resent_collision_collison</td><td>
 * The collisioned users have answered my resended Floor REQUEST message but there
 * are again collisions. Resets the timerGRANT for the next resending of the
 * Floor REQUEST message.
 * </td></tr>
 *
 * <tr><td>&nbsp;p2tRequestAnswered <br>
 * && allAnswered()=true <br>
 * && checkStates(P2T::STATUS_GRANT)=true</td><td>a12_resent_talk</td><td>
 * All users have granted my resended Floor REQUEST message. Sends Floor
 * TAKEN messages to all participants and the floor is granted to the
 * local user.
 * </td></tr>
 
 * <tr bgcolor="#E6E6E6"><td>&nbsp;p2tRequestAnswered <br>
 * && allAnswered()=false</td><td>a13_resent_resent</td><td>
 * The command p2tRequestAnswered informs SipDialogP2T that a user has answered the
 * request. But because not all users have done this, we are staying still in the
 * 'resent' state.
 * </td></tr>
 *
 * <tr><td>&nbsp;timerREVOKE</td><td>a14_listen_listen</td><td>
 * The timerREVOKE has expired the first or second time. It sends
 * a Floor REVOKE message as warning to the granted user, that the 
 * maximum floor time is reached. Resets the timerREVOKE.
 * </td></tr>
 * 
 * <tr bgcolor="#E6E6E6"><td>&nbsp;p2tRequestAnswered <br>
 * && allAnswered=false</td><td>a15_talkreq_talkreq</td><td>
 * Because not all users have answered we do nothing and are staying
 * still in the 'talkreq' state.
 * </td></tr>
 *
 * <tr><td>&nbsp;p2tFloorReleased <br>
 * && ...
 * </td><td>a16_releasepend_relaesepend</td><td>
 * Not all users have answered the Floor RELEASE message.
 * </td></tr>
 * 
 * <tr bgcolor="#E6E6E6"><td>&nbsp;timerREVOKE</td><td>a17_listen_idle_revoke</td><td>
 * The timerREVOKE expired the third time. A final Floor REVOKE message
 * is sent and the local user moves to the 'idle' state.
 * </td></tr>
 *
 * <tr><td>&nbsp;p2tREVOKE</td><td>a18_talk_talk_revoke</td><td>
 * A remote user sent a Floor REVOKE message. Inform the GUI about that.
 * </td></tr>
 *
 * <tr bgcolor="#E6E6E6"><td>&nbsp;p2tREQUEST</td><td>a19_listenreq_listenreq</td><td>
 * Another user sent a Floor REQUEST message. Starts a <code>
 * RtcpTransactionGrantFloor</code> transactiona also for this user. 
 * Because there is a collision and one of the collisioned user
 * will send a Floor TAKEN message.
 * </td></tr>
 * 
 * <tr><td>&nbsp;p2tTerminate</td><td>a80_idle_terminated</td><td>
 * terminates the P2T Session.
 * </td></tr>
 * 
 * <tr bgcolor="#E6E6E6"><td>&nbsp;p2tTerminate</td><td>a81_talkreq_terminated</td><td>
 * terminates the P2T Session.
 * </td></tr>
 * 
 * <tr><td>&nbsp;p2tTerminate</td><td>a82_listenreq_terminated</td><td>
 * terminates the P2T Session.
 * </td></tr>
 * 
 * <tr bgcolor="#E6E6E6"><td>&nbsp;p2tTerminate</td><td>a83_talk_terminated</td><td>
 * terminates the P2T Session.
 * </td></tr>
 * 
 * <tr><td>&nbsp;p2tTerminate</td><td>a84_collision_terminated</td><td>
 * terminates the P2T Session.
 * </td></tr>
 * 
 * <tr bgcolor="#E6E6E6"><td>&nbsp;p2tTerminate</td><td>a85_listen_terminated</td><td>
 * terminates the P2T Session.
 * </td></tr>
 * 
 * <tr><td>&nbsp;p2tTerminate</td><td>a86_releasepend_terminated</td><td>
 * terminates the P2T Session.
 * </td></tr>
 *
 * <tr><td>&nbsp;p2tTerminate</td><td>a87_resent_terminated</td><td>
 * terminates the P2T Session.
 * </td></tr></table>
 * 
 * <p><b>Transitions for Development</b><br>
 * There exists two transitions for development purposes:
 * a98_idle_idle_performance and a99_idle_idle_collisioner.<br>
 * If you send to <code>SipDialogP2T</code> a CommandString containing 'p2tPerformance',
 * <code>SipDialogP2T</code> will measure the Push-To-Talk Delay (the time from pressing
 * the button until the floor is granted) and save the results in the file specified in
 * <code>P2T::PERFORMANCE_FILE</code>.<br>
 * If you send a CommandString containing 'p2tCollisioner', <code>SipDialogP2T</code> is 
 * forced to produce a collision if another user requests the floor.
 *<p>
 
 * @author Florian Maurer, florian.maurer@floHweb.ch
 */
class LIBMINISIP_API SipDialogP2T: public SipDialog{
	public:
		
		/**
		 * Constructor
		 * @param dContainer  the SipDialogContainer
		 * @param callconfig  the Call configuration
		 * @param phoneconf   the Phone configuration
		 */
		SipDialogP2T(MRef<SipStack*> stack, MRef<SipDialogConfig*> callconfig, MRef<SipSoftPhoneConfiguration*> phoneconf);
		
		/**
		 * Deconstructor
		 */
		virtual ~SipDialogP2T();
		
		/**
		 * @return the name "SipDialogP2T"
		 */	
		virtual std::string getName() const {return "SipDialogP2T";}

		virtual std::string getMemObjectType() const {return getName();}

		void setCallId(std::string id){dialogState.callId = id;}
		std::string getCallId(){return dialogState.callId;}
		
		/**
		 * handles the incoming commands. Accepts commands only if they have the
		 * correct Call Identity and match a transition.
		 * @param command the incoming command
		 * @return true if the command was handled
		 */
		virtual bool handleCommand(const SipSMCommand &command);

		/**
		 * generates and sets up the state machine
		 */
		void setUpStateMachine();

		/**
		 * returns the sound sender that is used to send
		 * the RTP packets to the other users.
		 * @return <code>SoundSender</code> object
		 */
	//	SoundSender *getSoundSender(){return soundSender;}
		
		/**
		 * returns the sound receiver that receives 
		 * RTP packets from other users.
		 * @return <code>SoundReceiver</code> object
		 */
	//	SoundReceiver *getSoundReceiver(){return soundReceiver;}

		/**
		 * returns the phone configuration
		 * @return <code>SipSoftPhoneConfiguration</code> object
		 */
		MRef<SipSoftPhoneConfiguration*> getPhoneConfig(){return phoneconf;}
	
		/**
 		 * modify the data about a user in the P2T-Session. 
		 * @param uri	    the SIP URI of the user
		 * @param ip        the IP Address
		 * @param RTPport   the RTPport (media stream)
		 * @param RTCPport  the RTCPport (floor control)
		 * @param codec     the codec
		 * @param callId    the SipDialogP2Tuser callId
 		 * @return true if the user was successfully modified
 		 */
		bool modifyUser(std::string uri, IPAddress *ip, int RTPport, int RTCPport, Codec *codec, std::string callId);
		
		/**
		 * removes a user from the Group Member List.
		 * @param uri     the SIP URI of the user
		 * @param reason  for which reason the user has to be removed
		 * @param callId  the callId from the dialog that wants to remove
		 *                the user. Note: The user will be removed only
		 *                when the callId in the Group Member List and this
		 *                parameter are similar.
		 * @return true if the user was successfully removed
		 */
		bool removeUser(std::string user, std::string reason, std::string callId);

		/**
		 * get the RtcpSender that is responsible for sending
		 * the Floor Control Messages.
		 * @return <code>RtcpSender</code> object
		 */
		RtcpSender *getFloorControlSender(){return floorControlSender;}
		
		/**
		 * get the Group Member List for this P2T Session.
		 * @return <code>GroupList</code> object.
		 */
		MRef<GroupList*> getGroupList(){return grpList;}
		
		/**
		 * set the Group Member List for this P2T Session
		 * @param GrpList the Group Member List
		 */
		void setGroupList(MRef<GroupList*> GrpList);
		
		/**
		 * get the RTP port for the media stream.
		 * @return port number
		 */
		int getRTPPort(){return 0;/*soundReceiver->getContactPort();*/}
		
		/**
		 * checks the states of the users.
		 * @param status the status that should be checked
		 * @return true if all have the specified status or P2T_NOT_AVAILABLE
		 */
		bool checkStates(int status);
		
		/**
		 * check if all users have answered a REQUEST message
		 * @return true if all users have one of these states:
		 *         P2T::STATUS_GRANT, P2T::STATUS_COLLISION
		 *         P2T::RELEASED or P2T::STATUS_NOTAVAILABLE
		 */
		bool allAnswered();
		
		/**
		 * checks if the local user has the highest priority
		 * among the users with the specified status.
		 * If the priority is lower or the same, a reference
		 * to a vector containing strings with the username
		 * of those users who have highest priority is given.
		 * If the local user has highest priority this vector
		 * will be empty or contains the other users with the
		 * same priority. Note: The local user may not have
		 * the specified status.
		 * @param status the status that should be checked
		 * @param users  a vector containing strings
		 * @return boolean returns true if local user has highest priority.
		 */
		bool highestPrio(int status,std::vector<std::string> &users);
		
		/**
		 * sets the status of every user in the Group Member List
		 * to the specified state.
		 * @param status the status that should be set
		 */
		void setStates(int status);
		
		/**
		 * terminate the P2T session. 
		 * Sends to all SipDialogP2Tuser dialog a 
		 * hangup message.
		 */
		void terminateSession();
		
		/**
		 * counts how often the timerREVOKE
		 * has expired.
		 */
		 int counterRevoke;
		 
		 /**
		  * counts how often a collision has happen, and is used
		  * as value for the Collision Flag.
		  */
		 int counterCollision;
		 
		 /**
		  * used for development. Forces SipDialogP2T to
		  * make a collision in the floor control. The variable
		  * can be set by sending a CommandString "p2tCollisioner" to the
		  * dialog.
		  */
		 bool p2tCollisioner;
		 
		 /**
		  * used for development. If this variable is equals true
		  * SipDialogP2T measures the Push-to-talk delay. The variable can
		  * be set by sending a CommandString "p2tPerformance" to the dialog.
		  */
		 bool p2tPerformance;
	
	private:

		 bool a0_idle_talkreq( const SipSMCommand &command);
		 bool a1_talkreq_talk( const SipSMCommand &command);
		 bool a2_talk_releasepend( const SipSMCommand &command);
		 bool a3_releasepend_idle( const SipSMCommand &command);
		 bool a4_idle_listenreq( const SipSMCommand &command);
		 bool a5_listenreq_listen( const SipSMCommand &command);
		 bool a6_listen_idle( const SipSMCommand &command);
		 bool a7_talkreq_listenreq( const SipSMCommand &command);
		 bool a8_talkreq_collision( const SipSMCommand &command);
		 bool a9_collision_listenreq( const SipSMCommand &command);
		 bool a10_collision_resent_timer( const SipSMCommand &command);
		 bool a11_resent_collision_collision( const SipSMCommand &command);
		 bool a12_resent_talk( const SipSMCommand &command);
		 bool a13_resent_resent( const SipSMCommand &command);
		 bool a14_listen_listen( const SipSMCommand &command);
		 bool a15_talkreq_talkreq( const SipSMCommand &command);
		 bool a16_releasepend_releasepend( const SipSMCommand &command);
		 bool a17_listen_idle_revoke( const SipSMCommand &command);
		 bool a18_talk_talk_revoke( const SipSMCommand &command);
		 bool a19_listenreq_listenreq( const SipSMCommand &command);
		 bool a80_idle_terminated( const SipSMCommand &command);
		 bool a81_talkreq_terminated( const SipSMCommand &command);
		 bool a82_listenreq_terminated( const SipSMCommand &command);
		 bool a83_talk_terminated( const SipSMCommand &command);
		 bool a84_collision_terminated( const SipSMCommand &command);
		 bool a85_listen_terminated( const SipSMCommand &command);
		 bool a86_releasepend_terminated( const SipSMCommand &command);
		 bool a87_resent_terminated( const SipSMCommand &command);
		 bool a97_idle_talkreq_collisioner( const SipSMCommand &command);
		 bool a98_idle_idle_performance( const SipSMCommand &command);
		 bool a99_idle_idle_collisioner( const SipSMCommand &command);
		 
		///sends the media stream to the other users
		//SoundSender *soundSender;
		///receives the media stream from another user
		//SoundReceiver *soundReceiver;
		///sends the Floor Control Messages to the other users
		RtcpSender *floorControlSender;
		///receives the Floor Control Messages from the other users
		RtcpReceiver *floorControlReceiver;
		///The Group Member List for this P2T Session
		MRef<GroupList*> grpList;
		///The Phone Configuration
		MRef<SipSoftPhoneConfiguration*> phoneconf;
		


};

#endif

