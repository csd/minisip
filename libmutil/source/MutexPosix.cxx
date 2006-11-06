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

#include<stdio.h>
#include<stdlib.h>
#include<libmutil/massert.h>

// BSD 5.x: malloc.h has been replaced by stdlib.h
// #include<malloc.h>

#if HAVE_PTHREAD_H
#include<pthread.h>
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
	createMutex();
	massert(handle_ptr);
}

void Mutex::createMutex(){

	pthread_mutexattr_t *attr = NULL;

#ifdef DEBUG_OUTPUT
	pthread_mutexattr_t errorCheck;
	pthread_mutexattr_init( &errorCheck );
	pthread_mutexattr_settype( &errorCheck, PTHREAD_MUTEX_ERRORCHECK );
	attr = &errorCheck;
#endif

	handle_ptr = new pthread_mutex_t;
	pthread_mutex_init( (pthread_mutex_t*)handle_ptr, attr );

#ifdef DEBUG_OUTPUT
	pthread_mutexattr_destroy( &errorCheck );
#endif
}

Mutex::~Mutex(){
	massert(handle_ptr);
	pthread_mutex_destroy((pthread_mutex_t*)handle_ptr);
	delete (pthread_mutex_t*)handle_ptr;
	handle_ptr=NULL;
}


void Mutex::lock(){
	int ret;
	massert(handle_ptr);
	ret = pthread_mutex_lock((pthread_mutex_t*)handle_ptr);
	massert( ret == 0 );
}

void Mutex::unlock(){
	int ret;
	massert(handle_ptr);
	ret = pthread_mutex_unlock((pthread_mutex_t*)handle_ptr);
	massert( ret == 0 );
}
