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
*/

/* Name
 * 	SipDialogP2Tuser.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 *	Florian Maurer, florian.maurer@floHweb.ch
 * Purpose
 * 
*/



#ifndef SipDialogP2Tuser_H
#define SipDialogP2Tuser_H

#include<libminisip/libminisip_config.h>

#include<libmutil/StateMachine.h>

#include<libmsip/SipInvite.h>
#include<libmsip/SipBye.h>
#include<libmsip/SipResponse.h>

#include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>

//P2T//
#include<libminisip/signaling/p2t/P2T.h>
#include<libminisip/signaling/p2t/SipDialogP2T.h>


/** 
 * handles the SIP Messages in a P2T Session.
 * Every user in a P2T Session has an own SipDialogP2Tuser dialog
 * that handles the communication with SIP messages.
 * <p>
 * This dialog is more or less a simple copy of <code>SipDialogVoIP</code>
 * with the difference that no SoundSender or SoundReceiver is started
 * directly, but the negotiated codec and port numbers are reported to the
 * concerning <code>SipDialogP2T</code> dialog, and that the Sip INVITE
 * messages are sent with the Contact-Accept-Header. So for a detailled
 * description of this dialog, have a look at SipDialogVoIP.
 * <p> 
 * @author Erik Eliasson, eliasson@it.kth.se
 * @author Florian Maurer, florian.maurer@floHweb.ch
 */

class LIBMINISIP_API SipDialogP2Tuser: public SipDialog{
	public:
		
		/**
		 * Constructor.
		 * creates a new <code>SipDialogP2Tuser</code> object.
		 * @param dContainer the dialog container
		 * @param callconfig the call configuration
		 * @param phoneconf  the phone configuration
		 * @param p2tDialog  the <code>SipDialogP2T</code> where this dialog
		 *                   belongs to
		 */
		SipDialogP2Tuser(MRef<SipStack*> stack, MRef<SipDialogConfig*> callconfig, 
			MRef<SipSoftPhoneConfiguration*> phoneconf,
			MRef<SipDialogP2T*>p2tDialog);
		
		/**
		 * Deconstructor
		 */
		virtual ~SipDialogP2Tuser();
	
		void setCallId( std::string id){dialogState.callId = id;}
		 std::string getCallId(){return dialogState.callId;}
		
		/**
		 * returns the type of the dialog. Used by the memory management.
		 * @return "SipDialogP2Tuser" 
		 */	
		virtual std::string getMemObjectType() const {return "SipDialogP2Tuser";}
		
		/**
		 * returns the name of the dialog. Used by the memory management.
		 * @return "SipDialogP2Tuser" 
		 */
		virtual string getName(){return "SipDialogP2Tuser";}

		/**
		 * handles the incoming <code>SipSMCommand</code>s. Checks if the command
		 * matches a transition in the own state machine. Returns true if the
		 * command was handled or false if the command does not belong to this
		 * call.
		 * @param command the incoming <code>SipSMCommand</code>
		 * @return true if the command was handled 
		 */
		virtual bool handleCommand(const SipSMCommand &command);

	
//		virtual void timeout(const  std::string &c){
//                    SipSMCommand cmd(CommandString("",c),SipSMCommand::TU, SipSMCommand::TU);
//                    handleCommand(cmd);
//                };

		/**
		 * sets up the state machine that is used by this call.
		 */
		void setUpStateMachine();
		
		/**
		 * @return the last received INVITE message
		 */		
		MRef<SipInvite*> getLastInvite();
		
		/**
		 * sets the last returned INVITE message
		 * @param i the INVITE message
		 */
		void setLastInvite(MRef<SipInvite*> i);
		
//		MRef<SipResponse*> getLastResponse();
//		void setLastResponse(MRef<SipResponse*> r);


//		int getTimerT1(){return timerT1;}

		void sendInvite(const  std::string &branch);
		void sendAuthInvite(const  std::string &branch);
//		void sendAck();
		void sendBye(const  std::string &branch, int);
		void sendCancel(const  std::string &branch);
		void sendInviteOk(const  std::string &branch);
		void sendByeOk(MRef<SipBye*> bye, const  std::string &branch);
		void sendReject(const  std::string &branch);
		void sendRinging(const  std::string &branch);
		void sendNotAcceptable(const  std::string &branch);

		//SoundSender *getSoundSender(){return soundSender;}
		//SoundReceiver *getSoundReceiver(){return soundReceiver;}
		
