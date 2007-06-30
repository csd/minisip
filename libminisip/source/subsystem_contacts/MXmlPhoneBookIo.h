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

#ifndef MXML_PHONEBOOK_IO_H
#define MXML_PHONEBOOK_IO_H

#include<libminisip/libminisip_config.h>

#include<libminisip/contacts/PhoneBook.h>

#include<string>

/* Uses mutil/XMLParser to read and save a phonebook file */

class LIBMINISIP_API MXmlPhoneBookIo : public PhoneBookIo{
	public:
		MXmlPhoneBookIo( std::string fileName );

		virtual void save( MRef< PhoneBook * > book );
		virtual MRef< PhoneBook * > load();
		virtual std::string getMemObjectType() const {return "PhoneBookIo";}

		virtual std::string getPhoneBookId();

	private:
		std::string getDefaultPhoneBookString();
		void createDefault();
		std::string fileName;
};

class LIBMINISIP_API MXmlPhoneBookIoDriver : public PhoneBookIoDriver{
	public:
		MXmlPhoneBookIoDriver( MRef<Library *> lib );
		virtual ~MXmlPhoneBookIoDriver();

		// MObject Impl
		virtual std::string getMemObjectType() const {return "PhoneBookIo";}
		// MPlugin impl
		virtual std::string getDescription() const { return "MXml PhoneBook IO driver"; };
		virtual std::string getName() const { return "MXmlPhoneBookIo"; }
		virtual uint32_t getVersion() const {return 0x00000001;}

		// PhoneBookIo Impl
		virtual std::string getPrefix() const { return "file"; }
		virtual MRef<PhoneBookIo*> createPhoneBookIo(const std::string &name) const;
};

#endif
