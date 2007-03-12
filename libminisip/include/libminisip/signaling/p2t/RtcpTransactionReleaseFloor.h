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
 * 	RtcpTransactionReleaseFloor.h
 * Author
 * 	Florian Maurer, florian.maurer@floHweb.ch
 * Purpose
 * 
*/



#ifndef RTCPTRANSACTIONRELEASEFLOOR_H
#define RTCPTRANSACTIONRELEASEFLOOR_H

#include<libminisip/libminisip_config.h>

#include<libmsip/SipSMCommand.h>
#include<libmsip/SipTransaction.h>

#include<libminisip/signaling/p2t/P2T.h>

/** 
 * implements the Floor Release Procedure if the local user releases the floor.
 * <p><b>Floor Release Procedure:</b><br>
 * When a user wants to release the floor he repeatedly sends Floor RELEASE 
 * messages to every other user until he gets a Floor IDLE message from each of
 * them back.
 *
 * <p><b>RtcpTransactionReleaseFloor</b><br>
 * <CODE>RtcpTransactionReleaseFloor</CODE> is used for the interaction with one user
 * in a Floor Release Procedure. Every time the dialog <code>SipDialogP2T</code> wants
 * to leave the 'talk' state respectively to release the fllor, a <code>
 * RtcpTransactionReleaseFloor</code> transaction for every user contained in the
 * <code>GroupList</code> is started that interacts with that user.<p>
 *
 * <code>RtcpTransactionReleaseFloor</code> sends a Floor RELEASE message, starts
 * a timer called timerIDLE and waits for a Floor IDLE message as answer. Every
 * time the timer <code>timerIDLE</code> expires it resends the Floor RELEASE message
 * and resets the timer. The first five times the timer expires it doubles its value,
 * after that the same value is always used.<p>
 *
 * If the Floor IDLE message arrives or the timer <code>timerRelTIMEOUT</code> expires
 * it informs the dialog <code>SipDialogP2T</code> and enters the status into
 * the Group Member List.<p>
 *
 * <b>State Machine:</b><br>
 * The following state machine is implemented:<p>
 * <img src=material/RtcpTransactionReleaseFloor.gif><p>
 *
 * <b>Transitions:</b>
 * <table border="0" cellspacing="0" cellpadding="5">
 * <tr bgcolor="#999999">
 * <td><font color=white><b>&nbsp;<CODE>CommandString</CODE> Input:</b></font></td>
 * <td><font color=white><b>Action:</b></font></td>
 * <td><font color=white><b>Description:</b></font></td>
 * </tr>
 * 
 * <tr><td>&nbsp;p2tReleaseFloor</td><td>a0_start_relsent</td><td>
 * Sends a Floor IDLE message to the user and starts the timers.
 * </td></tr>
 * 
 * <tr bgcolor="#E6E6E6"><td>&nbsp;timerIDLE</td><td>a1_relsent_relsent</td>
 * <td>Resends the Floor IDLE message because the timerIDLE has expired and restarts
 * the timerIDLE. The first 5 times it doubles first the value of the timer, after that
 * it takes always the same value.
 * </td></tr>
 * 
 * <tr><td>&nbsp;p2tIDLE</td><td>a2_relsent_completed_idle</td><td>
 * Moves to the 'completed' state because a Floor IDLE message has arrived.
 * </td></tr>
 * 
 * <tr bgcolor="#E6E6E6"><td>&nbsp;timerRelTIMEOUT</td><td>a3_relsent_completed_timer</td><td>
 * The time we are willing to wait for a Floor IDLE message has expired. We do not resend any
 * longer Floor RELEASE messages.
 * </td></tr>
 * 
 * <tr><td>&nbsp;timerRelFloorTERMINATE</td><td>a4_completed_terminated</td><td>
 *  Moves to the 'terminated' state because the timer <CODE>timerGrantFloorTERMINATE</CODE> has
 * expired.
 * </td></tr>
 * 
 *<tr bgcolor="#E6E6E6"><td>&nbsp;timerRElFloorTERMINATE</td><td>a5_relsent_terminated</td><td>
 * Moves to the 'terminated' state because the timer <CODE>timerGrantFloorTERMINATE</CODE> has
 * expired.
 * </td></tr>
 *
 *<tr><td>&nbsp;p2tIDLE</td><td>a6_completed_completed</td><td>
 * If the remote user resent the Floor IDLE message, it will be catched by this
 * transition.
 * </td></tr></table>
 * <p>
 *
 * @see SipDialogP2T
 * @see GroupList
 * @author Florian Maurer, <a href=mailto:florian.maurer@floHweb.ch>florian.maurer@floHweb.ch</a>
 */

class LIBMINISIP_API RtcpTransactionReleaseFloor: public SipTransaction{
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
		RtcpTransactionReleaseFloor(MRef<SipDialog*> call, int seqNo, IPAddress *toaddr, int32_t port,  std::string callId, unsigned remoteSSRC);
		
		/**
		 * Destructor
		 */
		virtual ~RtcpTransactionReleaseFloor();

		void setCallId( std::string id){callId = id;}
		 std::string getCallId(){return callId;}

		/**
		 * returns the name of the transaction.
		 * Used by the Memory Handling of Minisip.
		 * @return the string 'RtcpTransactionIdleFloor'
		 */
		virtual  std::string getName(){return "RtcpTransactionReleaseFloor";}
		
		/**
		 * handles the incoming commands.
		 * The incoming commands are only accepted if the SSRC  
		 * and used sequence number
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
		 * sends a Floor RELEASE message to the user.
		 */
		void sendRelease();
		
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
		unsigned getSSRC(){return remoteSSRC;}
		
		
		/**
		 * report the specified status to the <CODE>GroupList</CODE> of the
		 * <CODE>SipDialogP2T</CODE> dialog.
		 * @param status the status that should be reported.
		 */
		void reportStatus(int status);
		
		
		/**
		 * the value that is used for the <CODE>timerIDLE</CODE>. This value
		 * is set in the Constructor with the predefined value
		 * in the <CODE>P2T</CODE> class and will be doubled
		 * 5 times when the timer expires. It's the value we wait 
		 * before resending a Floor RELEASE message. 
		 */
		 int tIdle;
		
		/**
		 * the value that is used for the <CODE>timerRelTIMEOUT</CODE>. This value
		 * is set in the Constructor with the predefined value
		 * in the <CODE>P2T</CODE> class. It's the value we are willing
		 * to wait for Floor IDLE message. 
		 */
		int tRelTimeout;
		
		/**
		 * the value that is used for the <CODE>timerReleaseFloorTERMINATE</CODE>. This value
		 * is set in the Constructor with the predefined value
		 * in the <CODE>P2T</CODE> class. If the timer expires the transaction
		 * moves to the 'terminate' state.
		 */
		int tRelFloorTerminate;
		
		/**
		 * counts how many times the Floor RELEASE message is resent resp
		 * how many times the <CODE>timerIDLE</CODE> has expired.
		 */
		int counter;

	private:

		bool a0_start_relsent( const SipSMCommand &command);
		bool a1_relsent_relsent( const SipSMCommand &command);
		bool a2_relsent_completed_idle( const SipSMCommand &command);
		bool a3_relsent_completed_timer( const SipSMCommand &command);
		bool a4_completed_terminated( const SipSMCommand &command);
		bool a5_relsent_terminated( const SipSMCommand &command);
		bool a6_completed_completed( const SipSMCommand &command);
		
		///the sequence number. All messages are sent with this sequence number.
		int32_t seqNo;
		///the remote used SSRC
		unsigned remoteSSRC;
};

#endif

