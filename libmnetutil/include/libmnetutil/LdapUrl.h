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

#ifndef _LDAPURL_H_
#define _LDAPURL_H_

#include<libmnetutil/libmnetutil_config.h>

#include <string>
#include <vector>

class LdapUrlExtension {
	public:
		LdapUrlExtension(std::string type, std::string value, bool critical) : type(type), value(value), critical(critical) {}

		bool critical;
		std::string type;
		std::string value;
};
/**
 * Conforms to the LDAP URL specification (RFC 4516) with the following exceptions:
 *  - Does not handle case-insensitive URL parts correctly (requires lower-cased reserved words)
 *  - b
 *  - c
 *  - d
 *
 * For details about the specification one should read http://tools.ietf.org/html/rfc4516.
 *
 * Other fancy notes:
 *  - There is not setExtension method
 *
 * @author	Mikael Svensson
 */
class LdapUrl {
	public:
		LdapUrl(std::string url);
		LdapUrl();

		/** Restore URL parts to their default values */
		void clear();
		bool isValid();

		/** Parse URL */
		void setUrl(const std::string url);

		/** Get string representation of URL, e.g. the actual URL */
		std::string getString() const;

		/** Prints all information about the URL */
		void printDebug();

		/* Getters and setters */
		std::string getHost() const;
		void setHost(std::string host_);

		int32_t getPort() const;
		void setPort(int32_t);

		std::vector<std::string> getAttributes() const;
		void setAttributes(std::vector<std::string>);

		std::vector<LdapUrlExtension> getExtensions() const;
		bool hasCriticalExtension() const;

		std::string getFilter() const;
		void setFilter(std::string);

		std::string getDn() const;
		void setDn(std::string);

		int32_t getScope() const;
		void setScope(int32_t);
	private:
		std::string			host;
		int32_t 			port;
		std::vector<std::string> 	attributes;
		std::vector<LdapUrlExtension> 	extensions;
		std::string 			filter;
		std::string 			dn;
		int32_t				scope;

		/**
		 * Unreserved Characters according to RFC 3986 (section 2.3).
		 *
		 * Characters that are allowed in a URI but do not have a reserved
		 * purpose are called unreserved.  These include uppercase and lowercase
		 * letters, decimal digits, hyphen, period, underscore, and tilde.
		 */
		bool isUnreservedChar(char in) const;

		/**
		 * Reserved Charachters according to RFC 3986 (section 2.2).
		 *
		 * URIs include components and subcomponents that are delimited by
		 * characters in the "reserved" set.  These characters are called
		 * "reserved" because they may (or may not) be defined as delimiters by
		 * the generic syntax, by each scheme-specific syntax, or by the
		 * implementation-specific syntax of a URI's dereferencing algorithm.
		 */
		bool isReservedChar(char in) const;

		std::string encodeChar(const char in) const;
		char decodeChar(const std::string in) const;
		int32_t charToNum(const char in) const;

		std::string percentDecode(const std::string & in) const;
		std::string percentEncode(const std::string & in, bool escapeComma, bool escapeQuestionmark = true) const;

		bool validUrl;
};
#endif
