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

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef MXML_PHONEBOOK_IO_H
#define MXML_PHONEBOOK_IO_H

#include<config.h>
#include"PhoneBook.h"

/* Uses mutil/XMLParser to read and save a phonebook file */



class MXmlPhoneBookIo : public PhoneBookIo{
        public:
                MXmlPhoneBookIo( string fileName );

                virtual void save( MRef< PhoneBook * > book );
                virtual MRef< PhoneBook * > load();
		virtual std::string getMemObjectType(){return "PhoneBookIo";}

		virtual std::string getPhoneBookId();

        private:
                std::string fileName;
};

#endif
