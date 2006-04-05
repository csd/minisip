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

#ifndef P2TDEFINITIONS_H
#define P2TDEFINITIONS_H

#include<libminisip/libminisip_config.h>

#include<string>

/**
 * first part of the application-dependent data in a RTCP APP packet.
 * <p><b>RTCP APP FLOOR Message:</b><br>
 * For the Floor Control in a P2T Session the following prototype of a 
 * RTCP APP message is used:
 * <p>
 * <img src=material/RtcpAPPFloor.gif>
 * <p>
 * This struct represents the first two fields 'sequence number' and
 * 'optional' with the correct bit size. This two fields are mandatory
 * for every Floor Control Message.<p>
 * The struct is defined as follows:<p>
 * <code>
 * struct rtcpAPPheader{<br>
 * &nbsp;&nbsp;unsigned sn:16; //the sequence number field<br>
 * &nbsp;&nbsp;unsigned opt:16; //the optional field<br>
 * };<br>
 * </code> 
 */
struct rtcpAPPfloorControl1{          
	unsigned sn:16;	//sequence number
	unsigned opt:16; //reason code, warning code or nothing
};

 /**
 * second part of the application-dependent data in a RTCP APP packet.
 * <p><b>RTCP APP FLOOR Message:</b><br>
 * For the Floor Control in a P2T Session the following prototype of a 
 * RTCP APP message is used:
 * <p>
 * <img src=material/RtcpAPPFloor.gif>
 * <p>
 * This struct represents the 'SDES item CNAME' header fields with the correct bit size. This field is only
 * used in the Floor REQUEST and Floor GRANT message.<p>
 * The struct is defined as follows:<p>
 * <code>
 * struct rtcpAPPheader{<br>
 * &nbsp;&nbsp;unsigned sdes:8; //the SDES type<br>
 * &nbsp;&nbsp;unsigned length:8; //the length of the SDES value<br>
 * };<br>
 * </code>
 */
struct rtcpAPPfloorControl2{          
	unsigned sdes:8;	//SDES item
	unsigned length:8; 	//length
};




/**
 * Definitions and constant values for P2T Sessions.
 * In this class global parameters for the whole P2T
 * functionality can be configured.
 * @author Florian Maurer florian.maurer@floHweb.ch
 */

class LIBMINISIP_API P2T{
	
	public:

	/**@name Performance Measurements */
	//@{
	///the filename where the results will be saved.
	static const std::string PERFORMANCE_FILE;
	//@}
	
	
	//--------Group Member List---------
		
	/**@name Group Member List */
	//@{
		
		/**@name Status field */
		//@{
		
		///standard value
		static const int STATUS_IDLE;
		
		///the user is connected
		static const int STATUS_CONNECTED;
		
		///the user granted the floor
		static const int STATUS_GRANT;
		
		///the user requested the floor as well
		static const int STATUS_COLLISION;
		
		///the user send a IDLE
		static const int STATUS_RELEASED;
		
		///the user is talking
		static const int STATUS_TALKING;
		
		///the user requested the floor
		static const int STATUS_REQUESTING;
	
		///is waiting that the local user accepts the SIP Session
		static const int STATUS_WAITACCEPT;
		
		///the user didn't send an answer back
		static const int STATUS_NOTAVAILABLE;
		
		/**
		 * returns the name of a status.
		 * @param status
		 * @param return a std::string containing the name
		 *               of the specified status
		 */
		static std::string getStatus(int status);
		
		//@}
		
		/**@name Session Type field */
		//@{
		
		///Instant Personal Talk
		static const int INSTANT_PERSONAL_TALK;
		
		///Ad-Hoc Instant Group Talk
		static const int ADHOC_INSTANT_GROUP;
		
		///Instant Group Talk
		static const int INSTANT_GROUP;
		
		///Chat Group Talk
		static const int CHAT_GROUP;
		
		/**
		 * returns the name of the session type
		 * @param type
		 * @param return a std::string containing the name
		 *               of the specified session type
		 */
		static std::string getSessionType(int type);
		
		//@}
		
		/**@name Membership field */
		//@{
		
		///Open membership without memberlist
		static const int MEMBERSHIP_OPEN;
		///Restricted membership with memberlist
		static const int MEMBERSHIP_RESTRICTED;
		/**
		 * returns the membership
		 * @param membership
		 * @return a std::string containing the membership
		 *         (open/restricted)      
		 */
		static std::string getMembership(int membership);
		//@}
	//@}
		
		
		
	//------------------Timer Values----------------
		
	/**@name Timer Values */
	//@{
		
		///the interval between resending Floor REVOKE messages.
		static const int timerREVOKE;
		///the maximum value for the collision time for resending a Floor REQUEST message
		static const float timerRESEND;
		///the time before resending a Floor REQUEST message
		static const int timerGRANT;
		///the time before resending a Floor RELEASE message
		static const int timerIDLE;
		///the time before resending a Floor GRANT message
		static const int timerTAKEN;
		///the time RtcpTransactionReleaseFloor waits for an answer
		static const int timerRelTIMEOUT;
		///the time RtcpTransactionGetFloor waits for an answer
		static const int timerGetTIMEOUT;
		///the time before RtcpTransactionGetFloor terminates
		static const int timerGetFloorTERMINATE;
		///the time before RtcpTransactionGrantFloor terminates
		static const int timerGrantFloorTERMINATE;
		///the time before RtcpTransactionReleaseFloor terminates
		static const int timerRelFloorTERMINATE;
		///the time before RtcpTransactionIdleFloor terminates
		static const int timerIdleFloorTERMINATE;
		///the time before RtcpTransactionTakenFloor terminates
		static const int timerTakenFloorTERMINATE;
		
		
		
	//@}

	//------------------RTCP APP packets----------------
		
	/**@name RTCP APP packets */
	//@{
		
		///the name for the RTCP APP packets set
		static const std::string APP_NAME;

		/**@name Subtype */
		//@{
		///Floor REQUEST message
		static const int APP_REQUEST;
		///Floor GRANT message
		static const int APP_GRANT;
		///Floor TAKEN message;
		static const int APP_TAKEN;
		///Floor DENY message
		static const int APP_DENY;
		///Floor RELEASE message
		static const int APP_RELEASE;
		///Floor IDLE message
		static const int APP_IDLE;
		///Floor REVOKE message
		static const int APP_REVOKE;
		//@}
		
	//@}
};

#endif
