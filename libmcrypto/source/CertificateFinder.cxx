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
 * Authors: Mikael Svensson, Erik Eliasson
*/

#include <config.h>
#include <libmcrypto/CertificateFinder.h>

#include <libmnetutil/LdapConnection.h>
#include <libmnetutil/LdapUrl.h>
#include <libmnetutil/LdapEntry.h>
#include <libmnetutil/LdapCredentials.h>
#include <libmnetutil/NetworkFunctions.h>

#include <libmutil/SipUri.h>
#include <libmutil/dbg.h>
#include <iostream>

CertificateFinder::CertificateFinder() : stats(NULL) {
}
CertificateFinder::CertificateFinder(MRef<CacheManager*> cm) : cacheManager(cm), stats(NULL) {
}

/**
 * Locates, and downloads if necessary, certificates matching the subject and issuer specified as parameters.
 *
 * The function first tries to find a suitable certificate in the local cache, then moves on to LDAP directories
 * mentioned in the issuer certificates (\p curCert). The point is that the function first tries the simple
 * stuff and then, if that fails, moves on to more advanced and more time-consuming methods.
 *
 * The \p effort parameter determines "the simplicity" of the first "certificate retrieval" method to try.
 * 0 means that the search starts with the local cache.
 *
 * @todo	Implement the certificate cache!
 * @todo	Add support for DNS SRV records.
 * @param	subjectUri	The subjectAltName of the certificate that we are looking for.
 * @param	curCert		Certificate with information about the issuer of the certificate we are looking for.
 * 				This certificate is used to determine the issuer DN of the requested certificate,
 * 				which LDAP server to query for the requested certificate (the subjectInfoAccess
 * 				extension or subjectAltName extension is used for this purpose as it is assumed
 * 				that the issuer also keeps track of its own certificates).
 * @param	effort		Determines at what "level of difficulty" the search should start.
 * @param	typeCrossCert	Determines if the requested certificates is a cross certificate. Note that a better
 * 				description would be "non-user certificates" instead of "cross certificates" as this
 * 				parameter tells other functions if the certificates should be located in some
 * 				inetOrgPerson object or in some certificationAuthority object.
 */
