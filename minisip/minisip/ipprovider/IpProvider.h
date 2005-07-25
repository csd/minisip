/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Copyright (C) 2004, 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef IP_PROVIDER_H
#define IP_PROVIDER_H

#ifdef _MSC_VER
#ifndef uint16_t
typedef unsigned short  uint16_t;
#endif
#else
#include<stdint.h>
#endif

#include<libmutil/MemObject.h>

class UDPSocket;
class SipSoftPhoneConfiguration;

class IpProvider: public MObject{
	public:
		virtual std::string getExternalIp()=0;
		virtual uint16_t getExternalPort( MRef<UDPSocket *> sock )=0;

		static MRef<IpProvider *> create( MRef<SipSoftPhoneConfiguration *> config );
		
};
#endif
