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

#ifndef SipTransport_H
#define SipTransport_H

#include<libmsip/libmsip_config.h>

#include<libmutil/MemObject.h>
#include<libmutil/MPlugin.h>
#include<libmutil/MSingleton.h>

#include<libmnetutil/StreamSocket.h>
#include<libmcrypto/cert.h>

#include<libmsip/SipSocketServer.h>
#include<libmsip/SipStack.h>

/**
 * A plugin representing a SIP transport protocol,
 * for example UDP, TCP or TLS
 */
class LIBMSIP_API SipTransport: public MPlugin{
	public:
		/** @return scheme of SIP(S) URI:s */
		virtual std::string getUriScheme() const;

		/** @return true if it is a encrypted SIPS transport */
		virtual bool isSecure() const=0;

		/** @return transport protocol id in lower case, such as udp */
		virtual std::string getProtocol() const=0;

		/**
		 * @return Via header transport protocol in upper case,
		 * for example DTLS-UDP
		 */
		virtual std::string getViaProtocol() const=0;

		/**
		 * @return 5061 for secure transports and 5060 otherwise.
		 */
		virtual int32_t getDefaultPort() const;

		/** @return srv prefix used when looking up SRV RR */
		virtual std::string getSrv() const;

		/** @return NAPTR services field value */
		virtual std::string getNaptrService() const=0;

		/**
		 * One of MSOCKET_TYPE_*
		 */
		virtual int32_t getSocketType() const=0;

		/**
		 * Setup a new listening socket.
		 * @arg prefPort Port to listen on. If 0, then it contains
		 * the chosen dynamic port after returning.
		 */
		virtual MRef<SipSocketServer *> createServer( MRef<SipSocketReceiver*> receiver, bool ipv6, const std::string &ipString, int32_t &prefPort, MRef<CertificateSet *> cert_db = NULL, MRef<CertificateChain *> certChain = NULL ) = 0;
		/**
		 * Setup a new connection, implemented by
		 * connection-oriented transports only
		 */
		virtual MRef<StreamSocket *> connect( const IPAddress &addr, uint16_t port, MRef<CertificateSet *> cert_db = NULL, MRef<CertificateChain *> certChain = NULL );

		// MPlugin
		std::string getPluginType() const { return "SipTransport"; }

	protected:
		SipTransport( MRef<Library *> lib );
		SipTransport();
};


class LIBMSIP_API SipTransportRegistry: public MPluginRegistry, public MSingleton<SipTransportRegistry>{
	public:
		virtual std::string getPluginType(){ return "SipTransport"; }

		std::list<std::string> getNaptrServices( bool secureOnly ) const;

		/*
		 * Search for transport by protocol (udp or tcp),
		 * and if it's secure or not (sips/sip)
		 */
		MRef<SipTransport*> findTransport( const std::string &protocol, bool secure=false ) const;

		/*
		 * Search for transport suitable for sending
		 * requests to the specified SIP URI.
		 */
		MRef<SipTransport*> findTransport( const SipUri &uri ) const;

		/*
		 * Search for transport by Via transport,
		 * like UDP, TCP, TLS or DTLS-UDP.
		 */
		MRef<SipTransport*> findViaTransport( const std::string &protocol ) const;

		/** Search for transport by socket type. */
		MRef<SipTransport*> findTransport( int32_t socketType ) const;

		/** Search for transport by plugin name */  
		MRef<SipTransport*> findTransportByName( const std::string &name ) const;

		/** Search for transport by NAPTR service field */
		MRef<SipTransport*> findTransportByNaptr( const std::string &service ) const;

		std::list<MRef<SipTransportConfig*> > createDefaultConfig() const;

	protected:
		SipTransportRegistry();

	private:
		friend class MSingleton<SipTransportRegistry>;
};

#endif
