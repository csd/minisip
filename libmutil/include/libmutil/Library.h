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

#include<string>
#include<libmutil/MemObject.h>

/**
 * Implements support for run-time linking of libraries on MS Windows and
 * systems having the dlopen/dlsym functions.
 * @author Erik Eliasson, eliasson@imit.kth.se
 */
class Library : public MObject{
public:
	/**
	 * Opens a library.
	 * @param path	File name/path to a library that will be opened.
	 */
	Library(std::string path);

	/**
	 * Closes the library
	 */
	~Library();

	/**
	 * NOTE: This method assumes that the function pointers
	 * can be returned as "void*".
	 * @param name 	Name of the function to look up.
	 */
	void *getFunctionPtr(std::string name);

	/**
	 * Defined in MObject. Used only for monitoring/debugging.
	 */
	std::string getMemObjectType(){ return "Library"; }
private:
	
	void *handle;	///Pointer to internal data structure
			///that is different on different 
			///platforms.
	
};


#endif