std::vector<MRef<Certificate*> > CertificateFinder::find(const std::string subjectUri, MRef<Certificate*> curCert, int & effort, const bool typeCrossCert) {
	mdbg("ucd") << "^^^ Start of " << __FUNCTION__ << std::endl;

	std::vector<MRef<Certificate*> > ret;

	std::string issuer = curCert->getName();

	/*
	Test to see if this query has failed before. If so, abort immediately.
	*/
	if (USE_FINDCERTSFAILED_CACHE) {
		if (cacheManager->findCertsFailedBefore(subjectUri, issuer)) {
			effort=MAX_EFFORT;
			return ret;
		}
	}
	/*
	Scan the local certificate cache
	*/
	if (effort == 0){
		if (USE_CERTIFICATE_CACHE != CERTCACHEUSE_NONE) {
			stats->cacheQueries++;
			ret = cacheManager->findCertificates(subjectUri, issuer);
			mdbg("ucd") << "    Found certificates in local cache: " << (int)ret.size() << std::endl;
			if (!ret.empty()){
				return ret;
			} else {
				stats->cacheQueriesNoResult++;
				effort = 1;
			}
		} else {
			effort = 1;
		}
	}

	/*
	See if the current certificate has an subjectInfoAccess extension and, if so, try to
	connect to the LDAP server specified in this extension. Once connected we try to
	download a suitable certificate.

	Note that if the certificate should happen to have multiple subjectInfoAccess extensions
	only the first will be used. Reason: simplicity (developer lazyness...)
	*/
	if (effort == 1){
		std::string siaUrl;
		std::vector<std::string> sias = curCert->getSubjectInfoAccess();
		if (!sias.empty()) {
			siaUrl = sias.at(0);

			LdapUrl url(sias.at(0));
			ret = downloadFromLdap(url, subjectUri, issuer, typeCrossCert);
			mdbg("ucd") << "    Found certificates using SIA: " << (int)ret.size() << std::endl;
			if (!ret.empty()) {
				return ret;
			}
		}
		effort = 2;
	}

	/*
	Try to find DNS SRV records specifying LDAP servers in the domain of the issuer.
	*/
	if (effort == 2){

		std::string domain = getSubjectDomain(curCert);
		mdbg("ucd") << "    DNS SRV record search:" << domain << std::endl;
		uint16_t port = 0;
		std::string server=NetworkFunctions::getHostHandlingService("_ldap._tcp",
				domain,port);

		server = "ldap://"+server;
		if (port != 0)
			server = server+":" + itoa(port);

		LdapUrl url(server);
		ret = downloadFromLdap(url, subjectUri, issuer, typeCrossCert);

		mdbg("ucd") << "    Found certificates using SRV: " << (int)ret.size() << std::endl;
		stats->dnsSrvQueries++;

		if (!ret.empty()) {
			stats->dnsSrvQueriesNoResult++;
			return ret;
		}

		effort = 3;
	}

	/*
	If all other options fail: make a wild guesss and try to connect to "ldap.<domain of the issuer>"
	and see if we by any chance get lucky and find ourselves a nice little server!
	*/
	if (effort == 3) {


		/*
		Note: An up-certificate is always issued to a CA, therefore the up-certificate
		will NOT have a SIP URI as the subjectAltName. Assume that the subjectAltName
		contains DNS addresses.
		*/

		std::vector<std::string> curAltNamesDomains = curCert->getAltName(Certificate::SAN_DNSNAME);
		if (curAltNamesDomains.size() > 0) {
			std::string guessName = "";
			guessName = "ldap." + curAltNamesDomains.at(0);

			ret = downloadFromLdap(LdapUrl("ldap://" + guessName), subjectUri, issuer, typeCrossCert);
			mdbg("ucd") << "    Found certificates using domain name guessing (guess:" << guessName << "): " << (int)ret.size() << std::endl;
			if (!ret.empty()) {
				effort = MAX_EFFORT;
				return ret;
			}
		}
	}
	if (USE_FINDCERTSFAILED_CACHE) {
		cacheManager->addFindCertsFailed(subjectUri, issuer);
	}

	effort=MAX_EFFORT;
	return std::vector<MRef<Certificate*> >();
}

/**
 * Using LDAP
 */
/*
std::vector<MRef<Certificate*> > CertificateFinder::findSubjectInfoAccess(const std::string subjectUri, const std::string issuer, const std::string siaUrl, const bool typeCrossCert) {
	mdbg("ucd") << "^^^ Start of " << __FUNCTION__ << std::endl;
	std::vector<MRef<Certificate*> > temp = downloadFromLdap(LdapUrl(siaUrl), subjectUri, issuer, typeCrossCert);
	mdbg("ucd") << "$$$ End of " << __FUNCTION__ << std::endl;
	return temp;
}
*/
/**
 * Generic function for downloading one (or many) certificates from directory \p url. The certificates
 * returned should have \p sipUri as an alternative name and \p issuer as the issuer DN.
 *
 * @param	url		LDAP directory location specifier.
 * @param	sipUri		String that must exist in the subjectAltName extension of returned certificates.
 * 				Note that, despite the parameter's name, it can also be a DNS name.
 * @param	issuer		DN of issuer.
 * @param	typeCrossCert	Set to true if the \em requested certificate is a CA certicate or false if
 * 				it is an end-user certificates.
 */
