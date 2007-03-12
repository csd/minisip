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


#ifndef GROUPLISTCLIENT_H
#define GROUPLISTCLIENT_H

#include<libminisip/libminisip_config.h>

#include<libmutil/MemObject.h>

#include<libmnetutil/IPAddress.h>

#include<libminisip/signaling/p2t/P2T.h>
#include<libminisip/signaling/p2t/GroupList.h>


/**
 * Connects to a server for retrieving the Group Member List
 * for a P2T Session.
 * <p>
 * <b>Group List Management</b><br>
 * If a user wants to participate in a P2T-Session he has to know which
 * other UAs are participating in order to set up the required SIP Sessions
 * resp. in order to start the required <CODE>SipDialogP2Tuser</CODE> dialogs.
 * <p>
 * <CODE>GroupListClient</CODE> offers two possibilities to retrieve a  
 * <CODE>GroupList</CODE> object. First it can connect to the
 * Group List Server from another user and request the Group Member List
 * or second it can connect to a HTTP Server and download a file containing
 * the desired Group Member List.
 * <p>
 * <b>Group List Protocol</b><br>
 * At the moment between the Group List Client and Group List Server a very simple
 * protocol is used: The client can request a Group Member List with 'GET Group Identity\n\n' and the
 * server sends a XML formatted Group Member List back, or the client can add a user to a 
 * Group Member List with 'ADD Group Identity URI' and the server sends an OK back, if the 
 * user was successfully added.
 * <p>
 * @author Florian Maurer, <a href=mailto:florian.maurer@floHweb.ch>florian.maurer@floHweb.ch</a>
 */

class LIBMINISIP_API GroupListClient: public MObject{
	public:
		
		/**
		 * Constructor
		 */
		GroupListClient();
		
		/**
		 * Destructor
		 */
		virtual ~GroupListClient();

		virtual std::string getMemObjectType() const {return "GroupListClient";}
		
		/**
		 * connects to the specified GroupList Server and returns
		 * a <code>GroupList</code> object for the specified 
		 * Group Identity. The server's response has to contain a 
		 * XML formatted Group Member List.
		 * @param GroupId  the group identity for which the GroupList should be
		 *                 downloaded from the server 
		 * @param srv_addr the address of the GroupList Server
		 * @param port     the port of the GroupList Server
		 * @return a <code>GroupList</code> object
		 */
		MRef<GroupList*> getGroupList( std::string GroupId, char *srv_addr, int port);
		
		/**
		 * connects to a HTTP server and download the specified file. The file must
		 * contain a XML formatted Group Member List. 
		 * @param file     the file containing the xml-code (e.g. '/grouplist.xml')
		 * @param srv_addr the address of the server
		 * @param port     the port of the server
		 * @return a <code>GroupList</code> object
		 */
		MRef<GroupList*> downloadGroupList( std::string file, char *srv_addr, int port);
	
		
	private:
		/**
		 * This function is used by <CODE>getGroupList</CODE> to
		 * connect to the server.
		 * @return  std::string with the GroupList in xml code
		 * @param GroupId  the group identity for which the GroupList should
		 *                 be downloaded
		 * @param srv_addr the address of the GroupList Server
		 * @param port     the port of the GroupList Server
		 */
		 std::string connectServer( std::string GroupId, char *srv_addr, int port);
		
		/**
		 * returns true if the  std::string <CODE>line</CODE> starts with the
		 *  std::string specified in <code>part</code>.
		 * @param line 
		 * @param part the  std::string where <CODE>line</CODE> has to start with
		 * @return true or false
		 */
		bool starts_with( std::string line,  std::string part);
};

#endif
