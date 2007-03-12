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

#ifndef RTCPTRANSACTIONGRANTFLOOR_H
#define RTCPTRANSACTIONGRANTFLOOR_H

#include<libminisip/libminisip_config.h>

#include<libmsip/SipSMCommand.h>
#include<libmsip/SipTransaction.h>

#include<libminisip/signaling/p2t/P2T.h>
#include<libminisip/signaling/p2t/SipDialogP2T.h>

/** 
 * implements the Floor Request Procedure if a remote user wants to take the floor.
 * <p><b>Floor Request Procedure:</b><br>
 * If the local user wants the floor he sends a Floor REQUEST message to all other
 * users that are participating in the P2T Session and waits until every user sends
 * an answer back. If this answer is a Floor GRANT message the remote user granted
 * the floor. If the answer is a Floor REQUEST message the remote user requested the
 * floor at the same time, and we have a collision. If no answer returns, the remote 
 * user is assumed as not available at the moment.
 *
 * <p><b>RtcpTransactionGrantFloor:</b><br>
 * <CODE>RtcpTransactionGrantFloor</CODE> is used for the interaction with a user that
 * sent a Floor REQUEST message. <code>RtcpTransactionGrantFloor</code> sends a
 * GRANT message, starts a timer called timerTAKEN and waits for the Floor TAKEN 
 * message.<br>
 * Every time the timer <code>timerTAKEN</code> expires it resends the 
 * Floor REQUEST message and resets the timer. The first five times the timer expires
 * it doubles its value, after that the same value is always used.<br>
 * If the remote user sent the Floor TAKEN message the dialog <code>SipDialogP2T</code>
 * will be informed.
 *
 * <p>
 * <b>State Machine:</b><br>
 * The following state machine is implemented:<p>
 * <img src=material/RtcpTransactionGrantFloor.gif><p>
 *
 * <b>Transitions:</b>
 * <table border="0" cellspacing="0" cellpadding="5">
 * <tr bgcolor="#999999">
 * <td><font color=white><b>&nbsp;<CODE>CommandString</CODE> Input:</b></font></td>
 * <td><font color=white><b>Action:</b></font></td>
 * <td><font color=white><b>Description:</b></font></td>
 * </tr>
 * 
 * <tr><td>&nbsp;p2tSendGrant</td><td>a0_start_grantsent</td><td>
 * Sends a Floor GRANT message to the user and starts the timers.
 * </td></tr>
 * 
 * <tr bgcolor="#E6E6E6"><td>&nbsp;p2tTAKEN</td><td>a1_grantsent_completed</td>
 * <td>Informs the <CODE>SipDialogP2T</CODE> dialog that a Floor TAKEN message
 * has been received.
 * </td></tr>
 * 
 * <tr><td>&nbsp;p2tREQUEST</td><td>a2_grantsent_grantsent</td><td>
 * Resend the Floor GRANT message because the remote user sent againg
 * a Floor REQUEST message.
 * </td></tr>
 * 
 * <tr bgcolor="#E6E6E6"><td>&nbsp;timerGrantFloorTEMRINATE</td><td>a3_completed_terminated</td><td>
 * Moves to the 'terminated' state because the timer <CODE>timerGrantFloorTERMINATE</CODE> has
 * expired.
 * </td></tr>
 * 
 * <tr><td>&nbsp;timerGrantFloorTERMINATE</td><td>a4_grantsent_terminated</td><td>
 * Moves to the 'terminated' state because the timer <CODE>timerGrantFloorTERMINATE</CODE> has
 * expired.
 * </td></tr>
 * 
 *<tr bgcolor="#E6E6E6"><td>&nbsp;timerTAKEN</td><td>a5_grantsent_grantsent_timer</td><td>
 * Resends the Floor GRANT message because no Floor TAKEN message has received until now.
 * Restarts the <CODE>timerTAKEN</CODE> timer. For the first 5 times, the timer value
 * will be doubled, after that always the same value will be used.
 * </td></tr>
 *
 *<tr><td>&nbsp;p2tTAKEN</td><td>a6_completed_completed</td><td>
 * If the remote user resent the Floor TAKEN message it will be catched by this transition.
 * </td></tr></table>
 * <p>
 *
 * @see SipDialogP2T
 * @author Florian Maurer, <a href=mailto:florian.maurer@floHweb.ch>florian.maurer@floHweb.ch</a>
 */

