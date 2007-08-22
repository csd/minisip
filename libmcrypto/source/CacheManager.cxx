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
#include <libmcrypto/CacheManager.h>
#include <libmutil/stringutils.h>
#include <libmcrypto/cert.h>
#include <libmnetutil/FileDownloader.h>
#include <libmutil/SipUri.h>
#include <libmutil/dbg.h>

#include <list>
#include <stack>

#include <iostream>

CacheManager::CacheManager() {
	mdbg("ucd") << "^^^ Start of " << __FUNCTION__ << std::endl;
	/*
	Certificate* cert;

	std::stack<std::string> certFiles;
	certFiles.push("/home/mikael/thesis/environments/kth/ucd-fast.SIGNEDBY.ucd-slow.certificate.der");
	certFiles.push("/home/mikael/thesis/environments/kth/ucd-slow.SIGNEDBY.ucd-fast.certificate.der");

	while (!certFiles.empty()) {
		FileDownloader file("file://" + certFiles.top());
		int len = 0;
		char* data = file.getChars(&len);

		Certificate* cert = Certificate::load(reinterpret_cast<unsigned char*>(data), len);
		fakeCache.push_back(MRef<Certificate*>(cert));

		certFiles.pop();
	}
	*/
	mdbg("ucd") << "$$$ End of " << __FUNCTION__ << std::endl;
}
MRef<DirectorySetItem*> CacheManager::findDirectory(const std::string domain, const std::string defaultSet) {
	mdbg("ucd") << "^^^ Start of " << __FUNCTION__ << std::endl;
	std::vector<MRef<DirectorySetItem*> > res;
	if (defaultSet.length() == 0) {
		// Scan all directory sets
		for (std::map<std::string, MRef<DirectorySet*> >::iterator i = directorySets.begin(); i != directorySets.end(); i++) {
			res = i->second->findItemsPrioritized(domain);
			if (!res.empty()) {
				mdbg("ucd") << "$$$ End of " << __FUNCTION__ << std::endl;
				return res.front();
			}
		}

	} else {
		// Scan only one directory set, the one mentioned in the function parameters.
		if (directorySets.find(defaultSet) != directorySets.end()) {
			res = directorySets[defaultSet]->findItemsPrioritized(domain);
			if (!res.empty()) {
				mdbg("ucd") << "$$$ End of " << __FUNCTION__ << std::endl;
				return res.front();
			}
		}
	}
	// Return empty item if no result found
	mdbg("ucd") << "$$$ End of " << __FUNCTION__ << std::endl;
	return MRef<DirectorySetItem*>();
}

MRef<DirectorySet*> CacheManager::getDirectorySet(std::string key) {
	mdbg("ucd") << "^^^ Start of " << __FUNCTION__ << std::endl;
	if (directorySets.find(key) != directorySets.end()) {
		mdbg("ucd") << "$$$ End of " << __FUNCTION__ << std::endl;
		return directorySets[key];
	}
	mdbg("ucd") << "$$$ End of " << __FUNCTION__ << std::endl;
	return MRef<DirectorySet*>();
}

std::string CacheManager::addDirectory(const MRef<DirectorySetItem*> dirItem, std::string setKey) {
	mdbg("ucd") << "^^^ Start of " << __FUNCTION__ << std::endl;
	if (0 == setKey.length()) {
		setKey = getNewDirectorySetKey();
	}
	if (directorySets.find(setKey) == directorySets.end()) {
		directorySets[setKey] = MRef<DirectorySet*>(new DirectorySet());
	}
	directorySets[setKey]->addItem(dirItem);
	mdbg("ucd") << "$$$ End of " << __FUNCTION__ << std::endl;
	return setKey;
}

std::string CacheManager::addDirectoryLdap(std::string url, std::string subTree, const std::string setKey) {
	mdbg("ucd") << "^^^ Start of " << __FUNCTION__ << std::endl;
	mdbg("ucd") << "$$$ End of " << __FUNCTION__ << std::endl;
	return addDirectory(MRef<DirectorySetItem*>(new DirectorySetItem(url, subTree)), setKey != "" ? setKey : getNewDirectorySetKey());
}

//void CacheManager::purgeCache();
//void CacheManager::removeFromCache(MRef<CacheItem*> item);

