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

#ifndef REMOTEGUI_H
#define REMOTEGUI_H

#include"../../util/string.h"
#include"Gui.h"
#include"../../netutil/IP4ServerSocket.h"

//#define MENU_STATE 1
//#define IN_CALL_STATE 2
//#define EXTERNAL_QUESTION_STATE 3

class RemoteGui : public Gui{
	public:
		RemoteGui(GuiCallback *callback, int32_t port);
		virtual ~RemoteGui();
		
		
		void registred_to_proxy();
		void display_remote_ringing();
		void incoming_avail(string from);
		void user_not_found();
		void remote_bye();
		void remote_accept_invite();
		void remote_cancelled_invite();
		string ask_password(){return "unknown";};
		string ask_username(){return "unknown";};
		bool ask_accept(string nr);
//		void call_closed();
//		void call_started();
		void refresh();
		void refresh(string mess);
		void wake_up();

		void gui_loop();
	private:
		void print_usage();
		void send_command(string command);
		
		IP4ServerSocket serv_sock;
		TCPSocket *sock;
//		int my_pid;
//		string prompt;
//		Bell *beeper;
//		int gui_state;
//		bool incoming_avail_flag;
//		string from;
	
};


#endif
