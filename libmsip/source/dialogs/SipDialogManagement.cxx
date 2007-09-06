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

#include<config.h>

#include<libmsip/SipDialogManagement.h>
#include<libmsip/SipStack.h>
#include"../SipStackInternal.h"
#include"../SipCommandDispatcher.h"

#ifdef DEBUG_OUTPUT
#include<libmutil/dbg.h>
#endif


//#include<libmsip/SipDialogContainer.h>
#include<libmsip/SipCommandString.h>

#include<libmsip/SipSMCommand.h>

#include<libmsip/SipTransitionUtils.h>

using namespace std;

//all timeouts in miliseconds, please
//the max total time it will take to shutdown (given you come up with problems),
//it is SHUTDOWN_CALLS_TIMEOUT + SHUTDOWN_DEREGISTER_TIMEOUT. Pick these values 
//adequately to fit your needs.
#define SHUTDOWN_CALLS_TIMEOUT 3000 
#define SHUTDOWN_DEREGISTER_TIMEOUT 3000 

//START OF SHUTDOWN RELATED FUNCTIONS
bool SipDialogManagement::a0_start_startShutdown_startShutdown( const SipSMCommand &command) {
	bool ret = false;
	if (transitionMatch(command, 
				SipCommandString::sip_stack_shutdown,
				SipSMCommand::dispatcher,
				SipSMCommand::dialog_layer)) {

		pendingHangUps = pendingDeRegs = 0;
		merr << endl;
		merr << "MiniSIP's SipStack is shutting down ... " << endl;
		merr << "     ... it won't take long to finish, be patient. Thanks!" << endl;
		SipSMCommand cmd( CommandString( "", SipCommandString::terminate_all_calls),
			SipSMCommand::dispatcher,
			SipSMCommand::dispatcher);
		getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE);
		ret = true;
	} else {
		ret = false;
	}
	return ret;
}

bool SipDialogManagement::a10_startSh_terminateCallsSh_terminateAll( const SipSMCommand &command) {
	bool ret = false;
	if (transitionMatch(command, 
				SipCommandString::terminate_all_calls,
				SipSMCommand::dispatcher,
				SipSMCommand::dialog_layer)) {

		terminateAllCalls();
		requestTimeout( SHUTDOWN_CALLS_TIMEOUT, "timer_terminate_calls" );
		ret = true;
	} else {
		ret = false;
	}
	return ret;
}

bool SipDialogManagement::a11_terminateCallsSh_callTerminatedEarly( const SipSMCommand &command) {
	bool ret = false;
	if (transitionMatch(command, 
				SipCommandString::call_terminated_early,
				SipSMCommand::dispatcher,
				SipSMCommand::dialog_layer) ) {

		receivedCallTerminateEarly();
		//mdbg << "shutdown: call terminated early: " << command.getCommandString().getDestinationId() << end;	
		ret = true;
	} else {
		ret = false;
	}
	return ret;
}

bool SipDialogManagement::a12_terminateCallsSh_timeIsUp( const SipSMCommand &command) {
	bool ret = false;
	if (transitionMatch(command, 
				"timer_terminate_calls",
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer) ) {
		
		SipSMCommand cmd( CommandString( "", SipCommandString::unregister_all_identities),
			SipSMCommand::dispatcher,
			SipSMCommand::dispatcher);
		getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE);
		ret = true;
	} else {
		ret = false;
	}
	return ret;
}

bool SipDialogManagement::a13_terminateCallsSh_allTerminated( const SipSMCommand &command) {
	bool ret = false;
	if (transitionMatch(command, 
				SipCommandString::terminate_all_calls_done,
				SipSMCommand::dispatcher,
				SipSMCommand::dialog_layer) ) {
		
		cancelTimeout( "timer_terminate_calls" );
		SipSMCommand cmd( CommandString( "", SipCommandString::unregister_all_identities),
			SipSMCommand::dispatcher,
			SipSMCommand::dispatcher);
		getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE);
		ret = true;
	} else {
		ret = false;
	}
	return ret;
}

