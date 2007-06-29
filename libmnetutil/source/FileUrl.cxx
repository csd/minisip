#include <libmnetutil/FileUrl.h>

#include <iostream>
#include <libmutil/stringutils.h>

FileUrl::FileUrl(std::string url, const int32_t type) {
	clear(); // Reset URL parts to default values
	this->type = type;
	setUrl(url); // Parse supplied URL
}

FileUrl::FileUrl(std::string url) {
	clear(); // Reset URL parts to default values
	setUrl(url); // Parse supplied URL
}

FileUrl::FileUrl() {
	clear(); // Reset URL parts to default values
}

void FileUrl::clear() {
	host = "";
	type = FILEURL_TYPE_UNKNOWN;
	path = "";
	validUrl = false;
}

bool FileUrl::isValid() const{
	return validUrl;
}

std::string FileUrl::getString() const {

	// Start off with the schema and host name
	std::string url("file://");
	url += host;

	// Append distinguished name (base DN)
	url += '/';

	// Split path into parts using "\" in Windows and "/" on other system
	char sep = (type == FILEURL_TYPE_WINDOWS ? '\\' : '/');
	std::vector<std::string> parts = split(path, false, sep, true);

	// Glue together each "path part" again
	for (int i=0; i<parts.size(); i++) {
		std::string decPart = percentEncode(parts.at(i));
		url += decPart + '/';
	}

	// Strip away the final trailing "/"
	url = url.substr(0, url.length() - 1);

	return url;
}
bool FileUrl::isUnreservedChar(char in) const {
	char* alphabetUnreserved = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~";
	for (int i=0; i<66; i++) {
		if (alphabetUnreserved[i] == in)
			return true;
	}
	return false;
}

bool FileUrl::isReservedChar(char in) const {
	char* alphabetReserved = ":/?#[]@!$&'()*+,;=";
	for (int i=0; i<18; i++) {
		if (alphabetReserved[i] == in)
			return true;
	}
	return false;
}

void FileUrl::setUrl(const std::string url) {
	std::string::size_type lastPos = 0, pos = 0, posTemp = 0;

	if (strCaseCmp(url.substr(0, 7).c_str(), "file://") == 0) {
		lastPos = 7;

		/**************************************************************
		* Search for host
		*/

		pos = url.find('/', lastPos);

		if (pos == std::string::npos) {
			// No slash after schema specifier. This means that no path has been specified. Illegal.
			validUrl = false;
			return;
		}

		if (pos != lastPos) {
			// Host found
			host = url.substr(lastPos, pos - lastPos);
		}
		lastPos = pos+1;

		if (lastPos < url.length()) {
			std::string restOfUrl = url.substr(lastPos);

			// Split the "path part of the URL" into pieces, each piece separated by "/" as specified by RFC 1738.
			std::vector<std::string> parts = split(restOfUrl, false, '/', true);

			// Glue the pieces together using an operating-system specific separator
			for (int i=0; i<parts.size(); i++) {
				std::string decPart = percentDecode(parts.at(i));
				if (type == FILEURL_TYPE_WINDOWS) {
					path += decPart + '\\';
				} else {
					path += decPart + '/';
				}
			}
			if (path.length() > 1) {
				// Remove trailing "path separator character"
				path = path.substr(0, path.length() - 1);
			} else {
				validUrl = false;
			}
		}
		validUrl = true;
	} else {
		validUrl = false;
	}
}
void FileUrl::printDebug() {

	std::cerr <<  "     VALID?      " << (validUrl ? "yes" : "NO") << std::endl;

	std::cerr <<  "     Host:       [" << host << "]" << std::endl;

	std::cerr <<  "     Path:       [" << path << "]" << std::endl;

	std::cerr <<  "     Type:       [";
	switch (type) {
		case FILEURL_TYPE_UNIX:
			std::cerr << " UNIX";
			break;
		case FILEURL_TYPE_WINDOWS:
			std::cerr << " Windows";
			break;
		default:
			std::cerr << " Unspecified";
			break;
	}
	std::cerr <<  "]" << std::endl;
}

std::string FileUrl::getHost() const {
	return host;
}
void FileUrl::setHost(std::string host) {
	this->host = host;
}
std::string FileUrl::getPath() const {
	return path;
}
void FileUrl::setPath(std::string path) {
	this->path = path;
}
int32_t FileUrl::getType() const {
	return type;
}
void FileUrl::setType(const int32_t type) {
	this->type = type;
}

std::string FileUrl::encodeChar(const char in) const {
	std::string res;
	res += '%';
	if (in < 10)
		res +='0';
	res += binToHex(reinterpret_cast<const unsigned char*>(&in), sizeof(in));
	return res;
}
char FileUrl::decodeChar(const std::string in) const {
	if (in.length() == 3) {
		return (charToNum(in[1]) << 4) + (charToNum(in[2]));
	} else {
		return '0';
	}
}

int32_t FileUrl::charToNum(const char in) const {
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

std::string FileUrl::percentEncode(const std::string & in) const {
	return percentEncode(in, true, true);
}

std::string FileUrl::percentEncode(const std::string & in, bool escapeComma, bool escapeQuestionmark) const {
	std::string res;
	for (int i=0; i < in.length(); i++) {
		if ((!isReservedChar(in[i]) && !isUnreservedChar(in[i])) || (escapeQuestionmark && in[i] == '?') || (escapeComma && in[i] == ','))
			res += encodeChar(in[i]);
		else
			res += in[i];
	}
	return res;
}
std::string FileUrl::percentDecode(const std::string & in) const {
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