		void registerSDP(uint32_t sourceId, MRef<SdpPacket*> sdppack);

		KeyAgreement *getKeyagreement();
		 std::string getKeyManagementMessage(){return key_mgmt;};

		void setKeyManagementMessage(const  std::string &key_mgmt){this->key_mgmt = key_mgmt;};

		void handleSdp(MRef<SdpPacket*> );

		void setLocalCalled(bool lc){localCalled=lc;}
		
//		void setLastBye(MRef<SipBye*> bye);
//		MRef<SipBye*> getLastBye();

		void setNonce(const  std::string &n){ nonce = n; }

		void setRealm(const  std::string &r){ realm = r; }

		MRef<SipSoftPhoneConfiguration*> getPhoneConfig(){return phoneconf;}

		MRef<LogEntry *> getLogEntry();
		void setLogEntry( MRef<LogEntry *> );
		
		/**
		 * returns the reference to the <code>SipDialogP2T</code>.
		 * @return <code>SipDialogP2T</code>
		 */
		MRef<SipDialogP2T*> getP2TDialog(){return p2tDialog;}
		
		/**
		 * reports the portnumbers, codec and status of
		 * the user to the SipDialogP2T dialog 
		 */
		void reportSipDialogP2T(int status);
		
	private:

		bool a0_start_callingnoauth_invite( const SipSMCommand &command);
		bool a1_callingnoauth_callingnoauth_18X( const SipSMCommand &command);
		bool a2_callingnoauth_callingnoauth_1xx( const SipSMCommand &command);
		bool a3_callingnoauth_incall_2xx( const SipSMCommand &command);
		bool a5_incall_termwait_BYE( const SipSMCommand &command);
		bool a6_incall_termwait_hangup(/*State<SipSMCommand,  std::string> *fromState,
				                State<SipSMCommand,  std::string> *toState,*/const SipSMCommand &command);
		bool a7_callingnoauth_termwait_CANCEL(/*State<SipSMCommand,  std::string> *fromState,
				                State<SipSMCommand,  std::string> *toState,*/const SipSMCommand &command);
		bool a8_callingnoauth_termwait_cancel(/*State<SipSMCommand,  std::string> *fromState,
				                State<SipSMCommand,  std::string> *toState,*/const SipSMCommand &command);
		bool a9_callingnoauth_termwait_36( const SipSMCommand &command);
		bool a10_start_ringing_INVITE( const SipSMCommand &command);
		bool a11_ringing_incall_accept( const SipSMCommand &command);
		bool a12_ringing_termwait_CANCEL( const SipSMCommand &command);
		bool a13_ringing_termwait_reject( const SipSMCommand &command);
		bool a16_start_termwait_INVITE( const SipSMCommand &command);
		bool a20_callingnoauth_callingauth_40X( const SipSMCommand &command);
		bool a21_callingauth_callingauth_18X( const SipSMCommand &command);
		bool a22_callingauth_callingauth_1xx( const SipSMCommand &command);
		bool a23_callingauth_incall_2xx( const SipSMCommand &command);
		bool a24_calling_termwait_2xx(/*State<SipSMCommand,  std::string> *fromState,
				                State<SipSMCommand,  std::string> *toState,*/const SipSMCommand &command);
		bool a25_termwait_terminated_notransactions( const SipSMCommand &command);
		bool a26_callingnoauth_termwait_transporterror( const SipSMCommand &command);
		
		//SoundSender *soundSender;
		//SoundReceiver *soundReceiver;

		MRef<LogEntry *> logEntry;

		MRef<SipInvite*> lastInvite;

//		MRef<SipResponse*> lastResponse;

//		 std::string callId;

		std::string key_mgmt;

		bool localCalled;
//		MRef<SipBye*> lastBye;
		std::string nonce;
		std::string realm;
		MRef<SipSoftPhoneConfiguration*> phoneconf;
		
		/**
		 * adds the P2T attributes to the SIP
		 * INVITE message
		 */
		void modifyP2TInvite(MRef<SipInvite*>inv);
		

		
		///the <code>SipDialogP2T</code> where this dialog belongs to
		MRef<SipDialogP2T*> p2tDialog;
		
		///users IP address
		IPAddress *myIp;
		
		///users RTPport
		int myRTPport;
		
		///users RTCPport
		int myRTCPport;
		
		///users CODEC
		Codec* myCodec;
};

#endif

