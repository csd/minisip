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

#ifndef GROUPLISTUSERELEMENT_H
#define GROUPLISTUSERELEMENT_H

#include<libminisip/libminisip_config.h>

#include<vector>

#include<libmutil/MemObject.h>

#include<libmnetutil/IPAddress.h>

#include<libminisip/media/codecs/Codec.h>
#include<libminisip/signaling/p2t/P2T.h>

/**
 * contains all information about a participating user in a <code>GroupList</code>.
 * <p><b>Group List Management</b><br>
 * If a user wants to participate in a P2T-Session he has to know which
 * other UAs are participating in order to set up the required SIP Sessions
 * resp. in order to start the required <CODE>SipDialogP2Tuser</CODE> dialogs.
 * <p>
 * <code>GroupListUserElement</code> stores all important information about
 * a participating user in a Group Member List. 
 * <p>
 * @author Florian Maurer, florian.maurer@floHweb.ch
 */
class LIBMINISIP_API GroupListUserElement: public MObject{
	public:
		/**
		 * Constructor
		 * @param uri       the uri of the user
		 * @param addr   the address of the user
		 * @param RTPport   the port for the RTP packets (media stream)
		 * @param RTCPport  the port for the RTCP packets (floor control)
		 * @param codec     the used codec
		 * @param priority  the priority of the user
		 * @param seqNo     the current sequence number the user is using
		 * @param status    the status of the user for the floor control. The possible states
		 *                  are defined in the P2T class.
		 * @param callId    the callId of the SipDialogP2Tuser dialog for this user
		 * @see P2T
		 */
		GroupListUserElement( std::string uri, IPAddress* addr, int RTPport, int RTCPport, Codec *codec,
			int priority=0,
			int seqNo=0,
			int status=0,
			int ssrc=0,
			 std::string callId="");
		
		/**
		 * Constructor
	 	 * @param uri       the uri of the user
		 * @param priority  the priority of the user
		 * @param status    the status of the user for the floor control. The possible 
		 *                  states are defined in the P2T class.
		 * @see P2T
		*/
		GroupListUserElement( std::string uri, int priority=0, int status=0);
		
		/**
		 * Destructor
		 */
		~GroupListUserElement();
		
		/**
		 * returns the name of the GroupListUserElement.
		 * Used by the Memory Handling of Minisip.
		 * @return the  std::string 'GroupListUserElement'
		 */
		virtual std::string getMemObjectType() const {return "GroupListUserElement";}

		/**
		 * set user's uri
		 * @param uri the uri of the user
		 */
		void setUri(std::string uri){this->uri=uri;}
		
		/**
		 * set user's address
		 * @param addr a <code>IPAddress</code> object with the address of the user
		 */
		void setAddress(IPAddress* addr){to_addr=addr;}
		
		/**
		 * set user's RTP port where he is listening to the media stream
		 * @param port the port number
		 */
		void setRTPport(int port){RTPport=port;}
		
		/**
		 * set user's RTCP port where he is receiving the floor control
		 * messages
		 * @param port the port number
		 */
		void setRTCPport(int port){RTCPport=port;}
		
		/**
		 * set user's priority
		 * @param prio the priority
		 */
		void setPriority(int prio){priority=prio;}
		
		/**
		 * set user's codec.
		 * @param codec a <code>Codec</code> object
		 */
		void setCodec(Codec* codec){this->codec=codec;}
		
		/**
		 * set user's sequence number
		 * @param seqNo the sequence number
		 */
		void setSeqNo(int seqNo){this->seqNo=seqNo;}
		
		/**
		 * set user's status
		 * @param status the status
		 */
		void setStatus(int status){this->status=status;}
		
		/**
		 * set user's ssrc
		 * @param status the status
		 */
		void setSSRC(int ssrc){this->ssrc=ssrc;}
		
		/**
		 * set user's callId
		 * @param callId the callId of the SipDialogP2Tuser dialog
		 */
		void setCallId( std::string callId){this->callId=callId;}
		
		/**
		 * set localStarted. 
		 * indicates that the SIP Session was initiated by
		 * the local user.
		 * @param localStarted
		 */
		void setLocalStarted(bool localStarted){this->localStarted=localStarted;}

		/**
		 *get user's uri
		 *@return a  std::string containing the uri
		 */
		 std::string getUri(){return uri;}
		
		/**
		 *get user's address
		 *@return a <code>IPAddress</code> object
		 */
		IPAddress* getAddress(){return to_addr;}
		
		/**
		 *get user's codec
		 *@return a <code>Codec</code> object
		 */
		Codec* getCodec(){return codec;}
		
		/**
		 *get user's priority
		 *@return an integer containing the priority
		 */
		int getPriority(){return priority;}
		
		/**
		 *get user's RTPport
		 *@return an integer with the port number
		 */
		int getRTPport(){return RTPport;}
		
		/**
		 *get user's RTCP port
		 *@return an integer with the port number
		 */
		int getRTCPport(){return RTCPport;}
		
		/**
		 *get user's current sequence number
		 *@return an integer containing the sequence number
		 */
		int getSeqNo(){return seqNo;}
		
		/**
		 *get user's uri
		 *@return an integer containing the status
		 */
		int getStatus(){return status;}
		/**
		 *get user's ssrc
		 *@return an integer containing the status
		 */
		int getSSRC(){return ssrc;}
		
		/**
		 *get user's callId 
		 *@return the callId of the SipDialogP2Tuser dialog
		 */
		 std::string getCallId(){return callId;}	
		
		/**
		 * return true if the SIP Session was initiated by
		 * the local user.
		 * @return true/false
		 */
		bool getLocalStarted(){return localStarted;}
		
			
		
	
	private:
		///the uri of the user
		 std::string uri;
		///the address of the user
		IPAddress* to_addr;
		///the priority of the user
		int priority;
		/// the port for RTP packets (media stream)
		int RTPport;
		///the port for RTCP packets (floor control)
		int RTCPport;
		///the used codec
		Codec* codec;
		///the sequence number the user is currently using
		int seqNo;
		///the ssrc the user is currently using
		int ssrc;
		///the status of the user for the floor control
		int status;
		///the callID of the SipDialogP2Tuser dialog
		 std::string callId;
		///indicates that the local user sent the INVITE message
		bool localStarted;
		

};

#endif
