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

/* Copyright (C) 2004-2007
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/

/*
  LDAP Phone book id syntax:
  ldap://<server>/<dn>?cn,mobile,telephoneNumber,homePhone?base?(objectclass=*)
*/

#include<config.h>

#include<libmnetutil/LdapConnection.h>
#include<libminisip/contacts/PhoneBook.h>
#include<libminisip/contacts/ContactDb.h>
#include"LdapPhoneBookIo.h"

using namespace std;

static std::list<std::string> pluginList;
static int initialized;

extern "C"
std::list<std::string> *ldappb_LTX_listPlugins( MRef<Library *> lib ){
	if( !initialized ){
		pluginList.push_back("getPlugin");
		initialized = true;
	}

	return &pluginList;
}

extern "C"
MPlugin * ldappb_LTX_getPlugin( MRef<Library *> lib ){
	return new LdapPhoneBookIoDriver( lib );
}

MRef<PhoneBookIo*> LdapPhoneBookIoDriver::createPhoneBookIo( const string &name ) const
{
	return new LdapPhoneBookIo( name );
}

LdapPhoneBookIoDriver::LdapPhoneBookIoDriver( MRef<Library *> lib ): PhoneBookIoDriver( lib )
{
}

LdapPhoneBookIoDriver::~LdapPhoneBookIoDriver()
{
}

LdapPhoneBookIo::LdapPhoneBookIo( string fn ): url( fn ){
//   ldap://server/bind_dn?attributes?scope(sub|)?(filter)
	url.printDebug();
}

MRef<PhoneBook *> LdapPhoneBookIo::load(){
	if (!url.isValid()) {
		return NULL;
	}

	MRef<PhoneBook *> phonebook = new PhoneBook;
	MRef<LdapConnection*> conn = new LdapConnection(url.getHost(), url.getPort());

	std::vector<MRef<LdapEntry*> > entries;
	vector<string> attrs = url.getAttributes();

	phonebook->setName( url.getDn() );
	// FIXME handle empty attrs
	string label = attrs[0];

	entries = conn->find( url.getDn(), url.getFilter(), attrs);
	
	std::vector<MRef<LdapEntry*> >::iterator i;
	for (i = entries.begin(); i != entries.end(); i++) {
		MRef<PhoneBookPerson *> person;
		MRef<LdapEntry*> entry = *i;
		string contact;

		try {
			contact = entry->getAttrValueString(label);
		} catch( LdapException & ) {
			continue;
		}

		cerr << "Reading: " << contact << endl;
		person = new PhoneBookPerson( contact );

		vector<string> attrs = entry->getAttrNames();
		vector<string>::iterator j;
		int num_entries = 0;

		for (j = attrs.begin(); j != attrs.end(); j++) {
			const string &attr_name = *j;

			if (attr_name == label)
				continue;

			const string &value = entry->getAttrValueString(attr_name);

			ContactEntry * entry;
			entry = new ContactEntry( value, attr_name, person );
			person->addEntry( entry );
			cerr << "Entry: " << attr_name << ":" << value << endl;
			num_entries++;
		}

		if( num_entries > 0 ){
			phonebook->addPerson( person );
		}
	}

	phonebook->setIo( this );
	return phonebook;
}

void LdapPhoneBookIo::save( MRef<PhoneBook *> pb){
	// Save unsupported
}

string LdapPhoneBookIo::getPhoneBookId(){
	return url.getString();
}
