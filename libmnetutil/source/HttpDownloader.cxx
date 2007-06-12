#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <libmutil/MemObject.h>
#include <libmnetutil/HttpDownloader.h>
#include <libmnetutil/StreamSocket.h>

#include <fstream>
#include <sstream>

#include <string>
#include <vector>
#include <map>
#include <iostream>

/**
 * The buffer size is set to a constant value to simplify the implementation. A production version of this class cannot have this limitation.
 */
#define BUFFERSIZE 4096

using namespace std;

const char* HttpDownloader::getChars() {
	int tries = 3;
	while (tries) {
		ostringstream body;
		int fetchRes = fetch(buildRequestString("GET ", remoteFile), body);
		if (fetchRes == HTTP_RESPONSECODE_MOVEDPERMANENTLY || respCode == HTTP_RESPONSECODE_MOVEDTEMPORARILY) {
			url = getHeader("Location");
			parseUrl();
			if (!followRedirect) {
				return "";
			}
		} else if (fetchRes == HTTP_RESPONSECODE_OK) {
			return body.str().c_str();
		} else {
			return "";
		}
		tries--;
	}
	return "";
}


int HttpDownloader::getResponseCode() const {
	return respCode;
}

void HttpDownloader::setFollowRedirects(bool const val) {
	followRedirect = val;
}

bool HttpDownloader::getFollowRedirects() const {
	return followRedirect;
}

int HttpDownloader::fetch(string request, ostream & bodyStream) {

	cerr << "(FETCH)" << endl;

	if (sock == NULL) {
		// TODO: Error check for socket object
	}

	stringstream headerStream;
	int fp = 0;
	unsigned int bytesWritten = 0, bytesRead = 0;
	struct sockaddr_in remoteAddr;
	/*
	This is what the hostent struct looks like:

	struct  hostent
	{
	char    *h_name;        // official name of host
	char    **h_aliases;    // alias list
	int     h_addrtype;     // host address type
	int     h_length;       // length of address
	char    **h_addr_list;  // list of addresses from name server
	#define h_addr  h_addr_list[0]  // address, for backward compatiblity
	};
	*/
	struct hostent *remoteHost;

	/* Buffer for holding data read from the network stream */
	char buffer[BUFFERSIZE];
	memset(buffer, 0, sizeof(buffer)); // Zero out the buffer used when recieving data

	/* Create socket */
	fp = socket(AF_INET, SOCK_STREAM, 0);
	if (fp == -1) {
		cerr << "Error: Could not create socket" << endl;
		return false;
	}
	//memset(remoteAddr, 0, sizeof(remoteAddr)); // Zero the data structure containing information about the remote host

	/* Fetch information about the remote host */
	remoteHost = gethostbyname(remoteHostname.c_str()); // Use host name stored in class instance
	if (remoteHost == NULL) {
		cerr << "Error: Could not resolve host name" << endl;
		return false;
	}
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons(remotePort); // Use port number stored in class instance

	// Copy information about remote host's IP address from one structure to another (to the one used by "connect()")
	bcopy((char *)remoteHost->h_addr, (char *)&remoteAddr.sin_addr.s_addr, sizeof(remoteHost->h_length));

	/* Send request */
	bytesWritten = sock->write(request.c_str(), request.length());
	if (bytesWritten < 0) {
		cerr << "Error: Could not send request" << endl;
		return false;
	} else if (bytesWritten < request.length()) {
		return false;
	}


	/* Read response */
	bool headerMode = true;
	int headerParseResult = 0;
	while ((bytesRead = sock->read(buffer, BUFFERSIZE)) > 0) {
		if (headerMode) {
			// Search for headers/body boundary in lastly fetched data
			char* bodyStr = strstr(buffer, "\r\n\r\n");
			int bodyLen = buffer + bytesRead - bodyStr;

			if (bodyStr != NULL) {
				// Found boundary
				headerStream.write(buffer, bodyStr - buffer);
				// Error checking!
				if (headerStream.fail()) {
					return 0;
				}

				bodyStream.write(bodyStr+4, bodyLen-4);
				// Error checking!
				if (bodyStream.fail()) {
					return 0;
				}

				headerMode = false;

				headerParseResult = parseHeaders(headerStream);
				switch (headerParseResult) {
					case HTTP_RESPONSECODE_OK:
						break;
					default:
						close(fp);
						return headerParseResult;
						break;

				}
			} else {
				headerStream.write(buffer, bytesRead);
				// Error checking!
				if (headerStream.fail()) {
					return 0;
				}
			}
		} else {
			bodyStream.write(buffer, bytesRead);
			// Error checking!
			if (bodyStream.fail()) {
				return 0;
			}
		}
	}
	if (internalSocketObject)
		sock->close();

	if (bytesRead < 0) {
		cerr << "Error: Could not receive response" << endl;
		return 0;
	}

	return HTTP_RESPONSECODE_OK;
}

void HttpDownloader::parseUrl() {
	int pos = 0;
	int lastPos = 0;
	// Find protocol
	if ((pos = url.find("://", 0)) != string::npos) {
		remoteProtocol = url.substr(lastPos, pos - lastPos);
		lastPos = pos + 3;
	}
	// Find host (and possibly file on remote host, e.g. "index.html")
	if ((pos = url.find("/", lastPos)) != string::npos) {
		// At least the root of the webserver should be fetched (e.g. "http://www.sunet.se/", but more likely "http://www.sunet.se/index.html")
		remoteHostname = url.substr(lastPos, pos - lastPos);
		lastPos = pos;
		remoteFile = url.substr(lastPos);
	} else {
		//Only host part specified (e.g. "http://www.sunet.se")
		remoteHostname = url.substr(lastPos);
	}
	// Find remote port
	if ((pos = remoteHostname.find(":", 0)) != string::npos) {
		remoteHostname = remoteHostname.substr(0, pos);
		remotePort = atoi(remoteHostname.substr(pos + 1).c_str());
	}
}

