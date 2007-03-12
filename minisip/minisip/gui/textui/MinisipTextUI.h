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
#include<libminisip/signaling/conference/ConferenceControl.h>
#include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>
#include<libminisip/gui/Gui.h>
#include<libminisip/signaling/p2t/GroupList.h>
#include<libminisip/signaling/p2t/GroupListClient.h>
#include<libminisip/signaling/p2t/GroupListUserElement.h>
#include<libminisip/signaling/p2t/P2T.h>
#include<libmutil/TextUI.h>
#include<libmutil/minilist.h>
#include<libmutil/Semaphore.h>

#include<string>

/**
 * A text user interface. 
 * The interface interacts with the user via the terminal, and with the <code>Sip</code> class
 * with <code>SipSMCommands</code> via the <code>MessageRouter</code>.
 */
class MinisipTextUI: public Gui, public TextUI, public TextUICompletionCallback{
	public:
		MinisipTextUI();
	
		std::string getMemObjectType() const {return "MinisipTextUI";}
		
		virtual void handleCommand(const CommandString&);
		virtual void setSipSoftPhoneConfiguration(MRef<SipSoftPhoneConfiguration *>sipphoneconfig);
		virtual void setContactDb(MRef<ContactDb *>){};
		virtual bool configDialog( MRef<SipSoftPhoneConfiguration *> conf );
	
		virtual void displayErrorMessage(std::string msg);
	
		virtual void run();
	
		virtual void guimain();
		virtual void setCallback(MRef<CommandReceiver*> callback);
	
		virtual void keyPressed(int key);
		virtual void guiExecute(std::string cmd);
		virtual void guiExecute(const MRef<QuestionDialog*> &d);
		virtual minilist<std::string> textuiCompletionSuggestion(std::string match);
	
	private:
		
		void showMem();
		
		ConferenceControl *currentconf;
		std::string currentconfname;
		std::string currentcaller;
		std::string input;
		std::string callId;
		std::string state;
		MRef<SipSoftPhoneConfiguration *> config;
		bool autoanswer;
		MRef<Semaphore *> semSipReady;
		
		
		///indicates that the user is in a call and cannot answer any other incoming calls
		bool inCall;

		///indicates that the TextUI is in the P2T Mode
		bool p2tmode;
#ifdef P2T_SUPPORT
		
		///the P2T Group Identity
		std::string p2tGroupId;
		
		///the user which invited to a P2T Session
		std::string inviting_user;
		
		///the dialog callID from the inviting user
		std::string inviting_callId;
		
		///a P2T Group Member List
		MRef<GroupList*>grpList;
		
		///shows the GroupList
		void showGroupList();
		
		///shows a P2T help
		void showP2TInfo();

#endif
};

#endif
