#include <sys/types.h>

#include <libmutil/MemObject.h>
#include <libmnetutil/FileUrl.h>
#include <libmnetutil/FileDownloader.h>

#include <fstream>
#include <sstream>

#include <string>
#include <iostream>

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

			inStream.seekg(0, std::ios::end);
			fileLen = inStream.tellg();
			inStream.seekg(0, std::ios::beg);

			*length = fileLen;
			char* res = new char[*length];
			inStream.read(res, *length);
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

bool FileDownloader::fetch(std::ostream & outStream) {

	if (url.isValid()) {

		std::ifstream inStream(url.getPath().c_str(), std::ios::in);
		if (inStream.is_open()) {

			int32_t bytesRead = 0;
			int32_t fileLen = 0;
			int32_t nextReadLen = BUFFERSIZE;

			inStream.seekg(0, std::ios::end);
			fileLen = inStream.tellg();
			inStream.seekg(0, std::ios::beg);

			char buffer[BUFFERSIZE];
			memset(buffer, 0, sizeof(buffer)); // Zero out the buffer used when recieving data

			bool error = false;
			do {
				if (bytesRead + BUFFERSIZE > fileLen)
					nextReadLen = fileLen - bytesRead;

				inStream.read(buffer, nextReadLen);
				outStream.write(buffer, nextReadLen);
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