std::vector<MRef<Certificate*> > CertificateFinder::downloadFromLdap(const LdapUrl & url, const std::string sipUri, const std::string issuer, const bool typeCrossCert) {
	mdbg("ucd") << "^^^ Start of " << __FUNCTION__ << std::endl;

	// Create empty result list
	std::vector<MRef<Certificate*> > res;

	// Input validation!
	if (!url.isValid()) {
		mdbg("ucd") << "$$$ End of " << __FUNCTION__ << std::endl;
		return res;
	}

	mdbg("ucd") << "    Looking for " << (typeCrossCert ? "CA (cross) certificate" : "end-user certificate") << " for " << sipUri << " (directory: " << url.getHost() << ")" << std::endl;

	if (stats != NULL) {
		stats->dnsQueries++;
		stats->ts.save("downloadFromLdap:Main:Start " + url.getHost());
	}

	// Try to connect to LDAP server
	MRef<LdapCredentials*> creds(new LdapCredentials("", ""));
	LdapConnection conn(url.getHost(), creds);
	std::string base;

	if (conn.isConnected(true)) {
		try {
			if (stats != NULL) stats->ldapQueries++;

			mdbg("ucd") << "    Connected" << std::endl;

			// If the supplied LDAP URL does not specify a base DN we must try to find it ourselves
			if (url.getDn().length() == 0)
				base = conn.getBaseDn();
			else
				base = url.getDn();

			std::vector<MRef<LdapEntry*> > result;
			std::vector<MRef<LdapEntry*> >::iterator iter;
			std::vector<std::string> attrs;

			mdbg("ucd") << "    Base: " << base << std::endl;
			try {
				/*
				If we are looking for cross certificates we fetch crossCertifiatePairs from
				certificationAuthority objects, otherwise we retrieve userCertificate attributes
				from inetOrgPerson objects.

				Note that we download ALL cross certificates from the LDAP server when looking
				for CA certificates (up, cross or down certificates), but when downloading
				certificates for end-users we only download the certifiates for the particular
				user we are interested in.

				Hence, searching for end-user certificates is much more bandwidth efficient than
				looking for CA certificates.

				Actually, the reason why searches for end-user certificates are efficient is
				because of the labeledUri attribute: it is an attribute that can store
				any type of URI and we use it to store the subjectAltName of the certificate
				stored in the userCertificate attribute (or rather, we assume that one of the
				labeledUri attributes match the subjectAltName of the userCertificate).
				*/
				if (stats != NULL)
					stats->ts.save("downloadFromLdap:Search:Start");

				if (typeCrossCert) {
					attrs.push_back("crossCertificatePair;binary");
					result = conn.find(base, "(objectClass=certificationAuthority)", attrs);
				} else {
					attrs.push_back("userCertificate;binary");
					result = conn.find(base, "(&(objectClass=inetOrgPerson)(labeledUri="+sipUri+" SIPURI))", attrs);
				}

				if (stats != NULL)
					stats->ts.save("downloadFromLdap:Search:End");

			} catch (LdapException & ex) {
				mdbg("ucd") << "    LdapException: " << ex.what() << std::endl;
			}
			mdbg("ucd") << "    " << (int)result.size() << " entries found" << std::endl;

			if (result.size() == 0)
				if (stats != NULL) stats->ldapQueriesNoResult++;

			for (iter = result.begin(); iter != result.end(); iter++) {
				mdbg("ucd") << "    Found object in LDAP database" << std::endl;
				std::vector<std::string> fileNames;
				std::vector< MRef<LdapEntryBinaryValue*> > certs;

				/*
				Since cross certificates are stored as pairs, and not single certificates,
				it is necessary to "extract" the two certificates from the pair.

				To simplify the rest of the function the two certificates are added to
				the "certs" vector without considering which certificate is the "forward"
				and "reverse" certificate.
				*/
				if (typeCrossCert) {
					std::vector< MRef<LdapEntryBinaryPairValue*> > certPairs;
					std::vector< MRef<LdapEntryBinaryPairValue*> >::iterator pairIter;
					certPairs = (*iter)->getAttrValuesBinaryPairs("crossCertificatePair;binary");
					for (pairIter = certPairs.begin(); pairIter != certPairs.end(); pairIter++) {
						certs.push_back((*pairIter)->first);
						certs.push_back((*pairIter)->second);
					}
				} else {
					certs = (*iter)->getAttrValuesBinary("userCertificate;binary");
				}

				MRef<Certificate*> cert;

				/*
				Load/parse each retrieved certificate and test if they match the conditions.
				*/
				for (size_t x=0; x<certs.size(); x++) {
					MRef<LdapEntryBinaryValue*> val = certs.at(x);
					cert = Certificate::load(reinterpret_cast<unsigned char*>(val->value), val->length);

					if (stats != NULL) stats->certsProcessed++;

					mdbg("ucd") << "    Found binary attribute in LDAP database" << std::endl;
					if (!cert.isNull()) {
						if (stats != NULL) stats->ldapCertsDownloaded++;

						mdbg("ucd") << "    Found certificate in LDAP database" << std::endl;
						mdbg("ucd") << "    What we are looking for:" << std::endl;
						mdbg("ucd") << "        Issuer: " << issuer << std::endl;
						mdbg("ucd") << "        URI: " << sipUri << std::endl;
						mdbg("ucd") << "    What we have:" << std::endl;
						mdbg("ucd") << "        Issuer: " << cert->getIssuer() << std::endl;
						mdbg("ucd") << "        URI in altName: " << cert->hasAltName(sipUri) << std::endl;
						if (cert->getIssuer() == issuer && cert->hasAltName(sipUri)) {
							/*
							Bingo!

							The current certificate has (at least) one matching subjectAltName
							and the correct issuer name. Add the certificate to the result "set".
							*/
							if (stats != NULL) stats->certsUseful++;
							mdbg("ucd") << "        Found MATCHING certificate in LDAP database" << std::endl;
							if (USE_CERTIFICATE_CACHE == CERTCACHEUSE_LOW) {
								std::vector<MRef<Certificate*> > temp = cacheManager->findCertificates(cert->getName(), cert->getIssuer(), CACHEMANAGER_CERTSET_DOWNLOADED);
								if (temp.size() == 0) {
									cacheManager->addCertificate(cert, CACHEMANAGER_CERTSET_DOWNLOADED);
								}
							}
							res.push_back(cert);
						}

						if (USE_CERTIFICATE_CACHE == CERTCACHEUSE_NORMAL) {
							std::vector<MRef<Certificate*> > temp = cacheManager->findCertificates(cert->getName(), cert->getIssuer(), CACHEMANAGER_CERTSET_DOWNLOADED);
							if (temp.size() == 0) {
								cacheManager->addCertificate(cert, CACHEMANAGER_CERTSET_DOWNLOADED);
							}
						}
					}
				}
			}
		} catch (LdapException & ex) {
			mdbg("ucd") << "LdapException: " << ex.what() << std::endl;
		}
	} else {
		if (stats != NULL) stats->ldapQueriesNoDirectory++;
	}

	if (stats != NULL)
		stats->ts.save("downloadFromLdap:Main:End");

	mdbg("ucd") << "$$$ End of " << __FUNCTION__ << std::endl;
	return res;
}

