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

#ifndef NETWORKFUNCTIONS_H
#define NETWORKFUNCTIONS_H

#include<libmnetutil/libmnetutil_config.h>

#include<vector>
#include<string>

#include<libmutil/mtypes.h>
#include<libmutil/MemObject.h>

class LIBMNETUTIL_API NetworkInterface : public MObject{
	public:
		const std::string &getName() const;

		const std::vector<std::string> &getIPStrings( bool ipv6 = false ) const;

		void addIPString( const std::string &ip, bool ipv6 = false );

		NetworkInterface(const std::string &name);
		~NetworkInterface();

	private:
		std::string m_name;
		std::vector<std::string> m_ip4Strs;
		std::vector<std::string> m_ip6Strs;
};

/**
Collection of network utilities
*/
class LIBMNETUTIL_API NetworkFunctions{
	public:

		static void init();

		/**
		@return a string vector with all available network interfaces
		*/
		static std::vector<std::string> getAllInterfaces();
		
		/**
		@return given the name of a network interface (obtained with Network::getAllInterfaces(),
			for example), it returns a string formatted IPv4 address (numerical).
		*/
		static std::string getInterfaceIPStr(std::string iface);

		/**
		 * Return a list containing all interfaces.
		 */
		static std::vector<MRef<NetworkInterface*> > getInterfaces();

		
		/**
		@param ipStr a string containing an ip address
		@return string containing the interface using this string, or an empty string if the ip is 
			not used by any interface
		*/
		static std::string getInterfaceOf(std::string ipStr);
		
		/**
		Does an SRV query
		@param service The service name, for example: _sip._udp
		@param domain name/ip of the domain we want to obtain the service (for example, minisip.org)
		@param ret_port a return param, filled by this function, contains the port number of the service
		@return the name (or ip address, numeric) of the host handling the requested service
		*/
		static std::string getHostHandlingService(std::string service, std::string domain, uint16_t &ret_port);
		
		/**
		Given a list of ips (strings) and a random ip, return whether that ip is in the list
		*/
		static bool isLocalIP(uint32_t ip, std::vector<std::string> &localIPs);

		//OBS: ip is host byte order
		/**
		@param ip integer format ip (in host byte order ... )
		@param strBufMin16 return parameter (the char * needs to be at least 16 chars long).
		*/
		static void binIp2String(uint32_t ip, char *strBufMin16);

};
#endif
