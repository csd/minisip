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

#ifndef GUICALLBACK_H
#define GUICALLBACK_H

#include<config.h>
#include"../../conf/ConferenceControl.h"
#include<libmutil/CommandString.h>

//TODO: Add "enqueueCommand" functionality to this class, and make "qtgui"
//package move to the callback as much as possible --EE

class GuiCallback{
	
	public:
		virtual string guicb_doInvite(string sip_url)=0;
                virtual void guicb_handleCommand(CommandString &)=0;
                virtual void guicb_handleMediaCommand(CommandString &)=0;
		virtual void guicb_handleConfCommand(string &conferencename)=0;
		virtual string guicb_confDoInvite(string sip_url)=0;
		virtual void setConferenceController(ConferenceControl *conf)=0;
		//virtual void confcb_handleSipCommand(string &)=0;
		//virtual void confcb_handleGuiCommand(string &)=0;
};

#endif
