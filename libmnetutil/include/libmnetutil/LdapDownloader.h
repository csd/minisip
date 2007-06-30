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

#ifndef _LDAPDOWNLOAD_H_
#define _LDAPDOWNLOAD_H_

#include<libmnetutil/libmnetutil_config.h>

#include <libmutil/MemObject.h>
#include <libmnetutil/Downloader.h>
#include <libmnetutil/LdapEntry.h>
#include <libmnetutil/LdapUrl.h>
#include <libmnetutil/LdapConnection.h>
#include <libmnetutil/LdapException.h>
#include <string>
#include <vector>
#include <map>

/**
 * This class simplifies retrieval of LDAP attributes using LDAP URLs.
 *
 * The constructor takes one argument, an LDAP URL, and fetches the
 * data pointed to by the URL. If several LDAP objects are returned they
 * will all be downloaded but only the first will be accessible using
 * this class.
 *
 * This class behaves differntly from HttpDownloader regarding when the
 * data is fetched from the remote server. HttpDownloader fetches the
 * requested document each time the user calls any of its download functions
 * whereas LdapDownloader fetches the remote data only once (when the first
 * attribute value is requested). This is only important to know if the
 * remote document/data changes during the life-time of an instance of
 * LdapDownloader or HttpDownloader.
 *
 * Since the primary motif of this class is to simplify the LDAP interaction
 * it lacks support for multi-valued attributes (only the first attribute
 * value may be retrieved using this class).
 *
 * @author	Mikael Svensson
 */
class LIBMNETUTIL_API LdapDownloader : public Downloader {
	public:
		/**
		 * Connects to LDAP server but does not fetch file.
		 *
		 * @param	url	File/document to fetch.
		 */
		LdapDownloader(std::string originalUrl);

		/**
		 * The default constructor deallocates memory, if allocated.
		 */
		//virtual ~LdapDownloader();

		/**
		 * Returns the data pointed to by the URL supplied to the constructor.
		 *
		 * One semantically tricky thing is what data to return if the URL points
		 * to several LDAP objects or multiple attributes. To work around this
		 * potential problem the first attribute of the first object will be
		 * returned (could be either a string or a binary value).
		 *
		 * @param	length	Pointer to integer that will be used to return
		 * 			the length of the returned character array.
		 */
		char* getChars(int *length);

		/**
		 * Fetches \em binary data from LDAP server and saves as files on local computer.
		 *
		 * @note	The function replaces existing files without informing the user.
		 * @param	attr		Name of attribute to fetch. Note that ";binary" should
		 * 				be appended by the user.
		 * @param	filenameBase	The "template" that tells under which names the downloaded
		 * 				binary attribute values will be saved. saveToFiles uses
		 * 				nextFilename internally to generate file names.
		 * @param	onlyFirst	Forces the method to only save the first returned value.
		 * 				This does not limit the amount of data downloaded to memory,
		 * 				only the amount saved to local files.
		 * @returns	List of strings, each string specifying a filename where a certificate has
		 * 		been saved. Note that an empty list is returned even if errors occur.
		 */
		std::vector<std::string> saveToFiles(std::string attr, std::string filenameBase, bool onlyFirst = false);

		/**
		 * Returns first string belonging to the specified attribute name.
		 *
		 * @todo	This function is untested!
		 */
		std::string getString(std::string attr) throw (LdapAttributeNotFoundException, LdapException);

		/**
		 * Returns first binary value belonging to the specified attribute name.
		 *
		 * @todo	This function is untested!
		 */
		MRef<LdapEntryBinaryValue*> getBinary(std::string attr) throw (LdapAttributeNotFoundException, LdapException);


		virtual std::string getMemObjectType() const {return "LdapDownloader";};

	private:
		// Variables
		LdapUrl url;
		MRef<LdapConnection*> conn;
		std::vector<MRef<LdapEntry*> > entries;
		bool isLoaded;

		// Functions
		void fetch();

		/**
		 * Simple helper function that returns \p baseName concatenated with \p num. \p num is, however, only
		 * appended if > 1. Additionally, if \p baseName has a file extension the \p num parameter is inserted
		 * before the file extension.
		 *
		 * Examples:
		 * \p baseName = "some_file" and \p num = 1 returns "some_file"
		 * \p baseName = "some_file.bin" and \p num = 1 returns "some_file.bin"
		 * \p baseName = "some_file.bin" and \p num = 3 returns "some_file.3.bin"
		 * \p baseName = "some_file.3.bin" and \p num = 4 returns "some_file.3.4.bin"
		 */
		std::string nextFilename(std::string baseName, int num);

};

#endif
