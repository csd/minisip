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
#include <libminisip/config/CertificatePathFinderUcd.h>
#include <libmsip/SipUri.h>

#include <iostream>

CertificatePathFinderUcd::CertificatePathFinderUcd(MRef<CacheManager*> cm) : stats (new CertificateFinderStats()) {
	std::cerr << "^^^ Start of " << __FUNCTION__ << std::endl;
	certFinder = MRef<CertificateFinder*>(new CertificateFinder(cm));
	certFinder->setStatsObject(stats);
	std::cerr << "$$$ End of " << __FUNCTION__ << std::endl;
}
CertificatePathFinderUcd::~CertificatePathFinderUcd() {
	std::cerr << "^^^ Start of " << __FUNCTION__ << std::endl;
	delete stats;
	std::cerr << "$$$ End of " << __FUNCTION__ << std::endl;
}
std::vector<MRef<Certificate*> > CertificatePathFinderUcd::findUcdPath(std::vector<MRef<Certificate*> > curPath, MRef<Certificate*> toCert) {
	std::cerr << "^^^ Start of " << __FUNCTION__ << std::endl;

	std::vector<MRef<Certificate*> > res;
	if (curPath.size() == 0)
		return res;

	// Choose first subjectAltName that is a valid SIP URI
	std::cerr << "    Pick out SIP URIs (or DNS names) from subjectAltName" << std::endl;

	MRef<Certificate*> curCert = curPath.back();

	stats->ts.save("findUcdPath:Main:Start");

	// Get list of alternative names for the last certifiate in the chain (the target of the search)
	std::vector<std::string> toAltNames = toCert->getAltName(Certificate::SAN_URI);
	if (toAltNames.size() == 0)
		toAltNames = toCert->getAltName(Certificate::SAN_DNSNAME);

	// Get list of alternative names for the current certificate in the chain (the certificate closest to the target)
	std::vector<std::string> lastAltNames = curCert->getAltName(Certificate::SAN_URI);
	if (lastAltNames.size() == 0)
		lastAltNames = curCert->getAltName(Certificate::SAN_DNSNAME);

	// Convert the alternative names to SIP URIs. The alternative names are often *not* SIP URIs but rather DNS names.
	// This, however, does not pose a problem since the SipUri class (for some reason) accepts DNS names as valid
	// SIP URIs. If this behaviour changes this function will have to be modified...

	SipUri toUri, curUri;
	for (std::vector<std::string>::iterator nameIter = toAltNames.begin(); nameIter != toAltNames.end(); nameIter++) {
		SipUri uri(*nameIter);
		if (uri.isValid()) {
			toUri = uri;
			break;
		}
	}
	for (std::vector<std::string>::iterator nameIter = lastAltNames.begin(); nameIter != lastAltNames.end(); nameIter++) {
		SipUri uri(*nameIter);
		if (uri.isValid()) {
			curUri = uri;
			break;
		}
	}
	if (!curUri.isValid() || !toUri.isValid()) {
		std::cerr << "$$$ End of " << __FUNCTION__ << std::endl;
		stats->ts.save("findUcdPath:Main:End");
		return res;
	}

	std::cerr << "    Found SIP URIs:" << std::endl;
	std::cerr << "        curUri=" << curUri << std::endl << "        toUri=" << toUri << std::endl;

	// Test if the last certificate in the chain can be verified using the second-to-last certificates.
	// If that cannot be done we abort the search as the chain is broken. An empty list is returned
	// to signify this fact.

	if (!verifyLastPair(curPath)) {
		std::cerr << "$$$ End of " << __FUNCTION__ << std::endl;
		stats->ts.save("findUcdPath:Main:End");
		return res;
	}

	// Test if the subject name of the last found certificate matches the issuer name
	// of the certificate that we are trying to get to. If so, we have found a (possible)
	// path and we must only verify it before we can return the entire chain to the user!

	if (toCert->getIssuer() == curCert->getName() ){
		curPath.push_back(toCert);
		if (verifyLastPair(curPath)) {
			// Bingo!
			std::cerr << "$$$ End of " << __FUNCTION__ << std::endl;
			stats->ts.save("findUcdPath:Main:End");
			return curPath;
		} else {
			std::cerr << "$$$ End of " << __FUNCTION__ << std::endl;
			stats->ts.save("findUcdPath:Main:End");
			return std::vector<MRef<Certificate*> >();
		}
	}


	// List with certificates that *probably* can be in the final chain/path
	std::vector<MRef<Certificate*> > nextCertCandidates;
	std::vector<MRef<Certificate*> >::iterator i;

	if (stringEndsWith(toUri.getIp(), curUri.getIp())) {
        	//DOWN mode
		int32_t downEffort = 0;
		int32_t findEffort = 0;
		//do {
			/**
			 *
			 *
			 *
			 *
			 *
			 * Infinite-loop warning: what happens if findUcdPath() returns an empty list?
			 * When, and where, are downEffort and findEffort increased?
			 *
			 *
			 *
			 *
			 *
			 */

			downEffort = 0;
			findEffort = 0;
			nextCertCandidates = findDownCerts(curCert, toCert, downEffort, findEffort);
			// Try each certificate that was returned from findDownCerts and recursively
			// see if it fits the path. This means that the algorithm is a DEPTH-FIRST alg.

			for (i = nextCertCandidates.begin(); i != nextCertCandidates.end(); i++) {

				std::cerr << "    DOWN-mode testing with " << (*i)->getCn() << " as last node in chain." << std::endl;

				std::vector<MRef<Certificate*> > testPath = curPath;
				testPath.push_back(*i);
				std::vector<MRef<Certificate*> > retPath = findUcdPath(testPath, toCert);
				if (!retPath.empty()) {
					stats->ts.save("findUcdPath:Main:End");
					std::cerr << "$$$ End of " << __FUNCTION__ << std::endl;
					return retPath;
				}
			}
		//} while ( ! (downEffort==MAX_EFFORT && findEffort==MAX_EFFORT) );
	} else {
        	//UP-CROSS mode
		int32_t crossEffort = 0;
		int32_t findEffort = 0;
		//do {
			crossEffort = 0;
			findEffort = 0;
			nextCertCandidates = findCrossCerts(curCert, toCert, crossEffort, findEffort);

			for (i = nextCertCandidates.begin(); i != nextCertCandidates.end(); i++) {

				std::cerr << "    CROSS-mode testing with " << (*i)->getCn() << " as last node in chain." << std::endl;

				std::vector<MRef<Certificate*> > testPath = curPath;
				testPath.push_back(*i);
				std::vector<MRef<Certificate*> > retPath = findUcdPath(testPath, toCert);
				if (!retPath.empty()) {
					std::cerr << "$$$ End of " << __FUNCTION__ << std::endl;
					stats->ts.save("findUcdPath:Main:End");
					return retPath;
				}
			}
		//} while ( ! (crossEffort==MAX_EFFORT && findEffort==MAX_EFFORT) );

		int32_t upEffort = 0;
		//do {
			upEffort = 0;
			findEffort = 0;
			nextCertCandidates = findUpCerts(curCert, toCert, upEffort, findEffort);
			for (i = nextCertCandidates.begin(); i != nextCertCandidates.end(); i++) {

				std::cerr << "    UP-mode testing with " << (*i)->getCn() << " as last node in chain." << std::endl;

				std::vector<MRef<Certificate*> > testPath = curPath;
				testPath.push_back(*i);
				std::vector<MRef<Certificate*> > retPath = findUcdPath(testPath, toCert);
				if (!retPath.empty()) {
					stats->ts.save("findUcdPath:Main:End");
					std::cerr << "$$$ End of " << __FUNCTION__ << std::endl;
					return retPath;
				}
			}
		//} while ( ! (upEffort==MAX_EFFORT && findEffort==MAX_EFFORT) );
	}
	std::cerr << "$$$ End of " << __FUNCTION__ << std::endl;
	stats->ts.save("findUcdPath:Main:End");
	return std::vector<MRef<Certificate*> >();
}

