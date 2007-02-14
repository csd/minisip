/*
  Copyright (C) 2007 Mikael Magnusson

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
 * Authors: Mikael Magnusson <mikma@users.sourceforge.net>
 */

#ifndef LIBMNETUTIL_DNSNAPTR_H
#define LIBMNETUTIL_DNSNAPTR_H

#include<libmnetutil/libmnetutil_config.h>
#include<libmutil/MemObject.h>
#include<list>
#include<string>

/**
 * NAPTR resolver with support for
 * SIP servers (RFC 3263)
 * ENUM (RFC 3761)
 * ISN (http://freenum.org)
 */
class LIBMNETUTIL_API DnsNaptrQuery: public MObject{
	public:
		virtual ~DnsNaptrQuery();

		static DnsNaptrQuery *create();

		enum ResultType {
			NONE = 0,
			SRV,
			ADDR,
			URI
		};

		void setEnumDomains( std::list<std::string> &enumDomains);

		/**
		 * Set acceptable NAPTR services.
		 */
		virtual void setAccept( const std::list<std::string> &acceptServices )=0;
		virtual ResultType getResultType() const=0;
		virtual const std::string &getResult() const=0;
		virtual const std::string &getService() const=0;

		/**
		 * @arg domain  The domain to retrieve NAPTR RRs for
		 * @arg target  The domain, an E.164 telephone number or ISN
		 */
		virtual bool resolve( const std::string &domain,
				      const std::string &target )=0;

		/**
		 * Resolve an ITAD Subscriber Numbers (ISN)
		 * from http://freenum.org/, handles E2U+SIP, and SIP+E2U
		 * @return true if successful, use  getResult() to
		 * get the result
		 */
		bool resolveIsn( const std::string &isn );

		/**
		 * Resolve an E.164 telephone number,
		 * handles E2U+SIP and SIP+E2U
		 * @return true if successful, use  getResult() to
		 * get the result
		 */
		bool resolveEnum( const std::string &e164 );

		/**
		 * Resolve a sip: server domain
		 * Handles SIPS+D2T, SIP+D2T and SIP+D2U
		 * @return true if successful, use  getResult() to
		 * get the result
		 */
		bool resolveSip( const std::string &domain );

		/**
		 * Resolve a sips: server domain, handles SIPS+D2T
		 * @return true if successful, use  getResult() to
		 * get the result
		 */
		bool resolveSips( const std::string &domain );

	protected:
		DnsNaptrQuery();

	private:
		bool resolveCommon( const std::string &domain,
				    const std::string &target );

		std::list<std::string> enumDomains;
};

#endif	// LIBMNETUTIL_DNSNAPTR_H
