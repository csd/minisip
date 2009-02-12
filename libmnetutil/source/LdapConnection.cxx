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

#ifdef ENABLE_LDAP
#ifdef _MSC_VER
#include <windows.h>
#include <winldap.h>
#else
//FIXME: We use a depricated version of the API.
#define LDAP_DEPRECATED 1
#include <ldap.h>
#endif
#endif

#ifndef LDAP_OPT_SUCCESS
#define LDAP_OPT_SUCCESS 0
#endif

LdapConnection::LdapConnection(std::string host, int32_t port) {
#ifdef ENABLE_LDAP
	init(host, port, NULL);
#else
	throw LdapException("LDAP support not enabled");
#endif
}
LdapConnection::LdapConnection(std::string host) {
#ifdef ENABLE_LDAP
	init(host, LDAP_PORT, NULL);
#else
	throw LdapException("LDAP support not enabled");
#endif
}
LdapConnection::LdapConnection(std::string host, int32_t port, MRef<LdapCredentials*> cred) {
#ifdef ENABLE_LDAP
	init(host, port, cred);
#else
	throw LdapException("LDAP support not enabled");
#endif
}
LdapConnection::LdapConnection(std::string host, MRef<LdapCredentials*> cred) {
#ifdef ENABLE_LDAP
	init(host, LDAP_PORT, cred);
#else
	throw LdapException("LDAP support not enabled");
#endif
}

void LdapConnection::init(std::string host, int aPort, MRef<LdapCredentials*> aCred){
#ifdef ENABLE_LDAP
	hostname = host;
	port = aPort;
	cred = aCred;
	ld = NULL;
	isBound = false;
	setCredentials(cred);
	try {
		connect();
	} catch (LdapException & /*e*/ ) {
		//std::cerr << e.what() << std::endl;
	}
#else
	throw LdapException("LDAP support not enabled");
#endif
}
LdapConnection::~LdapConnection() {
#ifdef ENABLE_LDAP
	disconnect();
#else
	throw LdapException("LDAP support not enabled");
#endif
}

bool LdapConnection::isConnected(bool alsoCheckBind) {
#ifdef ENABLE_LDAP
	if (NULL == ld)
		return false;
	if (alsoCheckBind && !isBound)
		return false;
	return true;
#else
	throw LdapException("LDAP support not enabled");
#endif
}
void LdapConnection::setCredentials(MRef<LdapCredentials*> cred) {
#ifdef ENABLE_LDAP
	this->cred = cred;
#else
	throw LdapException("LDAP support not enabled");
#endif
}
MRef<LdapCredentials*> LdapConnection::getCredentials() {
#ifdef ENABLE_LDAP
	return cred;
#else
	throw LdapException("LDAP support not enabled");
#endif
}
/**
 * Some of this code has been copied from an unknown source.
 */
void LdapConnection::connect() throw (LdapException) {
#ifdef ENABLE_LDAP
	int auth_method = LDAP_AUTH_SIMPLE;
	int desired_version = LDAP_VERSION3;

	if (isConnected()) {
		disconnect();
	}

	// Initialize LDAP library and open connection
	if ((ld = ldap_init((char*)hostname.c_str(), port)) == NULL ) {
		throw LdapException("Could not connect to server");
	}

	// Set protocol version
	if (ldap_set_option((LDAP*)ld, LDAP_OPT_PROTOCOL_VERSION, &desired_version) != LDAP_OPT_SUCCESS) {
		throw LdapException("Could not set connection options after setting up connection");
	}

	// Create LDAP bind
	const char *user = NULL;
	const char *pass = NULL;

	if (cred) {
		user = cred->username.c_str();
		pass = cred->password.c_str();
	}

	if (ldap_bind_s( (LDAP*)ld, 
#ifdef _MSC_VER
			(const PCHAR)
#endif
			user,
#ifdef _MSC_VER
			(const PCHAR)
#endif
			pass, 
			auth_method) != LDAP_SUCCESS) {
		throw LdapException("Could not bind to connected server");
	}

	isBound = true;
#else
	throw LdapException("LDAP support not enabled");
#endif
}

bool LdapConnection::disconnect() {
#ifdef ENABLE_LDAP
	if (isBound) {
		return ldap_unbind((LDAP*)ld) != 0;
	}else
		return false;
#else
	throw LdapException("LDAP support not enabled");
#endif
}

/**
 * Some of this code has been copied from an unknown source.
 */
std::vector<MRef<LdapEntry*> > LdapConnection::find(std::string baseDn, std::string query, std::vector<std::string> & attrs) throw (LdapNotConnectedException, LdapException) {
#ifdef ENABLE_LDAP
	return find(baseDn, query, attrs, LDAP_SCOPE_SUBTREE);
#else
	throw LdapException("LDAP support not enabled");
#endif
}
std::vector<MRef<LdapEntry*> > LdapConnection::find(std::string baseDn, std::string query, std::vector<std::string> & attrs, int scope) throw (LdapNotConnectedException, LdapException) {
#ifdef ENABLE_LDAP
	LDAPMessage* msg;
	std::vector<MRef<LdapEntry*> > entries = std::vector<MRef<LdapEntry*> >();
	char **searchAttrs = new char*[attrs.size()+1];
	LDAPMessage* entry;
	int i=0;

	// Test if client is connected
	if (!isConnected()) {
		throw LdapNotConnectedException();
	}

	// Convert vector of C++ strings to array of C-style strings
	for(i = 0; i < (int)attrs.size(); i++ ) {
		searchAttrs[i] = const_cast<char*>(attrs.at(i).c_str());
	}

	// OpenLDAP requires that the last entry is NULL (my guess is to avoid needing an additional "length variable")
	searchAttrs[attrs.size()] = NULL;

	// Send query (note that it is blocking!)
	if (ldap_search_s(
			(LDAP*)ld, 
#ifdef _MSC_VER
			(PCHAR)
#endif
			baseDn.c_str(), 
			scope,
#ifdef _MSC_VER
			(PCHAR)
#endif
			query.c_str(), 
			searchAttrs, 
			0, 
			&msg) != LDAP_SUCCESS)
	{
		delete []searchAttrs;
		// Return empty list
		throw LdapException("LdapException: Could not execute query");
	}


	// Push each returned object (entry) onto the result vector
	for( entry = ldap_first_entry((LDAP*)ld, msg);entry != NULL; entry = ldap_next_entry((LDAP*)ld,entry))
	{
		// Parse the returned entry directly (don't wait until the user actually "needs" it)
		entries.push_back(MRef<LdapEntry*>(new LdapEntry((LDAP*)ld, entry)));

	}

	// Clear up some memory
	ldap_msgfree(msg);

	delete []searchAttrs;

	return std::vector<MRef<LdapEntry*> >(entries);
#else
	throw LdapException("LDAP support not enabled");
#endif
}

std::string LdapConnection::getBaseDn() throw (LdapNotConnectedException, LdapUnsupportedException) {
#ifdef ENABLE_LDAP

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
		} catch (LdapAttributeNotFoundException & /*e*/ ) {
			throw LdapUnsupportedException("Retrieving base DN");
		}
	}
#else
	throw LdapException("LDAP support not enabled");
#endif
}
/*
LDAP* LdapConnection::getLdapObject() {
	return ld;
}
*/
