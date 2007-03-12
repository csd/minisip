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

#ifndef RtcpTransactionTakenFloor_H
#define RtcpTransactionTakenFloor_H

#include<libminisip/libminisip_config.h>

#include<libmsip/SipSMCommand.h>
#include<libmsip/SipTransaction.h>

#include<libminisip/signaling/p2t/P2T.h>
#include<libminisip/signaling/p2t/SipDialogP2T.h>

/** 
 * sends a Floor TAKEN message to a remote user.
 * <p><b>RtcpTransactionTakenFloor</b><br>
 * If all users in a Floor Request Procedure have granted the floor,
 * <CODE>RtcpTransactionTakenFloor</CODE> is used to send a Floor TAKEN message
 * to a remote user. If this message gets lost, the remote user will resend the
 * Floor GRANT message and causes this transaction to resend the Floor TAKEN message.
 * <b>State Machine:</b><br>
 * The following state machine is implemented:<p>
 * <img src=material/RtcpTransactionTakenFloor.gif><p>
 *
 * <b>Transitions:</b>
 * <table border="0" cellspacing="0" cellpadding="5">
 * <tr bgcolor="#999999">
 * <td><font color=white><b>&nbsp;<CODE>CommandString</CODE> Input:</b></font></td>
 * <td><font color=white><b>Action:</b></font></td>
 * <td><font color=white><b>Description:</b></font></td>
 * </tr>
 * 
 * <tr><td>&nbsp;p2tSendTaken</td><td>a0_start_idlesent</td><td>
 * Sends a Floor TAKEN message.
 * </td></tr>
 * 
 * <tr bgcolor="#E6E6E6"><td>&nbsp;p2tGRANT</td><td>a1_takensent_takensent</td>
 * <td> Sends a Floor TAKEN message because another Floor GRANT message from the
 * remote user has arrived.
 * </td></tr>
 * 
 * <tr><td>&nbsp;timerIdleFloorTERMINATE</td><td>a2_idlesent_terminated</td><td>
 * Moves to the terminated state.
 * </td></tr></table>
 * <p>
 *
 * @see SipDialogP2T
 * @author Florian Maurer, <a href=mailto:florian.maurer@floHweb.ch>florian.maurer@floHweb.ch</a>
 */

class LIBMINISIP_API RtcpTransactionTakenFloor: public SipTransaction{
	public:
		
		/**
		 * Constructor.
		 * @param call    the dialog that started this transaction       
		 * @param seqNo      the sequence number that have to be used in the floor control messages
		 * @param toaddr     the destination IP address for the floor control messages
		 * @param port       the destination port for the floor control messages
		 * @param remoteSSRC the SSRC from the remote user. The transaction accepts only floor control
		 *                   messages with this number.
		 */
		RtcpTransactionTakenFloor(MRef<SipDialog*> call, int seqNo, IPAddress *toaddr, int32_t port,  std::string callId, unsigned remoteSSRC);
		
		/**
		 * Destructor
		 */
		virtual ~RtcpTransactionTakenFloor();

		void setCallId( std::string id){callId = id;}
		 std::string getCallId(){return callId;}

		/**
		 * returns the name of the transaction.
		 * Used by the Memory Handling of Minisip.
		 * @return the  std::string 'RtcpTransactionTakenFloor'
		 */
		virtual  std::string getName(){return "RtcpTransactionTakenFloor";}

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
		 * sends a Floor TAKEN message to the user.
		 */
		void sendTaken();
		
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
		IPAddress *getAddress(){return toaddr;}
		
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
		 * the value that is used for the <CODE>timerTakenFloorTERMINATE</CODE>. 
		 * This value
		 * is set in the Constructor with the predefined value
		 * in the <CODE>P2T</CODE> class. If the timer expires the transaction
		 * moves to the 'terminate' state.
		 */
		int tTakenFloorTerminate;

	private:

		bool a0_start_takensent( const SipSMCommand &command);
		bool a1_takensent_takensent( const SipSMCommand &command);
		bool a2_takensent_terminated( const SipSMCommand &command);
			
		///the sequence number. All messages are sent with this sequence number.
		int32_t seqNo;
		
		///the remote used SSRC
		unsigned remoteSSRC;
};

#endif

