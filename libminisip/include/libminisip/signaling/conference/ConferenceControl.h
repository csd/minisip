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

#ifndef _CONFERENCECONTROL_H
#define _CONFERENCECONTROL_H

#include<libminisip/libminisip_config.h>

#include<string>

#include<libmutil/CommandString.h>
#include<libmutil/minilist.h>

#include<libminisip/signaling/conference/ConfCallback.h>
#include<libminisip/signaling/conference/ConfMember.h>

/**
 * A text user interface. 
 * The interface interacts with the user via the terminal, and with the <code>Sip</code> class
 * with <code>SipSMCommands</code> via the <code>MessageRouter</code>.
 */
typedef struct CCList {
    std::string uris[10];
    std::string callids[10];
    int numUser;
} cclist;

class LIBMINISIP_API ConferenceControl{
    public:
        ConferenceControl();
	ConferenceControl(std::string configUri, std::string cid, bool islocal);
	//void setGui(Gui *guiptr){gui = guiptr;};
	void handleGuiCommand(std::string cmd);
	void handleGuiCommand(const CommandString &command);
	void handleGuiDoInviteCommand(std::string sip_url);
	
	void handleSipCommand(const CommandString &cmd);
        //virtual void handleGuiCommand(std::string);
	void setCallback(ConfCallback *cb);
	void setPendingList(std::string user);
	//std::string[10] getPendingList();
	void setConnectedList(std::string user);
	//std::string[10] getConnectedList();
	ConfCallback* getCallback();
	
	
	/**
	* Moves a member from pending to connected and look for new members
	*/
	void handleOkAck(std::string callid, minilist<ConfMember> *list);
	
	/**
	* Print a list of conference members
	*/
	void printList(minilist<ConfMember> *list);
        minilist<ConfMember> * getConnectedList();

	//virtual void run();
	std::string confId;
    private:
	void sendUpdatesToGui();
	/**
	* Move a member from pending to connected status
	*/
	void pendingToConnected(std::string memberid);
	/**
	* Check for new members to connect to
	*/
	void updateLists(minilist<ConfMember> *list);
	void removeMember(std::string memberid); 
	std::string addDomainToPrefix(std::string remoteUri);
	//Gui *gui;
	bool incoming;
        std::string input;
        std::string callId;
        std::string state;
	ConfCallback *callback;
	int numConnected;
	int numPending;
	//std::string connectedList[10];

	
	//std::string pendingList[10];
	//cclist connectedList;
	
	minilist<ConfMember> connectedList;
	minilist<ConfMember> pendingList;
	std::string myUri;
	std::string myDomain;
	
	///a P2T Group Member List
	//MRef<GroupList*>grpList;
	
	//shows the GroupList
	//void showGroupList();
	
	
};

#endif
