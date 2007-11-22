/*
  Copyright (C) 2007 Erik Eliasson

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
 */

#ifndef HTTPFILESYSTEM_H
#define HTTPFILESYSTEM_H 

#include<libmnetutil/libmnetutil_config.h>
#include<libmutil/FileSystem.h>
#include<libmnetutil/StreamSocket.h>
//#include<libmnetutil/HttpDownloader.h>

#include<string>

/**
 *
 * Note:
 *   You MUST use a MRef to your HttpFileSystem object.
 *   You CAN NOT have HttpFileSystem objects on the stack.
 *   This is because if not, the garbage collector will delete it
 *   when your last file opened with the open method is deleted.
 *
 */
class LIBMNETUTIL_API HttpFileSystem : public FileSystem {
	public:
		HttpFileSystem( MRef<StreamSocket*> conn, std::string prefix );
		virtual void mkdir( const std::string & name );
		virtual MRef<File*> open( const std::string& path, bool createIfNotExist=false );

	private:
		//MRef<HttpDownloader*> httpDl;
		std::string prefix;
		MRef<StreamSocket*> conn;
};

#endif