std::string CacheManager::getNewDirectorySetKey() const {
	mdbg("ucd") << "^^^ Start of " << __FUNCTION__ << std::endl;
	std::string newName = "dirset";
	int num = 1;
	while (directorySets.find(newName + itoa(num)) != directorySets.end())
		num++;

	mdbg("ucd") << "$$$ End of " << __FUNCTION__ << std::endl;
	return newName;
}
std::string CacheManager::getNewCertificateSetKey() const {
	mdbg("ucd") << "^^^ Start of " << __FUNCTION__ << std::endl;
	std::string newName = "certset";
	int num = 1;
	while (certificateSets.find(newName + itoa(num)) != certificateSets.end())
		num++;

	mdbg("ucd") << "$$$ End of " << __FUNCTION__ << std::endl;
	return newName;
}

/**
 * @note	If a certificate has been index and unloaded from memory, this function will indirectly
 * 		(through its use of CertificateSetItem.getCertificate()) try to load the certifcate
 * 		once more. This may entain downloading the certificate from a remote host.
 * @todo	Subject and Issuer should NOT be used to identify certificates. Use *KeyIdentifier (?) and ??? instead.
 */
std::vector<MRef<Certificate*> > CacheManager::findCertificates(const std::string searchText, const std::string issuer, const std::string defaultSet) {
	mdbg("ucd") << "^^^ Start of " << __FUNCTION__ << std::endl;

	std::vector<MRef<CertificateSetItem*> > tempRes;
	std::vector<MRef<Certificate*> > res;
	std::vector<MRef<CertificateSetItem*> >::iterator iRes;

	if (defaultSet.length() == 0) {
		// Scan all directory sets
		for (std::map<std::string, MRef<CertificateSet*> >::iterator i = certificateSets.begin(); i != certificateSets.end(); i++) {
			tempRes = i->second->findItems(searchText, issuer);
			if (!tempRes.empty()) {
				for (iRes = tempRes.begin(); iRes != tempRes.end(); iRes++) {
					res.push_back((*iRes)->getCertificate());
				}
			}
		}

	} else {
		// Scan only one directory set, the one mentioned in the function parameters.
		if (certificateSets.find(defaultSet) != certificateSets.end()) {
			tempRes = certificateSets[defaultSet]->findItems(searchText, issuer);
			if (!tempRes.empty()) {
				for (iRes = tempRes.begin(); iRes != tempRes.end(); iRes++) {
					res.push_back((*iRes)->getCertificate());
				}
			}
		}
	}
	// Return empty item if no result found
	mdbg("ucd") << "$$$ End of " << __FUNCTION__ << std::endl;
	return res;

	/*
	std::vector<MRef<Certificate*> > res;
	SipUri uri(searchText);
	for (std::vector<MRef<Certificate*> >::iterator i = fakeCache.begin(); i != fakeCache.end(); i++) {
		if (uri.isValid()) {
			if ((*i)->hasAltNameSipUri(searchText) && (*i)->getIssuer() == issuer) {
				res.push_back(*i);
			}
		} else {
			if ((*i)->getName() == searchText && (*i)->getIssuer() == issuer) {
				res.push_back(*i);
			}
		}
	}
	return res;
	*/
}

void CacheManager::addCertificateSet(const MRef<CertificateSet*> certSet, const std::string setKey) {
	if (certificateSets.find(setKey) == certificateSets.end()) {
		certificateSets[setKey] = CertificateSet::create();
	}
	certificateSets[setKey] = certSet;
}

std::string CacheManager::addCertificate(const MRef<Certificate*> cert, std::string setKey) {
	mdbg("ucd") << "^^^ Start of " << __FUNCTION__ << std::endl;
	if (0 == setKey.length()) {
		setKey = getNewCertificateSetKey();
	}
	if (certificateSets.find(setKey) == certificateSets.end()) {
		certificateSets[setKey] = CertificateSet::create();
	}
	certificateSets[setKey]->addCertificate(cert);
	mdbg("ucd") << "$$$ End of " << __FUNCTION__ << std::endl;
	return setKey;
}


bool CacheManager::findCertsFailedBefore(const std::string searchText, const std::string issuer) {
	for (std::list<MRef<CertFindSettings*> >::iterator i = failedCertSearches.begin(); i != failedCertSearches.end(); i++) {
		if ((*i)->searchText == searchText && (*i)->issuer == issuer) {
			mdbg("ucd") << "$$$ End of " << __FUNCTION__ << std::endl;
			return true;
		}
	}
	mdbg("ucd") << "$$$ End of " << __FUNCTION__ << std::endl;
	return false;
}
void CacheManager::addFindCertsFailed(const std::string searchText, const std::string issuer) {
	failedCertSearches.push_back(MRef<CertFindSettings*>(new CertFindSettings(searchText, issuer)));
	mdbg("ucd") << ">>> Look-up failure using {" << searchText << ", " << issuer << "}" << std::endl;
}
