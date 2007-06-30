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
#include <libmnetutil/LdapConnection.h>

LdapConnection::LdapConnection(std::string host, int32_t port) {
	init(host, port, NULL);
}
LdapConnection::LdapConnection(std::string host) {
	init(host, LDAP_PORT, NULL);
}
LdapConnection::LdapConnection(std::string host, int32_t port, MRef<LdapCredentials*> cred) {
	init(host, port, cred);
}
LdapConnection::LdapConnection(std::string host, MRef<LdapCredentials*> cred) {
	init(host, LDAP_PORT, cred);
}

void LdapConnection::init(std::string host, int aPort, MRef<LdapCredentials*> aCred){
	hostname = host;
	port = aPort;
	cred = aCred;
	ld = NULL;
	isBound = false;
	setCredentials(cred);
	try {
		connect();
	} catch (LdapException & e) {
		//std::cerr << e.what() << std::endl;
	}
}
LdapConnection::~LdapConnection() {
	disconnect();
}

bool LdapConnection::isConnected(bool alsoCheckBind) {
	if (NULL == ld)
		return false;
	if (alsoCheckBind && !isBound)
		return false;
	return true;
}
void LdapConnection::setCredentials(MRef<LdapCredentials*> cred) {
	this->cred = cred;
}
MRef<LdapCredentials*> LdapConnection::getCredentials() {
	return cred;
}
/**
 * Some of this code has been copied from an unknown source.
 */
void LdapConnection::connect() throw (LdapException) {
	int auth_method = LDAP_AUTH_SIMPLE;
	int desired_version = LDAP_VERSION3;

	if (isConnected()) {
		disconnect();
	}

	// Initialize LDAP library and open connection
	if ((ld = ldap_init(hostname.c_str(), port)) == NULL ) {
		throw LdapException("Could not connect to server");
	}

	// Set protocol version
	if (ldap_set_option(ld, LDAP_OPT_PROTOCOL_VERSION, &desired_version) != LDAP_OPT_SUCCESS) {
		throw LdapException("Could not set connection options after setting up connection");
	}

	// Create LDAP bind
	const char *user = NULL;
	const char *pass = NULL;

	if (cred) {
		user = cred->username.c_str();
		pass = cred->password.c_str();
	}

	if (ldap_bind_s(ld, user, pass, auth_method) != LDAP_SUCCESS) {
		throw LdapException("Could not bind to connected server");
	}

	isBound = true;
}

bool LdapConnection::disconnect() {
	if (isBound) {
		ldap_unbind(ld);
	}
}

/**
 * Some of this code has been copied from an unknown source.
 */
std::vector<MRef<LdapEntry*> > LdapConnection::find(std::string baseDn, std::string query, std::vector<std::string> & attrs, int scope) throw (LdapNotConnectedException, LdapException) {
	LDAPMessage* msg;
	std::vector<MRef<LdapEntry*> > entries = std::vector<MRef<LdapEntry*> >();
	char* searchAttrs[attrs.size()+1];
	LDAPMessage* entry;
	char* attr;
	int i=0;

	// Test if client is connected
	if (!isConnected()) {
		throw LdapNotConnectedException();
	}

	// Convert vector of C++ strings to array of C-style strings
	for(i = 0; i < attrs.size(); i++ ) {
		searchAttrs[i] = const_cast<char*>(attrs.at(i).c_str());
	}

	// OpenLDAP requires that the last entry is NULL (my guess is to avoid needing an additional "length variable")
	searchAttrs[attrs.size()] = NULL;

	// Send query (note that it is blocking!)
	if (ldap_search_s(ld, baseDn.c_str(), scope, query.c_str(), searchAttrs, 0, &msg) != LDAP_SUCCESS)
	{
		// Return empty list
		throw LdapException("LdapException: Could not execute query");
	}


	// Push each returned object (entry) onto the result vector
	for( entry = ldap_first_entry(ld, msg);entry != NULL; entry = ldap_next_entry(ld,entry))
	{
		// Parse the returned entry directly (don't wait until the user actually "needs" it)
		entries.push_back(MRef<LdapEntry*>(new LdapEntry(ld, entry)));

	}

	// Clear up some memory
	ldap_msgfree(msg);

	return std::vector<MRef<LdapEntry*> >(entries);
}

std::string LdapConnection::getBaseDn() throw (LdapNotConnectedException, LdapUnsupportedException) {

	if (!isConnected()) {
		throw LdapNotConnectedException();
	}

	std::vector<MRef<LdapEntry*> > entries;
	std::vector<std::string> attrs;

	attrs.push_back("namingContexts");
	entries = find("", "(objectclass=*)", attrs, LDAP_SCOPE_BASE);

	if (0 == entries.size()) {
		throw LdapUnsupportedException("Retrieving base DN");
	} else {
		try {
			return entries.at(0)->getAttrValueString("namingContexts");
		} catch (LdapAttributeNotFoundException & e) {
			throw LdapUnsupportedException("Retrieving base DN");
		}
	}
}
/*
LDAP* LdapConnection::getLdapObject() {
	return ld;
}
*/
