/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  
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
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<config.h>
#include<libmsip/SipCommandString.h>

using namespace std;

const string SipCommandString::sip_stack_shutdown="sip_stack_shutdown";
const string SipCommandString::sip_stack_shutdown_done="sip_stack_shutdown_done";
const string SipCommandString::no_op="no_op";
const string SipCommandString::register_all_identities="register_all_identities";
const string SipCommandString::register_all_identities_done="register_all_identities_done";
const string SipCommandString::unregister_all_identities="unregister_all_identities";
const string SipCommandString::unregister_all_identities_done="unregister_all_identities_done";
const string SipCommandString::terminate_all_calls="terminate_all_calls";
const string SipCommandString::terminate_all_calls_done="terminate_all_calls_done";
		
const string SipCommandString::transaction_terminated="transaction_terminated";
const string SipCommandString::call_terminated="call_terminated";
const string SipCommandString::call_terminated_early="call_terminated_early";
const string SipCommandString::no_transactions="no_transactions";

const string SipCommandString::error_message="error_message";

const string SipCommandString::authentication_failed="authentication_failed";
const string SipCommandString::transport_error="transport_error";

const string SipCommandString::hang_up="hang_up";
const string SipCommandString::invite="invite";
const string SipCommandString::invite_ok="invite_ok";
const string SipCommandString::invite_no_reply="invite_no_reply";
const string SipCommandString::incoming_available="incoming_available";
const string SipCommandString::remote_hang_up="remote_hang_up";
const string SipCommandString::remote_ringing="remote_ringing";
const string SipCommandString::remote_reject="remote_reject";
const string SipCommandString::remote_unacceptable="remote_unacceptable";
const string SipCommandString::cancel="cancel";
const string SipCommandString::cancel_ok="cancel_ok";
const string SipCommandString::remote_user_not_found="remote_user_not_found";
const string SipCommandString::accept_invite="accept_invite";
const string SipCommandString::reject_invite="reject_invite";
const string SipCommandString::remote_cancelled_invite="remote_cancelled_invite";
const string SipCommandString::accept_insecure="accept_insecure";
const string SipCommandString::reject_insecure="reject_insecure";
const string SipCommandString::security_failed="security_failed";

const string SipCommandString::proxy_register="proxy_register";
const string SipCommandString::register_sent="register_sent";
const string SipCommandString::register_no_reply="register_no_reply";
const string SipCommandString::register_ok="register_ok";
const string SipCommandString::register_failed="register_failed";
const string SipCommandString::register_failed_authentication="register_failed_authentication";
const string SipCommandString::temp_unavail="temp_unavail";

const string SipCommandString::close_window="close_window";
const string SipCommandString::ask_password="ask_password";
const string SipCommandString::setpassword="setpassword";
const string SipCommandString::incoming_im="incoming_im";
const string SipCommandString::outgoing_im="outgoing_im";

const string SipCommandString::start_presence_client="start_presence_client";
const string SipCommandString::stop_presence_client="stop_presence_client";
const string SipCommandString::remote_presence_update="remote_presence_update";

const string SipCommandString::start_presence_server= "start_presence_server";
const string SipCommandString::stop_presence_server= "stop_presence_server";
const string SipCommandString::local_presence_update= "local_presence_update";

/* Transfer initiation */
const string SipCommandString::user_transfer = "user_transfer";
const string SipCommandString::transfer_pending = "transfer_pending";
const string SipCommandString::transfer_refused = "transfer_refused";
const string SipCommandString::transfer_failed = "transfer_failed";

/* Transfer answering */
const string SipCommandString::transfer_requested = "transfer_requested";
const string SipCommandString::user_transfer_accept = "user_transfer_accept";
const string SipCommandString::user_transfer_refuse = "user_transfer_refuse";
const string SipCommandString::call_transferred = "call_transferred";
