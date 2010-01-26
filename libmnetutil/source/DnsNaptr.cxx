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

#include<config.h>
#include<libmnetutil/DnsNaptr.h>
#include<list>
#include<udns.h>
#ifdef HAVE_REGEX_H
#include<regex.h>
#endif
#include<algorithm>
#include<string.h>
#include<stdio.h>

using namespace std;

//#define DEBUG_OUTPUT
//#undef HAVE_POSIX_REGCOMP

DnsNaptrQuery::DnsNaptrQuery()
{
	enumDomains.push_back("e164.arpa");
	enumDomains.push_back("e164.org");
}

DnsNaptrQuery::~DnsNaptrQuery()
{
}

static string num_domain( const string &str ){
	string num = str;

	if( str.at( 0 ) == '+' )
		num.erase( 0, 1 );

	string::reverse_iterator i;
	string::reverse_iterator last = num.rend();

	string domain;

	for( i = num.rbegin(); i != last; i++ ) {
		domain += *i;
		domain += '.';
	}

	return domain;
}


bool DnsNaptrQuery::resolveCommon( const string &domain,
				   const string &target ){
	list<string> acceptServices;
	acceptServices.push_back("E2U+SIP");
	acceptServices.push_back("SIP+E2U");

	setAccept( acceptServices );

	if( !resolve( domain, target ) )
		return false;

	return getResultType() == DnsNaptrQuery::URI;
}

bool DnsNaptrQuery::resolveIsn( const string &isn ){
	static const string prefix = ".freenum.org";

	string::size_type pos;

	pos = isn.find('*');

	if(pos == string::npos)
		return false;

	string exten = isn.substr( 0, pos );
	string itad = isn.substr( pos + 1 );

	string domain = num_domain( exten ) + itad + prefix;

	return resolveCommon( domain, isn );
}

bool DnsNaptrQuery::resolveEnum( const string &e164 ){
	list<string>::const_iterator i;

	for( i = enumDomains.begin(); i != enumDomains.end(); i++ ){
		string enumDomain = *i;

		string domain = num_domain( e164 ) + enumDomain;

		if( resolveCommon( domain, e164 ) )
			return true;
	}

	return false;
}

bool DnsNaptrQuery::resolveSip( const string &domain ){
	list<string> acceptServices;
	acceptServices.push_back("SIPS+D2T");
	acceptServices.push_back("SIP+D2T");
	acceptServices.push_back("SIP+D2U");

	setAccept( acceptServices );

	if( !resolve( domain, domain ) )
		return false;

	return getResultType() == DnsNaptrQuery::SRV;
}

bool DnsNaptrQuery::resolveSips( const string &domain ){
	list<string> acceptServices;
	acceptServices.push_back("SIPS+D2T");

	setAccept( acceptServices );

	if( !resolve( domain, domain ) )
		return false;

	return getResultType() == DnsNaptrQuery::SRV;
}

typedef list<dns_naptr*> NaptrList;

class DnsNaptrQueryPriv: public DnsNaptrQuery 
{
	public:
		DnsNaptrQueryPriv();
		virtual ~DnsNaptrQueryPriv();

		virtual void setAccept( const list<string> &acceptServices );

		virtual ResultType getResultType() const;
		virtual const std::string &getResult() const;
		virtual const std::string &getService() const;

		virtual bool resolve( const std::string &domain,
				      const std::string &target );

	protected:

	private:

		bool calcRegexp( const std::string &regexp );
		bool process( const NaptrList &lst );
		bool dump_naptr( dns_rr_naptr *naptr );

		dns_ctx *ctx;
		dns_rr_naptr *naptr;
		const std::list<std::string> *acceptServices;
		std::string target;
		ResultType resultType;
		std::string result;
		string service;
};



DnsNaptrQuery *DnsNaptrQuery::create()
{
	return new DnsNaptrQueryPriv();
}


DnsNaptrQueryPriv::DnsNaptrQueryPriv()
		:naptr( NULL ), acceptServices( NULL )
{
	ctx = dns_new(NULL);
//   if (argc > 2)
//     dns_add_serv(ctx, argv[2]);
	dns_open(ctx);
}