void HttpDownloader::split(string data, string token, vector<string> &res, int maxChars)
{
	int count = 0;
	int lastpos = 0;
	int tokenlen = token.length();
	int pos = data.find(token,lastpos);
	while(string::npos != pos && ((maxChars > 0 && pos < maxChars) || maxChars <= 0))
	{
		count = pos - lastpos;
		res.push_back(data.substr(lastpos,count));
		lastpos = pos + tokenlen;
		pos = data.find(token,lastpos);
	}
	/**
	 * If the entire string is to be scanned then we want to add the last part of the string/data
	 * to the result list. This splitis not necessary is there is an upper limit when no more characters
	 * are of interest.
	 */
	if (maxChars <= 0) {
		res.push_back(data.substr(lastpos));
	}
}

string HttpDownloader::trim(string s) {
	int trimLeftPos = s.find_first_not_of(" \n\t\r");
	int trimRightPos = s.find_last_not_of(" \n\t\r");
	int pos = 0;
	int len = 0;

	if (trimLeftPos != string::npos)
		pos = trimLeftPos;

	if (trimRightPos != string::npos)
		len = trimRightPos + 1 - pos;
	else
		len = s.length() - pos;

	return s.substr(pos, len);
}
bool HttpDownloader::downloadHeaders() {
	int tries = 3;
	while (tries) {
		ostringstream body;
		int fetchRes = fetch(buildRequestString("HEAD ", remoteFile), body);
		if (fetchRes == HTTP_RESPONSECODE_MOVEDPERMANENTLY || respCode == HTTP_RESPONSECODE_MOVEDTEMPORARILY) {
			url = getHeader("Location");
			parseUrl();
			if (!followRedirect) {
				return false;
			}
		} else if (fetchRes == HTTP_RESPONSECODE_OK) {
			return true;
		} else {
			return false;
		}
		tries--;
	}
	return false;
}

string HttpDownloader::downloadToString() {

	int tries = 3;
	while (tries) {
		ostringstream body;
		int fetchRes = fetch(buildRequestString("GET ", remoteFile), body);
		if (fetchRes == HTTP_RESPONSECODE_MOVEDPERMANENTLY || respCode == HTTP_RESPONSECODE_MOVEDTEMPORARILY) {
			url = getHeader("Location");
			parseUrl();
			if (!followRedirect) {
				return "";
			}
		} else if (fetchRes == HTTP_RESPONSECODE_OK) {
			return body.str();
		} else {
			return "";
		}
		tries--;
	}
	return "";
}
bool HttpDownloader::downloadToFile(string filename) {

	int tries = 3;
	while (tries) {

		ofstream file(filename.c_str(), ios_base::out | ios_base::trunc);
		if (!file) {
			return false;
		}

		int fetchRes = fetch(buildRequestString("GET ", remoteFile), file);
		if (fetchRes == HTTP_RESPONSECODE_MOVEDPERMANENTLY || respCode == HTTP_RESPONSECODE_MOVEDTEMPORARILY) {
			url = getHeader("Location");
			parseUrl();
			if (!followRedirect) {
				return false;
			}
		} else if (fetchRes == HTTP_RESPONSECODE_OK) {
			return !file.fail();
		} else {
			return false;
		}
		tries--;
	}
	return false;
}

int HttpDownloader::parseHeaders(stringstream & headers) {
	vector<string> lines;
	vector<string> initialLine;
	string bodySep = HTTP_HEADER_CRLF;
	bodySep += HTTP_HEADER_CRLF;

	split(headers.str(), HTTP_HEADER_CRLF, lines);
	split(lines[0], " ", initialLine);

	respCode = atoi(initialLine[1].c_str());

	for (int i=1; i<lines.size(); i++) {
		parseHeader(lines.at(i));
	}

	return respCode;
}

void HttpDownloader::parseHeader(string line) {
	int pos = line.find(':');
	if (pos != string::npos) {
		cout << "Found header: [" << line.substr(0, pos) << " = " << trim(line.substr(pos+1)) << "]" << endl;
		headers[line.substr(0, pos)] = trim(line.substr(pos+1));
	} else {
		cerr << "ERROR: Header \"" << line << "\" is not valid!" << endl;
	}
}

string HttpDownloader::getHeader(string header) {
	map<string, string>::iterator iter = headers.find(header);
	if (iter != headers.end()) {
		return iter->second;
	} else {
		return "";
	}
}
string HttpDownloader::buildRequestString(string method, string file) {
	string res = method + " " + file + " " + HTTP_METHOD_1_0;
	res.append(HTTP_HEADER_CRLF);

	res.append(HTTP_HEADER_FROM);
	res.append(": anonymous@minisip.org");
	res.append(HTTP_HEADER_CRLF);

	res.append(HTTP_HEADER_USERAGENT);
	res.append(": Minisip-FileDownloader/0.1");
	res.append(HTTP_HEADER_CRLF);

	res.append(HTTP_HEADER_CRLF);
	//res.append(HTTP_HEADER_CRLF);

	return res;
}