std::vector<MRef<Certificate*> > CertificatePathFinderUcd::findCrossCerts	(MRef<Certificate*> curCert, MRef<Certificate*> toCert, int& crossEffort,	int& findEffort) {
	std::cerr << "^^^ Start of " << __FUNCTION__ << std::endl;

	std::vector<std::string> candidates = candidateCrossPaths(toCert);
	std::vector<MRef<Certificate*> > temp = findCerts(candidates, curCert, toCert, crossEffort, findEffort);

	std::cerr << "$$$ End of " << __FUNCTION__ << std::endl;
	return temp;
}
std::vector<MRef<Certificate*> > CertificatePathFinderUcd::findUpCerts		(MRef<Certificate*> curCert, MRef<Certificate*> toCert,	int& upEffort, 	int& findEffort) {
	std::cerr << "^^^ Start of " << __FUNCTION__ << std::endl;

	std::vector<std::string> candidates = candidateUpPaths(curCert, toCert);
	std::vector<MRef<Certificate*> > temp = findCerts(candidates, curCert, toCert, upEffort, findEffort);

	std::cerr << "$$$ End of " << __FUNCTION__ << std::endl;
	return temp;
}
std::vector<MRef<Certificate*> > CertificatePathFinderUcd::findDownCerts	(MRef<Certificate*> curCert, MRef<Certificate*> toCert, int& downEffort, 	int& findEffort) {
	std::cerr << "^^^ Start of " << __FUNCTION__ << std::endl;

	std::vector<std::string> candidates = candidateDownPaths(curCert, toCert);
	std::vector<MRef<Certificate*> > temp = findCerts(candidates, curCert, toCert, downEffort, findEffort);

	std::cerr << "$$$ End of " << __FUNCTION__ << std::endl;
	return temp;
}

