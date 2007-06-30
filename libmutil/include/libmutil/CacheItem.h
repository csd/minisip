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

#ifndef _CACHEITEM_H_
#define _CACHEITEM_H_

#include <libmutil/libmutil_config.h>
#include <libmutil/MemObject.h>

#include <time.h>

/**
 * Represents a single item in a cache.
 *
 * This class is very generic by design and only contains properties that can be assumed
 * to be appropriate for any kind of cachable object. When implementing a cache that
 * can contain multiple types of objects all those "cachable objects" may inherit from this
 * class in order to provide some kind of uniformity.
 *
 * A class responsible for maintaining the entire cache by periodically removing old items
 * can easily use the same code to scan all cached items as they all have the same
 * basic properies, the properties defined in by the CacheItem class.
 *
 * @todo	Add property for "associated file path", but make sure it is not a mandatory
 * 		property as cached items not necessarily have anything to do with actual files.
 */
class LIBMUTIL_API CacheItem : public MObject {
	public:
		CacheItem();

		time_t getCacheDate() const;
		time_t getValidFrom() const;
		time_t getValidUntil() const;

		void setCacheDate(const time_t date);
		void setValidFrom(const time_t date);
		void setValidUntil(const time_t date);

		virtual std::string getMemObjectType() const {return "CacheItem";};
	private:
		time_t cacheDate;
		time_t validFrom;
		time_t validUntil;
};
#endif
