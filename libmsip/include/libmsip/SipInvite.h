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


/* Name
 * 	SipInvite.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/



#ifndef SIPINVITE_H
#define SIPINVITE_H

#ifdef _MSC_VER
#ifdef LIBMSIP_EXPORTS
#define LIBMSIP_API __declspec(dllexport)
#else
#define LIBMSIP_API __declspec(dllimport)
#endif
#else
#define LIBMSIP_API
#endif

#include<libmsip/SipMessage.h>
#include<vector>
#include<sys/types.h>

/**
 * Representation of a SIP INVITE method.
 * @author Erik Eliasson, eliasson@it.kth.se
 */
class LIBMSIP_API SipInvite : public SipMessage{

	public:
		const static int type;
                
		/**
		 * Parses a INVITE packet from a string.
		 */
		SipInvite(string &build_from);

		/**
		 * Creates a INVITE packet from parameters
		 * @param call_id Call ID of this call
		 * @param tel_no Number(/user) we are trying to contact
		 * @param proxy Remote proxy/user agent
		 * @param local_ip Local IP address of this user agent
		 * @param from_tel_no Local contact (tel no/user)
		 * @param seq_no Sequence number of this packet.
		 */
		SipInvite(const string &branch,
				const string &call_id, 
				const string &tel_no, 
				const string &proxy, 
				int32_t proxyPort, 
				const string &localIp,
				int32_t localSipPort, 
				const string &from_tel_no, 
				int32_t seq_no,
				const string &transport
				);
		
		/**
		 * Creates a INVITE packet from parameters
		 * @param call_id Call ID of this call
		 * @param tel_no Number(/user) we are trying to contact
		 * @param proxy Remote proxy/user agent
		 * @param local_ip Local IP address of this user agent
		 * @param from_tel_no Local contact (tel no/user)
		 * @param seq_no Sequence number of this packet.
		 * @param username Username to use to authenticate to the proxy with
		 * @param nonce Nonce to use to authenticate to the proxy with. Typically received with a 407 error message
		 * @param realm Realm to use to authenticate to the proxy with.
		 * 		Typically received with a 407 error message.
		 * @param password Password to use to authenticate to the proxy with.
		 */
		SipInvite(const string &branch,
				const string &call_id, 
				const string &tel_no,
				const string &proxy, 
				int32_t proxyPort,
				const string &localIp, 
				int32_t localSipPort, 
				const string &from_tel_no, 
				int32_t seq_no, 
				const string &username, 
				const string &nonce, 
				const string &realm, 
				const string &password,
				const string &transport
				);
		
		
		virtual ~SipInvite();


		virtual std::string getMemObjectType(){return "SipInvite";}
		
		/**
		 * @returns IP+port where the remote ua accepts sound data.
		 */
//		MRef<SdpPacket*> getSdp();
		void checkAcceptContact();

		string getRemoteTelNo();

		string getString();
		
		/**
		 * set the P2T flag
		 */
		void set_P2T();
		void set_ConfJoin();
		void set_ConfConnect();
		
		/**
		 *@returns true for a P2T invitation
		 */
		bool is_P2T();
		bool is_ConfJoin();
		bool is_ConfConnect();
		
	private:
		void createHeadersAndContent( const string &call_id,
				const string &tel_no,
				const string &proxyAddr,
				int32_t proxyPort,
				const string &localAddr,
				int32_t localSipPort,
				const string &from_tel_no,
				int32_t seq_no,
				const string &username,
				const string &nonce,
				const string &realm,
				const string &password,
				const string &transport);
		string username; //telephone number for example
		string ip;
		string user_type; //phone or ip
		
		/**
		 * indicates if the packet is an invitation
		 * to a P2T session
		 */
		bool P2T;
		bool ConfJoin;
		bool ConfConnect;
};

#endif

