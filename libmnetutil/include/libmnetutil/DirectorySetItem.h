/*
  Copyright (C) 2007, Mikael Svensson

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
 * Authors:	Mikael Svensson
*/

#ifndef _DIRECTORYSETITEM_H_
#define _DIRECTORYSETITEM_H_

#include <libmnetutil/libmnetutil_config.h>
#include <libmutil/CacheItem.h>

#include <string>

#define DIRECTORYSETITEM_TYPE_UNSPECIFIED 0
#define DIRECTORYSETITEM_TYPE_LDAP 1

/**
 * Represents a single directory service.
 */
class LIBMNETUTIL_API DirectorySetItem : public CacheItem {
	public:
		DirectorySetItem();
		DirectorySetItem(std::string url, std::string subTree);
		DirectorySetItem(int32_t type, std::string url, std::string subTree);
		DirectorySetItem(int32_t type);

		int32_t getType() const;
		std::string getUrl() const;
		std::string getSubTree() const;

		void setType(const int32_t type);
		void setUrl(const std::string url);
		void setSubTree(const std::string subTree);

		virtual std::string getMemObjectType() const {return "DirectorySetItem";};

		bool operator ==(const DirectorySetItem item2) const;
	private:
		/**
		 * The type of directory refered to by the url attribute.
		 *
	 	 * Currently the only supported value is \c DIRECTORYSETITEM_TYPE_LDAP.
		 */
		int32_t type;

		/**
		 * The URL used to connect to the directory. The interpretation of the URL is
		 * dependent on the type of directory it refers to. Therefore the URL may be
		 * an LDAP URL, or any other type of URL, as long as Minisip knows how to deal
		 * with it.
		 */
		std::string url;

		/**
		 * Specified which portion of a tree that the directory server is responsible for.
		 *
		 * The sub-tree specifier tells us the right-most part of the mail/SIP addresses
		 * that the server has information about.
		 */
		std::string subTree;
};

#endif
