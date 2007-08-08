/*
  Copyright (C) 2007, Mikael Svensson

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
 * Authors:	Mikael Svensson
*/

#ifndef _FILESYSTEMUTILS_H_
#define _FILESYSTEMUTILS_H_

#include <libmutil/libmutil_config.h>
#include <libmutil/MemObject.h>

/**
 * Various functions for accessing the local file system.
 */
class LIBMUTIL_API FileSystemUtils : public MObject {
	public:
		static std::list<std::string> directoryContents(std::string dir, bool includeSubdirectories);
	private:
		static void directoryContentsInternal(std::string dir, bool includeSubdirectories, std::list<std::string> & res);
};
#endif
