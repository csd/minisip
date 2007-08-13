/*
 Copyright (C) 2004-2007 The Minisip Team

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

#ifndef _CACHEMANAGER_H_
#define _CACHEMANAGER_H_

#include <libmcrypto/config.h>
#include <libmutil/MemObject.h>

#include <libmcrypto/cert.h>
#include <libmutil/CacheItem.h>
#include <libmnetutil/DirectorySet.h>
#include <libmnetutil/DirectorySetItem.h>

#include <string>
#include <map>
#include <list>
#include <vector>

#define CACHEMANAGER_CERTSET_DOWNLOADED "downloadedcerts"
#define CACHEMANAGER_CERTSET_ROOTCAS "rootcas"
#define CACHEMANAGER_DIRSET_MAIN "main"

class LIBMCRYPTO_API CertFindSettings : public MObject {
	public:
		CertFindSettings(const std::string s, const std::string i) : searchText(s), issuer(i) {}
		//CertFindSettings();
		std::string searchText;
		std::string issuer;
};

class LIBMCRYPTO_API CacheManager : public MObject {
	public:
		CacheManager();

		MRef<Certificate*> findCertificate();

		/**
		 * Returns a directory set item responsible for a particular domain.
		 *
		 * Note that even though several cached directories may match the given domain name only
		 * the first directory will be returned. Use DirectorySet directly if you need a list
		 * of all matching directories and not just any matching.
		 *
		 * @param	defaultSet	Specifies which set to scan. All sets are scanned if this
		 * 				parameter is left out or set to an empty string.
		 */
		MRef<DirectorySetItem*> findDirectory(const std::string domain, const std::string defaultSet = "");
		//MRef<CrlSetItem*> findCrl();

		MRef<DirectorySet*> getDirectorySet(std::string key);

		/**
		 * Adds a directory item to a directory set in the cache.
		 *
		 * @param	dirItem		The item to add.
		 * @param	setKey		The name of the directory set to which the item should be added.
		 * 				If the string is empty the item will be added to a new directory
		 * 				set with a random name.
		 * @returns	The name of the directory set to which the item was added.
		 */
		std::string addDirectory(MRef<DirectorySetItem*> dirItem, std::string setKey);

		/**
		 * Wrapper for addDirectory() that automatically creates an LDAP directory set item
		 * and adds it to a directory set in the cache.
		 *
		 * @param	url		URL speciying the location of the LDAP directory
		 * @param	subTree		Specifies which part of the DNS tree that the directory
		 * 				is responsible for.
		 * @param	setKey		The name of the directory set to which the item should be added.
		 * 				If the string is empty the item will be added to a new directory
		 * 				set with a random name.
		 * @returns	The name of the directory set to which the item was added.
		 */
		std::string addDirectoryLdap(std::string url, std::string subTree, const std::string setKey);

		//void purgeCache();
		//void removeFromCache(MRef<CacheItem*> item);

		/**
		 * @param       searchText      Can be either a SIP URI or an X.509 DN. If the parameter is a SIP URI then
		 *                              the function will match the URI against the certificate's subjectAltNames.
		 *                              Otherwise (if the parameter is not a SIP URI) the subject field is matched.
		 */
		std::vector<MRef<Certificate*> > findCertificates(const std::string searchText, const std::string issuer, const std::string defaultSet = "");

		void addCertificateSet(const MRef<CertificateSet*> certSet, const std::string setKey);
		std::string addCertificate(const MRef<Certificate*> certItem, std::string setKey);

		bool findCertsFailedBefore(const std::string searchText, const std::string issuer);
		void addFindCertsFailed(const std::string searchText, const std::string issuer);

	private:
		std::string getNewDirectorySetKey() const;
		std::string getNewCertificateSetKey() const;

		//std::string test;

		std::map<std::string, MRef<DirectorySet*> > directorySets;
		std::map<std::string, MRef<CertificateSet*> > certificateSets;
		//std::map<std::string, MRef<CertificateSet*> > certificateSets2;
		std::list<MRef<CertFindSettings*> > failedCertSearches;

		//std::vector<MRef<Certificate*> > fakeCache;
		//std::string test;
		//int test;
		/*
		int a;
		int b;
		int c;
		int d;
		int e;
		int f;
		int g;
		int h;
		int i;
		int j;
		int k;
		int l;
		int m;
		*/
};

#endif
