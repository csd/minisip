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

#ifndef RTCPTRANSACTIONGETFLOOR_H
#define RTCPTRANSACTIONGETFLOOR_H

#include<libminisip/libminisip_config.h>

#ifdef _WIN32_WCE
#	include<libmnetutil/IPAddress.h>
#endif

#include<libmsip/SipSMCommand.h>
#include<libmsip/SipTransaction.h>

#include<libminisip/signaling/p2t/P2T.h>
#include<libminisip/signaling/p2t/SipDialogP2T.h>
#include<libminisip/signaling/p2t/GroupListUserElement.h>

//#include<config.h>

/** 
 * implements the Floor Request Procedure if the local user wants to take the floor.
 * <p><b>Floor Request Procedure:</b><br>
 * If the local user wants the floor he sends a Floor REQUEST message to all other
 * users that are participating in the P2T Session and waits until every user sends
 * an answer back. If this answer is a Floor GRANT message the remote user granted
 * the floor. If the answer is a Floor REQUEST message the remote user requested the
 * floor at the same time, and we have a collision. If no answer returns, the remote 
 * user is assumed as not available at the moment.
 *
 * <p><b>RtcpTransactionGetFloor:</b><br>
 * <CODE>RtcpTransactionGetFloor</CODE> is used for the interaction with one user
 * in a Floor Request Procedure. Every time the <CODE>SipDialogP2T</CODE> dialog wants
 * to change to the 'talk' state respectively to get the floor, a <CODE>RtcpTransactionGetFloor</CODE>
 * for every user contained in the <CODE>GroupList</CODE>  is started that interacts with
 * that user.<p>
 * RtcpTransactionGetFloor sends a Floor REQUEST message, starts a timer called timerGRANT and
 * waits for an answer.<br>
 * Every time the timer <code>timerGRANT</code> expires it resends the Floor REQUEST
 * message and resets the timer. The first five times the timer expires it doubles
 * its value, after that the same value is always used.<br>
 * 
 * If the user answered the request with a Floor GRANT or Floor REQUEST message, 
 * or the timerGetTIMEOUT expired, it enters the new status in the Group Member List
 * and informs the dialog SipDialogP2T with a
 * CommandString: <br>
 * <code>
 * &nbsp;op=p2tRequestAnswered <br>
 * &nbsp;param=ssrc<br>
 * &nbsp;param2=seqNo<br>
 * &nbsp;param3=uri or CollisionCounter if >0<br>
 * </code>
 * <p>
 *
 * <b>State Machine:</b><br>
 * The following state machine is implemented:<p>
 * <img src=material/RtcpTransactionGetFloor.gif><p>
 *
 * <b>Transitions:</b>
 * <table border="0" cellspacing="0" cellpadding="5">
 * <tr bgcolor="#999999">
 * <td><font color=white><b>&nbsp;<CODE>CommandString</CODE> Input:</b></font></td>
 * <td><font color=white><b>Action:</b></font></td>
 * <td><font color=white><b>Description:</b></font></td>
 * </tr>
 * 
 * <tr><td>&nbsp;p2tSendRequest</td><td>a0_start_reqsent</td><td>
 * Sends a Floor REQUEST message to the user and starts the
 * different timers.
 * </td></tr>
 * 
 * <tr bgcolor="#E6E6E6"><td>&nbsp;timerGrant</td><td>a1_reqsent_reqsent</td>
 * <td>Resends the Floor REQUEST message because the timerGRANT
 * has expired. It restarts the timer. The first 5 times it doubles
 * the value of the timer, and then it takes always the same value.
 * </td></tr>
 * 
 * <tr><td>&nbsp;p2tGRANT</td><td>a2_reqsent_completed_grant</td><td>
 * The remote user sent a Floor GRANT message back. It enters
 * the received SSRC number and the granted status into the 
 * <CODE>GroupList</CODE> of the <CODE>SipDialogP2T</CODE> dialog.
 * From now on, it's possible to identify the user with his SSRC number.
 * </td></tr>
 * 
 * <tr bgcolor="#E6E6E6"><td>&nbsp;p2tREQUEST</td><td>a3_reqsent_completed_request</td><td>
 * The remote user sent a Floor REQUEST message back. This means we have a collision with 
 * this user. The collision status and his SSRC and used Sequence number in the message is
 * reported to the <CODE>GroupList</CODE> of the <CODE>SipDialogP2T</Code> dialog. The
 * sequence number is stored, because if this user wins the Collision Procedure we have to 
 * send him a Floor GRANT message with the correct sequence number.
 * </td></tr>
 * 
 * <tr><td>&nbsp;timerGetTIMEOUT</td><td>a4_reqsent_completed_timer</td><td>
 * The time we are willing to wait for an answer message has expired. We report
 * the non available status to the <CODE>GroupList</CODE> of the <CODE>SipDialogP2T</Code> dialog.
 * </td></tr>
 * 
 *<tr bgcolor="#E6E6E6"><td>&nbsp;timerGetFloorTERMINATE</td><td>a5_completed_terminated</td><td>
 * Actually once the 'completed' state is reached nothing more has to be done. But it's possible
 * that some delayed, doubled or resent messages for this transaction arrives that should be handled.
 * That's why the transition stays in the 'completed' state until the timer timerGetFloorTERMINATE
 * expires.
 * </td></tr>
 * 
 * <tr><td>&nbsp;timerGetFloorTERMINATE</td><td>a6_reqsent_terminated</td><td>
 * The timerGetFloorTERMINATE timer has expired, and <CODE>RtcpTransactionGetFloor</CODE>
 * moves to the 'terminated' state.
 * </td></tr>
 *
 * <tr bgcolor="#E6E6E6"><td>&nbsp;p2tGRANT</td><td>a7_completed_completed_grant</td><td>
 * If the remote user resent the Floor GRANT message, it will be catched by this
 * transition.
 * </td></tr>
 *
 * <tr><td>&nbsp;p2tREQUEST</td><td>a8_completed_completed_request</td><td>
 * If the remote user resent the Floor REQUEST message, it will be catched by this
 * transition.
 * </td></tr>
 * </table>
 * <p>
 *
 * @see SipDialogP2T
 * @see GroupList
 * @author Florian Maurer, <a href=mailto:florian.maurer@floHweb.ch>florian.maurer@floHweb.ch</a>
 */

