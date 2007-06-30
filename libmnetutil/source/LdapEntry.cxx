/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien

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
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 */

#include <config.h>
#include <libmnetutil/LdapEntry.h>

LdapEntry::LdapEntry(LDAP* ld, LDAPMessage* entry) {
	BerElement* ber;
	struct berval** binaries;
	char* attr;
	char** strings;
	int i=0;

	valuesBinary.clear();
	valuesStrings.clear();

	for( attr = ldap_first_attribute(ld, entry, &ber); attr != NULL; attr = ldap_next_attribute(ld, entry, ber)) {

		std::string attrName(attr);

		// Separate binary attributes from the rest simply to testing what the returned attribute is called. Simple and effective.
		if (stringEndsWith(attr, ";binary")) {
			// Process binary attributes
			if ((binaries = ldap_get_values_len(ld, entry, attr)) != NULL) {
				for (i=0; binaries[i] != NULL; i++) {
					valuesBinary[attrName].push_back(MRef<LdapEntryBinaryValue*>(new LdapEntryBinaryValue(binaries[i]->bv_val, binaries[i]->bv_len)));
				}
				ldap_value_free_len(binaries);
			}
		} else {
			// Process string attributes
			if ((strings = ldap_get_values(ld, entry, attr)) != NULL) {
				for (i = 0; strings[i] != NULL; i++) {
					valuesStrings[attrName].push_back(std::string(strings[i]));
				}
				ldap_value_free(strings);
			}
		}
		ldap_memfree(attr);
	}
	if (ber != NULL) {
		ber_free(ber,0);
	}
}

bool LdapEntry::hasAttribute(std::string attr) {
	std::map<std::string, std::vector<MRef<LdapEntryBinaryValue*> > >::iterator binaryIter = valuesBinary.find(attr);
	std::map<std::string, std::vector<std::string> >::iterator stringIter = valuesStrings.find(attr);

	if( binaryIter != valuesBinary.end() || stringIter != valuesStrings.end() ) {
		return true;
	}

	return false;
}

std::string LdapEntry::getAttrValueString(std::string attr) throw (LdapAttributeNotFoundException) {
	std::map<std::string, std::vector<std::string> >::iterator stringIter = valuesStrings.find(attr);

	if (stringIter != valuesStrings.end()) {
		return valuesStrings[attr].at(0);
	}

	throw LdapAttributeNotFoundException(attr);
}
std::vector<std::string> LdapEntry::getAttrValuesStrings(std::string attr) throw (LdapAttributeNotFoundException) {
	std::map<std::string, std::vector<std::string> >::iterator i = valuesStrings.find(attr);

	if (i != valuesStrings.end()) {
		return valuesStrings[attr];
	}

	throw LdapAttributeNotFoundException(attr);
}
std::vector< MRef<LdapEntryBinaryValue*> > LdapEntry::getAttrValuesBinary(std::string attr) throw (LdapAttributeNotFoundException) {
	std::map<std::string, std::vector<MRef<LdapEntryBinaryValue*> > >::iterator i = valuesBinary.find(attr);

	if (i != valuesBinary.end()) {
		return valuesBinary[attr];
	}

	throw LdapAttributeNotFoundException(attr);

}
std::vector<std::string> LdapEntry::getAttrNames() {
	std::vector<std::string> res;
	std::map<std::string, std::vector<std::string> >::iterator i;
	std::map<std::string, std::vector<MRef<LdapEntryBinaryValue*> > >::iterator j;

	for (i = valuesStrings.begin(); i != valuesStrings.end(); i++)
		res.push_back(i->first);

	for (j = valuesBinary.begin(); j != valuesBinary.end(); j++)
		res.push_back(j->first);

	return res;
}
