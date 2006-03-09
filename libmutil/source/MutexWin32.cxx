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


#include<config.h>

#include<libmutil/Mutex.h>
#include<libmutil/merror.h>

#include<stdio.h>
#include<stdlib.h>
#include<libmutil/massert.h>

// BSD 5.x: malloc.h has been replaced by stdlib.h
// #include<malloc.h>

#if defined _MSC_VER || __MINGW32__
#include<windows.h>
# define USE_WIN32_THREADS
#endif

#include<iostream>
using namespace std;

///See: http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dllproc/base/mutex_objects.asp

//TODO: Check return values
Mutex::Mutex(){
	createMutex();
}


//FIXME: Verify and comment this method!
Mutex& Mutex::operator=(const Mutex &){
	//Do not copy the Mutex reference - keep our own.
	return *this;
}

Mutex::Mutex(const Mutex &){
#ifdef _MSC_VER
	massert(sizeof(HANDLE)==sizeof(int));
#endif
	createMutex();
}

void Mutex::createMutex(){
#if defined USE_WIN32_THREADS
#define MINISIP_MUTEX_IMPLEMENTED
	handle_ptr = new HANDLE;
	*((HANDLE*)handle_ptr) = CreateMutex(NULL, FALSE, NULL);
	if (handle_ptr==NULL){  //TODO: handle better
		merror("Mutex::createMutex: CreateMutex");
		exit(1);
	}else{

	}

#elif defined WINCE
#define MINISIP_MUTEX_IMPLEMENTED
	handle_ptr = malloc(1, sizeof(HANDLE));
	*((HANDLE*)handle_ptr) = CreateMutex(NULL, FALSE, NULL);
	if (hMutex==NULL){  //TODO: handle better
		merror("Mutex::createMutex: CreateMutex");
		exit(1);
	}else{

	}
#endif

#ifndef MINISIP_MUTEX_IMPLEMENTED
#error Mutex not fully implemented
#endif

}

Mutex::~Mutex(){

#if defined USE_WIN32_THREADS
	if (!CloseHandle(*((HANDLE*)handle_ptr))){
		merror("Mutex::~Mutex: CloseHandle");
		massert(1==0); //TODO: Handle better - exception
	}
	delete (HANDLE*)handle_ptr;

#elif defined WINCE
#error Mutex delete not implemented
#endif
	
}


void Mutex::lock(){
#if defined USE_WIN32_THREADS
	if (WaitForSingleObject(*((HANDLE*)handle_ptr),INFINITE)==WAIT_FAILED){
		merror("Mutex::lock: WaitForSingleObject");
	}
#endif

}

void Mutex::unlock(){
#if defined USE_WIN32_THREADS
	if (!ReleaseMutex( *( (HANDLE*)handle_ptr) )){
		merror("Mutex::unlock: ReleaseMutex:");
		massert(1==0); // Could not release mutex TODO: Handle better - exception
	}

#elif defined WINCE
	    ReleaseMutex(*((HANDLE*)handle_ptr));
#endif
}