bool SipDialogManagement::a20_terminateCallsSh_deRegAllSh_allTerminated( const SipSMCommand &command) {
	bool ret = false;
	if (transitionMatch(command, 
				SipCommandString::unregister_all_identities,
				SipSMCommand::dispatcher,
				SipSMCommand::dialog_layer) ) {

		requestTimeout( SHUTDOWN_DEREGISTER_TIMEOUT, "timer_deRegisterAll" );
		deRegisterAll();
		ret = true;
	} else {
		ret = false;
	}
	return ret;
}

bool SipDialogManagement::a21_deRegAllSh_callTerminatedEarly( const SipSMCommand &command) {
	bool ret = false;
	if (transitionMatch(command, 
				SipCommandString::call_terminated_early,
				SipSMCommand::dispatcher,
				SipSMCommand::dialog_layer) ) {

		receivedCallTerminateEarly();
		ret = true;
	} else {
		ret = false;
	}
	return ret;
}

bool SipDialogManagement::a22_deRegAllSh_registerOk( const SipSMCommand &command) {
	bool ret = false;
	if (transitionMatch(command, 
				SipCommandString::register_ok,
				SipSMCommand::dispatcher,
				SipSMCommand::dialog_layer) ) {
		
		receivedRegisterOk(true); //we are deregistering ...
		ret = true;
	} else {
		ret = false;
	}
	return ret;
}


bool SipDialogManagement::a23_deRegAllSh_timeIsUp( const SipSMCommand &command) {
	bool ret = false;
	if (transitionMatch(command, 
				"timer_deRegisterAll",
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer) ) {
		
		shutdownDone( true ); //force shutdown done message
		ret = true;
	} else {
		ret = false;
	}
	return ret;

}

bool SipDialogManagement::a24_deRegAllSh_deRegAlldone( const SipSMCommand &command) {
	bool ret = false;
	if (transitionMatch(command, 
				SipCommandString::unregister_all_identities_done,
				SipSMCommand::dispatcher,
				SipSMCommand::dialog_layer) ) {
		
		shutdownDone( false ); //check if finished ... don't force
		ret = true;
	} else {
		ret = false;
	}
	return ret;

}

bool SipDialogManagement::a25_deRegAllSh_allTerminated( const SipSMCommand &command) {
	bool ret = false;
	if (transitionMatch(command, 
				SipCommandString::terminate_all_calls_done,
				SipSMCommand::dispatcher,
				SipSMCommand::dialog_layer) ) {
		
		shutdownDone( false ); //check if finished ... don't force
		ret = true;
	} else {
		ret = false;
	}
	return ret;

}

bool SipDialogManagement::a30_deRegAllSh_terminated_timeIsUp( const SipSMCommand &command) {
	//This transition is quite useless ... sip stack is finished anyway and this
	//"dialog" ain't going anyway, be in terminated or not.
	//But we do it anyway.
	bool ret = false;
	if (transitionMatch(command, 
				SipCommandString::sip_stack_shutdown_done,
				SipSMCommand::dispatcher,
				SipSMCommand::dialog_layer) ) {
		ret = true;
	} else {
		ret = false;
	}
	return ret;
}
//END OF SHUTDOWN RELATED FUNCTIONS

//START OF CALLS TERMINATION RELATED FUNCTIONS
bool SipDialogManagement::b0_start_terminateCallsOps_terminateAll( const SipSMCommand &command){
	bool ret = false;
	if (transitionMatch(command, 
				SipCommandString::terminate_all_calls,
				SipSMCommand::dispatcher,
				SipSMCommand::dialog_layer)) {
		
		pendingHangUps = pendingDeRegs = 0;
		terminateAllCalls();
		requestTimeout( SHUTDOWN_CALLS_TIMEOUT, "timer_terminate_calls" );
		ret = true;
	} else {
		ret = false;
	}
	return ret;
} 

bool SipDialogManagement::b11_terminateCallsOps_terminateEarly( const SipSMCommand &command){
	bool ret = false;
	if (transitionMatch(command, 
				SipCommandString::call_terminated_early,
				SipSMCommand::dispatcher,
				SipSMCommand::dialog_layer) ) {
		receivedCallTerminateEarly();
		//mdbg << "shutdown: call terminated early: " << command.getCommandString().getDestinationId() << end;	
		ret = true;
	} else {
		ret = false;
	}
	return ret;
}

