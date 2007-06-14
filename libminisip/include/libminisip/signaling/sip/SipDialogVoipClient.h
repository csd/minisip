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
#include<libmsip/SipResponse.h>

#include<libminisip/signaling/sip/SipDialogVoip.h>
#include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>

class Session;
class SipDialogContainer;
class SipDialogConfig;
class LogEntry;

class LIBMINISIP_API SipDialogVoipClient: public SipDialogVoip{
	public:
		SipDialogVoipClient(MRef<SipStack*> stack, MRef<SipIdentity*> ident, /*MRef<SipSoftPhoneConfiguration*> phoneconf*/ bool useStun, bool useAnat, MRef<Session *> mediaSession, std::string callId="");

		virtual ~SipDialogVoipClient();

		virtual std::string getMemObjectType() const {return "SipDialogVoipClient";}
		
		virtual std::string getName(){return "SipDialogVoipClient[callid="+dialogState.callId +"]";}

	private:
		bool useAnat;
		
		void setUpStateMachine();
		
		void sendInviteOk();
		
		void sendInvite();

		void sendAck();
		void sendPrack(MRef<SipResponse*>);

		bool a2001_start_calling_invite( const SipSMCommand &command);
		bool a2002_calling_calling_18X( const SipSMCommand &command);
		bool a2003_calling_calling_1xx( const SipSMCommand &command);
		bool a2004_calling_incall_2xx( const SipSMCommand &command);
		bool a2005_calling_termwait_CANCEL(const SipSMCommand &command);
		bool a2006_calling_termwait_cancel(const SipSMCommand &command);
		bool a2007_calling_termwait_36( const SipSMCommand &command);
		bool a2008_calling_calling_40X( const SipSMCommand &command);
		bool a2012_calling_termwait_2xx(const SipSMCommand &command);

		bool a2013_calling_termwait_transporterror( const SipSMCommand &command);
		bool a2017_any_any_2XX( const SipSMCommand &command);

		bool handleRel1xx( MRef<SipResponse*> resp );
};

#endif
