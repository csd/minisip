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

#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <libmnetutil/libmnetutil_config.h>

#include <libmutil/MemObject.h>
#include <libmnetutil/StreamSocket.h>
#include <string>

/**
 * Creates objects for downloading documents specified by and URI.
 *
 * This class provides an easy interface for fetching files over the Internet
 * using a variety of protocols. This class does not fetch any data but only
 * creates an object that later can be used for fetching the actual data.
 *
 * The class is very generic, hence very limited and since it is abstract it
 * cannot be instantiated by itself.
 *
 */
class LIBMNETUTIL_API Downloader : public MObject {

	public:
		/**
		 * Uses the protocol portion of the specified URI to determine
		 * what type of object should be created to handle the download.
		 *
		 * Currently, the following downloading URIs are supported:
		 * 	file://		Local file download
		 * 	http://		Download from web server
		 * 	ldap://		Download from LDAP server
		 *
		 * @param	uri	Full URI for a remote document (e.g. "http://www.kth.se/index.html")
		 * @param	conn	Given if an existing connection should be used. This is currently
		 * 			only supported for http:// URIs.
		 */
		static MRef<Downloader*> create(std::string const uri, MRef<StreamSocket*> conn=NULL);


		virtual std::string getMemObjectType() const {return "Downloader";}

		/**
		 * Returns the document in question. In order to support both
		 * textual and binary documents there is no function for
		 * retrieving the remote document as a string (this would not
		 * work well with binary data).
		 */
		virtual char* getChars(int *length) = 0;

};


#endif
