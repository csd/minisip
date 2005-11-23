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


#ifdef HAVE_CONFIG_H
#include<config.h>
#endif

#include<libmnetutil/NetworkException.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#ifdef HAVE_LINUX_SOCKIOS_H
#include <linux/sockios.h> /* for SIOCG* */
#endif

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h> /* inet_ntoa */
#include <sys/types.h> /* socket() */
#include <sys/socket.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include<unistd.h>
#include<arpa/nameser.h>
#include<resolv.h>
#endif

#ifdef HAVE_ARPA_NAMESER_COMPAT_H
#include<arpa/nameser_compat.h>
#endif

#include<errno.h>
#include<iostream>
#include<libmnetutil/NetworkFunctions.h>
#include<libmutil/itoa.h>
#include<stdio.h>

#define BUFFER_SIZE 1024 /* bytes */

//Thanks to linuxgazette tips, http://www.linuxgazette.com/issue84/misc/tips/interfaces.c.txt,
//seems to be public domain.

//TODO: FIXME: change from ioctl to getifaddrs to support IPv6 -EE

vector<string> NetworkFunctions::getAllInterfaces(){
	vector<string >res;
#ifndef WIN32
	int32_t sockfd, len;
	char *buf, *ptr;
	struct ifconf ifc;
	struct ifreq *ifrp;
	struct sockaddr_in *sockaddr_ptr;

	len = sizeof(struct sockaddr);

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
		ptr += sizeof(ifrp->ifr_name) + len;
#else
		res.push_back(string(ifrp->ifr_ifrn.ifrn_name));
		ptr += sizeof(ifrp->ifr_ifrn.ifrn_name) + len;
#endif
//		printf("%s: ",ifrp->ifr_ifrn.ifrn_name);
//		printf("%s\n",inet_ntoa(sockaddr_ptr->sin_addr.s_addr));

	} /* for */

	/* release resources */
	free(buf);
        close(sockfd);
#endif
	return res;
}

#ifndef IF_NAMESIZE
#define IF_NAMESIZE 16
#endif

string NetworkFunctions::getInterfaceIPStr(string iface){
    string ret;
#ifndef WIN32
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
		perror("Error on ioctl");
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


string NetworkFunctions::getHostHandlingService(string service, string domain, uint16_t &ret_port){
    string ret;
#ifndef WIN32
	int32_t port=-1;
	if (res_init()){
		throw ResolvError( errno );
	}
//	unsigned char answerbuffer[2048]={};
	unsigned char *answerbuffer=(unsigned char*)calloc(1,2048);
//	string q = string("_sip._udp.")+argv[1];
	string q = service+"."+domain;

	int32_t len = res_query(q.c_str(), C_IN, T_SRV, answerbuffer,2048);
	if (len<=0){
		#ifdef DEBUG_OUTPUT
		cerr <<"SRV Service [" << service << "." << domain << "] not found"<< endl;
		#endif
		return "";
	}


	HEADER *hdr = (HEADER*)&answerbuffer[0];
	int32_t qdcount = ntohs(hdr->qdcount);
	int32_t ancount = ntohs(hdr->ancount);
//	cerr << "Query count="<< qdcount << " and answer count="<<ancount <<endl;

	char hostname[256];
	unsigned char *messageindex = answerbuffer+sizeof(HEADER);
//	cerr << "Query fields returned:"<<endl;
	int32_t n,i;
	for (i=0; i< qdcount; i++){                         // 3.
		if ((n=dn_expand(answerbuffer,answerbuffer+len, messageindex, &hostname[0],256))<0){
			perror("dn_expand:");
		}
		messageindex+=n+QFIXEDSZ;
//		cerr << "\tName: "<< hostname << endl;
	}

//	cerr << "Answer fields returned:"<<endl;
	for (i=0; i< ancount; i++){                 // 4.
		if ((n=dn_expand(answerbuffer,answerbuffer+len, messageindex, &hostname[0],256))<0){
			perror("dn_expand:");
		}
		messageindex+=n;                        // 5.
//		cerr << "\tName: "<< hostname << endl;

		int32_t type = ntohs(*((short*)messageindex));
//		cerr << "\tType: "<< type << endl;
		messageindex+=sizeof(short);

//		int qclass = ntohs(*((short*)messageindex));
//		cerr << "\tClass: "<< qclass << endl;
		messageindex+=sizeof(short);

//		unsigned long ttl = ntohl(*((unsigned long*)messageindex));
//		cerr << "\tTTL: "<< ttl << endl;
		messageindex+=sizeof(unsigned long);


//		int dlen = ntohs(*((short*)messageindex));
//		cerr << "\tdlen: "<< dlen << endl;
		messageindex+=sizeof(short);

		if (type!=T_SRV){
			#ifdef DEBUG_OUTPUT
			cerr << "Returned type is not a SRV record"<< endl;
			#endif
			return "";
		}

//		int pref = ntohs(*((short*)messageindex));
//		cerr << "\tpref: "<< pref << endl;
		messageindex+=sizeof(short);

//		int weight = ntohs(*((short*)messageindex));
//		cerr << "\tweight: "<< weight << endl;
		messageindex+=sizeof(short);

		port = ntohs(*((unsigned short*)messageindex));
//		cerr << "\tport: "<< port << endl;
		messageindex+=sizeof(short);
		n= dn_expand(answerbuffer,answerbuffer+len,messageindex, hostname, 256);

		messageindex+=n;

//		cerr << "\tHost: "<< hostname << endl;
	}
	ret_port=port;
	ret = string(hostname);
	#ifdef DEBUG_OUTPUT
	cerr << "NetworkFunc:getHostHandlingServ = " << ret << ":" << ret_port << endl;
	#endif
#endif
    return ret;
}


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