bool SipDialogManagement::b12_terminateCallsOps_timeIsUp( const SipSMCommand &command){ 
	bool ret = false;
	if (transitionMatch(command, 
				"timer_terminate_calls",
				SipSMCommand::dispatcher,
				SipSMCommand::dialog_layer) ) {
		
		SipSMCommand cmd( CommandString( "", SipCommandString::unregister_all_identities),
			SipSMCommand::dispatcher,
			SipSMCommand::dispatcher);
		getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE);
		ret = true;
	} else {
		ret = false;
	}
	return ret;
}

bool SipDialogManagement::b30_terminateCallsOps_start_terminateAllDone( const SipSMCommand &command){
	bool ret = false;
	if (transitionMatch(command, 
				SipCommandString::terminate_all_calls_done,
				SipSMCommand::dispatcher,
				SipSMCommand::dialog_layer) ) {
		cancelTimeout( "timer_terminate_calls" );
		ret = true;
	} else {
		ret = false;
	}
	return ret;
}

bool SipDialogManagement::c0_start_deRegAllOps_deRegAll( const SipSMCommand &command){
	bool ret = false;
	if (transitionMatch(command, 
				SipCommandString::unregister_all_identities,
				SipSMCommand::dispatcher,
				SipSMCommand::dialog_layer) ) {
		pendingHangUps = pendingDeRegs = 0;
		requestTimeout( SHUTDOWN_DEREGISTER_TIMEOUT, "timer_deRegisterAll" );
		deRegisterAll();
		ret = true;
	} else {
		ret = false;
	}
	return ret;
}

bool SipDialogManagement::c11_deRegAllOps_registerOk( const SipSMCommand &command){
	bool ret = false;
	if (transitionMatch(command, 
				SipCommandString::register_ok,
				SipSMCommand::dispatcher,
				SipSMCommand::dialog_layer) ) {
		receivedRegisterOk(true); //we are deregistering ...
		ret = true;
	} else {
		ret = false;
	}
	return ret;
}

bool SipDialogManagement::c12_deRegAllOps_timeIsUp( const SipSMCommand &command){
	bool ret = false;
	if (transitionMatch(command, 
				"timer_deRegisterAll",
				SipSMCommand::dispatcher,
				SipSMCommand::dialog_layer) ) {
		ret = true;
	} else {
		ret = false;
	}
	return ret;
}

bool SipDialogManagement::c30_deRegAllOps_start_deRegAllDone( const SipSMCommand &command){
	bool ret = false;
	if (transitionMatch(command, 
				SipCommandString::unregister_all_identities_done,
				SipSMCommand::dispatcher,
				SipSMCommand::dialog_layer) ) {
		cancelTimeout( "timer_deRegisterAll" );
		ret = true;
	} else {
		ret = false;
	}
	return ret;
}

bool SipDialogManagement::d0_start_regAllOps_regAll( const SipSMCommand &command){
	bool ret = false;
	if (transitionMatch(command, 
				SipCommandString::register_all_identities,
				SipSMCommand::dispatcher,
				SipSMCommand::dialog_layer) ) {
		pendingHangUps = pendingDeRegs = 0;
		requestTimeout( SHUTDOWN_DEREGISTER_TIMEOUT, "timer_registerAll" );
		registerAll();
		ret = true;
	} else {
		ret = false;
	}
	return ret;
}
bool SipDialogManagement::d11_regAllOps_registerOk( const SipSMCommand &command){
	bool ret = false;
	if (transitionMatch(command, 
				SipCommandString::register_ok,
				SipSMCommand::dispatcher,
				SipSMCommand::dialog_layer) ) {
		receivedRegisterOk(false); //we are NOT deregistering ...
		ret = true;
	} else {
		ret = false;
	}
	return ret;
}
bool SipDialogManagement::d12_regAllOps_timeIsUp( const SipSMCommand &command){
	bool ret = false;
	if (transitionMatch(command, 
				"timer_registerAll",
				SipSMCommand::dispatcher,
				SipSMCommand::dialog_layer) ) {
		ret = true;
	} else {
		ret = false;
	}
	return ret;
}
bool SipDialogManagement::d30_regAllOps_start_regAllDone( const SipSMCommand &command){
	bool ret = false;
	if (transitionMatch(command, 
				SipCommandString::register_all_identities_done,
				SipSMCommand::dispatcher,
				SipSMCommand::dialog_layer) ) {
		cancelTimeout( "timer_registerAll" );
		ret = true;
	} else {
		ret = false;
	}
	return ret;
}

