/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  
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
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/


#include<config.h>

#include<libmnetutil/NetworkFunctions.h>

#include<iostream>

#include <stdlib.h>
#include <stdio.h>

#ifdef HAVE_LINUX_SOCKIOS_H
#include <linux/sockios.h> /* for SIOCG* */
#endif

#ifdef HAVE_ARPA_INET_H
#	include <arpa/inet.h> /* inet_ntoa */
#	include <sys/types.h> /* socket() */
#	include <sys/socket.h>
#	include <net/if.h>
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <sys/ioctl.h>
#	include<unistd.h>
#endif

#ifdef HAVE_NETDB_H
#include<netdb.h>
#endif
#ifdef HAVE_IFADDRS_H
#include<ifaddrs.h>
#endif
#include<map>
#include<udns.h>


/**
libresolv is only for linux ...
getaddrinfo works for linux and windows ... but in linux it does not correctly do SRV lookups ...and in windows is for xp or higher (and ce)
windns is only for windows ... but it is not restricted to xp or higher ... thus better use it
*/
#if defined _MSC_VER || __MINGW32__ || _WIN32_WCE
#	define USE_WIN32_API
#	ifdef _WIN32_WCE
#		define USE_WIN32_API_GETADDRINFO
#	else
#		define USE_WIN32_API_WINDNS
#	endif
#endif

#ifdef USE_WIN32_API
#	if defined USE_WIN32_API_GETADDRINFO && !_WIN32_WCE
#		define _WIN32_WINNT 0x0501 //eeee ... XP only! ... use if "getaddrinfo" ... with windns, not needed
#	endif
#	include<windows.h>
#	include "iphlpapi.h" //to obtain list of interfaces ...
	//do not use getaddrinfo in linux ... it does not do SRV lookup (as of March 2006) ...
	//in windows, it works fine ... for wce(?); and xp and higher.
#	ifdef USE_WIN32_API_GETADDRINFO
#		ifdef WIN32
#			include<winsock2.h>
#			include<ws2tcpip.h>
#		endif
#	endif
#endif

#include<libmnetutil/NetworkException.h>

#include<libmutil/merror.h>
#include<libmutil/stringutils.h>

#define BUFFER_SIZE 1024 /* bytes */

using namespace std;

//Linux: Thanks to linuxgazette tips, http://www.linuxgazette.com/issue84/misc/tips/interfaces.c.txt,
//seems to be public domain.
//W32: Thanks to MSDN documentation:
//http://msdn.microsoft.com/library/default.asp?url=/library/en-us/iphlp/iphlp/managing_interfaces_using_getinterfaceinfo.asp


NetworkInterface::NetworkInterface(const string &name)
{
	m_name = name;
}

NetworkInterface::~NetworkInterface()
{
}

const string &NetworkInterface::getName() const
{
	return m_name;
}

const vector<string> &NetworkInterface::getIPStrings( bool ipv6 ) const
{
	if( ipv6 )
		return m_ip6Strs;
	else
		return m_ip4Strs;
}

void NetworkInterface::addIPString( const string &ip, bool ipv6 )
{
	if( ipv6 )
		return m_ip6Strs.push_back(ip);
	else
		return m_ip4Strs.push_back(ip);
}

#ifdef HAVE_GETIFADDRS
static bool sa_get_addr(struct sockaddr *sa, bool &ipv6, string &ip)
{
#ifdef HAVE_IPV6
	char addr[INET6_ADDRSTRLEN] = "";
#else
	char addr[INET_ADDRSTRLEN] = "";
#endif

	socklen_t len = 0;

	if( sa->sa_family == AF_INET ){
		len = sizeof(struct sockaddr_in);
		ipv6 = false;
	}
#ifdef HAVE_IPV6
	else if( sa->sa_family == AF_INET6 ){
		len = sizeof(struct sockaddr_in6);
		ipv6 = true;
	}
#endif
	else{
		// Unsupported address family
		return false;
	}
	
	if( getnameinfo(sa, len, addr, sizeof(addr),
			NULL, 0, NI_NUMERICHOST) ){
		ip = "";
		return false;
	}

	ip = addr;
	return true;
}

 vector<string> NetworkFunctions::getAllInterfaces(){
 	vector<string >res;
	struct ifaddrs *ifs = NULL;
	struct ifaddrs *cur;

	if( getifaddrs (&ifs) || !ifs){
		return res;
	}

	for( cur = ifs; cur; cur = cur->ifa_next ){
		if( cur->ifa_flags & IFF_UP ){
			if( find(res.begin(), res.end(), cur->ifa_name) ==
			    res.end()){
				res.push_back( cur->ifa_name );
			}
		}
	}

	freeifaddrs( ifs );

	return res;
}

