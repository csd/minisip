/*
 Copyright (C) 2004-2006 the Minisip Team
 
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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2004, 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef IP_PROVIDER_H
#define IP_PROVIDER_H

#include<libminisip/libminisip_config.h>

#include<libmutil/MemObject.h>

#include<string>

class UDPSocket;
class SipSoftPhoneConfiguration;

class LIBMINISIP_API IpProvider: public MObject{
	public:
		virtual std::string getExternalIp()=0;
		virtual uint16_t getExternalPort( MRef<UDPSocket *> sock )=0;

		static MRef<IpProvider *> create( MRef<SipSoftPhoneConfiguration *> config, bool useIpv6 = false );
		
};

#include<libminisip/signaling/sip/SipSoftPhoneConfiguration.h>

#endif
