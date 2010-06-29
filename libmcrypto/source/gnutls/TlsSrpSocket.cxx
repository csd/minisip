/*
 *  Copyright (C) 2004-2006 the Minisip Team
 * 
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 * 
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 * 
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *  */

/* Copyright (C) 2006
 *
 * Authors: Erik Ehrlund <eehrlund@kth.se>
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gnutls/gnutls.h>
#include <gnutls/extra.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>

#include<libmcrypto/TlsSrpSocket.h>
#include<libmcrypto/TlsException.h>
#include<libmnetutil/NetworkException.h>
using namespace std;


/************************************************************************/
static void checkErr(int a)
{
	if(a<0)
	{	
		perror("An error has occured");
		throw TLSInitFailed();
		return;
	}  
}

/************************************************************************/
const int kx_priority[] =
{  
	GNUTLS_KX_SRP, 0
};

/*********************************************************************************/
/* constructor*/
TlsSrpSocket::TlsSrpSocket(string addrs, int32_t port, string user, string pass)
{
	type = MSOCKET_TYPE_TLSSRP;
	TlsSrpSocket::TlsSrpSocketSrp_init(addrs, port, user, pass);
}

/*********************************************************************************/
TlsSrpSocket::~TlsSrpSocket()
{  
	gnutls_bye (session, GNUTLS_SHUT_WR);
	gnutls_deinit (session);
	gnutls_srp_free_client_credentials (srp_cred);
	gnutls_global_deinit ();
	::close(fd);
}

/*********************************************************************************/
void TlsSrpSocket::TlsSrpSocketSrp_init(string addrs, int32_t port, string user, string pass)
{

	int err=0;
	const char *usr = user.c_str();
	const char *passw = pass.c_str();
	const char *address = addrs.c_str();
	/* init gnutls */
	gnutls_global_init ();
	gnutls_global_init_extra ();
	gnutls_srp_allocate_client_credentials (&srp_cred);
	gnutls_srp_set_client_credentials (srp_cred, usr, passw);

	/* fix dest address */
	struct in_addr *dstaddr;
	struct hostent *hst;
	struct sockaddr_in addr;

	memset (&addr, '\0', sizeof (addr));
	//cout<<"IPAddress: "<<address<<" usr: "<<usr<<" passw: "<<passw<<endl;
	hst  = gethostbyname(address);
	if(hst ==NULL)
	{
		perror("Could not resolve host address");
		throw ResolvError(-1);
		return;
	}

	dstaddr = (struct in_addr *)hst->h_addr;
	memcpy(&(addr.sin_addr), dstaddr, sizeof(struct in_addr));

	addr.sin_family=AF_INET;
	addr.sin_port = htons(port);
	memset(&(addr.sin_zero), '\0', 8);

	/* fix socket desc*/

	fd = socket(PF_INET, SOCK_STREAM, 0);
	if(fd<0){
	 	throw SocketFailed( -1 );
		return;
	}
	err = connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr));
	if(err<0)
	{	
		::close(fd);
		throw ConnectFailed(-1);
		return;
	}


	err = gnutls_init (&session, GNUTLS_CLIENT);
	checkErr(err);

	err= gnutls_set_default_priority (session); //use default cipher,  mac and key exchange
	checkErr(err);

	err = gnutls_kx_set_priority (session, kx_priority); //overides default key exchange
	checkErr(err);

	err = gnutls_credentials_set (session, GNUTLS_CRD_SRP, srp_cred);
	checkErr(err);

	gnutls_transport_set_ptr (session, (gnutls_transport_ptr_t) fd);

	err = gnutls_handshake (session);
	if (err<0)
	{
		perror("****** HANDSHAKE FAILED ********");
		gnutls_perror(err);
		throw "handshake failed";
		return;
	}
	return;
}

/********************************************************************************/

int32_t TlsSrpSocket::write(const void *msg, int length)
{
	int a ;
	a = gnutls_record_send (session, msg , length);
	return a;
}
/*********************************************************************************/
int32_t TlsSrpSocket::write(string msg)
{   
	int a ;
	a = gnutls_record_send (session, msg.c_str(), msg.size());
	return a;
}

/*********************************************************************************/
int32_t TlsSrpSocket::read (void *buf, int maxlength)
{  
	int recv;
	recv = gnutls_record_recv (session, buf, maxlength);
	return recv;
}
