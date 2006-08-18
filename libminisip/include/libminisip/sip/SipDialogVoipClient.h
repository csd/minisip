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
 *	    Joachim Orrblad <joachim@orrblad.com>
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/

/* Name
 * 	SipDialogVoipClient.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#ifndef SipDialogVoipClient_H
#define SipDialogVoipClient_H

#include<libminisip/libminisip_config.h>

#include<libmutil/StateMachine.h>

#include<libmsip/SipDialog.h>
#include<libmsip/SipTransaction.h>
#include<libmsip/SipResponse.h>

#include<libminisip/sip/SipDialogVoip.h>
#include<libminisip/sip/SipSoftPhoneConfiguration.h>
#ifdef IPSEC_SUPPORT
#	include<libminisip/ipsec/MsipIpsecAPI.h>
#endif

class Session;
class SipDialogContainer;
class SipDialogConfig;
class LogEntry;

class LIBMINISIP_API SipDialogVoipClient: public SipDialogVoip{
	public:
#ifdef IPSEC_SUPPORT
		SipDialogVoipClient(MRef<SipStack*> stack, MRef<SipDialogConfig*> callconfig, MRef<SipSoftPhoneConfiguration*> phoneconf, MRef<Session *> mediaSession,  std::string callId="", MRef<MsipIpsecAPI *> ipsecSession=NULL);
#else
		SipDialogVoipClient(MRef<SipStack*> stack, MRef<SipDialogConfig*> callconfig, MRef<SipSoftPhoneConfiguration*> phoneconf, MRef<Session *> mediaSession, std::string callId="");

#endif		
		virtual ~SipDialogVoipClient();

		virtual void dummy(){}

		virtual std::string getMemObjectType(){return "SipDialogVoipClient";}
		
		virtual std::string getName(){return "SipDialogVoipClient[callid="+dialogState.callId +"]";}

	private:
		
		std::string realm;
		std::string nonce;
		
		void setUpStateMachine();
		
		void sendInviteOk(const std::string &branch);
		
		void sendInvite(const std::string &branch);

		void sendAck();
		void sendPrack(MRef<SipResponse*>);

		bool a2001_start_callingnoauth_invite( const SipSMCommand &command);
		bool a2002_callingnoauth_callingnoauth_18X( const SipSMCommand &command);
		bool a2003_callingnoauth_callingnoauth_1xx( const SipSMCommand &command);
		bool a2004_callingnoauth_incall_2xx( const SipSMCommand &command);
		bool a2005_callingnoauth_termwait_CANCEL(const SipSMCommand &command);
		bool a2006_callingnoauth_termwait_cancel(const SipSMCommand &command);
		bool a2007_callingnoauth_termwait_36( const SipSMCommand &command);
		bool a2008_callingnoauth_callingauth_40X( const SipSMCommand &command);
		bool a2009_callingauth_callingauth_18X( const SipSMCommand &command);
		bool a2010_callingauth_callingauth_1xx( const SipSMCommand &command);
		bool a2011_callingauth_incall_2xx( const SipSMCommand &command);
		bool a2012_calling_termwait_2xx(const SipSMCommand &command);

		bool a2013_callingnoauth_termwait_transporterror( const SipSMCommand &command);
		bool a2014_callingauth_termwait_cancel( const SipSMCommand &command);

		bool a2017_any_any_2XX( const SipSMCommand &command);

		bool handleRel1xx( MRef<SipResponse*> resp );
};

#endif
