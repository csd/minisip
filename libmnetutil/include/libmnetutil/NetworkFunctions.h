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

#ifndef NETWORKFUNCTIONS_H
#define NETWORKFUNCTIONS_H

#ifdef _MSC_VER
#ifdef LIBMNETUTIL_EXPORTS
#define LIBMNETUTIL_API __declspec(dllexport)
#else
#define LIBMNETUTIL_API __declspec(dllimport)
#endif
#else
#define LIBMNETUTIL_API
#endif

#include<vector>
#include<libmnetutil/IPAddress.h>
#include<libmutil/mtypes.h>

using namespace std;

class LIBMNETUTIL_API NetworkFunctions{
	public:
		static vector<string> getAllInterfaces();
		static string getInterfaceIPStr(string iface);
		static string getHostHandlingService(string service, string domain, uint16_t &ret_port);
		static bool isLocalIP(uint32_t ip, vector<string> &localIPs);

		//OBS: ip is host byte order
		static void binIp2String(uint32_t ip, char *strBufMin16);

	private:

};
#endif
