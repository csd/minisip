/*
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2004, 2005, 2006
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/

#ifndef SIMPLE_IP6_PROVIDER
#define SIMPLE_IP6_PROVIDER

#include<libminisip/ipprovider/IpProvider.h>

/**
 * Selects one of the local IPv6 addresses with the greatest scope as
 * both the local and external ip. The external and local port numbers
 * are identical.
 */
class SimpleIp6Provider: public IpProvider{
	public:
		SimpleIp6Provider( MRef<SipSoftPhoneConfiguration *> config );
		
		virtual std::string getExternalIp();
		virtual std::string getLocalIp();
		virtual uint16_t getExternalPort( MRef<UDPSocket *> sock );

		virtual std::string getMemObjectType() const {return "SimpleIp6Provider";};

		enum Scope {
			INVALID = 0,
			LINK_LOCAL = 2,		// fe80::/10
			SITE_LOCAL = 5,		// fec0::/10
			UNIQUE_LOCAL = 7,	// fc00::/7
			GLOBAL = 0xE		// 2000::/3
		};

		static Scope ipScope( std::string ip );

	private:
		std::string localIp;

};

#endif