//END OF CALLS/REGISTER RELATED FUNCTIONS


void SipDialogManagement::setUpStateMachine(){
	
	State<SipSMCommand,string> *s_start=
		new State<SipSMCommand,string>(this,"start");
	addState(s_start);
	
	setUpStateMachine_shutdown(s_start);
	setUpStateMachine_dialogops(s_start);
	
	setCurrentState(s_start);
}

//separate for the sake of clarity
void SipDialogManagement::setUpStateMachine_shutdown(State<SipSMCommand,string> *s_start){

	State<SipSMCommand,string> *s_start_sh=
		new State<SipSMCommand,string>(this,"start_shutdown");
	addState(s_start_sh);
	
	State<SipSMCommand,string> *s_terminateCalls_sh=
		new State<SipSMCommand,string>(this,"terminateCalls_shutdown");
	addState(s_terminateCalls_sh);

	State<SipSMCommand,string> *s_deRegAll_sh=
		new State<SipSMCommand,string>(this,"deRegisterAll_shutdown");
	addState(s_deRegAll_sh);

	State<SipSMCommand,string> *s_terminated=
		new State<SipSMCommand,string>(this,"terminated");
	addState(s_terminated);


	new StateTransition<SipSMCommand,string>(
		this, 
		"transition_start_startSh_startShutdown",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) 
			&SipDialogManagement::a0_start_startShutdown_startShutdown, 
		s_start, 
		s_start_sh);

	new StateTransition<SipSMCommand,string>(
		this, 
		"transition_startSh_terminateCallsSh_startShutdown",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) 
			&SipDialogManagement::a10_startSh_terminateCallsSh_terminateAll, 
		s_start_sh, 
		s_terminateCalls_sh);
	
	new StateTransition<SipSMCommand,string>(
		this, 
		"transition_terminateCallsSh_callTerminatedEarly",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) 
			&SipDialogManagement::a11_terminateCallsSh_callTerminatedEarly, 
		s_terminateCalls_sh, 
		s_terminateCalls_sh);
	
	new StateTransition<SipSMCommand,string>(
		this, 
		"transition_terminateCallsSh_timeIsUp",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) 
			&SipDialogManagement::a12_terminateCallsSh_timeIsUp, 
		s_terminateCalls_sh,
		s_terminateCalls_sh);
	
	new StateTransition<SipSMCommand,string>(
		this, 
		"transition_terminateCallsSh_allTerminated",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) 
			&SipDialogManagement::a13_terminateCallsSh_allTerminated, 
		s_terminateCalls_sh,
		s_terminateCalls_sh);
	
	new StateTransition<SipSMCommand,string>(
		this, 
		"transition_terminateCallsSh_s_deRegAllSh_allTerminated",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) 
			&SipDialogManagement::a20_terminateCallsSh_deRegAllSh_allTerminated, 
		s_terminateCalls_sh,
		s_deRegAll_sh);
	
	new StateTransition<SipSMCommand,string>(
		this, 
		"transition_deRegAllSh_deRegAllSh_callTerminatedEarly",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) 
			&SipDialogManagement::a21_deRegAllSh_callTerminatedEarly, 
		s_deRegAll_sh,
		s_deRegAll_sh);
	
	new StateTransition<SipSMCommand,string>(
		this, 
		"transition_deRegAllSh_deRegAllSh_registerOk",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) 
			&SipDialogManagement::a22_deRegAllSh_registerOk, 
		s_deRegAll_sh,
		s_deRegAll_sh);
		
	new StateTransition<SipSMCommand,string>(
		this, 
		"transition_deRegAllSh_deRegAllSh_timeIsUp",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) 
			&SipDialogManagement::a23_deRegAllSh_timeIsUp, 
		s_deRegAll_sh,
		s_deRegAll_sh);

	new StateTransition<SipSMCommand,string>(
		this, 
		"transition_deRegAllSh_deRegAllSh_deRegAlldone",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) 
			&SipDialogManagement::a24_deRegAllSh_deRegAlldone, 
		s_deRegAll_sh,
		s_deRegAll_sh);

	new StateTransition<SipSMCommand,string>(
		this, 
		"transition_deRegAllSh_deRegAllSh_allTerminated",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) 
			&SipDialogManagement::a25_deRegAllSh_allTerminated, 
		s_deRegAll_sh,
		s_deRegAll_sh);

	new StateTransition<SipSMCommand,string>(
		this, 
		"transition_deRegAllSh_terminated_shutdownDone",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) 
			&SipDialogManagement::a30_deRegAllSh_terminated_timeIsUp, 
		s_deRegAll_sh, 
		s_terminated);
	
	
	
}

