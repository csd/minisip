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

#ifdef _MSC_VER
#ifdef LIBMSIP_EXPORTS
#define LIBMSIP_API __declspec(dllexport)
#else
#define LIBMSIP_API __declspec(dllimport)
#endif
#else
#define LIBMSIP_API
#endif

#include<libmsip/SipDialog.h>
#include<libmutil/StateMachine.h>

class LIBMSIP_API SipDialogManagement: public SipDialog{
	public:
		SipDialogManagement(MRef<SipStack*> stack);
		
		virtual ~SipDialogManagement();

		virtual std::string getMemObjectType(){return "SipDialogManagement";}
		
		virtual string getName(){return "SipDialogManagement (The one and only)";}

		//virtual bool handleCommand(const SipSMCommand &command);

	private:
		
		void setUpStateMachine();
		void setUpStateMachine_shutdown(State<SipSMCommand,string> *s_start);
		void setUpStateMachine_dialogops(State<SipSMCommand,string> *s_start);
		
		//SHUTDOWN RELATED TRANSITIONS
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
		bool b0_start_terminateCallsOps_terminateAll( const SipSMCommand &command);
		bool b11_terminateCallsOps_terminateEarly( const SipSMCommand &command);
		bool b12_terminateCallsOps_timeIsUp( const SipSMCommand &command);
		bool b30_terminateCallsOps_start_terminateAllDone( const SipSMCommand &command);

		bool c0_start_terminateCallsOps_terminateAll( const SipSMCommand &command);
		bool c11_deRegAllOps_registerOk( const SipSMCommand &command);
		bool c12_deRegAllOps_timeIsUp( const SipSMCommand &command);
		bool c30_deRegAllOps_start_terminateAllDone( const SipSMCommand &command);

		bool d0_start_regAllOps_terminateAll( const SipSMCommand &command);
		bool d11_regAllOps_registerOk( const SipSMCommand &command);
		bool d12_regAllOps_timeIsUp( const SipSMCommand &command);
		bool d30_regAllOps_start_terminateAllDone( const SipSMCommand &command);

		//END OF MANAGEMENT OPS RELATED FUNCTIONS
		
		bool terminateAllCalls();
		bool receivedCallTerminateEarly();
		bool deRegisterAll();
		bool registerAll();
		bool receivedRegisterOk(bool deregistering);
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
