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

#ifndef _MINISIPTEXTUI_H
#define _MINISIPTEXTUI_H

#include<libmutil/CommandString.h>
#include"../../../conf/ConferenceControl.h"
#include"../../../sip/SipSoftPhoneConfiguration.h"
#include"../Gui.h"
#include"../../../p2t/GroupList.h"
#include"../../../p2t/GroupListClient.h"
#include"../../../p2t/GroupListUserElement.h"
#include"../../../p2t/P2T.h"
#include<libmutil/TextUI.h>
#include<libmutil/minilist.h>

/**
 * A text user interface. 
 * The interface interacts with the user via the terminal, and with the <code>Sip</code> class
 * with <code>SipSMCommands</code> via the <code>MessageRouter</code>.
 */
class MinisipTextUI: public Gui, public TextUI, public TextUICompletionCallback{
    public:
        MinisipTextUI();
        virtual void handleCommand(CommandString);
        virtual void setSipSoftPhoneConfiguration(MRef<SipSoftPhoneConfiguration *>sipphoneconfig);
        virtual void setContactDb(MRef<ContactDb *>){};
        virtual bool configDialog( MRef<SipSoftPhoneConfiguration *> conf );

	virtual void displayErrorMessage(string msg);

	virtual void run();

	virtual void keyPressed(int key);
	virtual void guiExecute(string cmd);
	virtual minilist<std::string> textuiCompletionSuggestion(string match);
    private:
	
        void showMem();
        void showCalls(string);
        void showStat(string);
        void showTransactions(string);
        void showTimeouts(string);
	void showDialogInfo(MRef<SipDialog*> d, bool usesStateMachine, string header);
	
	ConferenceControl *currentconf;
	string currentconfname;
        string input;
        string callId;
        string state;
        MRef<SipSoftPhoneConfiguration *> config;
	bool autoanswer;
	
	///indicates that the user is in a call and cannot answer any other incoming calls
	bool inCall;
	
	///indicates that the TextUI is in the P2T Mode
	bool p2tmode;
	
	///the P2T Group Identity
	string p2tGroupId;
	
	///the user which invited to a P2T Session
	string inviting_user;
	
	///the dialog callID from the inviting user
	string inviting_callId;
	
	///a P2T Group Member List
	MRef<GroupList*>grpList;
	
	///shows the GroupList
	void showGroupList();
	
	///shows a P2T help
	void showP2TInfo();
};

#endif