std::vector<MRef<Certificate*> > CertificatePathFinderUcd::findCerts	(std::vector<std::string> candidates, MRef<Certificate*> curCert, MRef<Certificate*> toCert, int& phaseEffort, int& findEffort) {
	std::cerr << "^^^ Start of " << __FUNCTION__ << std::endl;

	stats->ts.save("findCerts:Main:Start");

	do {
		/*
		If the list of candidates is empty or if the "effort level" (which specifies
		which of the candidates that should be tested first) exceed the number of
		candidates then we should abort the function because there is nothing more to be done.
		*/
		if (candidates.empty() || phaseEffort >= candidates.size()) {
			phaseEffort = MAX_EFFORT;
			findEffort = MAX_EFFORT;
			break;
		}

		/*
		There at least one candidate that has not yet been treid...
		*/
		if (phaseEffort != MAX_EFFORT) {
			/*
			Try to find a certificate issued TO the current candidate, issued BY the
			"currenct certificates subject". The current certificate can be any certificate, in theory,
			but is in actuality always the last found certificate in the chain.
			*/
			std::vector<MRef<Certificate*> > foundCerts = certFinder->find(candidates.at(phaseEffort), curCert, findEffort, true);
			if (!foundCerts.empty()) {
				// If any certificate where found we return them and feel happy about it!
				std::cerr << "$$$ End of " << __FUNCTION__ << std::endl;
				stats->ts.save("findCerts:Main:End");
				return foundCerts;
			}
		}

		/*
		If we didn't find any certificates we must increase our efforts by advancing to
		the next candidate in the list. Since we are changing the candidate we also reset
		the "find effort" so that the next search start all over again by looking in the
		local certificate cache (instead of using whatever method was successful the last
		time).

		Increasing the effort (the "phase effort", that is) usually means jumping a step
		in the domain hierarchy.
		*/
		if (findEffort==MAX_EFFORT && phaseEffort!=MAX_EFFORT){
			phaseEffort++;
			findEffort = 0;
		}

		// Will the loop EVER break using this condition????
	} while (! (phaseEffort==MAX_EFFORT && findEffort==MAX_EFFORT));

	std::cerr << "$$$ End of " << __FUNCTION__ << std::endl;
	stats->ts.save("findCerts:Main:End");
	return std::vector<MRef<Certificate*> >();
}


