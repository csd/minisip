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
#include <libmnetutil/FileUrl.h>
#include <libmnetutil/FileDownloader.h>

#include <fstream>
#include <sstream>

#include <string>
#include <iostream>
#include <string.h>

#define BUFFERSIZE 4096

FileDownloader::FileDownloader(std::string originalUrl) {
#ifdef WIN32
	url = FileUrl(originalUrl, FILEURL_TYPE_WINDOWS);
#else
	url = FileUrl(originalUrl, FILEURL_TYPE_UNKNOWN);
#endif
}

char* FileDownloader::getChars(int *length) {
	*length = 0;

	if (url.isValid()) {

		std::ifstream inStream(url.getPath().c_str(), std::ios::in);
		if (inStream.is_open()) {

			int32_t fileLen = 0;

			// Determine size of file
			inStream.seekg(0, std::ios::end);
			fileLen = inStream.tellg();
			inStream.seekg(0, std::ios::beg);

			*length = fileLen;

			// Allocate enough memory to keep the entire file in memory
			char* res = new char[*length];

			// Read from file directly into memory
			inStream.read(res, *length);

			// Test if the input stream is still good, meaning that no error occurred.
			if (!inStream.good()) {
				inStream.close();
				return NULL;
			}
			inStream.close();
			return res;
		} else {
			return NULL;
		}
	} else {
		return NULL;
	}
}

/**
 * @todo	Merge this function with saveToFile.
 */
bool FileDownloader::fetch(std::ostream & outStream) {

	if (url.isValid()) {

		std::ifstream inStream(url.getPath().c_str(), std::ios::in);
		if (inStream.is_open()) {

			int32_t fileLen = 0;

			// Determine size of file
			inStream.seekg(0, std::ios::end);
			fileLen = inStream.tellg();
			inStream.seekg(0, std::ios::beg);

			int32_t bytesRead = 0;
			int32_t nextReadLen = BUFFERSIZE;

			char buffer[BUFFERSIZE];
			memset(buffer, 0, sizeof(buffer)); // Zero out the buffer used when recieving data

			bool error = false;
			do {
				// Test if it is possible to read BUFFERSIZE bytes from the file
				if (bytesRead + BUFFERSIZE > fileLen)
					nextReadLen = fileLen - bytesRead;

				// Read bytes from input file and immediately write them to the output file
				inStream.read(buffer, nextReadLen);
				outStream.write(buffer, nextReadLen);

				// Test if the streams are still OK.
				if (!inStream.good() || !outStream.good()) {
					error = true;
					break;
				}
				bytesRead += nextReadLen;
			} while (bytesRead < fileLen);

			inStream.close();

			return !error;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

/**
 * @todo	Merge this function with fetch.
 */
void FileDownloader::saveToFile(std::string filename) throw (FileDownloaderException) {
	std::ofstream file(filename.c_str());
	if (file.good()) {
		if (!fetch(file)) {
			file.close();
			throw FileDownloaderException("Could not save file");
		}
		file.close();
	}
}

