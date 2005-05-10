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

#ifndef MESSAGE_ROUTER_H
#define MESSAGE_ROUTER_H

#include<libmsip/SipCallback.h>
#include<libmsip/SipInvite.h>
#include<libmsip/SipResponse.h>
#include<libmutil/minilist.h>
//#include"SoundSender.h"
//#include"SoundReceiver.h"
#include"gui/GuiCallback.h"
//#include"sip/SipStateMachine.h"
#include"../sip/Sip.h"
#include"gui/Gui.h"
#include"../conf/ConferenceControl.h" 
#include "../conf/ConfCallback.h"
#include"../sip/SipSoftPhoneConfiguration.h"

#include<config.h>


class MessageRouter: public SipCallback, public GuiCallback, public ConfCallback{
	public:
		MessageRouter();
		virtual ~MessageRouter(){}
		
		void setSip(MRef<Sip*> ssp);
		void setGui(Gui *guiptr){gui = guiptr;};
		virtual void setConferenceController(ConferenceControl *conf);
		virtual void removeConferenceController(ConferenceControl *conf);
		//void setConfControl(ConferenceControl *confptr){conf = confptr;};
		//void setConference(ConferenceControl *confptr){conf = confptr;};//bm
		void setMediaHandler(MRef<MediaHandler *> mediaHandler){
			this->mediaHandler = mediaHandler;}

		virtual void sipcb_handleCommand(CommandString &command);
		virtual void sipcb_handleConfCommand(CommandString &command);
		//virtual void sipcb_handleConfCommand(CommandString &command);
		//virtual void confcb_handleSipCommand(CommandString &command);
		//void confcb_handleGuiCommand(CommandString &command);
		virtual void guicb_handleCommand(CommandString &command);
		virtual void guicb_handleConfCommand(string &conferencename);
		virtual void guicb_handleConfCommand(CommandString &command);
		virtual string guicb_confDoInvite(string sip_url);
		virtual void guicb_handleMediaCommand(CommandString &command);
		
		virtual string guicb_doInvite(string sip_url);
		
		virtual string confcb_doJoin(string user, minilist<ConfMember> *list, string congId);
		virtual string confcb_doConnect(string user, string confId);
		//virtual void guicb_handleConfCommand(ConferenceControl *conf){}
		virtual void confcb_handleSipCommand(string &command){}
		virtual void confcb_handleSipCommand(CommandString &command);
		virtual void confcb_handleGuiCommand(CommandString &command);	
		virtual ConferenceControl* getConferenceController(string confid);
	private:
		
		Gui *gui;
		minilist<ConferenceControl *> confrout;//bm
		MRef<Sip*> sip;
		MRef<MediaHandler *> mediaHandler;
};

#endif
