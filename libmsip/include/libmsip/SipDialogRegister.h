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

#include<libmsip/libmsip_config.h>

#include<libmsip/SipDialog.h>
#include<libmsip/SipSMCommand.h>
#include<libmsip/SipResponse.h>
#include<libmutil/stringutils.h>
#include<libmutil/MemObject.h>

class LIBMSIP_API SipDialogRegister : public SipDialog{
	public:
		SipDialogRegister(MRef<SipStack*> stack, MRef<SipIdentity*> identity);
		virtual ~SipDialogRegister();


		virtual bool handleCommand(const SipSMCommand &command);

		virtual std::string getName(){return "SipDialogRegister["+getDialogConfig()->sipIdentity->getSipUri().getIp()+"]";}
		
		virtual std::string getMemObjectType() const {return "SipDialogRegister";}
		
		void updateFailCount(){failCount++;};
		uint32_t getFailCount(){return failCount;};

		bool hasPassword();

		bool getGuiFeedback(){return guiFeedback;}
		void setGuiFeedback(bool fb){guiFeedback=fb;}

		void send_register(std::string branch);
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
		bool a14_noauth_noauth_1xx( const SipSMCommand &command);
		bool a15_stored_stored_1xx( const SipSMCommand &command);
		
		void setUpStateMachine();

		uint32_t failCount;
		bool guiFeedback;

		std::string myDomain; // Only used for debuggin - used in getMemObjectType();
};


#endif
