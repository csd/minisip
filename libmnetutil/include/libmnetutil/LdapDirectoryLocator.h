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

#ifndef LDAPDIRECTORYLOCATOR_H_
#define LDAPDIRECTORYLOCATOR_H_

#include<libmnetutil/libmnetutil_config.h>

#include <libmutil/MemObject.h>

/**
 * Provides a selection of methods for locating LDAP servers. NOT IMPLEMENTED!
 *
 * @author	Mikael Svensson
 */
class LIBMNETUTIL_API LdapDirectoryLocator : public MObject {
	public:
		static std::string findDnsSrv();
		static std::string findDnsAlias();
		static std::string findUserConfig();
		static std::string findUserCache();
		static std::string find();
};
#endif
