/*
 Copyright (C) 2007 the Minisip Team

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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2007
 *
 * Authors: Mikael Svensson
*/

#include <config.h>
#include <libminisip/config/CacheManager.h>
#include <libmutil/stringutils.h>

#include <list>

MRef<DirectorySetItem*> CacheManager::findDirectory(const std::string domain, const std::string defaultSet) {
	std::vector<MRef<DirectorySetItem*> > res;
	if (defaultSet.length() == 0) {
		// Scan all directory sets
		//std::map<const std::string, MRef<DirectorySet*> >::iterator i = directorySets.begin();
		//while (i != directorySets.end()) {
		for (std::map<std::string, MRef<DirectorySet*> >::iterator i = directorySets.begin(); i != directorySets.end(); i++) {
			res = i->second->findItemsPrioritized(domain);
			if (!res.empty())
				return res.front();
		}

	} else {
		// Scan only one directory set, the one mentioned in the function parameters.
		if (directorySets.find(defaultSet) != directorySets.end()) {
			res = directorySets[defaultSet]->findItemsPrioritized(domain);
			if (!res.empty())
				return res.front();
		}
	}
	// Return empty item if no result found
	return MRef<DirectorySetItem*>();
}

MRef<DirectorySet*> CacheManager::getDirectorySet(std::string key) {
	if (directorySets.find(key) != directorySets.end())
		return directorySets[key];
	return MRef<DirectorySet*>();
}

std::string CacheManager::addDirectory(const MRef<DirectorySetItem*> dirItem, std::string setKey) {
	if (0 == setKey.length()) {
		setKey = getNewDirectoryKey();
	}
	if (directorySets.find(setKey) == directorySets.end()) {
		directorySets[setKey] = MRef<DirectorySet*>(new DirectorySet());
	}
	directorySets[setKey]->addItem(dirItem);
	return setKey;
}

std::string CacheManager::addDirectoryLdap(std::string url, std::string subTree, const std::string setKey) {
	return addDirectory(MRef<DirectorySetItem*>(new DirectorySetItem(url, subTree)), setKey != "" ? setKey : getNewDirectoryKey());
}

//void CacheManager::purgeCache();
//void CacheManager::removeFromCache(MRef<CacheItem*> item);

std::string CacheManager::getNewDirectoryKey() const {
	std::string newName = "dirset";
	int num = 1;
	while (directorySets.find(newName + itoa(num)) != directorySets.end())
		num++;
	return newName;
}
