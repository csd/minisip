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

#ifndef CONF_MESSAGE_ROUTER_H
#define CONF_MESSAGE_ROUTER_H

#include<config.h>

#include<libmutil/minilist.h>
#include"../sip/Sip.h"
#include"gui/Gui.h"
#include"../conf/ConferenceControl.h" 
#include "../conf/ConfCallback.h"
#include"../sip/SipSoftPhoneConfiguration.h"

class ConfMessageRouter: 	//public SipCallback, 
			//public GuiCallback, 
			public ConfCallback, public CommandReceiver{
	public:
		ConfMessageRouter();
		virtual ~ConfMessageRouter();
		
		void handleCommand(string subsystem, const CommandString &cmd){
			if (subsystem=="sip_conf"){
				sipcb_handleConfCommand(cmd);
				return;
			}
			
			assert(1==0);
		}

		CommandString handleCommandResp(string subsystem, const CommandString &cmd){
			
			CommandString ret("","command_not_understood");
			return ret;
		}
		
		void setSip(MRef<Sip*> ssp);
		void setGui(MRef<Gui *> guiptr){gui = guiptr;};
		virtual void setConferenceController(ConferenceControl *conf);
		virtual void removeConferenceController(ConferenceControl *conf);
		void setMediaHandler(MRef<MediaHandler *> mediaHandler){
			this->mediaHandler = mediaHandler;}

		virtual void sipcb_handleCommand(const CommandString &command);
		virtual void sipcb_handleConfCommand(const CommandString &command);
		virtual void guicb_handleCommand(const CommandString &command);
		virtual void guicb_handleConfCommand(const string &conferencename);
		virtual void guicb_handleConfCommand(const CommandString &command);
		virtual void guicb_handleMediaCommand(const CommandString &command);
		
		virtual string confcb_doJoin(string user, minilist<ConfMember> *list, string congId);
		virtual string confcb_doConnect(string user, string confId);
		virtual void confcb_handleSipCommand(string &command){}
		virtual void confcb_handleSipCommand(const CommandString &command);
		virtual void confcb_handleGuiCommand(const CommandString &command);	
		virtual ConferenceControl* getConferenceController(string confid);
	private:
		
		MRef<Gui *> gui;
		minilist<ConferenceControl *> confrout;//bm
		MRef<Sip*> sip;
		MRef<MediaHandler *> mediaHandler;
};

#endif
