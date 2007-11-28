/*
 Copyright (C) 2006 the Minisip Team
 
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
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/* Copyright (C) 2007
 *
 * Authors: Erik Eliasson <ere@kth.se>
 */

#ifndef MFILESYSTEM_H
#define MFILESYSTEM_H

#include <libmutil/libmutil_config.h>
#include <libmutil/Exception.h>
#include <libmutil/MemObject.h>

class LIBMUTIL_API FileException : public Exception{
	public:
		FileException(const std::string why);
};

class LIBMUTIL_API FileSystemException : public Exception{
	public:
		FileSystemException(const std::string why);
};

/**
 * Defines an API for how to manipulate files. A file object
 * is created using the FileSystem::open() method.
 *
 * Note that File objects may represent remote files, and
 * the flush() is more likely to fail on such files than when
 * doing traditional file I/O.
 */
class LIBMUTIL_API File : public MObject {
	public:
		/**
		 * @return Number of bytes read. This can be less than the
		 * requested number of bytes in case EOF is reached.
		 */
		virtual int32_t read(void *buf, int32_t count) = 0;
		virtual void write(void *buf, int32_t count) = 0;

		virtual bool eof() = 0;

		/**
		 * Sets the read position offset from the beginning of the
		 * file. 
		 */
		virtual void seek(int64_t pos ) = 0;

		/**
		 * @return Current read position from the start of the file
		 * 	in bytes.
		 */
		virtual int64_t offset() = 0;
		virtual int64_t size() = 0;
		virtual void flush() = 0;
};

/**
 * Defines the API to a file storage area (file system) where
 * the user can create directories, open files, and set default
 * paths.
 */
class LIBMUTIL_API FileSystem : public MObject {
	public:
		virtual void mkdir( const std::string & name ) = 0;
		virtual MRef<File*> open( const std::string& path, bool createIfNotExist=false ) = 0;

		/**
		 * If a path does not start with a forward slash, then a
		 * string is prepended to it. This is similar to a
		 * "cd" (change current working directory) command.
		 *
		 * For local file systems, not setting the default path at
		 * all means using the applications current working
		 * directory.
		 */
		virtual void setDefaultPath(std::string);

		/**
		 * Returns the prefix added to all paths not starting with
		 * a forward slash that are given as parameters to open().
		 * @see setDefaultPath()
		 */
		virtual std::string getDefaultPath();

	protected:
		/**
		 * Prefix that should be added to paths not starting with
		 * a forward slash character.
		 * 
		 */
		std::string defPrefix;
};

class LIBMUTIL_API LocalFileSystem : public FileSystem {
	public:
		virtual void mkdir( const std::string & name );
		virtual MRef<File*> open( const std::string& path, bool createIfNotExist=false );

	private:

};

#endif	// MFILESYSTEM_H

