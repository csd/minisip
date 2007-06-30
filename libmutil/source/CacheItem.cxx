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

#include <config.h>
#include <libmutil/CacheItem.h>

CacheItem::CacheItem() {
	cacheDate = time(NULL);
	if (-1 == cacheDate)
		cacheDate = 0;
	validFrom = 0;
	validUntil = 0;
}

time_t CacheItem::getCacheDate() const {
	return cacheDate;
}

time_t CacheItem::getValidFrom() const {
	return validFrom;
}
time_t CacheItem::getValidUntil() const {
	return validUntil;
}

void CacheItem::setCacheDate(const time_t date) {
	cacheDate = date;
}
void CacheItem::setValidFrom(const time_t date) {
	validFrom = date;
}
void CacheItem::setValidUntil(const time_t date) {
	validUntil = date;
}