std::vector<std::string> CertificatePathFinderUcd::candidateUpPaths(MRef<Certificate*> curCert, MRef<Certificate*> toCert) {
	std::cerr << "^^^ Start of " << __FUNCTION__ << std::endl;
	std::vector<std::string> tempCurrent = candidateCrossPaths(curCert);
	std::vector<std::string> tempTo = candidateCrossPaths(toCert);

	/*
	candidateCrossPaths returns the parents as well as the child itself. This is useless when looking for
	up-certificates as we are not interested if KTH.SE has an up-certificate to KTH.SE.

	This is best examplified by looking at the (next) example: If curCert.domain is "ucd-fast.ssvl.kth.se"
	then that name will be returned from candidateCrossPaths as well, but it is useless to search for
	up-certificates to "ucd-fast.ssvl.kth.se" in the directory of "ucd-fast.ssvl.kth.se" -- we are only
	interested in what lies *above* curCert.domain.
	*/

	std::cerr << "    tempCurrent.size()=" << tempCurrent.size() << ", tempTo.size()=" << tempTo.size() << std::endl;

	if (tempCurrent.size() > 0)
		tempCurrent.erase(tempCurrent.begin());
	if (tempTo.size() > 0)
		tempTo.erase(tempTo.begin());

	/*
	Remove suggested paths/URIs that both candidateCrossPath(curCert) and candidateCrossPath(toCert) have in common.
	Perhaps an example will explain this better:

	If curCert.domain is "ucd-fast.ssvl.kth.se" and toCert.domain is "ssvl.kth.se"
	then candidateCrossPath(curCert) will return all these domains:
	 - ucd-fast.ssvl.kth.se
	 - ssvl.kth.se
	 - kth.se

	But looking for up-certificates for kth.se would be useless since kth.se has not been involved
	in any of the two certificates that we are concerned about. This function should therefore remove
	the kth.se entry from the result before returning.
	*/
	if (!tempCurrent.empty() && !tempTo.empty()) {
		while (tempCurrent.back() == tempTo.back()) {
			tempCurrent.pop_back();
			tempTo.pop_back();
		}
	}

	std::cerr << "$$$ End of " << __FUNCTION__ << std::endl;
	return tempCurrent;
}

/**
 * @note	What happens if a certificate contains multiple subjectAltNames URIs which all
 * 		point to the same domain (this function will resturn duplicates in the result...)
 */
std::vector<std::string> CertificatePathFinderUcd::candidateCrossPaths(MRef<Certificate*> toCert) {
	std::cerr << "^^^ Start of " << __FUNCTION__ << std::endl;

	/*
	Get list of alternative names of the intended target. Since the target can be both a
	CA and end-user the alternative names can be either a DNS name or and URI.
	*/
	std::vector<std::string> altNames = toCert->getAltName(Certificate::SAN_URI);
	if (altNames.size() == 0) {
		altNames = toCert->getAltName(Certificate::SAN_DNSNAME);
	}
	std::vector<std::string>::iterator nameIter;
	std::vector<std::string> resDomains;

	std::cerr << "    Certificate belonging to " << toCert->getCn() << " has " << altNames.size() << " subjectAltNames" << std::endl;
	/*
	For each of the alt. names we calculate all possible "parent name". Note that it is
	VERY unlikely that a CA certificate has multiple alternative names, an end-user may
	on the other hand have several names specified. Different names for different services,
	and one of those services might be up-cross-down certificate retrieval in SIP!

	Note that duplicates are not eliminated: If a user has several alternative names, and
	all of them belong to the same domain (e.g. "sip:mikael@kth.se" and "mail:mikael@kth.se"),
	then the returned list will contain duplicates (e.g. {"kth.se", "kth.se"}).
	*/
	for (nameIter = altNames.begin(); nameIter != altNames.end(); nameIter++) {
		SipUri uri(*nameIter);
		std::cerr << "    Processing URI " << (*nameIter) << (uri.isValid() ? " (valid)" : " (NOT valid)") << std::endl;

		/*
		The SipUri class, for some reason, accepts DNS names as valid SIP URIs. This behavious
		seems odd but is useful in this context: we can test for valid alternative names without
		consideration of that type of name we are testing.
		*/
		if (uri.isValid()) {
			/*
			Since we are looking for cross-paths we should always include the target
			itself in the list of possible cross-certifcate subjects.
			*/
			if (uri.getUserName().length() > 0)
				resDomains.push_back("sip:" + uri.getUserIpString());

			std::string host = uri.getIp();

			if (host.length() > 0) {
				std::string::size_type pos = 0;
				do {
					std::string newDomain = host.substr(pos);

					resDomains.push_back(newDomain);

					pos = host.find('.', pos)+1;
				} while (pos != std::string::npos+1);

				// Remove last entry if it doesn't contain a dot (as is the case with "org", "se" and "com")
				if (resDomains.back().find('.',0) == std::string::npos)
					resDomains.pop_back();
			}
		}
	}
	std::cerr << "$$$ End of " << __FUNCTION__ << std::endl;
	return resDomains;
}

