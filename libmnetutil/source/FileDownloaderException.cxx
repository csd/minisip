#include <libmnetutil/FileDownloaderException.h>

/**
 * @author	Mikael Svensson
 * @author	Erik Eliasson <eliasson@it.kth.se>
 * @author	Johan Bilien <jobi@via.ecp.fr>
 */

FileDownloaderException::FileDownloaderException() {
	msg = "FileDownloaderException";
};

FileDownloaderException::FileDownloaderException(std::string msg) {
	this->msg = "FileDownloaderException: " + msg;
};

const char* FileDownloaderException::what()const throw(){
	return msg.c_str();
}

FileDownloaderNotFoundException::FileDownloaderNotFoundException(std::string message) {
	msg = "FileDownloaderNotFoundException: Could not find file " + message;
}
