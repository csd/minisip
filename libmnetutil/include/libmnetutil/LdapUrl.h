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

/**
 * Represents an LDAP URL Extension.
 *
 * Does not have any methods, only data members.
 */
class LIBMNETUTIL_API LdapUrlExtension {
	public:
		LdapUrlExtension(std::string type, std::string value, bool critical) : type(type), value(value), critical(critical) {}

		std::string type;
		std::string value;
		bool critical;
};

/**
 * Represents and parsed LDAP URLs.
 *
 * Known problems with respect to LDAP URL specification (RFC 4516):
 *  - Does not handle case-insensitive URL parts correctly (requires lower-cased reserved words)
 *  - It is not known if the distinguished name and attribute selectors are parsed correctly
 *
 * Other fancy notes:
 *  - There is not setExtension method
 *
 * @author	Mikael Svensson
 */
class LIBMNETUTIL_API LdapUrl {
	public:
		LdapUrl(std::string url);
		LdapUrl();

		/**
		 * Restore URL parts to their default values
		 */
		void clear();

		/**
		 * Tests whether or not the currenct LdapUrl instance represents a valid LDAP URL.
		 *
		 * At the moment an LDAP URL is considered invalid only if it doesn't start with "ldap://".
		 */
		bool isValid() const;

		/**
		 * Parse URL
		 */
		void setUrl(const std::string url);

		/**
		 * Get string representation of URL, e.g. the actual URL.
		 *
		 * This representation may not look exactly the same as the original URL as the parser
		 * replaces omitted values with default values. This is not a problem in itself as
		 * the generated URLs are still valid, albeit more explicit than the original ones.
		 *
		 * An example of this:
		 * @code
LdapUrl url("ldap://ldap.example.net/?");
cout << url.getString();
		 * @endcode
		 * Output:
		 * @code
ldap://ldap.example.net:389/??base?(objectClass=*)
		 * @endcode
		 */
		std::string getString() const;

		/**
		 * Prints all information about the URL. Probably only useful when debugging.
		 */
		void printDebug();

		/* Getters and setters */
		std::string getHost() const;
		void setHost(std::string host_);

		int32_t getPort() const;
		void setPort(int32_t);

		std::vector<std::string> getAttributes() const;
		void setAttributes(std::vector<std::string>);

		std::vector<LdapUrlExtension> getExtensions() const;

		/**
		 * Returns whether or not the LDAP URL has a critical extension.
		 *
		 * The LDAP URL RFC describes why this is important:
		 *
		 * "If an LDAP URL extension is implemented (that is, if the
		 * implementation understands it and is able to use it), the
		 * implementation MUST make use of it.  If an extension is not
		 * implemented and is marked critical, the implementation MUST NOT
		 * process the URL.  If an extension is not implemented and is not
		 * marked critical, the implementation MUST ignore the extension."
		 */
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
		 * Tests whether or not a character is an "unreserved character".
		 *
		 * Unreserved Characters according to RFC 3986 (section 2.3):
		 *
		 * Characters that are allowed in a URI but do not have a reserved
		 * purpose are called unreserved.  These include uppercase and lowercase
		 * letters, decimal digits, hyphen, period, underscore, and tilde.
		 */
		bool isUnreservedChar(char in) const;

		/**
		 * Tests whether or not a character is a "reserved character".
		 *
		 * Reserved Charachters according to RFC 3986 (section 2.2):
		 *
		 * "URIs include components and subcomponents that are delimited by
		 * characters in the 'reserved' set.  These characters are called
		 * 'reserved' because they may (or may not) be defined as delimiters by
		 * the generic syntax, by each scheme-specific syntax, or by the
		 * implementation-specific syntax of a URI's dereferencing algorithm."
		 */
		bool isReservedChar(char in) const;

		/**
		 * Percent-encode a character.
		 *
		 * Converts a space to "%20" and so on. Note that the function converts the input character
		 * regardsless of whether or not a conversion is actually necessary (if an "a" is sent
		 * to the function it will be converted to "%61" even though it is within the US-ACII set).
		 */
		std::string encodeChar(const char in) const;

		/**
		 * Decode an percent-encoded character.
		 *
		 * Assumes that the input string is three characters long, that the first
		 * character is the "%" sign and that the last two charachters are hexadecimal digits.
		 */
		char decodeChar(const std::string in) const;

		/**
		 * Converts hexadecimal (or decial) digit into the corresponding integer value.
		 */
		int32_t charToNum(const char in) const;

		/**
		 * Parses string and converts all its percent-encoded characters to single characters.
		 */
		std::string percentDecode(const std::string & in) const;

		/**
		 * Percent-encodes string according to rules stated in section 2.1 of RFC 4516.
		 */
		std::string percentEncode(const std::string & in, bool escapeComma, bool escapeQuestionmark = true) const;

		bool validUrl;
};
#endif
