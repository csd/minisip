/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  Copyright (C) 2005-2007  Mikael Magnusson
  
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

#include<config.h>
#include<libmnetutil/NetworkException.h>
#include<libmnetutil/TCPSocket.h>
#include<libmnetutil/TcpServerSocket.h>
#include"SipTransportTcp.h"

static std::list<std::string> pluginList;
static int initialized;

using namespace std;

extern "C" LIBMSIP_API
std::list<std::string> *mtcp_LTX_listPlugins( MRef<Library*> lib ){
	if( !initialized ){
		pluginList.push_back("getPlugin");
		initialized = true;
	}

	return &pluginList;
}

extern "C" LIBMSIP_API
MPlugin * mtcp_LTX_getPlugin( MRef<Library*> lib ){
	return new SipTransportTcp( lib );
}


SipTransportTcp::SipTransportTcp( MRef<Library*> lib ) : SipTransport( lib ){
}

SipTransportTcp::~SipTransportTcp(){
}



MRef<SipSocketServer *> SipTransportTcp::createServer( MRef<SipSocketReceiver*> receiver, bool ipv6, const string &ipString, int32_t &prefPort, MRef<CertificateSet *> cert_db, MRef<CertificateChain *> certChain )
{
	MRef<ServerSocket *> sock;
	MRef<SipSocketServer *> server;
	int32_t port = prefPort;

	sock = TcpServerSocket::create( port, ipv6 );
	server = new StreamSocketServer( receiver, sock );
	server->setExternalIp( ipString );

	prefPort = sock->getPort();

	return server;
}

MRef<StreamSocket *> SipTransportTcp::connect( const IPAddress &addr, uint16_t port, MRef<CertificateSet *> cert_db, MRef<CertificateChain *> certChain ){
	return new TCPSocket( addr, port );
}

uint32_t SipTransportTcp::getVersion() const{
	return 0x00000001;
}
