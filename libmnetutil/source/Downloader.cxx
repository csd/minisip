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

#include <config.h>
#include <libmnetutil/Downloader.h>
#include <libmnetutil/HttpDownloader.h>
#include <libmnetutil/FileDownloader.h>

#ifdef ENABLE_LDAP
#include <libmnetutil/LdapDownloader.h>
#endif

#include <string>

MRef<Downloader*> Downloader::create(std::string const uri, MRef<StreamSocket*> conn) {
	size_t pos = uri.find("://");
	if (std::string::npos != pos) {
		std::string protocol = uri.substr(0, pos);
		if (protocol == "http")
			return new HttpDownloader(uri, conn);
		else if (protocol == "file")
			return new FileDownloader(uri);
#ifdef ENABLE_LDAP
		else if (protocol == "ldap")
			return new LdapDownloader(uri);
#endif
	}
	return NULL;
}

