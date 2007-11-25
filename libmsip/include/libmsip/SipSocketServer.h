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


#ifndef SipSocketServer_H
#define SipSocketServer_H

#include<libmsip/libmsip_config.h>

#include<libmnetutil/Socket.h>
#include<libmnetutil/ServerSocket.h>
#include<libmnetutil/SocketServer.h>
#include<libmnetutil/DatagramSocket.h>
#include<libmutil/Thread.h>

class SipLayerTransport;

class SipSocketReceiver : public virtual MObject {
	public:
		virtual void addSocket(MRef<StreamSocket *> sock) = 0;
		virtual void datagramSocketRead(MRef<DatagramSocket *> sock) = 0;
};

/**
 * Purpose: Listens on a TCP or TLS server socket and reports
 * when a client connects to it.
 *
 */
class SipSocketServer : public SocketServer, InputReadyHandler {
	public:
		SipSocketServer(MRef<SipSocketReceiver*> r, MRef<Socket*> sock );
		virtual ~SipSocketServer();
		void free();
		std::string getMemObjectType() const {return "SipSocketServer";}

		MRef<Socket *> getSocket() const;
		MRef<SipSocketReceiver *> getReceiver() const;
		void setReceiver(MRef<SipSocketReceiver *> r);

		bool isIpv6() const;
		int32_t getType() const;

		const std::string &getExternalIp() const { return externalIp; }
		void setExternalIp( const std::string &ip ) { externalIp = ip; }

		/** Override server port  */
		void setExternalPort(int32_t port) { externalPort = port; }
		int32_t getExternalPort() const { return externalPort; }

		virtual void inputReady()=0;

	protected:
		virtual void inputReady( MRef<Socket*> socket );

	private:
		MRef<Socket *> ssock;
		MRef<SipSocketReceiver *> receiver;
		std::string externalIp;
		int32_t externalPort;
};


// 
// StreamSocketServer
// 

class StreamSocketServer : public SipSocketServer{
	public:
		StreamSocketServer(MRef<SipSocketReceiver*> r, MRef<ServerSocket*> sock );
		std::string getMemObjectType(){return "StreamSocketServer";}
		virtual void inputReady();
};


// 
// DatagramSocketServer
// 
class DatagramSocketServer : public SipSocketServer{
	public:
		DatagramSocketServer(MRef<SipSocketReceiver*> r, MRef<DatagramSocket*> sock );
		std::string getMemObjectType(){return "DatagramSocketServer";}
		virtual void inputReady();
};


#endif