string NetworkFunctions::getInterfaceIPStr(string iface){
	string ret;

	struct ifaddrs *ifs = NULL;
	struct ifaddrs *cur;

	if( getifaddrs (&ifs) || !ifs ){
		return "";
	}

	for( cur = ifs; cur; cur = cur->ifa_next ){
		if( iface != cur->ifa_name ){
			continue;
		}

		if( !( cur->ifa_flags & IFF_UP ) ){
			continue;
		}
		/* some interfaces (e.g. DSL modem) don't have an address */
		if (NULL == cur->ifa_addr) 
			continue;

		bool isIpv6;
		string addr;
		
		if( !sa_get_addr( cur->ifa_addr, isIpv6, addr) )
			continue;

		if( isIpv6 ){
			// getInterfaceIPStr doesn't support IPv6 addresses
			cerr << "Skipping IPV6 interface: " << addr << endl;
			continue;
		}

		ret = addr;
		break;
	}

	freeifaddrs( ifs );

	return ret;
}

vector<MRef<NetworkInterface*> > mapToVector( map<string, MRef<NetworkInterface*> > &input )
{
	map<string, MRef<NetworkInterface*> >::iterator i;
	vector<MRef<NetworkInterface*> > res;
	
	for( i = input.begin(); i != input.end(); i++ ){
		MRef<NetworkInterface*> interface = i->second;

		if( !interface ){
			mdbg << "NetworkFunctions::mapToVector: No interface!" << end;
			continue;
		}

		res.push_back( interface );
	}

	return res;
}

vector<MRef<NetworkInterface*> > NetworkFunctions::getInterfaces(){
	map<string, MRef<NetworkInterface*> > interfaces;

	vector<MRef<NetworkInterface*> > res;

	struct ifaddrs *ifs = NULL;
	struct ifaddrs *cur;

	if( getifaddrs (&ifs) || !ifs){
		return res;
	}

	for( cur = ifs; cur; cur = cur->ifa_next ){
		if( !cur->ifa_addr )
			continue;

		if( cur->ifa_flags & IFF_UP ){
			const char *name = cur->ifa_name;

			MRef<NetworkInterface*> interface = interfaces[ name ];

			if( !interface ){
				interface = new NetworkInterface( name );
				interfaces[ name ] = interface;
			}

			bool ipv6;
			string addr;
		
			if( !sa_get_addr( cur->ifa_addr, ipv6, addr) )
				continue;

			interface->addIPString( addr, ipv6 );
		}
	}

	freeifaddrs( ifs );

	return mapToVector( interfaces );	
}

#else  // HAVE_GETIFADDRS
vector<string> NetworkFunctions::getAllInterfaces(){
	vector<string >res;

#ifdef USE_WIN32_API
	ULONG           ulOutBufLen;
	DWORD           dwRetVal;

	IP_ADAPTER_INFO         *pAdapterInfo;
	IP_ADAPTER_INFO         *pAdapter;

	pAdapterInfo = (IP_ADAPTER_INFO *) calloc( 1, sizeof(IP_ADAPTER_INFO) );
	ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	if ((dwRetVal=GetAdaptersInfo( pAdapterInfo, &ulOutBufLen)) != ERROR_SUCCESS) {
		//GlobalFree (pAdapterInfo);
		free(pAdapterInfo);
		
		pAdapterInfo = (IP_ADAPTER_INFO *) calloc (1, ulOutBufLen);

		if ((dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen)) != NO_ERROR) {
			printf("Call to GetAdaptersInfo failed.\n");
		}
	}

	pAdapter = pAdapterInfo;


	while (pAdapter) {
		res.push_back(pAdapter->AdapterName);
	#ifdef DEBUG_OUTPUT
		printf("\tNetworkFunctions::getAllInterfaces - Adapter Name: \t%s\n", pAdapter->AdapterName);
	#endif
		pAdapter = pAdapter->Next;
	}
	free(pAdapterInfo);