class LIBMINISIP_API RtcpTransactionGetFloor: public SipTransaction{
	public:
		
		/**
		 * Constructor.
		 * @param dialog     the dialog that started this transaction       
		 * @param seqNo      the sequence number that have to be used in the floor control messages
		 * @param toaddr     the destination IP address for the floor control messages
		 * @param port       the destination port for the floor control messages
		 * @param remoteSSRC the SSRC from the remote user. The transaction accepts only floor control
		 *                   messages with this number or the specified user (<CODE>setUser()</CODE>).
		 *                   This parameter is optional if you set the user.
		 */
		RtcpTransactionGetFloor(MRef<SipDialog*> dialog, int seqNo, IPAddress *toaddr, int32_t port,  std::string callId, unsigned remoteSSRC=0);
		
		/**
		 * Destructor
		 */
		virtual ~RtcpTransactionGetFloor();


		void setCallId( std::string id){callId = id;}
		 std::string getCallId(){return callId;}
		/**
		 * returns the name of the transaction.
		 * Used by the Memory Handling of Minisip.
		 * @return the string 'RtcpTransactionGetFloor'
		 */
		virtual  std::string getName(){return "RtcpTransactionGetFloor";}

		/**
		 * handles the incoming commands.
		 * The incoming commands are only accepted if the type of the command
		 * is <CODE>CommandString</CODE> and the user (or SSRC) and used sequence number
		 * in the messages are correct.
		 * @param  command the incoming command that should be handled.
		 * @return true if the command is handled by this transaction.
		 */
		virtual bool handleCommand(const SipSMCommand &command);

		/**
		 * starts the implemented state machine.
		 */
		void setUpStateMachine();

		/**
		 * sends a Floor REQUEST message to the user.
		 */
		void sendRequest();
		
