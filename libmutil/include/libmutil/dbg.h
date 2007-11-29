/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  Copyright (C) 2006 Mikael Magnusson
  
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
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/


#ifndef _DBG_H
#define _DBG_H

#include <libmutil/libmutil_config.h>

#include<string>
#include<iostream>
#include<sstream>
#include<set>

#ifdef _MSC_VER
#pragma warning (disable: 4251)
#endif

/**
 * Normal operation:
 *   merr << "hello"<<endl;
 *   mdbg << "hello"<<endl;
 * Filters:
 *   You can filter what output you want to include/exclude.
 *   Example 1, make output with a "output class" associated with it:
 *      dbg("myapp/gui") << "Gui started"<<endl;
 *
 *   Example 2, set dbg to default exclude anything that is not explicitely
 *   included: 
 *      dbg.exclude("");
 * 
 *   Example 3, make dbg default include everything
 *      dbg.include("");
 *
 *   Example 4, make dgb include "myapp/gui" and all sub-classes.
 *      dbg.include("myapp/gui");
 *    
*/

class LIBMUTIL_API DbgEndl{
public:
	DbgEndl(){}
private:
	int i;
};


class LIBMUTIL_API DbgHandler{
public:
	virtual ~DbgHandler(){}
protected:
	virtual void displayMessage(std::string output,int style=-1)=0;
private:
	friend class Dbg;
};

class Mutex;

class LIBMUTIL_API Dbg{
public:
	Dbg(std::string name="", bool error_output=false, bool enabled=true);
	~Dbg();

	Dbg &operator<<( const std::string& );
	Dbg &operator<<( int );
	Dbg &operator<<( unsigned int );
	Dbg &operator<<( long long );
	Dbg &operator<<( const char );
	Dbg &operator<<( const char *);
	Dbg &operator<<( void *);

	//accepts std::endl as parameter
	Dbg &operator<<( std::ostream&(*arg)(std::ostream&) );

	void setEnabled(bool enabled);
	bool getEnabled();
	void setExternalHandler(DbgHandler * dbgHandler);

	/**
	 * Set to true to make the output stream prefix all lines with
	 * the name of the stream.
	 * Example if true
	 *     mout << "hello" << endl;
	 *   results in
	 *     [mout] hello
	 * Example if false
	 *     mout << "hello" << endl;
	 *   results in
	 *     hello
	 */
	void setPrintStreamName(bool b);

	Dbg& operator()(std::string oClass);
	void include(std::string);
	void exclude(std::string);


private:
	void updateFilter();
	std::string name;
	bool error_out;
	bool enabled;
	std::string str;
	DbgHandler * debugHandler;

	bool defaultInclude;            // include or exclude by default
	std::string curClass;           // currently set output class. Reset by endl.
	std::set< std::string > includeSet;
	std::set< std::string > excludeSet;
	bool filterBlocking;
	Mutex *lock;
	bool printName;
};

extern LIBMUTIL_API Dbg mout;
extern LIBMUTIL_API Dbg merr;
extern LIBMUTIL_API Dbg mdbg;

extern LIBMUTIL_API DbgEndl end; /// DEPRECATED - use std::endl instead

extern LIBMUTIL_API bool outputStateMachineDebug;

#endif

