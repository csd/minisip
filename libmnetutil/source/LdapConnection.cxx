#include <libmnetutil/LdapConnection.h>

LdapConnection::LdapConnection(std::string host, int32_t port) : hostname(host), port(port), ld(NULL), isBound(false) {
}
LdapConnection::LdapConnection(std::string host) : hostname(host), port(LDAP_PORT), ld(NULL), isBound(false) {
}
LdapConnection::LdapConnection(std::string host, int32_t port, MRef<LdapCredentials*> cred) : hostname(host), port(port), ld(NULL), isBound(false) {
	setCredentials(cred);
	try {
		connect();
	} catch (LdapException & e) {
		//std::cerr << e.what() << std::endl;
	}
}
LdapConnection::LdapConnection(std::string host, MRef<LdapCredentials*> cred) : hostname(host), port(LDAP_PORT), ld(NULL), isBound(false) {
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
	if (cred.isNull()) {
		throw LdapException("Could not connect since no credentials have been specified");
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
	if (ldap_bind_s(ld, cred->username.c_str(), cred->password.c_str(), auth_method) != LDAP_SUCCESS) {
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
