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


#ifndef SIPDialogREGISTER_H
#define SIPDialogREGISTER_H

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
#include<libmsip/SipSMCommand.h>
#include<libmsip/SipResponse.h>
#include<libmutil/itoa.h>
#include<libmutil/MemObject.h>

class LIBMSIP_API SipDialogRegister : public SipDialog{
	public:
		SipDialogRegister(MRef<SipStack*> stack, MRef<SipDialogConfig*> conf);
		virtual ~SipDialogRegister();


		virtual bool handleCommand(const SipSMCommand &command);

		virtual string getName(){return "SipDialogRegister["+getDialogConfig()->inherited.sipIdentity->sipDomain+"]";}
		
//#ifdef DEBUG_OUTPUT	
		virtual std::string getMemObjectType(){return "SipDialogRegister["+getDialogConfig()->inherited.sipIdentity->sipDomain+"]";}
//#endif


		
		string getRealm(){return realm;};
		string getNonce(){return nonce;};
		void setRealm(string r){realm = r;};
		void setNonce(string n){nonce = n;};

		void updateFailCount(){failCount++;};
		uint32_t getFailCount(){return failCount;};

		bool hasPassword();

		bool getGuiFeedback(){return guiFeedback;}
		void setGuiFeedback(bool fb){guiFeedback=fb;}

		void send_noauth(string branch);
		void send_auth(string branch);
	private:
		bool a0_start_tryingnoauth_register( const SipSMCommand &command);
		bool a1_tryingnoauth_registred_2xx( const SipSMCommand &command);
		bool a2_tryingnoauth_tryingstored_401haspass( const SipSMCommand &command);
		bool a3_tryingnoauth_askpassword_401nopass( const SipSMCommand &command);
		bool a4_tryingstored_askpassword_401( const SipSMCommand &command);
		bool a5_askpassword_askpassword_setpassword( const SipSMCommand &command);
		bool a6_askpassword_registred_2xx( const SipSMCommand &command);
		bool a7_askpassword_askpassword_401( const SipSMCommand &command);
		bool a8_tryingstored_registred_2xx( const SipSMCommand &command);
		bool a9_askpassword_failed_cancel( const SipSMCommand &command);
		bool a10_tryingnoauth_failed_transporterror( const SipSMCommand &command);
		bool a12_registred_tryingnoauth_proxyregister( const SipSMCommand &command);
		bool a13_failed_terminated_notransactions( const SipSMCommand &command);
		
		void setUpStateMachine();

		string realm;
		string nonce;
		uint32_t failCount;
		bool guiFeedback;
};


#endif
