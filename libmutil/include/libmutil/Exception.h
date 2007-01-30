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

#ifndef _MEXCEPTION_H
#define _MEXCEPTION_H

#include <libmutil/libmutil_config.h>

#include<string>

#include<libmutil/mtypes.h>
#include<exception>


/**
 * A generic exception class that is a good candidate
 * as base class for other exceptions. When the exception
 * is created it takes a snapshot of the current stack and
 * you can get a string representation of it by calling
 * "stackTrace()".
 *
 * Threads created using the Thread class in libmutil
 * will catch all Exceptions and output the stack trace.
 * This is useful when debugging.
 * 
 * The stack trace output contains the "mangled" name.
 * You can "un-mangle" it by using the "c++filt" command 
 * in linux.
 * 
 * Note: Stack trace is not supported on W32 and WinCE.
 * 
 */ 
class LIBMUTIL_API Exception : public std::exception {
public:
	/**
	 * Creates an exception without any description. Sub-classes
	 * can set the description text by accessing the protected
	 * "msg" string.
	 */
	Exception();
	
	/**
	 * @param what	Description of the exception. Can be retrieved
	 * using the "what()" method.
	 */
	Exception(char const* what);

	Exception(const std::string& what);

	Exception(const Exception &);
	
	~Exception() throw ();
	
	/**
	 * Returns a description of the exception. This overloads
	 * the method in the "exception" base class.
	 */
	virtual const char *what() const throw();

	/**
	 * If support for stack traces was found when building
	 * libmutil, this method will return a string describing
	 * the stack. If not supported, an empty string is returned.
	 */
	std::string stackTrace() const;

protected:
	std::string msg;
	
private:
	void **stack;   // Internal stack trace storage.
	int stackDepth;	// We need to know how many levels - only 
	                // used internally.
};

/**
 * Returns a string representation of the current stack state.
 * Note that the stack will contain the call to this function 
 * as well.
 */
LIBMUTIL_API std::string getStackTraceString();

#endif