DnsNaptrQueryPriv::~DnsNaptrQueryPriv()
{
}

void DnsNaptrQueryPriv::setAccept( const list<string> &theAcceptServices )
{
	acceptServices = &theAcceptServices;
}


DnsNaptrQuery::ResultType DnsNaptrQueryPriv::getResultType() const
{
	return resultType;
}

const std::string &DnsNaptrQueryPriv::getResult() const
{
	return result;
}

const std::string &DnsNaptrQueryPriv::getService() const
{
	return service;
}


#ifdef DEBUG_OUTPUT
void dump_srv(dns_rr_srv *srv)
{
	for (int i=0; i < srv->dnssrv_nrr; i++) {
		dns_srv *rr = &srv->dnssrv_srv[i];
		cerr << rr->priority << " "
		     << rr->weight << " " 
		     << rr->port << " \"" 
		     << rr->name << endl;
	}
}

void dump_naptr_entry(dns_naptr *rr)
{
	cerr << rr->order << " "
	     << rr->preference << " \"" 
	     << rr->flags << "\" \"" 
	     << rr->service << "\" \"" 
	     << rr->regexp << "\" " 
	     << rr->replacement << endl;
}

void dump_naptr_list(const NaptrList &lst)
{
	NaptrList::const_iterator last = lst.end();
	NaptrList::const_iterator i;
	for(i = lst.begin(); i != last; i++){
		dump_naptr_entry(*i);
	}
}
#endif

bool dns_naptr_pred( const dns_naptr* lhs, const dns_naptr* rhs )
{
	if( lhs->order != rhs->order ){
		bool res;
		res = lhs->order < rhs->order;
#ifdef DEBUG_OUTPUT
		cerr << "order " << res << endl;
#endif
		return res;
	}

	if( lhs->preference != rhs->preference ){
		bool res;
		res = lhs->preference < rhs->preference;
#ifdef DEBUG_OUTPUT
		cerr << "preference " << res << endl;
#endif
		return res;
	}

	return 0;
}

bool DnsNaptrQueryPriv::calcRegexp( const string &regexp )
{
#ifndef HAVE_POSIX_REGCOMP
	return false;
#else
	static const string back_slash = "\\";
	regex_t preg;
	regmatch_t pmatch[9];

	if( regexp.empty() )
		return false;

	memset(&preg, 0, sizeof(preg));
	memset(pmatch, 0, sizeof(pmatch));

	char c = regexp[0];
	string::size_type pos = regexp.find( c, 1 );
	string::size_type posFlags = regexp.find( c, pos + 1 );

	if( pos == string::npos || posFlags == string::npos ){
		cerr << "Bad regexp" << endl;
		return false;
	}

	string pattern = regexp.substr( 1, pos - 1 );
	result = regexp.substr( pos + 1, posFlags - pos - 1 );
	string flags = regexp.substr( posFlags + 1,
				      regexp.length() - posFlags - 1 );
	// TODO support regexp flags, ie. case flag "i"

#ifdef DEBUG_OUTPUT
	cerr << "Try Regex " << pattern << "," << result << "," << flags << endl;
#endif
	if( regcomp(&preg, pattern.c_str(), REG_EXTENDED) ){
		perror("regcomp");
		return false;
	}

	int res = regexec(&preg, target.c_str(),
			  sizeof(pmatch)/sizeof(pmatch[0]), pmatch, 0);
	if( res ){
		char buf[256]="";
		regerror(res, &preg, buf, sizeof(buf));
		cerr << "Error " << buf << endl;
		return false;
	}


	for( int i = 1; i <= 9; i++ ){
		string::size_type so;
		string tag = back_slash + string( 1, '0' + i );

		if( pmatch[i].rm_so == -1 )
			continue;

		so = result.find( tag );

		if( so == string::npos )
			continue;

		string replacement = target.substr( pmatch[i].rm_so,
						    pmatch[i].rm_eo - pmatch[i].rm_so );

#ifdef DEBUG_OUTPUT
		cerr << "MATCH REGEX " << pmatch[i].rm_so << " " << pmatch[i].rm_eo << "," << replacement << endl;
		cerr << "result " << result << endl;
		int ins_len = replacement.length() - tag.length();
		cerr << "ins_len " << ins_len << endl;

		cerr << "before replace '" << result << "'" << endl;
#endif

		result.replace( so, tag.length(), replacement, 0, replacement.length() );
	}
	
	regfree(&preg);
#ifdef DEBUG_OUTPUT
	cerr << "MATCH REGEX " << result << endl;
#endif

	return true;
#endif	// HAVE_POSIX_REGCOMP
}


