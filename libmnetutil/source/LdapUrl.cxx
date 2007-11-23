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
#include <libmnetutil/LdapUrl.h>
#include <libmnetutil/LdapException.h>

#include <iostream>
#include <libmutil/stringutils.h>

#ifdef ENABLE_LDAP

#ifdef WIN32
#include <windows.h>
#include <winldap.h>
#else
#include <ldap.h>
#endif

#endif

LdapUrl::LdapUrl(std::string url) {
#ifdef ENABLE_LDAP
	clear(); // Reset URL parts to default values
	setUrl(url); // Parse supplied URL
#else
	throw LdapException("LDAP support not enabled");
#endif
}

LdapUrl::LdapUrl() {
	clear(); // Reset URL parts to default values
}

void LdapUrl::clear() {
#ifdef ENABLE_LDAP
	host = "";
	port = LDAP_PORT;
	filter = "(objectClass=*)";
	dn = "";
	scope = LDAP_SCOPE_BASE;
	validUrl = false;
	attributes = std::vector<std::string>();
	extensions = std::vector<LdapUrlExtension>();
#else
	throw LdapException("LDAP support not enabled");
#endif
}

bool LdapUrl::isValid() const {
	return validUrl;
}

std::string LdapUrl::getString() const {
#ifdef ENABLE_LDAP
	// Start off with the schema and host name
	std::string url("ldap://");
	url += host;
	if (port > 0) {
		url += ':';
		url += itoa(port);
	}

	// Append distinguished name (base DN)
	url += '/';
	url += percentEncode(dn, false, true);

	// Append attributes
	url += '?';
	if (attributes.size() > 0) {
		for (size_t i=0; i<attributes.size(); i++) {
			if (i>0)
				url += ',';
			url += percentEncode(attributes.at(i), false, true);
		}
	}

	// Append scope
	url += '?';
	url += (scope == LDAP_SCOPE_BASE ? "base" : (scope == LDAP_SCOPE_SUBTREE ? "sub" : "one"));

	// Append filter
	url += '?';
	if (filter.length() > 0) {
		url += filter;
	}

	// Append extensions
	if (extensions.size() > 0) {
		url += '?';
		for (size_t i=0; i<extensions.size(); i++) {
			if (i>0)
				url += ',';

			LdapUrlExtension ext = extensions.at(i);
			if (ext.critical)
				url += '!';
			url += percentEncode(ext.type, false, true);
			url += '=';
			url += percentEncode(ext.value, true, true);
		}
	}

	return url;
#else
	throw LdapException("LDAP support not enabled");
#endif
}
bool LdapUrl::isUnreservedChar(char in) const {
	const char* alphabetUnreserved = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~";
	for (int i=0; i<66; i++) {
		if (alphabetUnreserved[i] == in)
			return true;
	}
	return false;
}

bool LdapUrl::isReservedChar(char in) const {
	const char* alphabetReserved = ":/?#[]@!$&'()*+,;=";
	for (int i=0; i<18; i++) {
		if (alphabetReserved[i] == in)
			return true;
	}
	return false;
}

