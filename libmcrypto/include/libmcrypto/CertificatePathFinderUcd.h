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

#ifndef _CERTIFICATEPATHFINDERUCD_H_
#define _CERTIFICATEPATHFINDERUCD_H_

#include <libmcrypto/config.h>

#include <libmutil/MemObject.h>
#include <libmutil/Timestamp.h>
#include <libmcrypto/cert.h>
#include <libmcrypto/CertificateFinder.h>
#include <libmcrypto/CacheManager.h>

#include <vector>
#include <string>

#ifndef MAX_EFFORT
#define MAX_EFFORT -1
#endif

class LIBMCRYPTO_API CertificatePathFinderUcd : public MObject {
	public:
		CertificatePathFinderUcd(MRef<CacheManager*> cm);
		~CertificatePathFinderUcd();

		/**
		 * Tries to find a path to certificate \p toCert.
		 *
		 * The function two stages:
		 *  - Prepare/validate: Checks if all the necessary information can be derived from the function parameters
		 *    as well as testing if \p curPath already specifies a correct path to \p toCert.
		 *  - Find: The actual process of acquiring the missing certificates.
		 *
		 * The find stage should probably be described a bit better:
		 *
		 *  - First: check if we are looking for a down certificate or not. If not, first look for cross
		 *    certificates and then for down certificates.
		 *
		 *  - Second: Then the "direction" has been determined it is time to actually find the certificates
		 *    in "that direction". This is done by calling findCrossCerts, findUpCerts or findDownCerts and
		 *    all those functions return lists of certificates that have "appropriate subjectAltNames".
		 *
		 *  - Third: Since the second step returns certificates with the correct subjects we can be pretty
		 *    sure that they are "good and useful" certificates, but we cannot be sure. The returned
		 *    certificate can have expired or have subjectInfoAccess values that point to non-existing
		 *    directories. It is therefor necessary to, for each of the returned certificates, recursively
		 *    call this function (findUcdPath) with the newest/latest certificate as the last item in
		 *    \p curPath. This way we test each possible path in a sequential manner.
		 *
		 *    If a path turns out to be a dead end \c findUcdpath returns an empty list (this is done in the
		 *    second step) and the function that called \c findUcdPath can "react to the empty list" by
		 *    persuing a different path.
		 *
		 * @param	curPath		Vector containing the user's own certificates (the start of the chain).
		 * @param	toCert		The certificate that the algorithm should find a path to.
		 */
		//std::vector<MRef<Certificate*> > findUcdPath(std::vector<MRef<Certificate*> > curPath, MRef<Certificate*> toCert);
		MRef<CertificateChain*> findUcdPath(MRef<CertificateChain*> curPath, MRef<CertificateSet*> & rootCerts, MRef<Certificate*> & toCert);

		MRef<CertificateChain*> findUcdPath(MRef<Certificate*> selfCert, MRef<Certificate*> upCert, MRef<Certificate*> toCert);

		/**
		 * Prints statistics for the current CertificatePathFinderUcd instance.
		 *
		 * Note that the statistics are accumulated from all calls to findUcdPath, not just the last one.
		 */
		void printStats(std::string prefix, std::string timeStampFile = "");
	private:
		/**
		 * Locates cross certificate by first calling \c candidateCrossPaths and the \c findCerts.
		 */
		std::vector<MRef<Certificate*> > findCrossCerts	(MRef<Certificate*> curCert, MRef<Certificate*> toCert, int& crossEffort,	int& findEffort);
		/**
		 * Locates cross certificate by first calling \c candidateUpPaths and the \c findCerts.
		 */
		std::vector<MRef<Certificate*> > findUpCerts	(MRef<Certificate*> curCert, MRef<Certificate*> toCert,	int& upEffort, 		int& findEffort);
		/**
		 * Locates cross certificate by first calling \c candidateDownPaths and the \c findCerts.
		 */
		std::vector<MRef<Certificate*> > findDownCerts	(MRef<Certificate*> curCert, MRef<Certificate*> toCert, int& downEffort, 	int& findEffort);

		/**
		 * Tries to find a certificate with one of the strings in \p candidates as the subjectAltName and
		 * the DN of \p curCert as the issuer. The items in \p candidates are tested in order (the first
		 * item in this list should therefor specify the "best possible option", i.e. the subjectAltName
		 * that we would like to find the most).
		 *
		 * @param	candidates	List of subjectAltNames that we are looking for.
		 * @param	curCert		The certificate of the issuer (i.e. the \em subject of this certificate
		 * 				must be the \em issuer of the certificates we are looking for.
		 * @param	phaseEffort	The position in \p candidates where the search should begin (if this
		 * 				parameter is \c 1 then the first item/name of \p candidates will not
		 * 				be searched for).
		 * @param	findEffort	???
		 */
		std::vector<MRef<Certificate*> > findCerts	(std::vector<std::string> candidates, MRef<Certificate*> curCert, MRef<Certificate*> toCert, int& phaseEffort, int& findEffort);

		/**
		 * Returns list with the subjectAltNames of \p toCert and the \em probable subjectAltNames of
		 * any CA that is a parent of \p toCert.
		 */
		std::vector<std::string> candidateCrossPaths(MRef<Certificate*> toCert);

		/**
		 * Returns list of the \em probable subjectAltNames of the CAs that are above \p toCert in the hierarchy.
		 */
		std::vector<std::string> candidateUpPaths(MRef<Certificate*> curCert, MRef<Certificate*> toCert);

		/**
		 * Returns list of the \em probable subjectAltNames of the CAs between \p curCert and \p toCert. The returned
		 * list also includes the subjectAltName of \p toCert.
		 */
		std::vector<std::string> candidateDownPaths(MRef<Certificate*> curCert, MRef<Certificate*> toCert);

		/**
		 * Validates whether or not the last certificate in \p certList has been issued by the owner of the
		 * second-to-last certificate in the list.
		 */
		bool verifyLastPair(std::vector<MRef<Certificate*> > & certList);

		MRef<CertificateFinder*> certFinder;
		CertificateFinderStats* stats;
};

#endif
