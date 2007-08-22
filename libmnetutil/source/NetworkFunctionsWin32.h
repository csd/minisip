/*
  Copyright (C) 2006-2007 Mikael Magnusson
  
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

#ifndef NETWORKFUNCTIONSWIN32_H
#define NETWORKFUNCTIONSWIN32_H

#include<libmnetutil/libmnetutil_config.h>
//#ifdef HAVE_WS2TCPIP_H
//# include<ws2tcpip.h>
//#endif
#include<winsock2.h>
#include<windows.h>
#ifdef HAVE_IPHLPAPI_H
# include<iphlpapi.h>
#endif
#include<ws2tcpip.h>

extern HINSTANCE hiphlpapi;
extern HINSTANCE hws2tcpip;

#ifdef HAVE_GETADAPTERSADDRESSES
typedef ULONG WINAPI (*PGETADAPTERSADDRESSES)(ULONG,ULONG,PVOID,PIP_ADAPTER_ADDRESSES,PULONG);

extern PGETADAPTERSADDRESSES hGetAdaptersAddresses;
#define GetAdaptersAddresses hGetAdaptersAddresses
#endif // HAVE_GETADAPTERSADDRESSES

#ifndef WSAAPI
#error NO WSAPI
#endif

typedef int (WSAAPI * PGETNAMEINFO)(const struct sockaddr*,socklen_t,char*,
				   DWORD,char*,DWORD,int);
extern PGETNAMEINFO hgetnameinfo;
#define getnameinfo hgetnameinfo
#define HAVE_GETNAMEINFO 1

typedef int (WSAAPI *PGETADDRINFO)(const char*,const char*,const struct addrinfo*, struct addrinfo**);
extern PGETADDRINFO hgetaddrinfo;
#define getaddrinfo hgetaddrinfo
#define HAVE_GETADDRINFO 1

typedef void (WSAAPI *PFREEADDRINFO)(struct addrinfo*);
extern PFREEADDRINFO hfreeaddrinfo;
#define freeaddrinfo hfreeaddrinfo
#define HAVE_FREEADDRINFO 1

bool WINXP_init();
void WINXP_exit();

#endif	// NETWORKFUNCTIONSWIN32_H