void LdapUrl::setUrl(const std::string url) {
#ifdef ENABLE_LDAP
	std::string::size_type lastPos = 0, pos = 0, posTemp = 0;

	if (strCaseCmp(url.substr(0, 7).c_str(), "ldap://") == 0) {
		lastPos = 7;

		/**************************************************************
		 * Search for host and port
		 */

		pos = url.find('/', lastPos);

		if (pos == std::string::npos) {
			// No slash after schema specifier. This means that the URL at most specified a host and a port.
			pos = url.length();
		}

		if (pos != lastPos) {
			// Host or port found
			posTemp = url.find(':', lastPos);
			if (posTemp != std::string::npos && posTemp < pos) {
				// Port found
				host = url.substr(lastPos, posTemp - lastPos);
				port = atoi(url.substr(posTemp+1).c_str());
			} else {
				host = url.substr(lastPos, pos - lastPos);
			}
		}
		lastPos = pos+1;

		if (lastPos < url.length()) {
			std::string restOfUrl = url.substr(lastPos);

			std::vector<std::string> parts = split(restOfUrl, false, '?', true);

			switch (parts.size()) {
				case 5: {
					std::vector<std::string> exts = split(parts.at(4), false, ',', true);
					for (size_t i=0; i<exts.size(); i++) {
						std::string ext = exts.at(i);
						std::string::size_type colonPos = ext.find('=', 0);
						bool critical = (ext[0] == '!');
						int criticalOffset = (critical ? 1 : 0);
						if (colonPos != std::string::npos) {
							extensions.push_back(LdapUrlExtension(percentDecode(ext.substr(criticalOffset,colonPos-criticalOffset)),percentDecode(ext.substr(colonPos+1)), critical));
						} else {
							extensions.push_back(LdapUrlExtension(percentDecode(ext.substr(criticalOffset)),"", critical));
						}
					}
				}
				case 4:
					filter = parts.at(3);
				case 3:
					if (0 == strCaseCmp(parts.at(2).c_str(), "one")) {
						scope = LDAP_SCOPE_ONELEVEL;
					} else if (0 == strCaseCmp(parts.at(2).c_str(), "base")) {
						scope = LDAP_SCOPE_BASE;
					} else if (0 == strCaseCmp(parts.at(2).c_str(), "sub")) {
						scope = LDAP_SCOPE_SUBTREE;
					} else {
						validUrl = false;
					}

				case 2:
					attributes = split(parts.at(1), false, ',', true);
					for (size_t i=0; i<attributes.size(); i++) {
						attributes.at(i) = percentDecode(attributes.at(i));
					}
				case 1:
					dn = percentDecode(parts.at(0));
			}
		}
		validUrl = true;
	} else {
		validUrl = false;
	}
#endif
}
void LdapUrl::printDebug() {
#ifdef ENABLE_LDAP
	std::cerr <<  "     VALID?      " << (validUrl ? "yes" : "NO") << std::endl;

	std::cerr <<  "     Host:       [" << host << "]" << std::endl;

	std::cerr <<  "     Port:       [" << port << "]" << std::endl;

	std::cerr <<  "     Attributes: " << std::endl;
	for (size_t i=0; i<attributes.size(); i++)
		std::cerr <<  "                 [" << attributes.at(i) << "]" << std::endl;

	std::cerr <<  "     Extensions: " << std::endl;
	for (size_t i=0; i<extensions.size(); i++)
		std::cerr <<  "                 [" << extensions.at(i).type << "=" << extensions.at(i).value << "]" << (extensions.at(i).critical ? " (critical!)" : "") << std::endl;

	std::cerr <<  "     Filter:     [" << filter << "]" << std::endl;

	std::cerr <<  "     DN:         [" << dn << "]" << std::endl;

	std::cerr <<  "     Scope:      [" << (scope == LDAP_SCOPE_BASE ? "base" : (scope == LDAP_SCOPE_ONELEVEL ? "one" : "sub")) << "]" << std::endl;
#else
	throw LdapException("LDAP support not enabled");
#endif
}

bool LdapUrl::hasCriticalExtension() const {
	for (size_t i=0; i<extensions.size(); i++)
		if (extensions.at(i).critical)
			return true;
	return false;
}
std::string LdapUrl::getHost() const {
	return host;
}
void LdapUrl::setHost(std::string host) {
	this->host = host;
}

int32_t LdapUrl::getPort() const {
	return port;
}
void LdapUrl::setPort(int32_t port) {
	this->port = port;
}

std::vector<std::string> LdapUrl::getAttributes() const {
	return attributes;
}
void LdapUrl::setAttributes(std::vector<std::string> attributes) {
	this->attributes = attributes;
}

std::vector<LdapUrlExtension> LdapUrl::getExtensions() const {
	return extensions;
}

std::string LdapUrl::getFilter() const {
	return filter;
}
void LdapUrl::setFilter(std::string filter) {
	this->filter = filter;
}

std::string LdapUrl::getDn() const {
	return dn;
}
void LdapUrl::setDn(std::string dn) {
	this->dn = dn;
}

int32_t LdapUrl::getScope() const {
	return scope;
}
void LdapUrl::setScope(int32_t scope) {
	this->scope = scope;
}

std::string LdapUrl::encodeChar(const char in) const {
	std::string res;
	res += '%';
	if (in < 10)
		res +='0';
	res += binToHex(reinterpret_cast<const unsigned char*>(&in), sizeof(in));
	return res;
}

char LdapUrl::decodeChar(const std::string in) const {
	if (in.length() == 3) {
		return (charToNum(in[1]) << 4) + (charToNum(in[2]));
	} else {
		return '0';
	}
}

int32_t LdapUrl::charToNum(const char in) const {
	if (in >= '0' && in <= '9') {
		return (in - '0');
	} else if (in >= 'A' && in <= 'F') {
		return (in - 'A' + 10);
	} else if (in >= 'a' && in <= 'f') {
		return (in - 'a' + 10);
	} else {
		return -1;
	}
}

std::string LdapUrl::percentEncode(const std::string & in, bool escapeComma, bool escapeQuestionmark) const {
	std::string res;
	for (size_t i=0; i < in.length(); i++) {
		if ((!isReservedChar(in[i]) && !isUnreservedChar(in[i])) || (escapeQuestionmark && in[i] == '?') || (escapeComma && in[i] == ','))
			res += encodeChar(in[i]);
		else
			res += in[i];
	}
	return res;
}

std::string LdapUrl::percentDecode(const std::string & in) const {
	std::string res;
	for (size_t i=0; i < in.length(); i++) {
		if ('%' == in[i]) {
			res += decodeChar(in.substr(i, 3));
			i+=2;
		} else
			res += in[i];
	}
	return res;
}

