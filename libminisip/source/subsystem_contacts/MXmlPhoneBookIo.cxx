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

#include<config.h>

#include"MXmlPhoneBookIo.h"

#include<libminisip/contacts/PhoneBook.h>
#include<libminisip/contacts/ContactDb.h>

#include<libmutil/XMLParser.h>
#include<libmutil/stringutils.h>

#include<string>
#include<fstream>

using namespace std;

#if 0
// Linked in PhoneBookIoDriver
static std::list<std::string> pluginList;
static int initialized;

extern "C"
std::list<std::string> *mxmlpb_LTX_listPlugins( MRef<Library *> lib ){
	if( !initialized ){
		pluginList.push_back("getPlugin");
		initialized = true;
	}

	return &pluginList;
}

extern "C"
MPlugin * mxmlpb_LTX_getPlugin( MRef<Library *> lib ){
	return new MXmlPhoneBookIoDriver( lib );
}
#endif

MRef<PhoneBookIo*> MXmlPhoneBookIoDriver::createPhoneBookIo( const string &name ) const
{
	return new MXmlPhoneBookIo( name );
}

MXmlPhoneBookIoDriver::MXmlPhoneBookIoDriver( MRef<Library *> lib ): PhoneBookIoDriver( lib )
{
}

MXmlPhoneBookIoDriver::~MXmlPhoneBookIoDriver()
{
}

MXmlPhoneBookIo::MXmlPhoneBookIo( string fn ){
	if (fn.substr(0,7)=="file://"){
		fileName = fn.substr(7);
	}
}

MRef<PhoneBook *> MXmlPhoneBookIo::load(){
	MRef<PhoneBook *> phonebook = new PhoneBook;
	ContactEntry * entry;
	XMLParser * parser;

	if( fileName.empty() ){
		return NULL;
	}

	try{
		parser = new XMLFileParser( fileName ); 
	}

	catch( XMLFileNotFound& ){
		cerr << "Phonebook file not found. Creating default one." << endl;
		createDefault();
		try{
			 parser = new XMLFileParser( fileName );
		}
		catch( XMLException & ){
			return NULL;
		}
		
	}
	if( parser == NULL ){
		return NULL;
	}
	
	string q = "phonebook/name";
	string name =  parser->getValue( q,"" );
	phonebook->setName( name );

	if( name != "" ){
		string contact;
		int nContacts = 0;

		do{
			string q = "phonebook/contact["+ itoa( nContacts )+"]/name";
			contact = parser->getValue( q, "" );
			if( contact != "" ){
				MRef<PhoneBookPerson *> person
					= new PhoneBookPerson( contact );

				phonebook->addPerson( person );
				
				int nEntries = 0;
				string qbase;
				string desc;
				do{
					string uri;
					string qbase = "phonebook/contact[" +
						itoa( nContacts ) +
						"]/pop[" + 
						itoa( nEntries ) + "]/";

					desc = parser->getValue( qbase+"desc","");
					if( desc != "" ){
						uri = parser->getValue( qbase+"uri", "" );
						entry = new ContactEntry( uri, desc, person );
						person->addEntry( entry );
					}
					nEntries ++;
				} while( desc != "" );

			}
			nContacts ++;
		} while( contact != "" );

	}

	delete parser;
	phonebook->setIo( this );
	return phonebook;
}

void MXmlPhoneBookIo::save( MRef<PhoneBook *> pb){
	XMLFileParser parser;

        list< MRef<PhoneBookPerson *> >::iterator iPerson;
        list< MRef<PhoneBookPerson *> >persons = pb->getPersons();
        list< MRef<ContactEntry *> >::iterator iContact;
        list< MRef<ContactEntry *> > contacts;
	string personPath, contactPath;
	int nPerson = 0;
	int nContact = 0;

	parser.changeValue( "phonebook/name", pb->getName() );

	for( iPerson = persons.begin(); iPerson != persons.end() ; iPerson++ ){
		personPath = "phonebook/contact[" + itoa(nPerson) + "]";
		
		parser.changeValue( personPath+"/name", (*iPerson)->getName() );

		nContact = 0;
		contacts = (*iPerson)->getEntries();
		for( iContact = contacts.begin(); iContact != contacts.end(); iContact ++ ){
			contactPath = personPath + "/pop[" + itoa( nContact ) + "]";
			parser.changeValue( contactPath+"/desc", (*iContact)->getDesc() );
			parser.changeValue( contactPath+"/uri", (*iContact)->getUri() );
			nContact ++;
		}
		nPerson ++;
	}
	parser.saveToFile( fileName );
}

string MXmlPhoneBookIo::getPhoneBookId(){
	return "file://" + fileName;
}

void MXmlPhoneBookIo::createDefault(){
	ofstream phonebookFile( fileName.c_str() );

	phonebookFile << getDefaultPhoneBookString() << endl;

}


string MXmlPhoneBookIo::getDefaultPhoneBookString(){
        string defaultPhonebook =
//                "<version>" CONFIG_FILE_VERSION_REQUIRED_STR "</version>"
                "<phonebook name=Example>\n"
                "<contact name=\"Contact\">\n"
                "<pop desc=\"Phone\" uri=\"0000000000\"></pop>\n"
                "<pop desc=\"Laptop\" uri=\"sip:contact@minisip.org\"></pop>\n"
                "</contact>\n"
                "</phonebook>\n";
        return defaultPhonebook;
}
