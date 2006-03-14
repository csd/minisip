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

#ifndef CONFCALLBACK_H
#define CONFCALLBACK_H

#include<config.h>

#include "ConfMember.h"

#include<libmutil/CommandString.h>

//TODO: Add "enqueueCommand" functionality to this class, and make "qtgui"
//package move to the callback as much as possible --EE

class ConfCallback{
	
	public:
		virtual ~ConfCallback() {}
		//virtual void guicb_handleConfCommand(string &)=0;
		virtual void confcb_handleSipCommand(string &)=0;
		virtual void confcb_handleSipCommand(CommandString &)=0;
		virtual void confcb_handleGuiCommand(CommandString &)=0;
		virtual string confcb_doJoin(string user,minilist <ConfMember> *list, string confId)=0;
		virtual string confcb_doConnect(string user, string confId)=0;
};

#endif