#else
	int32_t sockfd;
	char *buf, *ptr;
	struct ifconf ifc;
	struct ifreq *ifrp;
	struct sockaddr_in *sockaddr_ptr;

	/* socket needed for ioctl() operations */
	sockfd = socket(AF_INET,SOCK_DGRAM,0);

	/* get ourself a buffer that is properly aligned */
	buf = (char *)malloc(BUFFER_SIZE);

	/* buffer length and reference are configured in ifconf{} structure */
	ifc.ifc_len = BUFFER_SIZE;
	ifc.ifc_ifcu.ifcu_buf = buf;

	/* if we have an error condition, just exit */
	if (ioctl(sockfd,SIOCGIFCONF,&ifc) < 0)
	{
		printf("ioctl() failure\n");
		free(buf); /* release resources -- yes, it's redundant */
		exit(1);
	} /* if */

	/* traverse array of ifreq{} structures and display interface names */
	for (ptr = buf; ptr < (buf + ifc.ifc_len);)
	{
		ifrp = (struct ifreq *)ptr;
		sockaddr_ptr = (struct sockaddr_in *)&ifrp->ifr_ifru.ifru_addr;

#ifdef DARWIN
		res.push_back(string(ifrp->ifr_name));
#else
		res.push_back(string(ifrp->ifr_ifrn.ifrn_name));
#endif
		ptr += sizeof(struct ifreq);
//		printf("IPIF names: %s: \n",ifrp->ifr_ifrn.ifrn_name);
//		printf("%s\n",inet_ntoa(sockaddr_ptr->sin_addr.s_addr));

	} /* for */

	/* release resources */
	free(buf);
	close(sockfd);
#endif //USE_WIN32_API

	return res;
}

#ifndef IF_NAMESIZE
#define IF_NAMESIZE 16
#endif

string NetworkFunctions::getInterfaceIPStr(string iface){
	string ret;

#ifdef USE_WIN32_API
	ULONG           ulOutBufLen;
	DWORD           dwRetVal;
	
	IP_ADAPTER_INFO         *pAdapterInfo;
	IP_ADAPTER_INFO         *pAdapter;

	pAdapterInfo = (IP_ADAPTER_INFO *) calloc( 1, sizeof(IP_ADAPTER_INFO) );
	ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) != ERROR_SUCCESS) {
		//We fail once to find out the buffer size
		free (pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *) calloc (1,ulOutBufLen);
		
		if ((dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen)) != NO_ERROR) {
			printf("Call to GetAdaptersInfo failed.\n");
		}
	}
	pAdapter = pAdapterInfo;

	while (pAdapter) {
		if (pAdapter->AdapterName==iface){
			string tmp =pAdapter->IpAddressList.IpAddress.String;
			ret=tmp;
		}
		pAdapter = pAdapter->Next;
	}

	free(pAdapterInfo);

#else
	
	struct ifreq ifr;
	struct sockaddr_in *ifaddr;
	int32_t fd;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if ( fd == -1 ) {
		throw SocketFailed( errno );
	}

	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_name, iface.c_str(), IF_NAMESIZE);

	if ( ioctl(fd, SIOCGIFADDR, &ifr) == -1 ) {
		merror("Error on ioctl");
		close(fd);
		return "";
	}

	/* the cast is naughty --> should check for address type first! */
	ifaddr = (struct sockaddr_in *)&ifr.ifr_addr;

	ret = string(inet_ntoa(ifaddr->sin_addr));

	close(fd);
