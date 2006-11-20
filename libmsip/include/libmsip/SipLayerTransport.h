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
#include<libmutil/Thread.h>
#include<libmutil/Semaphore.h>
#include<libmutil/MemObject.h>
#include<libmcrypto/cert.h>
#include<libmsip/SipSMCommand.h>
#include<libmsip/SipMessage.h>
#include<libmsip/SipSocketServer.h>

#include<list>

class SipMessage;

class SipLayerTransport;
class SipCommandDispatcher;

class SipLayerTransport : public SipSMCommandReceiver {
	public:
		SipLayerTransport( MRef<certificate_chain *> cchain=NULL,
				   MRef<ca_db *> cert_db = NULL
		);

		virtual ~SipLayerTransport();
  
		virtual void stop();

		bool handleCommand(const SipSMCommand& cmd);
		
		
		void setDispatcher(MRef<SipCommandDispatcher*> d);

		virtual std::string getMemObjectType() const {return "SipLayerTransport";}

		void sendMessage(MRef<SipMessage*> pack, const std::string &branch,
				 bool addVia);

		void addSocket(MRef<StreamSocket *> sock);

		void addServer(MRef<SipSocketServer *> server);

		MRef<certificate_chain *> getCertificateChain();
		MRef<certificate*> getMyCertificate();
		MRef<ca_db *> getCA_db ();

		void datagramSocketRead(MRef<DatagramSocket *> sock);

	protected:
		void sendMessage(MRef<SipMessage*> pack, 
				 const std::string &toaddr,
				int32_t port, 
				std::string branch,
				std::string preferredTransport,
				bool addVia
				);

		virtual MRef<SipSocketServer *> findServer( int32_t type, bool ipv6);
		virtual MRef<Socket *> findServerSocket( int32_t type, bool ipv6);

	private:
		bool getDestination(MRef<SipMessage*> pack, string &destAddr,
				int32_t &destPort, string &destTransport);
		void addViaHeader( MRef<SipMessage*> pack, MRef<SipSocketServer*> server, MRef<Socket *> socket, std::string branch );
		MRef<StreamSocket *> findStreamSocket(IPAddress&, uint16_t);
		bool findSocket(const std::string &transport,
					 IPAddress &addr,
					 uint16_t port,
					 MRef<SipSocketServer*> &server,
					 MRef<Socket*> &socket);
		
                
		Mutex serversLock;
		std::list<MRef<SipSocketServer *> > servers;

		Mutex socksLock;
		std::list<MRef<StreamSocket *> > socks;
		Mutex socksPendingLock;
		std::list<MRef<StreamSocket *> > socksPending;

		MRef<certificate_chain *> cert_chain;
		MRef<ca_db *> cert_db;
		void * tls_ctx;

		MRef<SipCommandDispatcher*> dispatcher;

		Semaphore semaphore;

		friend class StreamThreadData;

};

void set_debug_print_packets(bool);
bool get_debug_print_packets();


#endif
