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


#ifndef SipLayerTransport_H
#define SipLayerTransport_H

#include<libmsip/libmsip_config.h>

#include<libmnetutil/UDPSocket.h>
#include<libmnetutil/TCPSocket.h>
#include<libmnetutil/TLSSocket.h>
#include<libmnetutil/ServerSocket.h>
#include<libmutil/Mutex.h>
#include<libmutil/Thread.h>
#include<libmutil/Semaphore.h>
#include<libmutil/MemObject.h>
#include<libmsip/SipSMCommand.h>
#include<libmsip/SipMessage.h>

#include<list>

class SipMessage;

class SipLayerTransport;
class SipCommandDispatcher;

/**
 * Purpose: Listens on a TCP or TLS server socket and reports
 * when a client connects to it.
 *
 */
class SocketServer : public Runnable{
	public:
		SocketServer(MRef<ServerSocket*> sock, MRef<SipLayerTransport*> r);
		std::string getMemObjectType(){return "SocketServer";}
		void run();
		void start();
		void stop();

	private:
		MRef<ServerSocket *> ssock;
		MRef<SipLayerTransport *> receiver;
		bool doStop;
};


class SipLayerTransport : public SipSMCommandReceiver {
	public:
		SipLayerTransport(std::string local_ip, 
							std::string contactIP,
							int32_t externalContactUdpPort=5060, 
							int32_t local_udp_port=5060, 
							int32_t local_tcp_port=5060 
							,int32_t local_tls_port=5061,
							MRef<certificate_chain *> cchain=NULL, 
							MRef<ca_db *> cert_db = NULL
		);

		void startTcpServer();
		void stopTcpServer();

		void startTlsServer();
		void stopTlsServer();

		bool handleCommand(const SipSMCommand& cmd);
		
		
		void setDispatcher(MRef<SipCommandDispatcher*> d);

		virtual std::string getMemObjectType(){return "SipLayerTransport";}

		void sendMessage(MRef<SipMessage*> pack, const std::string &branch,
				 bool addVia);

		void addSocket(MRef<StreamSocket *> sock);

		std::string getLocalIP(){return localIP;};

		int32_t getLocalUDPPort(){return localUDPPort;};
		int32_t getLocalTCPPort(){return localTCPPort;};
		int32_t getLocalTLSPort(){return localTLSPort;};

		MRef<certificate_chain *> getCertificateChain(){ return cert_chain; };
		MRef<certificate*> getMyCertificate(){ return cert_chain->get_first(); };
		MRef<ca_db *> getCA_db () { return cert_db; };

		void udpSocketRead();

	protected:
		void sendMessage(MRef<SipMessage*> pack, 
				std::string toaddr, 
				int32_t port, 
				std::string branch,
				std::string preferredTransport,
				bool addVia
				);
			
	private:
		void addViaHeader( MRef<SipMessage*> pack, MRef<Socket *> socket, std::string branch );
		MRef<StreamSocket *> findStreamSocket( std::string, uint16_t);
		MRef<Socket*> findSocket(const std::string &transport,
					 std::string addr,
					 uint16_t port);
		
		MRef<UDPSocket*> udpsock;
		MRef<SocketServer*> tcpSocketServer;
		MRef<SocketServer*> tlsSocketServer;
                
		Mutex socksLock;
		std::list<MRef<StreamSocket *> > socks;
		Mutex socksPendingLock;
		std::list<MRef<StreamSocket *> > socksPending;

		std::string localIP;
		std::string contactIP;
		int32_t externalContactUdpPort;
		int32_t localUDPPort;
		int32_t localTCPPort;
		int32_t localTLSPort;
		
		MRef<certificate_chain *> cert_chain;
		MRef<ca_db *> cert_db;
		void * tls_ctx;

		MRef<SipCommandDispatcher*> dispatcher;

		Semaphore semaphore;

		friend class StreamThreadData;

};

#include<libmsip/SipMessage.h>


void set_debug_print_packets(bool);
bool get_debug_print_packets();


#endif