class LIBMINISIP_API RtcpTransactionGrantFloor: public SipTransaction{
	public:
		/**
		 * Constructor.
		 * @param dialog     the dialog that started this transaction       
		 * @param seqNo      the sequence number that have to be used in the floor control messages
		 * @param toaddr     the destination IP address for the floor control messages
		 * @param port       the destination port for the floor control messages
		 * @param remoteSSRC the SSRC from the remote user. The transaction accepts only floor control
		 *                   messages with this number.
		 */
		RtcpTransactionGrantFloor(MRef<SipDialog*> dialog, int seqNo, IPAddress *toaddr, int32_t port,  std::string callId, unsigned remoteSSRC);
		
		/**
		 * Destructor
		 */
		virtual ~RtcpTransactionGrantFloor();
		
		void setCallId( std::string id){callId = id;}
		 std::string getCallId(){return callId;}

		/**
		 * returns the name of the transaction.
		 * Used by the Memory Handling of Minisip.
		 * @return the  std::string 'RtcpTransactionGrantFloor'
		 */
		virtual  std::string getName(){return "RtcpTransactionGrantFloor";}
	
		/**
		 * handles the incoming commands.
		 * The incoming commands are only accepted if the type of the command
		 * is <CODE>CommandString</CODE> and the SSRC  and used sequence number
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
		 * sends a Floor GRANT message to the user.
		 */
		void sendGrant();
		
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
		IPAddress *getAddr(){return toaddr;}
		
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
		 * returns the used sequence number in this transaction.
		 * @return <CODE>int32_t</CODE> value.
		 */		
		int32_t getSeqNo(){return seqNo;}
		
		/**
		 * report the specified status to the <CODE>GroupList</CODE> of the
		 * <CODE>SipDialogP2T</CODE> dialog.
		 * @param status the status that should be reported.
		 */
		void reportStatus(int status);
		
		/**
		 * the value that is used for the <CODE>timerTAKEN</CODE>. This value
		 * is set in the Constructor with the predefined value
		 * in the <CODE>P2T</CODE> class and will be doubled
		 * 5 times when the timer expires. It's the value we wait 
		 * before resending a Floor GRANT message. 
		 */
		int tTaken;
		
		/**
		 * the value that is used for the <CODE>timerGrantFloorTERMINATE</CODE>. This value
		 * is set in the Constructor with the predefined value
		 * in the <CODE>P2T</CODE> class. If the timer expires the transaction
		 * moves to the 'terminate' state.
		 */
		int tGrantFloorTerminate;
		
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
		 * counts how many times the Floor REQUEST message is resent resp
		 * how many times the <CODE>timerGRANT</CODE> has expired.
		 */
		int counter;

	private:

		bool a0_start_grantsent( const SipSMCommand &command);
		bool a1_grantsent_completed( const SipSMCommand &command);
		bool a2_grantsent_grantsent( const SipSMCommand &command);
		bool a3_completed_terminated( const SipSMCommand &command);
		bool a4_grantsent_terminated( const SipSMCommand &command);
		bool a5_grantsent_grantsent_timer( const SipSMCommand &command);
		bool a6_completed_completed( const SipSMCommand &command);
		
		///the sequence number. All messages are sent with this sequence number.
		int32_t seqNo;
		
		///the remote used SSRC
		unsigned remoteSSRC; 
		
		///the Collision Counter value that is used in the Floor GRANT messages.
		int CollisionCounter;
	

};


#endif

