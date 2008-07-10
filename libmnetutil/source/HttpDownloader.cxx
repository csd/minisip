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
#include <sys/types.h>

#include <libmutil/MemObject.h>
#include <libmnetutil/HttpDownloader.h>
#include <libmnetutil/StreamSocket.h>

#include <fstream>
#include <sstream>

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <string.h>
#include <stdlib.h>

#define BUFFERSIZE 4096

#define HTTP_METHOD_1_0                         "HTTP/1.0"
#define HTTP_HEADER_CRLF                        "\r\n"

HttpDownloader::HttpDownloader(std::string url) : url (url), remotePort(80), respCode(-1), followRedirect(true) {
	parseUrl();
	if (remotePort > 0 && remoteHostname != "") {
		sock = new TCPSocket(remoteHostname, remotePort);
	}
}

HttpDownloader::HttpDownloader(std::string url, MRef<StreamSocket*> sock): url (url), remotePort(80), respCode(-1), followRedirect(true), sock(sock) {
	parseUrl();
}

HttpDownloader::~HttpDownloader() {
}

char* HttpDownloader::getChars(int *length) {
	int tries = 3;
	*length = 0;
	while (tries) {
		std::ostringstream body;
		int fetchRes = fetch(buildRequestString("GET ", remoteFile), body);
		if (fetchRes == HTTP_RESPONSECODE_MOVEDPERMANENTLY || respCode == HTTP_RESPONSECODE_MOVEDTEMPORARILY) {
			url = getHeader("Location");
			parseUrl();
			if (!followRedirect) {
				return NULL;
			}
		} else if (fetchRes == HTTP_RESPONSECODE_OK) {
			*length = (int)body.str().length();
			char* res = new char[*length];
			memcpy(res, body.str().c_str(), *length);
			return res;
		} else {
			return NULL;
		}
		tries--;
	}
	return NULL;
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

int HttpDownloader::fetch(std::string request, std::ostream & bodyStream) {

	if ( sock.isNull() ) {
		// TODO: Error check for socket object
	}

	std::stringstream headerStream;
	int32_t bytesWritten = 0, bytesRead = 0;

	/* Buffer for holding data read from the network stream */
	char buffer[BUFFERSIZE];
	memset(buffer, 0, sizeof(buffer)); // Zero out the buffer used when recieving data


	/* Send request */
	bytesWritten = sock->write(request.c_str(), (int32_t)request.length());
	if (bytesWritten < 0) {
		//cerr << "Error: Could not send request" << endl;
		return false;
	} else if (bytesWritten < (int32_t)request.length()) {
		return false;
	}


	/* Read response */
	bool headerMode = true;
	int headerParseResult = 0;
	while ((bytesRead = sock->read(buffer, BUFFERSIZE)) > 0) {
		if (headerMode) {
			// Search for headers/body boundary in lastly fetched data
			char* bodyStr = strstr(buffer, "\r\n\r\n");
			int bodyLen = ((int)(buffer-bodyStr)) + bytesRead;

			if (bodyStr != NULL) {
				// Found boundary
				headerStream.write(buffer, (int)(bodyStr - (char*)buffer));
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

/*
	if (bytesRead < 0) {
		//cerr << "Error: Could not receive response" << endl;
		return 0;
	}
*/
	return HTTP_RESPONSECODE_OK;
}

void HttpDownloader::parseUrl() {
	size_t pos = 0;
	size_t lastPos = 0;
	// Find protocol
	if ((pos = url.find("://", 0)) != std::string::npos) {
		remoteProtocol = url.substr(lastPos, pos - lastPos);
		lastPos = pos + 3;
	}
	// Find host (and possibly file on remote host, e.g. "index.html")
	if ((pos = url.find("/", lastPos)) != std::string::npos) {
		// At least the root of the webserver should be fetched (e.g. "http://www.sunet.se/", but more likely "http://www.sunet.se/index.html")
		remoteHostname = url.substr(lastPos, pos - lastPos);
		lastPos = pos;
		remoteFile = url.substr(lastPos);
	} else {
		//Only host part specified (e.g. "http://www.sunet.se")
		remoteHostname = url.substr(lastPos);
	}
	// Find remote port
	if ((pos = remoteHostname.find(":", 0)) != std::string::npos) {
		remoteHostname = remoteHostname.substr(0, pos);
		remotePort = atoi(remoteHostname.substr(pos + 1).c_str());
	}
}

void HttpDownloader::split(std::string data, std::string token, std::vector<std::string> &res, int maxChars)
{
	size_t count = 0;
	size_t lastpos = 0;
	size_t tokenlen = token.length();
	size_t pos = data.find(token,lastpos);
	while(std::string::npos != pos && ((maxChars > 0 && (int)pos < maxChars) || maxChars <= 0))
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

std::string HttpDownloader::trim(std::string s) {
	size_t trimLeftPos = s.find_first_not_of(" \n\t\r");
	size_t trimRightPos = s.find_last_not_of(" \n\t\r");
	size_t pos = 0;
	size_t len = 0;

	if (trimLeftPos != std::string::npos)
		pos = trimLeftPos;

	if (trimRightPos != std::string::npos)
		len = trimRightPos + 1 - pos;
	else
		len = s.length() - pos;

	return s.substr(pos, len);
}
bool HttpDownloader::downloadHeaders() {
	int tries = 3;
	while (tries) {
		std::ostringstream body;
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

std::string HttpDownloader::downloadToString() {

	int tries = 3;
	while (tries) {
		std::ostringstream body;
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
bool HttpDownloader::downloadToFile(std::string filename) {

	int tries = 3;
	while (tries) {

		std::ofstream file(filename.c_str(), std::ios_base::out | std::ios_base::trunc);
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

int HttpDownloader::parseHeaders(std::stringstream & headers) {
	std::vector<std::string> lines;
	std::vector<std::string> initialLine;
	std::string bodySep = HTTP_HEADER_CRLF;
	bodySep += HTTP_HEADER_CRLF;

	split(headers.str(), HTTP_HEADER_CRLF, lines);
	split(lines[0], " ", initialLine);

	respCode = atoi(initialLine[1].c_str());

	for (size_t i=1; i<lines.size(); i++) {
		parseHeader(lines.at(i));
	}

	return respCode;
}

void HttpDownloader::parseHeader(std::string line) {
	size_t pos = line.find(':');
	if (pos != std::string::npos) {
		//cout << "Found header: [" << line.substr(0, pos) << " = " << trim(line.substr(pos+1)) << "]" << endl;
		headers[line.substr(0, pos)] = trim(line.substr(pos+1));
	} else {
		//cerr << "ERROR: Header \"" << line << "\" is not valid!" << endl;
	}
}

std::string HttpDownloader::getHeader(std::string header) {
	std::map<std::string, std::string>::iterator iter = headers.find(header);
	if (iter != headers.end()) {
		return iter->second;
	} else {
		return "";
	}
}
std::string HttpDownloader::buildRequestString(std::string method, std::string file) {
	std::string res = method + " " + file + " " + HTTP_METHOD_1_0;
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
