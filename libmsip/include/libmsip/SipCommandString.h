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

#ifndef SIPCOMMANDSTRING_H
#define SIPCOMMANDSTRING_H


#include<string>

using namespace std;

class SipCommandString{
	public:
		/*
		 * Predefined command strings
		 */
//		static const string garbage_collect;
		static const string transaction_terminated;
		static const string call_terminated;
		static const string no_transactions;
                
		static const string error_message;
		static const string transport_error;
		static const string authentication_failed;
		static const string hang_up;
		static const string invite;
		static const string invite_ok;
		static const string invite_no_reply;
		static const string incoming_available;
		static const string remote_hang_up;
		static const string cancel;
		static const string cancel_ok;
		static const string remote_user_not_found;
		static const string accept_invite;
		static const string reject_invite;
		static const string remote_cancelled_invite;
		static const string remote_ringing;
		static const string remote_reject;
		static const string remote_unacceptable;
		static const string accept_insecure;
		static const string reject_insecure;
		static const string security_failed;
		static const string setpassword;
		
		static const string proxy_register;
		static const string register_sent;
		static const string register_no_reply;
		static const string register_ok;
		static const string register_failed;
		static const string register_failed_authentication;
		static const string temp_unavail;

		static const string close_window;
		static const string ask_password;

		static const string incoming_im;
		static const string outgoing_im;

		static const string start_presence;
		static const string stop_presence;
		
};

#endif
