/*
  Copyright (C) 2006-2007 Mikael Magnusson
  
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
 * Authors: Mikael Magnusson <mikma@users.sourceforge.net>
*/

#ifndef SIP_TRANSPORT_UDP_H
#define SIP_TRANSPORT_UDP_H

#include<libmsip/libmsip_config.h>

#include<string>
#include<libmsip/SipTransport.h>

class SipTransportUdp: public SipTransport{
	public:
		SipTransportUdp( MRef<Library *> lib );
		virtual ~SipTransportUdp();

		// SipTransport
		virtual bool isSecure() const { return false; }

		virtual std::string getProtocol() const { return "udp"; }

		virtual std::string getViaProtocol() const { return "UDP"; }

		virtual int32_t getSocketType() const { return MSOCKET_TYPE_UDP; }

		virtual std::string getNaptrService() const { return "SIP+D2U"; }

		virtual MRef<SipSocketServer *> createServer( MRef<SipSocketReceiver*> receiver, bool ipv6, const std::string &ipString, int32_t &prefPort, MRef<CertificateSet *> cert_db = NULL, MRef<CertificateChain *> certChain = NULL );

		// MPlugin
		virtual std::string getName() const { return "UDP"; }

		virtual uint32_t getVersion() const;

		virtual std::string getDescription() const { return "SIP Transport UDP"; };

		// MObject
		virtual std::string getMemObjectType() const { return getName(); }
};

#endif
