/*
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2004-5
 *
 * Authors: Cesc Santasusana <cesc dot santa at gmail dot com>
*/

/* Name
 * 	SipDialogManagement.h
 * Author
 * 	Cesc Santasusana <cesc dot santa at gmail dot com>
 * Purpose
 * 	Control exhisting dialogs in the sip stack and perform
 *		operations on them.
*/



#ifndef SipDialogManagement_H
#define SipDialogManagement_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipDialog.h>
#include<libmutil/StateMachine.h>

class LIBMSIP_API SipDialogManagement: public SipDialog{
	public:
		SipDialogManagement(MRef<SipStack*> stack);
		
		virtual ~SipDialogManagement();

		virtual std::string getMemObjectType() const {return "SipDialogManagement";}
		
		virtual std::string getName(){return "SipDialogManagement (The one and only)";}

		//virtual bool handleCommand(const SipSMCommand &command);

	private:
		
		/**
		The State Machine has an initial state called START.
		From here, we have transitions, named a0, b0, c0, d0 and so on,
		which are the management ops we can perform.
		a0 - start shutdown
		b0 - terminate all calls
		c0 - de register all identities
		d0 - register all identities
		*/
		void setUpStateMachine();
		void setUpStateMachine_shutdown(State<SipSMCommand,std::string> *s_start);
		void setUpStateMachine_dialogops(State<SipSMCommand,std::string> *s_start);
		
		//SHUTDOWN RELATED TRANSITIONS
		/**
		Shutdown operation.
		It hangs_up all running non-register dialogs in the sip stack, then 
		it de-registers all identities. Each of this operations is controlled
		by a timer, thus the shutdown process will always finish, even if
		there were problems. 
		
		a0 is the transition from START to STARTSHUTDOWN states, which happens
		when we receive the sip_stack_shutdown command. We generate a terminate_all_calls
		command, which we will be receiving later.
		
		The terminate_all_calls commands forces as to transition to the 
		TERMINATE_CALLS_SHUTDOWN state. Here, we generate hang_up commands for each
		running non-register dialog in the sip stack.
		
		Once the hang_ups are sent, we wait for call_terminated_early responses from
		the dialogs. When all calls have been hang up (pendingHangUps = 0 again) we 
		generate a terminate_all_calls_done, which we receive later and forces us 
		to generate an unregister_all_identities command, thus transitioning to the
		next state, DEREGISTER_ALL_SHUTDOWN. 
		
		The transition to DEREGISTER_ALL_SHUTDOWN state can also be caused by a
		timer. In this case, we generate the unregister_all_identities command
		and go to the next state (we are still willing to receive call_terminated_early
		notifications).
		
		When we receive the unregister_all_identities command, we send to all 
		register dialogs a proxy_register command, with param3 set to 0 (un-register).
		Now we just need to wait for register_ok notifications from the dialogs. 
		When all identites have been de-registered (pendingDeRegs = 0 again), an
		unregister_all_identities_done command is generated.
		
		In this last state (DEREGISTER_ALL_SHUTDOWN), we are checking register_ok and 
		also call_terminated_early notifications. As long as the timer does not fire,
		we keep receiving them. When both counters are back to zero, we can notify
		about the shutdown process being finished, with a sip_stack_shutdown_done
		command.
		
		The sip dialog management goes to the TERMINATED state once the 
		sip_stack_shutdown_done command is received ... just to be nice.
		*/
		bool a0_start_startShutdown_startShutdown( const SipSMCommand &command);
		bool a10_startSh_terminateCallsSh_terminateAll( const SipSMCommand &command);
		bool a11_terminateCallsSh_callTerminatedEarly( const SipSMCommand &command);
		bool a12_terminateCallsSh_timeIsUp( const SipSMCommand &command);
		bool a13_terminateCallsSh_allTerminated( const SipSMCommand &command);
		bool a20_terminateCallsSh_deRegAllSh_allTerminated( const SipSMCommand &command);
		bool a21_deRegAllSh_callTerminatedEarly( const SipSMCommand &command);
		bool a22_deRegAllSh_registerOk( const SipSMCommand &command);
		bool a23_deRegAllSh_timeIsUp( const SipSMCommand &command);
		bool a24_deRegAllSh_deRegAlldone( const SipSMCommand &command);
		bool a25_deRegAllSh_allTerminated( const SipSMCommand &command);
		bool a30_deRegAllSh_terminated_timeIsUp( const SipSMCommand &command);
		
