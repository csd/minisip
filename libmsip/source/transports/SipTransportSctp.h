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

#ifndef SIP_TRANSPORT_SCTP_H
#define SIP_TRANSPORT_SCTP_H

#include<libmsip/libmsip_config.h>

#include<string>
#include<libmsip/SipTransport.h>
#include<libmnetutil/SctpSocket.h>

class SipTransportSctp: public SipTransport{
	public:
		SipTransportSctp( MRef<Library *> lib );
		virtual ~SipTransportSctp();

		// SipTransport
		virtual bool isSecure() const { return false; }

		virtual std::string getProtocol() const { return "sctp"; }

		virtual std::string getViaProtocol() const { return "SCTP"; }

		virtual int32_t getSocketType() const { return MSOCKET_TYPE_SCTP; }

		virtual std::string getNaptrService() const { return "SIP+D2S"; }

		virtual MRef<SipSocketServer *> createServer( MRef<SipSocketReceiver*> receiver, bool ipv6, const std::string &ipString, int32_t &prefPort, MRef<CertificateSet *> cert_db = NULL, MRef<CertificateChain *> certChain = NULL );

		virtual MRef<StreamSocket *> connect( const IPAddress &addr, uint16_t port, MRef<CertificateSet *> cert_db = NULL, MRef<CertificateChain *> certChain = NULL );

		// MPlugin
		virtual std::string getName() const { return "SCTP"; }

		virtual uint32_t getVersion() const;

		virtual std::string getDescription() const { return "SIP Transport SCTP"; };

		// MObject
		virtual std::string getMemObjectType() const { return getName(); }
};

#endif