void SipDialogManagement::setUpStateMachine_dialogops(State<SipSMCommand,string> *s_start){
	//TERMINATE ALL CALLS SETUP
	State<SipSMCommand,string> *s_terminateCalls_ops=
		new State<SipSMCommand,string>(this,"terminateCalls_ops");
	addState(s_terminateCalls_ops);
	
	new StateTransition<SipSMCommand,string>(
		this, 
		"transition_start_terminateCallsOps_terminateAll",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) 
			&SipDialogManagement::b0_start_terminateCallsOps_terminateAll, 
		s_start, 
		s_terminateCalls_ops);
	
	new StateTransition<SipSMCommand,string>(
		this, 
		"transition_terminateCallsOps_terminateEarly",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) 
			&SipDialogManagement::b11_terminateCallsOps_terminateEarly, 
		s_terminateCalls_ops,
		s_terminateCalls_ops);

	new StateTransition<SipSMCommand,string>(
		this, 
		"transition_terminateCallsOps_timeIsUp",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) 
			&SipDialogManagement::b12_terminateCallsOps_timeIsUp, 
		s_terminateCalls_ops, 
		s_terminateCalls_ops);
	
	new StateTransition<SipSMCommand,string>(
		this, 
		"transition_terminateCallsOps_start_terminateAllDone",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) 
			&SipDialogManagement::b30_terminateCallsOps_start_terminateAllDone, 
		s_terminateCalls_ops,
		s_start);
	
	//UNREGISTER ALL IDENTITIES SETUP
	State<SipSMCommand,string> *s_deRegAll_ops=
		new State<SipSMCommand,string>(this,"deRegAll_ops");
	addState(s_deRegAll_ops);
	
	new StateTransition<SipSMCommand,string>(
		this, 
		"transition_start_deRegAllOps_terminateAll",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) 
			&SipDialogManagement::c0_start_deRegAllOps_deRegAll, 
		s_start, 
		s_deRegAll_ops);
	
	new StateTransition<SipSMCommand,string>(
		this, 
		"transition_deRegAllOps_registerOk",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) 
			&SipDialogManagement::c11_deRegAllOps_registerOk, 
		s_deRegAll_ops,
		s_deRegAll_ops);

	new StateTransition<SipSMCommand,string>(
		this, 
		"transition_deRegAllOps_timeIsUp",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) 
			&SipDialogManagement::c12_deRegAllOps_timeIsUp, 
		s_deRegAll_ops, 
		s_deRegAll_ops);
	
	new StateTransition<SipSMCommand,string>(
		this, 
		"transition_deRegAllOps_start_terminateAllDone",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) 
			&SipDialogManagement::c30_deRegAllOps_start_deRegAllDone, 
		s_deRegAll_ops,
		s_start);
	
	//REGISTER ALL IDENTITIES SETUP
	State<SipSMCommand,string> *s_regAll_ops=
		new State<SipSMCommand,string>(this,"regAll_ops");
	addState(s_regAll_ops);
	
	new StateTransition<SipSMCommand,string>(
		this, 
		"transition_start_deRegAllOps_terminateAll",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) 
			&SipDialogManagement::d0_start_regAllOps_regAll, 
		s_start, 
		s_regAll_ops);
	
	new StateTransition<SipSMCommand,string>(
		this, 
		"transition_deRegAllOps_registerOk",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) 
			&SipDialogManagement::d11_regAllOps_registerOk, 
		s_regAll_ops,
		s_regAll_ops);

	new StateTransition<SipSMCommand,string>(
		this, 
		"transition_deRegAllOps_timeIsUp",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) 
			&SipDialogManagement::d12_regAllOps_timeIsUp, 
		s_regAll_ops, 
		s_regAll_ops);
	
	new StateTransition<SipSMCommand,string>(
		this, 
		"transition_deRegAllOps_start_terminateAllDone",
		(bool (StateMachine<SipSMCommand,string>::*)(const SipSMCommand&)) 
			&SipDialogManagement::d30_regAllOps_start_regAllDone, 
		s_regAll_ops,
		s_start);	
}

