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

#ifndef _FILEDOWNLOADER_H_
#define _FILEDOWNLOADER_H_

#include<libmnetutil/libmnetutil_config.h>

#include <libmutil/MemObject.h>
#include <libmnetutil/FileUrl.h>
#include <libmnetutil/FileDownloaderException.h>
#include <libmnetutil/Downloader.h>
#include <string>
#include <vector>
#include <map>

/**
 * This class simplifies retrieval of \em locally stored files using "file URLs".
 *
 * It is important to note that this class \em cannot retrieve files from remote hosts,
 * even though it uses FileUrl object (which can handle URIs with remote hosts).
 *
 * @author	Mikael Svensson
 */
class LIBMNETUTIL_API FileDownloader : public Downloader {
	public:
		/**
		* Connects to LDAP server but does not fetch file.
		*
		* @param	url	File/document to fetch.
		 */
		FileDownloader(std::string originalUrl);

		/**
		 * Returns the data pointed to by the URL supplied to the constructor.
		 *
		 * This function allocates enough memory to keep the entire source file in memory.
		 *
		 * @param	length	Pointer to integer that will be used to return
		 * 			the length of the returned character array.
		 */
		char* getChars(int *length);

		/**
		 * Fetches \em binary data from LDAP server and saves as files on local computer.
		 *
		 * This functions has a small memory-footprint as it copies one "chunk" at a time
		 * from the source file.
		 *
		 * @note	The function replaces existing files without informing the user.
		 * @param	filename	The path to where the retrieved file should be stored.
		 */
		void saveToFile(std::string filename) throw (FileDownloaderException);

		/**
		 * Returns first string belonging to the specified attribute name.
		 *
		 * @todo	This function is untested!
		 */
		std::string getString() throw (FileDownloaderException);


		virtual std::string getMemObjectType() const {return "FileDownloader";};

	private:
		// Variables
		FileUrl url;

		// Functions
		bool fetch(std::ostream & outStream);
};

#endif
