/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  Copyright (C) 2006 Mikael Magnusson
  
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
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/


#ifndef SipLayerTransport_H
#define SipLayerTransport_H

#include<libmsip/libmsip_config.h>

#include<libmnetutil/DatagramSocket.h>
#include<libmnetutil/StreamSocket.h>
#include<libmutil/Mutex.h>
#include<libmutil/Semaphore.h>
#include<libmutil/MemObject.h>
#include<libmcrypto/cert.h>
#include<libmsip/SipSMCommand.h>
#include<libmsip/SipMessage.h>
#include<libmsip/SipSocketServer.h>
#include"SipCommandDispatcher.h"

#include<list>

class SipMessage;

class SipCommandDispatcher;
class SipTransport;
class StreamThreadData;
class StreamThreadServer;

class SipLayerTransport : public SipSMCommandReceiver,
			  public SipSocketReceiver {
	public:
		SipLayerTransport( MRef<CertificateChain *> cchain=NULL,
				   MRef<CertificateSet *> cert_db = NULL
		);

		virtual ~SipLayerTransport();
  
		virtual void stop();

		bool handleCommand(const SipSMCommand& cmd);
		
		
		void setDispatcher(MRef<SipCommandDispatcher*> d);

		virtual std::string getMemObjectType() const {return "SipLayerTransport";}

		void sendMessage(MRef<SipMessage*> pack, const std::string &branch,
				 bool addVia);

		void addSocket(MRef<StreamSocket *> sock);
		void removeSocket(MRef<StreamSocket *> sock);

		void addServer(MRef<SipSocketServer *> server);

		MRef<CertificateChain *> getCertificateChain();
		MRef<Certificate*> getMyCertificate();
		MRef<CertificateSet *> getCertificateSet ();

		void datagramSocketRead(MRef<DatagramSocket *> sock);

		void startServer( MRef<SipTransport*> transport, const std::string & ipString, const std::string & ip6String, int32_t &prefPort, int32_t externalUdpPort, MRef<CertificateChain *> certChain = NULL, MRef<CertificateSet *> cert_db = NULL);

		void stopServer( MRef<SipTransport*> transport );

		int32_t getLocalSipPort( const std::string &transport );

	protected:

		void sendMessage(MRef<SipMessage*> pack, 
				 const std::string &toaddr,
				int32_t port, 
				std::string branch,
				MRef<SipTransport*> transport,
				bool addVia
				);

		virtual MRef<SipSocketServer *> findServer( int32_t type, bool ipv6);
		virtual MRef<Socket *> findServerSocket( int32_t type, bool ipv6);

	private:
		
		/** 
		 * Checks if the message is valid and 
		 * the SIP stack should continue processing
		 * the message.
		 *
		 * If the method returns "false" no further
		 * processing should be made.
		 * 
		 * Note that the method itself can send SIP error
		 * responses.
		 */
		bool validateIncoming(MRef<SipMessage *> msg);

		bool getDestination(MRef<SipMessage*> pack, std::string &destAddr,
				    int32_t &destPort, MRef<SipTransport*> &destTransport);
		void addViaHeader( MRef<SipMessage*> pack, MRef<SipSocketServer*> server, MRef<Socket *> socket, std::string branch );
		MRef<StreamSocket *> findStreamSocket(IPAddress&, uint16_t);
		bool findSocket( MRef<SipTransport*> transport,
					 IPAddress &addr,
					 uint16_t port,
					 MRef<SipSocketServer*> &server,
					 MRef<Socket*> &socket);
		
		/**
		 * Set contact uri host and port to external ip and
		 * port configured on the server or local address and
		 * port of the socket it if contains "minisip" param.
		 */
		void updateContact(MRef<SipMessage*> pack,
				   MRef<SipSocketServer *> server,
				   MRef<Socket *> socket);
                

		/**
		 * Cache what UDP/TCP/TLS server ports are used.
		 * These values are used when creating SIP via headers.
		 *
		 * Note that the values are the one that will be added
		 * to the Via header, and NAT handling techniques may
		 * modify these values so that they differ from 
		 * the low layer socket values. 
		 */
		int contactUdpPort;
		int contactSipPort;
		int contactSipsPort;

		Mutex serversLock;
		std::list<MRef<SipSocketServer *> > servers;
		MRef<SocketServer*> manager;

		MRef<CertificateChain *> cert_chain;
		MRef<CertificateSet *> cert_db;
		void * tls_ctx;

		MRef<SipCommandDispatcher*> dispatcher;

		friend class StreamThreadData;

};

void set_debug_print_packets(bool);
bool get_debug_print_packets();


#endif
