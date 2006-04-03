/*
  Copyright (C) 2005 Mikael Magnusson
  
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
 * Author(s): Mikael Magnusson <mikma@users.sourceforge.net>
 *
*/


/* Name
 * 	SipRequest.h
 * Authors
 * 	Mikael Magnusson, mikma@users.sourceforge.net
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 *	Base class of classes representing sip request messages, and
 *      used when parsing unsupported request methods.
*/



#ifndef SIPREQUEST_H
#define SIPREQUEST_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipMessage.h>
#include<libmutil/MemObject.h>

class SipStack;

class LIBMSIP_API SipRequest : public SipMessage{

	public:

		
		/**
		 *
		 * Note: The message will have a sequence
		 * number equal to the packet given as arguement
		 * to the function. This is ok for ACK packets,
		 * but for PRACK requests you will have to adjust it.
		 */
		static MRef<SipRequest*> createSipMessageAck(
				string branch,
				MRef<SipMessage*> pack,
				string to_tel_no,
				bool provisional=false);

		static MRef<SipRequest*> createSipMessageBye(
				string branch,
				string callId,
				string target,
				string to_uri,
				string from_uri,
				int32_t seq_no);

		static MRef<SipRequest*> createSipMessageCancel(
				string branch,
				MRef<SipRequest*> inv,
				string to_uri
				);

		static MRef<SipRequest*> createSipMessageIMMessage(
				string branch,
				string call_id,
				std::string toUri,
				const SipUri& fromUri,
				int32_t seq_no,
				string msg);


		static MRef<SipRequest*> createSipMessageInvite(
				const string &branch,
				const string &call_id,
				const string &tel_no,
				const string &proxyAddr,
				int32_t proxyPort,
				const string &localAddr,
				int32_t localSipPort,
				const string &from_tel_no,
				int32_t seq_no,
				const string &transport,
				MRef<SipStack*> stack
				);

		static MRef<SipRequest*> createSipMessageInvite(
				const string &branch,
				const string &call_id,
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
				const string &transport,
				MRef<SipStack*> stack);

		static MRef<SipRequest*> createSipMessageNotify(
				string branch,
				string call_id,
				const SipUri& toUri,
				const SipUri& fromUri,
				int32_t seq_no
				);

		static MRef<SipRequest*> createSipMessageRefer(
				string branch,
				MRef<SipRequest*> inv,
				string to_uri,
				string from_uri,
				string referredUri,
				int cSeqNo);

		static MRef<SipRequest*> createSipMessageRegister(
				string branch,
				string call_id,
				string domainarg,
				string localIp,
				int32_t sip_listen_port,
				string from_tel_no,
				int32_t seq_no,
				string transport,
				int expires,
				string auth_id="",
				string realm="",
				string nonce="",
				string password="");

		static MRef<SipRequest*> createSipMessageSubscribe(
				string branch,
				string call_id,
				const SipUri& toUri,
				const SipUri& fromUri,
				int32_t seq_no);




		SipRequest(string &build_from);

		SipRequest(string branch, const string &method,
				const string &uri = "");

		virtual ~SipRequest();

		virtual std::string getMemObjectType(){return "SipRequest("+method+")";}

		virtual string getString();

		virtual const string& getType();

		virtual void setMethod(const string &method);
		virtual string getMethod();

		virtual void setUri(const string &uri);
		virtual string getUri();

		/**
		 * Insert a route first in the list of routes, forcing
		 * the request through the proxy.
		 */
		void addRoute(const string &route);

		/**
		 * Insert a loose route first in the list of routes,
		 * forcing the request through the proxy with the
		 * specified address. Using the default transport if
		 * it's an empty string, and the default port if port
		 * is set to zero.
		 */
		void addRoute(const string &addr, int32_t port,
			      const string &transport);


		/**
		 * Adds From, To, CSeq, MaxForwards and CallId headers to
		 * the request. These are the required headers 
		 * that are added by the application layer (TU).
		 * (Via and Contact(?) headers are handled by the
		 *  transport layer.)
		 *
		 * This function is typically used when creating
		 * a SIP new request message.
		 *
		 * @param fromUri	Used to create From header
		 * @param toUri		Used to create To header
		 * @param method	Used to create CSeq header
		 * @param seqNo		Used to create CSeq header
		 * @param callId	Used to create CallId header. If
		 * 			empty string then a random value
		 * 			will be generated.
		 * 
		 */
		void addDefaultHeaders(const SipUri& fromUri, 
				const SipUri& toUri, 
				const string& method, 
				int seqNo, 
				const string& callId="");
		
	protected:

		virtual void init(string &build_from);

	private:
		string method;
		string uri;
};




#endif