		//END OF SHUTDOWN RELATED TRANSITIONS
		
		//MANAGEMENT OPS RELATED FUNCTIONS
		/**
		Terminate all ongoing calls
		This command (terminate_all_calls) tries to nicely hang up all ongoing
		calls. It will give up after a set amount of time.
		
		The idea is the same used in the sutdown process:
		receive the terminate_all_calls command, send hang ups to all non-register
		dialogs, wait for the call_terminated_early notifications (till the timer
		fires), once finished generate a terminate_all_calls_done command to go 
		back to the START state.
		*/
		bool b0_start_terminateCallsOps_terminateAll( const SipSMCommand &command);
		bool b11_terminateCallsOps_terminateEarly( const SipSMCommand &command);
		bool b12_terminateCallsOps_timeIsUp( const SipSMCommand &command);
		bool b30_terminateCallsOps_start_terminateAllDone( const SipSMCommand &command);

		/**
		De-register all identities
		This command tries to de-register all identities (only those which have registered
		previously). It will give up after a set amount of time.
		
		The idea is the same used in the shutdown process:
		receive a unregister_all_identities command, send proxy_register commands with 
		expires=0 to all register-dialogs, wait for register_ok notifications (till the 
		timer fires), and once finished generate an unregister_all_identities_done command
		to go back to the START state.
		*/
		bool c0_start_deRegAllOps_deRegAll( const SipSMCommand &command);
		bool c11_deRegAllOps_registerOk( const SipSMCommand &command);
		bool c12_deRegAllOps_timeIsUp( const SipSMCommand &command);
		bool c30_deRegAllOps_start_deRegAllDone( const SipSMCommand &command);

		/**
		Register all identities
		This command tries to force all identities who require it to register to their 
		proxy.
		
		See the de-register operation for an explanation.
		*/
		bool d0_start_regAllOps_regAll( const SipSMCommand &command);
		bool d11_regAllOps_registerOk( const SipSMCommand &command);
		bool d12_regAllOps_timeIsUp( const SipSMCommand &command);
		bool d30_regAllOps_start_regAllDone( const SipSMCommand &command);

		//END OF MANAGEMENT OPS RELATED FUNCTIONS
		
		/**
		Send hang_up commands to all non-register dialogs. If no calls
		have been hang up, generate the terminate_all_calls_done command.
		*/
		bool terminateAllCalls();
		
		/**
		Use this function to mantain the counter of how many calls have
		been terminated (early notification). 
		*/
		bool receivedCallTerminateEarly();
		
		/**
		Send un-register proxy_register commands to all register dialogs.
		If no identities need to be de-registered, generate the done command.
		*/
		bool deRegisterAll();
		
		/**
		Same as deRegisterAll function, but with an expires value different 
		from zero (use the value set by the user in the config file).
		*/
		bool registerAll();
		
		/**
		Process each of the register_ok messages we receive. 
		The parameter is needed, to know what are we actually counting.
		*/
		bool receivedRegisterOk(bool deregistering);
		
		/**
		When we are in a state in which we may be able to shutdown, 
		call this function: if we can shutdown (process is finished),
		it will generate the done command. 
		If force=true, it will generate the done command no matter what.
		*/
		bool shutdownDone( bool force );
		
		/**
		When terminating calls, counter to now how many hang ups are pending ... 
		*/
		int pendingHangUps;
		
		/**
		When de-registering identities, counter to now how many are pending ... 
		(used also when registering all identities)
		*/
		int pendingDeRegs;
};

#endif
