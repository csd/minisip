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


#ifndef SIPMESSAGETRANSPORT_H
#define SIPMESSAGETRANSPORT_H

#ifdef _MSC_VER
#ifdef LIBMSIP_EXPORTS
#define LIBMSIP_API __declspec(dllexport)
#else
#define LIBMSIP_API __declspec(dllimport)
#endif
#else
#define LIBMSIP_API
#endif

#include<libmnetutil/UDPSocket.h>
#include<libmnetutil/TCPSocket.h>
#include<libmnetutil/TLSSocket.h>
#include<libmutil/cert.h>
#include<libmutil/Mutex.h>
#include<libmutil/Semaphore.h>
#include<libmutil/MemObject.h>
#include<libmsip/SipSMCommand.h>
#include<list>
#include<libmsip/SipMessage.h>

class SipDialogContainer;

class SipMessage;

class LIBMSIP_API SipMessageTransport : public virtual MObject{
	public:
		SipMessageTransport(string local_ip, 
                        string contactIP,
			string preferredTransport="UDP",
                        int32_t externalContactUdpPort=5060, 
                        int32_t local_udp_port=5060, 
                        int32_t local_tcp_port=5060 
#ifndef NO_SECURITY			
                        ,int32_t local_tls_port=5061,
			MRef<certificate_chain *> cchain=NULL, 
                        MRef<ca_db *> cert_db = NULL
#endif
			);
		
		void setSipSMCommandReceiver(MRef<SipSMCommandReceiver*> rec);
		
		void setCommandReceiver(MRef<CommandStringReceiver* > rcvr);

                virtual std::string getMemObjectType(){return "SipMessageTransport";}

		void sendMessage(MRef<SipMessage*> pack, 
				IPAddress &toaddr, 
				int32_t port, 
				string branch,
				bool addVia
				);
			
		void addSocket(MRef<StreamSocket *> sock);

		string getLocalIP(){return localIP;};

		int32_t getLocalUDPPort(){return localUDPPort;};
		int32_t getLocalTCPPort(){return localTCPPort;};
		int32_t getLocalTLSPort(){return localTLSPort;};

#ifndef NO_SECURITY
		MRef<certificate_chain *> getCertificateChain(){ return cert_chain; };
		MRef<certificate*> getMyCertificate(){ return cert_chain->get_first(); };
#endif

		void udpSocketRead();

	private:
		void addViaHeader( MRef<SipMessage*> pack, MRef<StreamSocket *> socket, string branch );
		MRef<StreamSocket *> findStreamSocket(IPAddress&, uint16_t);
		
		UDPSocket udpsock;
                
                Mutex socksLock;
		list<MRef<StreamSocket *> > socks;
                Mutex socksPendingLock;
		list<MRef<StreamSocket *> > socksPending;

		string localIP;
		string contactIP;
		string preferredTransport;
		int32_t externalContactUdpPort;
		int32_t localUDPPort;
		int32_t localTCPPort;
		int32_t localTLSPort;

#ifndef NO_SECURITY
		MRef<certificate_chain *> cert_chain;
		MRef<ca_db *> cert_db;
		void * tls_ctx;
#endif

		MRef<SipSMCommandReceiver *> commandReceiver;

                Semaphore semaphore;

		friend class StreamThreadData;

};

#include<libmsip/SipMessage.h>
#include<libmsip/SipDialogContainer.h>


#endif