SipDialogManagement::SipDialogManagement(MRef<SipStack*> stack):
					SipDialog(stack, NULL, "shutdown_dialog")
{
	setUpStateMachine();
	pendingHangUps = 0;
	pendingDeRegs = 0;
}
	
SipDialogManagement::~SipDialogManagement(){	
}

bool SipDialogManagement::terminateAllCalls() {
	list<MRef<SipDialog *> > dlgs;
	dlgs = getSipStack()->getDialogs();

	merr << endl; 
	merr << "Terminating all ongoing calls:" << endl;
	for( list<MRef<SipDialog *> >::iterator it = dlgs.begin();
							it != dlgs.end();
							it ++  ) {
		//First, skip the register dialogs ... we'll deal with them later
		if( (*it)->getMemObjectType() == "SipDialogRegister" ) {
			//mdbg << "SipDialogManagement::terminateAllCalls : dialog skipped" << end;
			continue;
		}
		SipSMCommand cmd( CommandString( (*it)->dialogState.callId, SipCommandString::hang_up),
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer);
		getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE);
		pendingHangUps++;
		merr << "    - Hanging up " << (*it)->dialogState.remoteUri << endl;
	}
	if( pendingHangUps <= 0 ) {
		merr << "    CALLS: No ongoing calls!" << endl;
		//if we have not sent any hang_ups ... notify all calls terminated
		SipSMCommand cmd( CommandString( "", SipCommandString::terminate_all_calls_done),
				SipSMCommand::dispatcher,
				SipSMCommand::dispatcher);
		getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE);
	}
	return true;
}

bool SipDialogManagement::receivedCallTerminateEarly() {
	pendingHangUps --;
	if( pendingHangUps <= 0 ) {
		merr << "    CALLS: all calls have been terminated!" << endl;
		SipSMCommand cmd( CommandString( "", SipCommandString::terminate_all_calls_done),
				SipSMCommand::dispatcher,
				SipSMCommand::dispatcher);
		getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE);
	}
	return true;
}

bool SipDialogManagement::deRegisterAll() {
	list<MRef<SipDialog *> > dlgs;
	dlgs = getSipStack()->getDialogs();
	merr << endl; 
	merr << "De-Registering all identities from their registrar:" << endl;
	for( list<MRef<SipDialog *> >::iterator it = dlgs.begin();
							it != dlgs.end();
							it ++  ) {
		//First, skip the register dialogs ... we'll deal with them later
		if( (*it)->getMemObjectType() != "SipDialogRegister" ) {
			//mdbg << "SipDialogManagement::deRegisterAll : non-reg dialog skipped" << end;
			continue;
		}
		if(! (*it)->getDialogConfig()->sipIdentity->isRegistered() ) {
			//mdbg << "SipDialogManagement::deRegisterAll : skipping already de-registered identity" << end;
			continue;
		}
		
		CommandString cmdstr( (*it)->dialogState.callId, SipCommandString::proxy_register);
		cmdstr["proxy_domain"] = (*it)->getDialogConfig()->sipIdentity->getSipUri().getIp();
		cmdstr.setParam3("0"); //expires = 0 ==> de-register
		
		SipSMCommand cmd( cmdstr,
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer);
		getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE);
		pendingDeRegs++;
		merr << "    De-registration request sent (username = " << 
			(*it)->getDialogConfig()->sipIdentity->getSipUri().getString() << ")" << endl;
	}
	if( pendingDeRegs == 0 ) {
		//if we have not sent any de-regs ... notify all un-registered
		merr << "    DE-REGISTER: all identities were already not registered!" << endl;
		SipSMCommand cmd( CommandString( "", SipCommandString::unregister_all_identities_done),
				SipSMCommand::dispatcher,
				SipSMCommand::dispatcher);
		getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE);
	}
	return true;
}

