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

#define _WIN32_WINNT 0x0501
#include<config.h>
#include<libmutil/dbg.h>
#include"NetworkFunctionsWin32.h"

#include<iostream>

using namespace std;

HINSTANCE hiphlpapi;
HINSTANCE hws2tcpip;

#ifdef HAVE_GETADAPTERSADDRESSES
PGETADAPTERSADDRESSES hGetAdaptersAddresses;
#endif

PGETADDRINFO hgetaddrinfo;
PFREEADDRINFO hfreeaddrinfo;
PGETNAMEINFO hgetnameinfo;

void WINXP_exit(){
#ifdef HAVE_GETADAPTERSADDRESSES
	hGetAdaptersAddresses = NULL;
#endif
	hgetaddrinfo = NULL;
	hfreeaddrinfo = NULL;
	hgetnameinfo = NULL;
	if( hiphlpapi )
		FreeLibrary( hiphlpapi );
	hiphlpapi = NULL;
	if( hws2tcpip )
		FreeLibrary( hws2tcpip );
	hws2tcpip = NULL;
}

bool WINXP_init(){
	hiphlpapi = LoadLibrary("iphlpapi");

	if( !hiphlpapi ){
		mdbg << "NetworkFunctionsWin32: no hiphlpapi" << endl;
		WINXP_exit();
		return false;
	}

#ifdef HAVE_GETADAPTERSADDRESSES
	hGetAdaptersAddresses = (PGETADAPTERSADDRESSES)GetProcAddress(hiphlpapi, "GetAdaptersAddresses");
	
	if( !hGetAdaptersAddresses ){
		mdbg << "NetworkFunctionsWin32: no GetAdaptersAddresses" << endl;
		WINXP_exit();
		return false;
	}
#endif // HAVE_GETADAPTERSADDRESSES

	hws2tcpip = LoadLibrary("ws2_32");
	if( !hws2tcpip ){
		mdbg << "NetworkFunctionsWin32: no ws2tcpip" << endl;
		WINXP_exit();
		return false;
	}

	hgetaddrinfo = (PGETADDRINFO)GetProcAddress(hws2tcpip, "getaddrinfo");

	if( !hgetaddrinfo ){
		mdbg << "NetworkFunctionsWin32: no getaddrinfo" << endl;
		WINXP_exit();
		return false;
	}

	hfreeaddrinfo = (PFREEADDRINFO)GetProcAddress(hws2tcpip, "freeaddrinfo");

	if( !hfreeaddrinfo ){
		mdbg << "NetworkFunctionsWin32: no freeaddrinfo" << endl;
		WINXP_exit();
		return false;
	}

	hgetnameinfo = (PGETNAMEINFO)GetProcAddress(hws2tcpip, "getnameinfo");

	if( !hgetnameinfo ){
		mdbg << "NetworkFunctionsWin32: no getnameinfo" << endl;
		WINXP_exit();
		return false;
	}

	mdbg << "NetworkFunctionsWin32: functions loaded ok" << endl;
	return true;
}