bool DnsNaptrQueryPriv::process( const NaptrList &lst )
{
	NaptrList::const_iterator last = lst.end();
	NaptrList::const_iterator i;
	for(i = lst.begin(); i != last; i++){
		dns_naptr *rr = *i;
		bool res;

		if( strlen(rr->replacement) ){
#ifdef DEBUG_OUTPUT
			cerr << "MATCH len " << strlen(rr->replacement) << " " << "'" << rr->replacement << "'" << endl;
#endif
			resultType = NONE;
			result = rr->replacement;
			res = true;
		}
		else{
			string regexp = rr->regexp;

			res = calcRegexp( regexp );
		}

		if( res ){
			switch (toupper(rr->flags[0])) {
				case 'S':
					resultType = SRV;
					break;
				case 'A':
					resultType = ADDR;
					break;
				case 'U':
					resultType = URI;
					break;
			}

			service = rr->service;
			
			transform( service.begin(), service.end(),
				   service.begin(), (int(*)(int))toupper );

#ifdef DEBUG_OUTPUT			       
			dump_naptr_entry(rr);
#endif
			return true;
		}
	}

	return false;
}

bool DnsNaptrQueryPriv::dump_naptr( dns_rr_naptr *naptr )
{
	list<dns_naptr *> lst;

	for (int i=0; i < naptr->dnsnaptr_nrr; i++) {
		bool handle = false;
		dns_naptr *rr = &naptr->dnsnaptr_naptr[i];
#ifdef DEBUG_OUTPUT
		dump_naptr_entry(rr);
#endif

		if (strlen(rr->flags) > 1)
			continue;

		switch (rr->flags[0]) {
			case 'S':
			case 's':
			case 'U':
			case 'u':
//       dns_rr_srv *srv = (dns_rr_srv*)
// 	dns_resolve_p(ctx, rr->replacement, DNS_C_IN, DNS_T_SRV, DNS_NOSRCH,
// 		      dns_parse_srv);

//       dump_srv(srv);
				handle = true;
				break;
			default:
#ifdef DEBUG_OUTPUT
				cerr << "Unhandled" << endl;
#else
				;
#endif
		}

		string service = rr->service;

		transform( service.begin(), service.end(),
			   service.begin(), (int(*)(int))toupper );

		list<string>::const_iterator iter = 
			find(acceptServices->begin(), acceptServices->end(),
			     service);

		if( iter == acceptServices->end() ){
#ifdef DEBUG_OUTPUT
			cerr << "Unhandled " << rr->service << endl;
#endif
			handle = false;
		}


		if( handle )
			lst.push_back(rr);
	}

	lst.sort(dns_naptr_pred);

	bool res = process( lst );

#ifdef DEBUG_OUTPUT
	cerr << endl << "DUMP" << endl;
	dump_naptr_list(lst);
#endif

	return res;
}

bool DnsNaptrQueryPriv::resolve( const string &init_domain,
				 const string &theTarget )
{
	string domain = init_domain;

	target = theTarget;

	for( int max_depth = 10 ; max_depth > 0; max_depth-- ){

		dns_rr_naptr *naptr = NULL;
 
		naptr = dns_resolve_naptr(ctx, domain.c_str(), DNS_NOSRCH);

		if (!naptr) {
			return false;
		}

#ifdef DEBUG_OUTPUT
		cerr << "#records " << naptr->dnsnaptr_nrr << endl;
#endif

		bool res = dump_naptr(naptr);

		free(naptr);

		if( !res )
			return false;

		if ( resultType != 0 ){
#ifdef DEBUG_OUTPUT
			cerr << "resolve " << getResult() << endl;
#endif
			return true;
		}

		domain = result;
	}

	return false;
}