#endif

	return ret;

}

vector<MRef<NetworkInterface*> > NetworkFunctions::getInterfaces()
{
	vector<string> names = getAllInterfaces();
	vector<string>::iterator i;
 
	vector<MRef<NetworkInterface*> > interfaces;

	for( i = names.begin(); i != names.end(); i++ ){
		string name = *i;
		string ip = getInterfaceIPStr( name );

		if( ip.length() > 0 ){
			MRef<NetworkInterface*> iface;

			iface = new NetworkInterface(name);
			iface->addIPString( ip );
			interfaces.push_back( iface );
		}
	}

	return interfaces;
}

#endif  // HAVE_GETIFADDRS

string NetworkFunctions::getInterfaceOf( string ipStr ) {
	vector<MRef<NetworkInterface*> > ifaces;
	vector<MRef<NetworkInterface*> >::iterator iter;
	bool isIpv6 = ipStr.find(':');
	
	ifaces = NetworkFunctions::getInterfaces();
	for( iter = ifaces.begin(); iter != ifaces.end(); iter++ ){
		MRef<NetworkInterface*> iface = *iter;
		const vector<string> &ipStrs = iface->getIPStrings( isIpv6 );

		unsigned int i;
		for( i=0; i<ipStrs.size(); i++ ) {
			string ifaceIP = ipStrs[ i ];

			if( ifaceIP == ipStr ) {
				return iface->getName();
			}
		}
	}
	return "";
}

string NetworkFunctions::getHostHandlingService(string service, string domain, uint16_t &ret_port){
	string ret;
	string hostname;
	int32_t port=-1;

	#ifdef DEBUG_OUTPUT
	cerr <<"NetworkFunctions::getHostHandlingService - Using LIBRESOLV"<< endl;
	#endif

	string q;
	if( service == "" )
		q = domain;
	else
		q = service+"."+domain;

	dns_ctx *ctx = dns_new(NULL);
	if( !ctx )
		throw ResolvError( errno ); // FIXME
	if( dns_open(ctx) < 0 )
		throw ResolvError( errno ); // FIXME

	dns_rr_srv *srv = 
		(dns_rr_srv*) dns_resolve_p(ctx, q.c_str(), DNS_C_IN,
					    DNS_T_SRV, DNS_NOSRCH,
					    dns_parse_srv);

	if (!srv){
		#ifdef DEBUG_OUTPUT
		cerr <<"SRV Service [" << service << "." << domain << "] not found"<< endl;
		#endif
		return "";
	}

	// FIXME sort by priority and weight
//	cerr << "Answer fields returned:"<<endl;
	for (int i=0; i< srv->dnssrv_nrr; i++){
		dns_srv *rr = &srv->dnssrv_srv[i];

		port = rr->port;
		hostname = rr->name;
		break;
	}

	free(srv);
	dns_free(ctx);
	ret_port=port;
	ret = string(hostname);
	#ifdef DEBUG_OUTPUT
	cerr << "NetworkFunc:getHostHandlingServ = " << ret << ":" << ret_port << endl;
	#endif
	
	return ret;
}

/**
uint32_t ... in host order!
*/
void NetworkFunctions::binIp2String(uint32_t ip, char *strBufMin16){
//      uint32_t nip = htonl(ip);
//      inet_ntop(AF_INET, &nip, strBufMin16, 16);
	sprintf( strBufMin16, "%i.%i.%i.%i", ( ip>>24 )&0xFF,
						( ip>>16 )&0xFF,
						( ip>> 8 )&0xFF,
						( ip     )&0xFF );
}

bool NetworkFunctions::isLocalIP(uint32_t ip, vector<string> &localIPs){
	char sip[20];
	binIp2String(ip,sip);
	string ssip(sip);
	
	for (vector<string>::iterator i=localIPs.begin(); i!=localIPs.end(); i++)
		if (ssip == (*i))
		return true;
	
	return false;
}

