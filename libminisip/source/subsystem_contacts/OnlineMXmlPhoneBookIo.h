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

/* Copyright (C) 2006 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Erik Ehrlund <eehrlund@kth.se>
*/

#ifndef OnlineMXML_PHONEBOOK_IO_H
#define OnlineMXML_PHONEBOOK_IO_H

#include<libminisip/libminisip_config.h>

#include<libminisip/contacts/PhoneBook.h>

#include<string>

/* Uses mutil/XMLParser to read and save a phonebook file */
class OnlineConfBack;
class OnlineMXmlPhoneBookIo : public PhoneBookIo{
	public:
                OnlineMXmlPhoneBookIo(OnlineConfBack *confback );
		virtual void save( MRef< PhoneBook * > book );
		virtual MRef< PhoneBook * > load();
		virtual std::string getMemObjectType() const {return "PhoneBookIo";}

		virtual std::string getPhoneBookId();

	private:
		std::string getDefaultPhoneBookString();
		void createDefault();
		std::string fileName;
                OnlineConfBack *conf;
};

#endif
