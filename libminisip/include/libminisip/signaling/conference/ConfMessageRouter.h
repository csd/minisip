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

#ifndef CONF_MESSAGE_ROUTER_H
#define CONF_MESSAGE_ROUTER_H

#include<libminisip/libminisip_config.h>

#include<libmutil/minilist.h>

#include<libminisip/signaling/sip/Sip.h>
#include<libminisip/gui/Gui.h>
#include<libminisip/signaling/conference/ConferenceControl.h> 
#include<libminisip/signaling/conference/ConfCallback.h>

class Gui;

class LIBMINISIP_API ConfMessageRouter: 	//public SipCallback, 
			//public GuiCallback, 
			public ConfCallback, public CommandReceiver{
	public:
		ConfMessageRouter();
		virtual ~ConfMessageRouter();
		
		void handleCommand(std::string subsystem, const CommandString &cmd){
			if (subsystem=="sip_conf"){
				sipcb_handleConfCommand(cmd);
				return;
			}
			
			assert(1==0);
		}

		CommandString handleCommandResp(std::string /*subsystem*/, const CommandString &/*cmd*/){
			
			CommandString ret("","command_not_understood");
			return ret;
		}
		
		void setSip(MRef<Sip*> ssp);
		void setGui(MRef<Gui *> g){gui = g;};
		virtual void setConferenceController(ConferenceControl *conf);
		virtual void removeConferenceController(ConferenceControl *conf);
		void setMediaHandler(MRef<SubsystemMedia*> sm){
			this->subsystemMedia = sm;}

		virtual void sipcb_handleCommand(const CommandString &command);
		virtual void sipcb_handleConfCommand(const CommandString &command);
		virtual void guicb_handleCommand(const CommandString &command);
		virtual void guicb_handleConfCommand(const std::string &conferencename);
		virtual void guicb_handleConfCommand(const CommandString &command);
		virtual void guicb_handleMediaCommand(const CommandString &command);
		
		virtual std::string confcb_doJoin(std::string user, minilist<ConfMember> *conflist, std::string congId);
		virtual std::string confcb_doConnect(std::string user, std::string confId);
		virtual void confcb_handleSipCommand(std::string &/*command*/){}
		virtual void confcb_handleSipCommand(const CommandString &command);
		virtual void confcb_handleGuiCommand(const CommandString &command);	
		virtual ConferenceControl* getConferenceController(std::string confid);
	private:
		
		MRef<Gui *> gui;
		minilist<ConferenceControl *> confrout;//bm
		MRef<Sip*> sip;
		MRef<SubsystemMedia*> subsystemMedia;
};

#endif
