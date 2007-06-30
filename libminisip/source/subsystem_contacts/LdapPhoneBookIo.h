/*
 Copyright (C) 2004-2007 the Minisip Team
 
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

/* Copyright (C) 2007
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/

#ifndef LDAP_PHONEBOOK_IO_H
#define LDAP_PHONEBOOK_IO_H

#include<libminisip/libminisip_config.h>

#include<libmnetutil/LdapUrl.h>
#include<libminisip/contacts/PhoneBook.h>

#include<string>

class LIBMINISIP_API LdapPhoneBookIo : public PhoneBookIo{
	public:
		LdapPhoneBookIo( std::string fileName );

		virtual MRef<PhoneBook *> load();
		virtual void save(MRef<PhoneBook *> pb);
		virtual std::string getMemObjectType() const {return "LdapPhoneBookIo";}

		virtual std::string getPhoneBookId();

	private:
		LdapUrl url;
};

class LIBMINISIP_API LdapPhoneBookIoDriver : public PhoneBookIoDriver{
	public:
		LdapPhoneBookIoDriver( MRef<Library *> lib );
		virtual ~LdapPhoneBookIoDriver();

		// MObject Impl
		virtual std::string getMemObjectType() const {return "LdapPhoneBookIoDriver";}
		// MPlugin impl
		virtual std::string getDescription() const { return "Ldap PhoneBook IO driver"; };
		virtual std::string getName() const { return "LdapPhoneBookIo"; }
		virtual uint32_t getVersion() const {return 0x00000001;}

		// PhoneBookIo Impl
		virtual std::string getPrefix() const { return "ldap"; }
		virtual MRef<PhoneBookIo*> createPhoneBookIo(const std::string &name) const;
};

#endif
