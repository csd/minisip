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

#ifndef RTCPSENDER_H
#define RTCPSENDER_H

#include<libminisip/libminisip_config.h>

#ifdef HAVE_NETINET_IN_H
#	include<sys/socket.h>
#	include<netinet/in.h>
#endif

#ifdef WIN32
#	include<winsock2.h>
#endif

#include<libmnetutil/IPAddress.h>
#include<libmnetutil/UDPSocket.h>
#include<libminisip/signaling/p2t/P2T.h>

#include<libmsip/SipDialog.h>


/**
 * sends RTCP packets.
 * <p>
 * RTCP packets are used for the Floor Control in a P2T Session. Thus every <code>SipDialogP2T</code>
 * has a <code>RtcpSender</code> for sending Floor Control messages to other users. At the moment
 * only RTCP APP packets and especially Floor Control Messages are implemented.
 */

class LIBMINISIP_API RtcpSender{
	public:
		
		/**
		 * Constructor.
		 * @param rtcp_socket the socket that can be used to send RTCP messages.
		 */
		RtcpSender(UDPSocket *rtcp_socket);
		

		/**
		 * sends a RTCP APP packet without application data.
		 * @param subtype the subype value
		 * @param ssrc    the ssrc
		 * @param name    the name
		 * @param to      the destination address
		 * @param port    the destination port
		 */
		void send_APP(unsigned subtype, unsigned ssrc,  std::string name, IPAddress *to, int port);

		/**
		 * sends a RTCP APP packet with application data especially for floor
		 * control messages.
		 * @param subtype  the subtype value
		 * @param ssrc     the ssrc
		 * @param name     the name
		 * @param to       the destination address
		 * @param port     the destination port
		 * @param seqNo    the sequence number
		 * @param optional the optional field. Used for warning code, deny reason and collision flag.
		 */
		void send_APP_FC(unsigned subtype, unsigned ssrc,  std::string name, IPAddress *to, int port,
				int seqNo,
				int optional=0);
		
		/**
		 * get the call, that started this <code>RtcpSender</code>.
		 * @return <code>SipDialog</code>
		 */
		MRef<SipDialog*> getCall() {return call;}
		
		/**
		 * set the call, that started this <code>RtcpSender</code>.
		 * @param call the concerning <code>SipDialog</code>
		 */
		void setCall(MRef<SipDialog*> call);

	private:
		int seqNo;
		///UDP socket for sending the packets.
		UDPSocket *rtcp_sock;
		///connected <code>SipDialog</code>
		MRef<SipDialog*> call;
		int32_t fd;
};


#endif
