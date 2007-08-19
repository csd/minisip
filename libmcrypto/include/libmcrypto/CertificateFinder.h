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

#ifndef _CERTIFICATEFINDER_H_
#define _CERTIFICATEFINDER_H_

#include <libmcrypto/config.h>

#include <libmutil/MemObject.h>
#include <libmutil/Timestamp.h>
#include <libmcrypto/cert.h>
#include <libmnetutil/Downloader.h>
#include <libmnetutil/LdapDownloader.h>
#include <libmcrypto/CacheManager.h>

#include <string>
#include <vector>

#ifndef MAX_EFFORT
#define MAX_EFFORT -1
#endif

/* Don't cache any certificates, don't even try to use the cache */
#define CERTCACHEUSE_NONE 0
/* Cache only certificates in the path, no irrelevant certificates */
#define CERTCACHEUSE_LOW 1
/* Cache all downloaded certificates */
#define CERTCACHEUSE_NORMAL 2
/* The constant used to determine the cache level */
#define USE_CERTIFICATE_CACHE CERTCACHEUSE_NORMAL

#define USE_FINDCERTSFAILED_CACHE 1

class LIBMCRYPTO_API CertificateFinderStats : public MObject {
	public:

		CertificateFinderStats() : ldapQueries (0),
					ldapQueriesNoResult (0),
					ldapQueriesNoDirectory (0),
					ldapCertsDownloaded (0),
					dnsQueries (0),
					dnsQueriesNoResult (0),
					dnsSrvQueries (0),
					dnsSrvQueriesNoResult (0),
					cacheQueries (0),
					cacheQueriesNoResult (0),
					certsProcessed (0),
					certsUseful (0)
		{ }

		int ldapQueries;
		int ldapQueriesNoResult;
		int ldapQueriesNoDirectory;
		int ldapCertsDownloaded;
		int dnsQueries;
		int dnsQueriesNoResult;
		int dnsSrvQueries;
		int dnsSrvQueriesNoResult;
		int cacheQueries;
		int cacheQueriesNoResult;
		int certsProcessed;
		int certsUseful;
		Timestamp ts;
};

/**
 * Finds certificates mathching certain subjects and issuers.
 */
class LIBMCRYPTO_API CertificateFinder : public MObject {

	public:
		CertificateFinder();
		CertificateFinder(MRef<CacheManager*> cm);

		void setStatsObject(CertificateFinderStats * stats) {
			this->stats = stats;
		}

		/**
		* Using all the other functions of this class
		*/
		std::vector<MRef<Certificate*> > find(const std::string subjectUri, MRef<Certificate*> curCert, int & effort, const bool typeCrossCert);

		/**
		* Using local certificate cache
		*/
		//std::vector<MRef<Certificate*> > findCache(const std::string subjectUri, const std::string issuerUri);

		/**
		* Using LDAP
		*/
		//std::vector<MRef<Certificate*> > findSubjectInfoAccess(const std::string subjectUri, const std::string issuer, const std::string siaUrl, const bool typeCrossCert);

		/**
		* Using LDAP
		*/
		//std::vector<MRef<Certificate*> > findDnsSrv(const std::string subjectUri);

		/**
		* Using LDAP
		*/
		//std::vector<MRef<Certificate*> > findDnsGuessing(const std::string subjectUri);

		void setAutoCacheCerts(const bool value);

		bool getAutoCacheCerts() const;
		/**
		 * Get the (first found) domain name specified in the subjectAltName extensions of certificate \p cert.
		 *
		 * Handles both DNS names (obviously) and URIs of SIP type.
		 */
		std::string getSubjectDomain(MRef<Certificate*> cert);

	private:
		std::vector<MRef<Certificate*> > downloadFromLdap(const LdapUrl & url, const std::string sipUri, const std::string issuer, const bool typeCrossCert);

		MRef<CacheManager*> cacheManager;
		bool autoAddToCache;

		CertificateFinderStats * stats;
};
#endif