std::vector<std::string> CertificatePathFinderUcd::candidateDownPaths(MRef<Certificate*> curCert, MRef<Certificate*> toCert) {
	std::cerr << "^^^ Start of " << __FUNCTION__ << std::endl;

	std::vector<std::string> toAltNames = toCert->getAltName(Certificate::SAN_URI);

	/*
	When looking for down-certificates we must know where in the hierarchy
	we are right now (so that we don't accidentally "mark" a parent as a child).
	*/
	std::string curDomain = getSubjectDomain(curCert);

	std::vector<std::string> resDomains;

	if (curDomain.length() > 0) {
		for (std::vector<std::string>::iterator nameIter = toAltNames.begin(); nameIter != toAltNames.end(); nameIter++) {
			SipUri uri(*nameIter);
			std::cerr << "    Testing subjectAltName " << *nameIter << " and extracting domain names:" << std::endl;
			std::cerr << "        uri.isValid() = " << uri.isValid() << ", stringEndsWith(uri.getIp(), curDomain) = " << stringEndsWith(uri.getIp(), curDomain) << std::endl;

			/*
			Test if the current alt. name is a proper one AND that it represents
			an entity further down in the hierarchy. The latter is tested simply
			by checking what text the alternative name ends with.
			*/
			if (uri.isValid() && stringEndsWith(uri.getIp(), curDomain)) {
				std::string host = uri.getIp();
				std::string::size_type pos = 0;
				while (pos != std::string::npos) {
					std::string newDomain = host.substr(pos);
					if (newDomain == curDomain)
						break;

					resDomains.push_back(newDomain);
					std::cerr << "    candidateDownPath: " << newDomain << std::endl;

					pos = host.find('.', pos)+1;
				}
			}
		}
	}
	std::cerr << "$$$ End of " << __FUNCTION__ << std::endl;
	return resDomains;
}

/**
 * @todo	Implement the function!
 */
bool CertificatePathFinderUcd::verifyLastPair(std::vector<MRef<Certificate*> > & certList) {
	std::cerr << "^^^ Start of " << __FUNCTION__ << std::endl;
	std::cerr << "$$$ End of " << __FUNCTION__ << std::endl;
	return true;
}

void CertificatePathFinderUcd::printStats(std::string prefix, std::string timeStampFile) {
	if (stats != NULL) {
		std::cout << prefix << "cacheQueries:            " << stats->cacheQueries << std::endl;
		std::cout << prefix << "cacheQueriesNoResult:    " << stats->cacheQueriesNoResult << std::endl;
		std::cout << prefix << "dnsQueries:              " << stats->dnsQueries << std::endl;
		std::cout << prefix << "ldapQueries:             " << stats->ldapQueries << std::endl;
		std::cout << prefix << "ldapQueriesNoResult:     " << stats->ldapQueriesNoResult << std::endl;
		std::cout << prefix << "ldapQueriesNoDirectory:  " << stats->ldapQueriesNoDirectory << std::endl;
		std::cout << prefix << "ldapCertsDownloaded:     " << stats->ldapCertsDownloaded << std::endl;
		//std::cout << "dnsQueriesNoResult:      " << stats->dnsQueriesNoResult << std::endl;
		//std::cout << "dnsSrvQueries:           " << stats->dnsSrvQueries << std::endl;
		//std::cout << "dnsSrvQueriesNoResult:   " << stats->dnsSrvQueriesNoResult << std::endl;
		//std::cout << "certsProcessed:          " << stats->certsProcessed << std::endl;
		//std::cout << "certsUseful:             " << stats->certsUseful << std::endl;
		if (timeStampFile.length() > 0) {
			stats->ts.print(timeStampFile);
		}
	} else {
		std::cout << "No stats collected." << std::endl;
	}
}
std::string CertificatePathFinderUcd::getSubjectDomain(MRef<Certificate*> cert) {
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
