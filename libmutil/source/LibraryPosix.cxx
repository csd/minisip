/*
  Copyright (C) 2004-2006 the Minisip Team
  
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
 * Copyright (C) 2004-2006
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Mikael Magnusson <mikma@users.sourceforge.net>
*/

#include<config.h>
#include<libmutil/Library.h>
#include<libmutil/massert.h>

#include<iostream>

// Use ltdl symbols defined in ltdl.c
#define LT_SCOPE extern
#include<ltdl.h>

using namespace std;

int Library::refCount = 0;

Library::Library(const string &path_):path(path_){

	if( refCount == 0 )
		lt_dlinit();

	refCount++;

	handle = lt_dlopenext(path.c_str());
#ifdef DEBUG_OUTPUT
	if( !handle ){
		cerr << "Library opening \""<< path << "\": " << lt_dlerror() << endl;
	}
#endif
}

Library::~Library(){
	if(handle){
		lt_dlclose((lt_dlhandle)handle);
		handle=NULL;
	}

	refCount--;
	if( refCount == 0 )
		lt_dlexit();
}

void *Library::getFunctionPtr(string name){
	void * ptr = lt_dlsym((lt_dlhandle)handle, name.c_str());
	return ptr;
}

MRef<Library *> Library::open(const string &path){
	MRef<Library *> ret = new Library(path);
	if(ret->handle){
		const lt_dlinfo *info;

		info = lt_dlgetinfo( (lt_dlhandle)ret->handle );
		if( info && info->filename ){
			ret->path = info->filename;
		}

		return ret;
	}
	ret = NULL;
	return ret;
}

const string &Library::getPath(){
	return path;
}
