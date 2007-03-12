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

#ifndef CONFCALLBACK_H
#define CONFCALLBACK_H

#include<libminisip/libminisip_config.h>

#include<libmutil/CommandString.h>

#include<libminisip/signaling/conference/ConfMember.h>

#include<string>

//TODO: Add "enqueueCommand" functionality to this class, and make "qtgui"
//package move to the callback as much as possible --EE

class LIBMINISIP_API ConfCallback{
	
	public:
		virtual ~ConfCallback() {}
		//virtual void guicb_handleConfCommand(std::string &)=0;
		virtual void confcb_handleSipCommand(std::string &)=0;
		virtual void confcb_handleSipCommand(const CommandString &)=0;
		virtual void confcb_handleGuiCommand(const CommandString &)=0;
		virtual std::string confcb_doJoin(std::string user,minilist <ConfMember> *list, std::string confId)=0;
		virtual std::string confcb_doConnect(std::string user, std::string confId)=0;
};

#endif
