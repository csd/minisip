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

#include<config.h>

#include"OnlineMXmlPhoneBookIo.h"

#include<libminisip/contacts/PhoneBook.h>
#include<libminisip/contacts/ContactDb.h>

#include<libmutil/XMLParser.h>
#include<libmutil/stringutils.h>

#include<string>
#include<fstream>
#include<vector>
#include<libminisip/config/OnlineConfBackend.h>
using namespace std;

OnlineMXmlPhoneBookIo::OnlineMXmlPhoneBookIo(OnlineConfBack *confback)
{
	conf = confback;
}

MRef<PhoneBook *> OnlineMXmlPhoneBookIo::load(){
	MRef<PhoneBook *> phonebook = new PhoneBook;
	ContactEntry * entry;
	XMLParser * parser;
	int success;
	vector<struct contdata *> res;
	cout<<"downloading phonebook"<<endl;
	success = conf->downloadReq(conf->getUser(),"phonebook", res);
	string phonebookstring(res.at(0)->data, res.at(0)->size);
	if(success>0)
	{
		parser = new XMLstringParser(phonebookstring ); 
	}

	else {
		cerr << "Phonebook file not found. Creating default one." << endl;
		createDefault();
		try{
			fileName =".minisip.addr";
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

void OnlineMXmlPhoneBookIo::save( MRef<PhoneBook *> pb){
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
	//parser.saveToFile( fileName );
	string commit;
	commit = parser.xmlstring();
	string enc = conf->base64Encode((char*)commit.c_str(),commit.size());
	string attach = conf->attachFile("", enc);
	conf->uploadReq(conf->getUser(),"phonebook",attach);
}

string OnlineMXmlPhoneBookIo::getPhoneBookId(){
	return "httpsrp:///" + conf->getUser() + "/phonebook";
}

void OnlineMXmlPhoneBookIo::createDefault(){
	ofstream phonebookFile( fileName.c_str() );

	phonebookFile << getDefaultPhoneBookString() << endl;

}


string OnlineMXmlPhoneBookIo::getDefaultPhoneBookString(){
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
