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

#ifndef _LIBRARY_H
#define _LIBRARY_H

#include <libmutil/libmutil_config.h>

#include<string>
#include<libmutil/MemObject.h>

/**
 * Implements support for run-time linking of libraries on MS Windows and
 * systems having the dlopen/dlsym functions.
 * @author Erik Eliasson, eliasson@imit.kth.se
 * @author Johan Bilien, jobi@via.ecp.fr
 */
class LIBMUTIL_API Library : public MObject{
public:
	/**
	 * Opens a library.
	 * @param path	File name/path to a library that will be opened.
	 * @returns a reference to the resulting Library object, or NULL
	 *  if the opening failed.
	 */
	static MRef<Library *> open(const std::string &path);

	/**
	 * Closes the library
	 */
	virtual ~Library();

	/**
	 * NOTE: This method assumes that the function pointers
	 * can be returned as "void*".
	 * @param name 	Name of the function to look up.
	 */
	void *getFunctionPtr(std::string name);

	/**
	 * @returns the file name from which the library was loaded.
	 *	    On Win32 this is the same as the argument given to open( ).
	 */
	const std::string &getPath();

	/**
	 * Defined in MObject. Used only for monitoring/debugging.
	 */
	std::string getMemObjectType() const { return "Library"; }

protected:
	/**
	 * Opens a library.
	 * @param path	File name/path to a library that will be opened.
	 */
	Library(const std::string &path);


private:
	
	void *handle;	///Pointer to internal data structure
			///that is different on different 
			///platforms.
	std::string path; // file name from which the library was opened.
	static int refCount;
};


#endif

