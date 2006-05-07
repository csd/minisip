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

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef SIP_DIALOG_SECURITY_CONFIG
#define SIP_DIALOG_SECURITY_CONFIG

#include<libminisip/libminisip_config.h>

#include<libmutil/MemObject.h>

#define KEY_MGMT_METHOD_NULL            0x00
#define KEY_MGMT_METHOD_MIKEY           0x10
#define KEY_MGMT_METHOD_MIKEY_DH        0x11
#define KEY_MGMT_METHOD_MIKEY_PSK       0x12
#define KEY_MGMT_METHOD_MIKEY_PK        0x13

class XMLFileParser;
class certificate_chain;
class ca_db;
class SipIdentity;
class ConfBackend;

class LIBMINISIP_API SipDialogSecurityConfig{
	public:
		SipDialogSecurityConfig();

		void useIdentity( MRef<SipIdentity *> );

		bool secured;
		int ka_type;

		bool use_srtp;		
		bool use_ipsec;
		bool use_zrtp;
		
		MRef<certificate_chain *> cert;
		MRef<ca_db *> cert_db;
		bool psk_enabled;
		unsigned char * psk;
		unsigned int psk_length;
		bool dh_enabled;
		bool check_cert;

		void save( MRef<ConfBackend *> backend );
		void load( MRef<ConfBackend *> backend );
};


#endif
