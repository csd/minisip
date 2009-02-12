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

#ifdef ENABLE_LDAP
#ifdef _MSC_VER
#include <windows.h>
#include <winldap.h>
#include <winber.h>
#else
// FIXME: We use the deprecated API. This should be updated to the newest
// one.
#define LDAP_DEPRECATED 1
#include <ldap.h>
#include <lber.h>
#endif
#endif

#include <string>

LdapEntry::LdapEntry(void* ld, void* entry) {
#ifdef ENABLE_LDAP
	BerElement* ber;
	struct berval** binaries;
	char* attr;
	char** strings;
	int i=0;

	valuesBinary.clear();
	valuesStrings.clear();

	for( attr = ldap_first_attribute((LDAP*)ld, (LDAPMessage*)entry, &ber); attr != NULL; attr = ldap_next_attribute((LDAP*)ld, (LDAPMessage*)entry, ber)) {

		std::string attrName(attr);

		// Separate binary attributes from the rest simply to testing what the returned attribute is called. Simple and effective.
		if (stringEndsWith(attr, ";binary")) {
			// Process binary attributes
			if ((binaries = ldap_get_values_len((LDAP*)ld, (LDAPMessage*)entry, attr)) != NULL) {
				for (i=0; binaries[i] != NULL; i++) {
					valuesBinary[attrName].push_back(MRef<LdapEntryBinaryValue*>(new LdapEntryBinaryValue(binaries[i]->bv_val, binaries[i]->bv_len)));
				}
				ldap_value_free_len(binaries);
			}
		} else {
			// Process string attributes
			if ((strings = ldap_get_values((LDAP*)ld, (LDAPMessage*)entry, attr)) != NULL) {
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
#else
	throw LdapException("LDAP support not enabled");
#endif
}

bool LdapEntry::hasAttribute(std::string attr) {
#ifdef ENABLE_LDAP
	std::map<std::string, std::vector<MRef<LdapEntryBinaryValue*> > >::iterator binaryIter = valuesBinary.find(attr);
	std::map<std::string, std::vector<std::string> >::iterator stringIter = valuesStrings.find(attr);

	if( binaryIter != valuesBinary.end() || stringIter != valuesStrings.end() ) {
		return true;
	}

	return false;
#else
	throw LdapException("LDAP support not enabled");
#endif
}

std::string LdapEntry::getAttrValueString(std::string attr) throw (LdapAttributeNotFoundException) {
#ifdef ENABLE_LDAP
	std::map<std::string, std::vector<std::string> >::iterator stringIter = valuesStrings.find(attr);

	if (stringIter != valuesStrings.end()) {
		return valuesStrings[attr].at(0);
	}

	throw LdapAttributeNotFoundException(attr);
#else
	throw LdapException("LDAP support not enabled");
#endif
}
std::vector<std::string> LdapEntry::getAttrValuesStrings(std::string attr) throw (LdapAttributeNotFoundException) {
#ifdef ENABLE_LDAP
	std::map<std::string, std::vector<std::string> >::iterator i = valuesStrings.find(attr);

	if (i != valuesStrings.end()) {
		return valuesStrings[attr];
	}

	throw LdapAttributeNotFoundException(attr);
#else
	throw LdapException("LDAP support not enabled");
#endif
}
std::vector< MRef<LdapEntryBinaryValue*> > LdapEntry::getAttrValuesBinary(std::string attr) throw (LdapAttributeNotFoundException) {
#ifdef ENABLE_LDAP
	std::map<std::string, std::vector<MRef<LdapEntryBinaryValue*> > >::iterator i = valuesBinary.find(attr);

	if (i != valuesBinary.end()) {
		return valuesBinary[attr];
	}

	throw LdapAttributeNotFoundException(attr);

#else
	throw LdapException("LDAP support not enabled");
#endif
}
std::vector< MRef<LdapEntryBinaryPairValue*> > LdapEntry::getAttrValuesBinaryPairs(std::string attr) throw (LdapAttributeNotFoundException) {
#ifdef ENABLE_LDAP
	std::vector< MRef<LdapEntryBinaryValue*> > rawBinary;
	std::vector< MRef<LdapEntryBinaryValue*> >::iterator rawIter;
	std::vector< MRef<LdapEntryBinaryPairValue*> > result;

	try {
		rawBinary = getAttrValuesBinary(attr);
		if (rawBinary.size() > 0) {
			for (rawIter = rawBinary.begin(); rawIter != rawBinary.end(); rawIter++) {

				int len = (*rawIter)->length;
				char* dataPair = new char[len];
				memcpy(dataPair, (*rawIter)->value, len);

				BerElement *berPair;
				struct berval *bervalPair;


				bervalPair = new struct berval;
				bervalPair->bv_val = dataPair;
				bervalPair->bv_len = len;

				berPair = ber_init(bervalPair);

				struct berval *bervalCertIssuedTo, *bervalCertIssuedBy;
				bervalCertIssuedTo = new struct berval;
				bervalCertIssuedBy = new struct berval;

				ber_scanf(berPair, "{oo}", bervalCertIssuedTo, bervalCertIssuedBy);

				/*
				std::cout << "Analysis of certificate-pair file generated for inclusion in directory of " << params->shortNameA << ":" << std::endl;
				std::cout << "  Length of certificate in issued-TO-this-CA field: " << bervalCaACertIssuedTo->bv_len << std::endl;
				std::cout << "  Length of certificate in issued-BY-this-CA field: " << bervalCaACertIssuedBy->bv_len << std::endl;

				std::cout << "Analysis of certificate-pair file generated for inclusion in directory of " << params->shortNameB << ":" << std::endl;
				std::cout << "  Length of certificate in issued-TO-this-CA field: " << bervalCaBCertIssuedTo->bv_len << std::endl;
				std::cout << "  Length of certificate in issued-BY-this-CA field: " << bervalCaBCertIssuedBy->bv_len << std::endl;
				*/
				char* dataCertIssuedTo = new char[bervalCertIssuedTo->bv_len];
				char* dataCertIssuedBy = new char[bervalCertIssuedBy->bv_len];
				memcpy(dataCertIssuedTo, bervalCertIssuedTo->bv_val, bervalCertIssuedTo->bv_len);
				memcpy(dataCertIssuedBy, bervalCertIssuedBy->bv_val, bervalCertIssuedBy->bv_len);

				MRef<LdapEntryBinaryValue*> first(new LdapEntryBinaryValue(dataCertIssuedTo, bervalCertIssuedTo->bv_len));
				MRef<LdapEntryBinaryValue*> second(new LdapEntryBinaryValue(dataCertIssuedBy, bervalCertIssuedBy->bv_len));

				result.push_back(MRef<LdapEntryBinaryPairValue*>(new LdapEntryBinaryPairValue(first, second)));

				ber_bvfree(bervalPair);
				ber_bvfree(bervalCertIssuedTo);
				ber_bvfree(bervalCertIssuedBy);
				/*
				delete bervalPair;
				delete bervalCertIssuedTo;
				delete bervalCertIssuedBy;
				*/
				ber_free(berPair, 1);
			}
		}
	} catch (LdapAttributeNotFoundException & /*ex*/) {
		throw; // Re-throw exception
	}
	return result;
#else
	throw LdapException("LDAP support not enabled");
#endif
}

std::vector<std::string> LdapEntry::getAttrNames() {
#ifdef ENABLE_LDAP
	std::vector<std::string> res;
	std::map<std::string, std::vector<std::string> >::iterator i;
	std::map<std::string, std::vector<MRef<LdapEntryBinaryValue*> > >::iterator j;

	for (i = valuesStrings.begin(); i != valuesStrings.end(); i++)
		res.push_back(i->first);

	for (j = valuesBinary.begin(); j != valuesBinary.end(); j++)
		res.push_back(j->first);

	return res;
#else
	throw LdapException("LDAP support not enabled");
#endif
}
