/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef _CONFERENCECONTROL_H
#define _CONFERENCECONTROL_H

#include<config.h>

#include<string>

#include<libmutil/CommandString.h>
#include<libmutil/minilist.h>

#include "ConfCallback.h"
#include "ConfMember.h"

/**
 * A text user interface. 
 * The interface interacts with the user via the terminal, and with the <code>Sip</code> class
 * with <code>SipSMCommands</code> via the <code>MessageRouter</code>.
 */
typedef struct CCList {
    string uris[10];
    string callids[10];
    int numUser;
} cclist;

class ConferenceControl{
    public:
        ConferenceControl();
	ConferenceControl(string configUri, string cid, bool islocal);
	//void setGui(Gui *guiptr){gui = guiptr;};
	void handleGuiCommand(string cmd);
	void handleGuiCommand(const CommandString &command);
	void handleGuiDoInviteCommand(string sip_url);
	
	void handleSipCommand(const CommandString &cmd);
        //virtual void handleGuiCommand(string);
	void setCallback(ConfCallback *cb);
	void setPendingList(string user);
	//string[10] getPendingList();
	void setConnectedList(string user);
	//string[10] getConnectedList();
	ConfCallback* getCallback();
	
	
	/**
	* Moves a member from pending to connected and look for new members
	*/
	void handleOkAck(string callid, minilist<ConfMember> *list);
	
	/**
	* Print a list of conference members
	*/
	void printList(minilist<ConfMember> *list);
        minilist<ConfMember> * getConnectedList();
        
	//virtual void run();
	string confId;
    private:
	void sendUpdatesToGui();
	/**
	* Move a member from pending to connected status
	*/
	void pendingToConnected(string memberid);
	/**
	* Check for new members to connect to
	*/
	void updateLists(minilist<ConfMember> *list);
	void removeMember(string memberid); 
	string addDomainToPrefix(string remoteUri);
	//Gui *gui;
	bool incoming;
        string input;
        string callId;
        string state;
	ConfCallback *callback;
	int numConnected;
	int numPending;
	//string connectedList[10];

	
	//string pendingList[10];
	//cclist connectedList;
	
	minilist<ConfMember> connectedList;
	minilist<ConfMember> pendingList;
	string myUri;
	string myDomain;
	
	///a P2T Group Member List
	//MRef<GroupList*>grpList;
	
	//shows the GroupList
	//void showGroupList();
	
	
};

#endif