/**
* Using LDAP
*/
//std::vector<MRef<Certificate*> > CertificateFinder::findDnsSrv(const std::string subjectUri);

/**
* Using LDAP
*/
//std::vector<MRef<Certificate*> > CertificateFinder::findDnsGuessing(const std::string subjectUri);

void CertificateFinder::setAutoCacheCerts(const bool value) {
	autoAddToCache = value;
}
bool CertificateFinder::getAutoCacheCerts() const {
	return autoAddToCache;
}
std::string CertificateFinder::getSubjectDomain(MRef<Certificate*> cert) {
	std::vector<std::string> curAltNames = cert->getAltName(Certificate::SAN_URI);
	if (curAltNames.size() > 0) {
		// First try to determine the "current domain" by analyzing the subjectAltNames and assuming that the "current certificate" is an end-user certificate
		for (std::vector<std::string>::iterator nameIter = curAltNames.begin(); nameIter != curAltNames.end(); nameIter++) {
			SipUri uri(*nameIter);
			if (uri.isValid()) {
				return uri.getIp();
			}
		}

	} else {
		// No SIP URIs were found in the subjectAltNames. Try looking for DNS names instead (i.e. assume that the current certificate is a CA certificate instead of an end-user certificate)
		curAltNames = cert->getAltName(Certificate::SAN_DNSNAME);
		if (curAltNames.size() > 0)
			return curAltNames.at(0);
	}
	return "";
}