bool SipDialogManagement::registerAll() {
	list<MRef<SipDialog *> > dlgs;
	dlgs = getSipStack()->getDialogs();
	merr << endl; 
	merr << "Registering all identities to their registrar:" << endl;
	for( list<MRef<SipDialog *> >::iterator it = dlgs.begin();
							it != dlgs.end();
							it ++  ) {
		//First, skip the register dialogs ... we'll deal with them later
		if( (*it)->getMemObjectType() != "SipDialogRegister" ) {
			//mdbg << "SipDialogManagement::registerAll : non-reg dialog skipped" << end;
			continue;
		}
		if( (*it)->getDialogConfig()->sipIdentity->isRegistered() ) {
			//mdbg << "SipDialogManagement::registerAll : skipping already registered identity" << end;
			continue;
		}
		
		CommandString cmdstr( (*it)->dialogState.callId, SipCommandString::proxy_register);
		cmdstr["proxy_domain"] = (*it)->getDialogConfig()->sipIdentity->getSipUri().getIp();
		//expires = defaultExpires, read from the config file
		cmdstr.setParam3((*it)->getDialogConfig()->sipIdentity->getSipRegistrar()->getDefaultExpires()); 
		
		SipSMCommand cmd( cmdstr,
				SipSMCommand::dialog_layer,
				SipSMCommand::dialog_layer);
		getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE);
		pendingDeRegs++;
		merr << "    Registration request sent (username = " << 
			(*it)->getDialogConfig()->sipIdentity->getSipUri().getString() << ")" << endl;
	}
	if( pendingDeRegs == 0 ) {
		//if we have not sent any de-regs ... notify all un-registered
		merr << "    REGISTER: all identities were already registered!" << endl;
		SipSMCommand cmd( CommandString( "", SipCommandString::unregister_all_identities_done),
				SipSMCommand::dispatcher,
				SipSMCommand::dispatcher);
		getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE);
	}
	return true;
}

bool SipDialogManagement::receivedRegisterOk(bool deregistering){
	//FIXME: Should do some kind of checking whether the register_ok is correct
	//   It may be a problem if minisip shutsdown at the same moment it was re-registering ...
	pendingDeRegs--;
	if( pendingDeRegs <= 0 ) {
		if( deregistering ) {
			merr << "    DE-REGISTER: all identities have been de-registered correctly!" << endl;
			SipSMCommand cmd( CommandString( "", SipCommandString::unregister_all_identities_done),
				SipSMCommand::dispatcher,
				SipSMCommand::dispatcher);
			getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE);
		} else {
			merr << "    REGISTER: all identities have been registered correctly!" << endl;
			SipSMCommand cmd( CommandString( "", SipCommandString::register_all_identities_done),
				SipSMCommand::dispatcher,
				SipSMCommand::dispatcher);
			getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE);
		}
	}
	return true;
}

bool SipDialogManagement::shutdownDone( bool force ) {
	if( !force ) {
		if( pendingHangUps > 0 || pendingDeRegs > 0 ) {
			//Still can wait a bit more ... 
			//mdbg << "CESC: shutdown: still not finished ... wait ... "<< end;
			return false;
		} 
		//else ... shutdown ... nothing else to do ...
		merr << endl << "SipStack Shutdown process is completed."<< endl;
	} else {
		merr << "Shutdown process timed out (there was some problem): "<< endl;
		if( pendingHangUps > 0 ) {
			merr << "      CALLS: Not all calls could be correctly hung up."<< endl;
		}
		if( pendingDeRegs > 0 ) {
			merr << "      DE-REGISTER: Not all identities were correctly de-registered."<< endl;
		}
	}
	
	SipSMCommand cmd(
		CommandString( "", SipCommandString::sip_stack_shutdown_done),
		SipSMCommand::dispatcher, 
		SipSMCommand::dispatcher);
	getSipStack()->enqueueCommand(cmd, HIGH_PRIO_QUEUE);
	return true;
}