		/**
		 * returns the <CODE>SipDialogP2T</CODE> dialog that
		 * started the transaction.
		 * @return <CODE>SipDialogP2T</CODE> object.
		 */
		MRef<SipDialogP2T*> getDialogP2T();
		
		
		/**
		 * returns the destination IP Address for the Floor
		 * Control Messages.
		 * @return <CODE>IPAdress</CODE> object.
		 */
		IPAddress *getAddr(){ return NULL; //was (the field does not exhist anymore in siptransaction): toaddr;}
		
		/**
		 * returns the destination port for the Floor Control
		 * Messages.
		 * @return <CODE>int32_t</CODE> value.
		 */
		int32_t getPort(){return port;}
		
		/**
		 * returns the SSRC of the remote user.
		 */
		unsigned getRemoteSSRC(){return remoteSSRC;}
		
		/**
		 * sets the SSRC of the remote user.
		 * @param ssrc the SSRC of the remote user.
		 */
		void setRemoteSSRC(unsigned ssrc){remoteSSRC=ssrc;}
		
		/**
		 * sets the user's URI. Only messages with the specified
		 * SSRC or this URI are accepted by the transaction.
		 * @param user SIP URI of the remote user.
		 */
		void setUser( std::string user);
		
		/**
		 * returns the SIP URI of the remote user.
		 * @return <CODE>string</CODE> value.
		 */
		 std::string getUser(){return user;}
		
		/** 
		 * returns the used sequence number
		 * @return sequence number
		 */
		int getSeqNo(){return seqNo;}
		
		/**
		 * report the specified status to the <CODE>GroupList</CODE> of the
		 * <CODE>SipDialogP2T</CODE> dialog.
		 * @param status the status that should be reported.
		 */		
		void reportStatus(int status);
		
		/**
		 * returns the value of the Collision Flag that is used
		 * in the Floor REQUEST messages.
		 * @return <CODE>int</CODE> value
		 */
		int getCollisionCounter(){return CollisionCounter;}
		
		/**
		 * sets the value of the Collision Flag that is used
		 * in the Floor REQUEST messages. 
		 */
		void setCollisionCounter(int value){CollisionCounter=value;}
		
		/**
		 * returns the value of the sequence number of the
		 * remote user.
		 * @return <CODE>int</CODE> value
		 */
		int getRemoteSeqNo(){return remoteSeqNo;}
		
		/**
		 * sets the value of the sequence number of the
		 * remote user if a Floor REQUEST message arrives.
		 * @param value the sequence number
		 */
		void setRemoteSeqNo(int value){remoteSeqNo=value;}

		/**
		 * the value that is used for the <CODE>timerGRANT</CODE>. This value
		 * is set in the Constructor with the predefined value
		 * in the <CODE>P2T</CODE> class and will be doubled
		 * 5 times when the timer expires. It's the value we wait 
		 * before resending a Floor REQUEST message. 
		 */
		int tGrant;
		
		/**
		 * the value that is used for the <CODE>timerGetTIMEOUT</CODE>. This value
		 * is set in the Constructor with the predefined value
		 * in the <CODE>P2T</CODE> class. It's the value we are willing
		 * to wait for an answer to our Floor REQUEST message. If the 
		 * timer expires the user is assumed as not available.
		 */
		int tGetTimeout;
		
		/**
		 * the value that is used for the <CODE>timerGetFloorTERMINATE</CODE>. This value
		 * is set in the Constructor with the predefined value
		 * in the <CODE>P2T</CODE> class. If the timer expires the transaction
		 * moves to the 'terminate' state.
		 */
		int tGetFloorTerminate;
		
		/**
		 * counts how many times the Floor REQUEST message is resent resp
		 * how many times the <CODE>timerGRANT</CODE> has expired.
		 */
		int counter;
		
		//only used for the collision simulation to indicate
		//that the remote user was the user that sent the
		//first REQUEST message. Used in a8.
		bool first_invitation;
		

	private:
		
		bool a0_start_reqsent( const SipSMCommand &command);
		bool a1_reqsent_reqsent( const SipSMCommand &command);
		bool a2_reqsent_completed_grant( const SipSMCommand &command);
		bool a3_reqsent_completed_request( const SipSMCommand &command);
		bool a4_reqsent_completed_timer( const SipSMCommand &command);
		bool a5_completed_terminated( const SipSMCommand &command);
		bool a6_reqsent_terminated( const SipSMCommand &command);
		bool a7_completed_completed_grant( const SipSMCommand &command);
		bool a8_completed_completed_request( const SipSMCommand &command);
		
		///the sequence number. All messages are sent with this sequence number.
		int32_t seqNo;
		
		///the remote used SSRC
		unsigned remoteSSRC; 
		
		/**
		 * the sequence number of the remote UA.
		 * in the 'completed' state the transaction will only accept REQUEST
		 * messages with this sequence number.
		 */
		int remoteSeqNo;
		
		///the SIP URI of the remote user
		 std::string user;
		
		///the Collision Counter value that is used in the Floor REQUEST messages.
		int CollisionCounter;
		
		

};

#endif

