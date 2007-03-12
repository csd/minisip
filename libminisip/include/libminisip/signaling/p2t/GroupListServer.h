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


#ifndef GROUPLISTSERVER_H
#define GROUPLISTSERVER_H

#include<libminisip/libminisip_config.h>

#include<vector>

#include<libmutil/dbg.h>
#include<libmutil/Thread.h>
#include<libmutil/MemObject.h>

#include<libmnetutil/ServerSocket.h>
#include<libmnetutil/TCPSocket.h>

#include<libminisip/signaling/p2t/P2T.h>
#include<libminisip/signaling/p2t/GroupList.h>
#include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>



/**
 * a server where other SIP User Agents can request Group Member Lists.
 * <p>
 * <b>Group List Management</b><br>
 * If a user wants to participate in a P2T-Session he has to know which
 * other UAs are participating in order to set up the required SIP Sessions
 * resp. in order to start the required <CODE>SipDialogP2Tuser</CODE> dialogs.
 * <p>
 * <code>GroupListServer</code> is a server thread where other users can request
 * Group Member List for specific P2T Sessions.
 * <p>
 * <b>Group List Protocol</b><br>
 * At the moment between a client and a Group List Server a very simple
 * protocol is used: The client can request a Group Member List with 'GET Group Identity\n\n' and the
 * server sends a XML formatted Group Member List back: 'GET \n\n XML-Code', or the client can add a user to a 
 * Group Member List with 'ADD Group Identity URI' and the server sends an 'ADD \n\n OK' back, if the 
 * user was successfully added.
 * <p>
 * @author Florian Maurer, florian.maurer@floHweb.ch
 */

class LIBMINISIP_API GroupListServer: public Runnable{
	public:
		
		/**
		 * Constructor.
		 * Creates a <code>GroupListServer</code> object and generates a Server Socket.
		 * The port value 0 means that a random port number will be chosen. The
		 * chosen port number will be entered in the SipSoftPhoneConfiguration. The server
		 * listens to the same ip address the phone has registered at the Registrar Server 
		 * resp. to the externalContactIP address that is entered in the 
		 * SipSoftPhoneConfiguration.
		 * @param config    the SipSoftPhoneConfiguration.
		 * @param localIP   the ip address of the server
		 */
		GroupListServer(MRef<SipSoftPhoneConfiguration*>config, int32_t localPort);
		
		virtual std::string getMemObjectType() const {return "GroupListServer";}
		
		/**
		 * Destructor
		 */
		virtual ~GroupListServer();
		
		/**
		 * server starts listening
		 */
		void start();
		
		/**
		 * server stops listening
		 */
		void stop();
		
		//void addGroup(GroupList *grpList);
		
		
		/**
		 * the main function for the server. It handles the incoming
		 * requests and sends the answers back.
		 */
		virtual void run();
		
		/**
		 * get the port where the server is listening to.
		 * @return the port of the server
		 */
		int getPort(){return port;}
		
		/**
		 * get the IP address of the server.
		 * @return the IP address of the server
		 */
		 std::string getIp(){return ip;}
		
		/**
		 * add a Group Member List to the server. 
		 * @param grpList the Group Member List
		 */
		void addGroupList(MRef<GroupList*> grpList);
		
		/**
		 * get a Group Member List  
		 * @param groupId the Group Identity
		 */
		MRef<GroupList*> getGroupList( std::string groupId);

		
		
	private:
		///the server socket
		ServerSocket *srv_socket;
		
		///the port of the server
		int32_t port;
		
		///the ip address of the server
		 std::string ip;
		
		///the server runs as long this variable is true
		bool running;
		
		///contains all Group Member Lists
		std::vector<MRef<GroupList*> > grpLists;
		
		///the SipSoftPhoneConfiguration
		MRef<SipSoftPhoneConfiguration*>phoneconfig;
		

		/**
		 * Checks if the string specified in <code>line</code> starts
		 * with the string specified in <code>part</code>.
		 * @return true/false
		 */
		bool starts_with( std::string line,  std::string part);
};

#endif

