#include <libmnetutil/LdapUrl.h>

#include <iostream>
#include <libmutil/stringutils.h>
#include <ldap.h>

LdapUrl::LdapUrl(std::string url) {
	clear();
	setUrl(url);
}

LdapUrl::LdapUrl() {
	clear();
}

void LdapUrl::clear() {
	host = "";
	port = LDAP_PORT;
	filter = "(objectClass=*)";
	dn = "";
	scope = LDAP_SCOPE_BASE;
	validUrl = false;
	attributes = std::vector<std::string>();
	extensions = std::vector<LdapUrlExtension>();
}
std::string LdapUrl::getString() const {

	std::string url("ldap://");
	url += host;
	if (port > 0) {
		url += ':';
		url += itoa(port);
	}
	url += '/';
	url += percentEncode(dn, false, true);
	url += '?';
	if (attributes.size() > 0) {
		for (int i=0; i<attributes.size(); i++) {
			if (i>0)
				url += ',';
			url += percentEncode(attributes.at(i), false, true);
		}
	}
	url += '?';
	url += (scope == LDAP_SCOPE_BASE ? "base" : (scope == LDAP_SCOPE_SUBTREE ? "sub" : "one"));
	url += '?';
	if (filter.length() > 0) {
		url += filter;
	}
	if (extensions.size() > 0) {
		url += '?';
		for (int i=0; i<extensions.size(); i++) {
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
}
bool LdapUrl::isUnreservedChar(char in) const {
	char* alphabetUnreserved = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~";
	for (int i=0; i<66; i++) {
		if (alphabetUnreserved[i] == in)
			return true;
	}
	return false;
}

bool LdapUrl::isReservedChar(char in) const {
	char* alphabetReserved = ":/?#[]@!$&'()*+,;=";
	for (int i=0; i<18; i++) {
		if (alphabetReserved[i] == in)
			return true;
	}
	return false;
}
/*
      ldapurl     = scheme COLON SLASH SLASH [host [COLON port]]
                       [SLASH dn [QUESTION [attributes]
                       [QUESTION [scope] [QUESTION [filter]
                       [QUESTION extensions]]]]]
*/
void LdapUrl::setUrl(const std::string url) {
	std::string::size_type lastPos = 0, pos = 0, posTemp = 0;

	if (strCaseCmp(url.substr(0, 7).c_str(), "ldap://") == 0) {
		lastPos = 7;

		/**************************************************************
		 * Search for host and port
		 */

		pos = url.find('/', lastPos);

		// lastPos = <first char after schema specifier>, pos = <first "/" after schema specifier>

		if (pos == std::string::npos) {
			// No slash after schema specifier. This means that the URL at most specified a host and a port.
			pos = url.length();
			//lastPos = pos;
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
			//std::cerr << restOfUrl;

			std::vector<std::string> parts = split(restOfUrl, false, '?', true);

			switch (parts.size()) {
				case 5: {
					std::vector<std::string> exts = split(parts.at(4), false, ',', true);
					for (int i=0; i<exts.size(); i++) {
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
					for (int i=0; i<attributes.size(); i++) {
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
}
void LdapUrl::printDebug() {

	std::cerr <<  "     VALID?      " << (validUrl ? "yes" : "NO") << std::endl;

	std::cerr <<  "     Host:       [" << host << "]" << std::endl;

	std::cerr <<  "     Port:       [" << port << "]" << std::endl;

	std::cerr <<  "     Attributes: " << std::endl;
	for (int i=0; i<attributes.size(); i++)
		std::cerr <<  "                 [" << attributes.at(i) << "]" << std::endl;

	std::cerr <<  "     Extensions: " << std::endl;
	for (int i=0; i<extensions.size(); i++)
		std::cerr <<  "                 [" << extensions.at(i).type << "=" << extensions.at(i).value << "]" << (extensions.at(i).critical ? " (critical!)" : "") << std::endl;

	std::cerr <<  "     Filter:     [" << filter << "]" << std::endl;

	std::cerr <<  "     DN:         [" << dn << "]" << std::endl;

	std::cerr <<  "     Scope:      [" << (scope == LDAP_SCOPE_BASE ? "base" : (scope == LDAP_SCOPE_ONELEVEL ? "one" : "sub")) << "]" << std::endl;
}

bool LdapUrl::hasCriticalExtension() const {
	for (int i=0; i<extensions.size(); i++)
		if (extensions.at(i).critical)
			return true;
	return false;
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
	for (int i=0; i < in.length(); i++) {
		if ((!isReservedChar(in[i]) && !isUnreservedChar(in[i])) || (escapeQuestionmark && in[i] == '?') || (escapeComma && in[i] == ','))
			res += encodeChar(in[i]);
		else
			res += in[i];
	}
	return res;
}
std::string LdapUrl::percentDecode(const std::string & in) const {
	std::string res;
	for (int i=0; i < in.length(); i++) {
		if ('%' == in[i]) {
			res += decodeChar(in.substr(i, 3));
			i+=2;
		} else
			res += in[i];
	}
	return res;
}
