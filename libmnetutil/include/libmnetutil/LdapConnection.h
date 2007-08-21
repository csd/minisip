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

#ifndef LDAPCONNECTION_H_
#define LDAPCONNECTION_H_

#include<libmnetutil/libmnetutil_config.h>

#include <libmutil/MemObject.h>
#include <libmnetutil/LdapException.h>
#include <libmnetutil/LdapCredentials.h>
#include <libmnetutil/LdapEntry.h>

#include <string>
#include <vector>

/**
 * Manages one connection to one LDAP server.
 *
 * @author	Mikael Svensson
 */
class LIBMNETUTIL_API LdapConnection : public MObject {
	public:
		LdapConnection(std::string host, int32_t port);
		LdapConnection(std::string host, int32_t port, MRef<LdapCredentials*> cred);
		LdapConnection(std::string host);
		LdapConnection(std::string host, MRef<LdapCredentials*> cred);
		~LdapConnection();

		/**
		 * Connects AND binds to the specified server. Automatically disconnects
		 * any existing connection.
		 */
		void 			connect() throw (LdapException);

		/**
		 * Tests if a connection has been set up.
		 *
		 * @param	alsoCheckBind	If set the function checks both if a connection has been established and if the client is "bound".
		 */
		bool 			isConnected(bool alsoCheckBind = false);

		/**
		 * Disconnect from server (unbind).
		 * @return true if successful, false othervise.
		 *	If not connected, then false will be returned.
		 */
		bool 			disconnect();

		/**
		 * Sends a query to the LDAP server and blocks until the entire response has been returned.
		 *
		 * @param	baseDn	The root path on the server where the search should commence.
		 * @param	query	The LDAP query filter to be used to select entries/objects.
		 * @param	attrs	String vector containing the attribute names to return.
		 * @param	scope	The scope of the search. Possible values: LDAP_SCOPE_SUBTREE, LDAP_SCOPE_BASE or LDAP_SCOPE_ONE.
		 * @return	A vector containing LdapEntry instances. Each LdapEntry represents a returned object and each instance is already populated with the appropriate values when returned.
		 */
		std::vector<MRef<LdapEntry*> > find(std::string baseDn, std::string query, std::vector<std::string> & attrs) throw (LdapNotConnectedException, LdapException);
		std::vector<MRef<LdapEntry*> > find(std::string baseDn, std::string query, std::vector<std::string> & attrs, int scope) throw (LdapNotConnectedException, LdapException);

		/**
		 * Determine the base DN of the connected server.
		 *
		 * This is done by requesting the namingContexts attribute of all objects in the directory root.
		 * This is, obviously, not a query that includes sub-folder in the LDAP tree.
		 *
		 * This feature is not supported by all servers. If the method cannot determine the base DN
		 * of the directory it will throw an LdapUnsupportedException.
		 */
		std::string 		getBaseDn() throw (LdapNotConnectedException, LdapUnsupportedException);

		/**
		 * Set the credentials to use. Currently only simple authentication with a username and password is supported.
		 */
		void			setCredentials(MRef<LdapCredentials*> cred);

		MRef<LdapCredentials*>	getCredentials();

		/**
		 * Return raw LDAP object pointer.
		 */
		//LDAP*			getLdapObject();
	private:
		//LDAP* 			ld;
		void*			ld;
		std::string 		hostname;
		int32_t			port;
		MRef<LdapCredentials*> 	cred;

		/**
		 * Used internally to determine whether or not the client has executed a bind operation.
		 */
		bool			isBound;

	protected:
		void init(std::string host, int32_t port, MRef<LdapCredentials*> cred);
};

#endif
