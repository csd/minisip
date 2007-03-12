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
 
#ifndef RTCPRECEIVER_H
#define RTCPRECEIVER_H

#include<libminisip/libminisip_config.h>

#include<libmutil/Thread.h>

#include<libmnetutil/UDPSocket.h>

#include<libmsip/SipDialog.h>

#include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>
#include<libminisip/gui/Gui.h>
#include<libminisip/signaling/p2t/P2T.h>

/**
 * receives RTCP packets.
 * <p>
 * opens a UDP socket to receive RTCP packets. At the moment
 * it's assumed that this port is one port above the
 * port the <code>SoundReceiver</code>  is using for the RTP packets.
 * <p>
 * RTCP packets are used for the Floor Control in a P2T Session. So every
 * <code>SipDialogP2T</code>dialog has its own <code>RtcpReceiver</code>.
 * The receiver parses the incoming messages (currently only Floor 
 * Control messages) and sends the information in a <code>SipSMCommand</code>
 * containing a <code>CommandString</code>
 * via the Dialog Container to the <code>SipDialogP2T</code> dialog.<p>
 * The <code>CommandString</code>s contain the following values:
 * <table cellspacing="0" cellpadding="0">
 * <tr>
 * <td>DestinationID:</td><td>The CallID of the <code>SipDialogP2T</code> dialog.</td>
 * </tr>
 * <tr>
 * <td>Op:</td><td>
 * The name of the received packet. For example: p2tREQUEST, p2tGRANT, ...</td>
 * </tr>
 * <tr>
 * <td>Param:</td><td>The SSRC from the received message.</td>
 * </tr>
 * <tr>
 * <td>Param2:</td><td>The sequence number</td>
 * </tr>
 * <tr>
 * <td>Param3:</td><td>
 * contains extra information from the message as for example the SIP URI or
 * warning code, or collision flag and so on.</td>
 * </tr>
 * </table>
 * <p>
 *
 * @author Florian Maurer, florian.maurer@floHweb.ch
 */
class LIBMINISIP_API RtcpReceiver : public Runnable{
	public:
		
		/**
		 * Constructor.
		 * @param config   the phone configuration 
		 * @param RTPport  the port where the <code>SoundListener</code> listens to the 
		 *                 RTP packets. <code>RtcpReceiver</code> will open the UDP socket
		 *                 one port above this port.
		 */
		RtcpReceiver(MRef<SipSoftPhoneConfiguration*> config, int RTPport);
		
		/**
		 * Deconstructor.
		 */
		virtual ~RtcpReceiver();
		
		/**
		 * return the opened UDP socket.
		 * @return <code>UDPSocket</code>
		 */
		UDPSocket *getSocket(){return rtcp_sock;}
		
		/**
		 * the main function when the thread is started.
		 */
		virtual void run();
		
		/**
		 * starts listening to incoming packets.
		 */
		void start();
		
		/**
		 * stops listening to incoming packets.
		 */
		void stop();
		
		/**
		 * flushes the socket.
		 */
		void flush();
		
		/**
		 * return the dialog that is connected with this <code>RtcpReceiver</code>.
		 * @return <code>SipDialog</code>
		 */
		MRef<SipDialog*> getCall(){return call;}
		
		/**
		 * set the dialog for which the received messages should be delivered.
		 * @param dialog  a <code>SipDialog</code>
		 */
		void setCall(MRef<SipDialog*> dialog);
		
		/**
		 * get the IP address of the RTCP receiver.
		 * @return string containing the IP address of the server
		 */
		 std::string getContactIp(){return contactMediaIP;}
		
		/**
		 * get the port of the RTCP receiver.
		 * @return port number
		 */
                int getContactPort(){return contactMediaPort;}

	
	private:
		///flushes the socket
		void do_flush_socket();
		
		///UDP Socket that is listening
		UDPSocket *rtcp_sock;
		
		///connected SipDialog
		MRef<SipDialog*> call;
		
		///can be set to flush the socket
		bool flush_flag;
		
		///the IP address where the receiver is listening.
		 std::string contactMediaIP;
		
		///the port where the receiver is listening.
		int contactMediaPort;
		
		///can be used to start/stop the receiver
		bool stopped;	
		
};
#endif
