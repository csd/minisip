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



#include<libmutil/Mutex.h>

#include<config.h>
#include<stdio.h>
#include<stdlib.h>
#include<libmutil/massert.h>

// BSD 5.x: malloc.h has been replaced by stdlib.h
// #include<malloc.h>

#ifdef _MSC_VER
#include<windows.h>
#endif


#ifdef HAVE_PTHREAD_H
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
#ifdef _MSC_VER
	massert(sizeof(HANDLE)==sizeof(int));
#endif
	createMutex();
}

void Mutex::createMutex(){
#ifdef HAVE_PTHREAD_H
#define MINISIP_MUTEX_IMPLEMENTED
	handle_ptr = new pthread_mutex_t;
	pthread_mutex_init( (pthread_mutex_t*)handle_ptr, NULL);

#elif defined _MSC_VER
#define MINISIP_MUTEX_IMPLEMENTED
	handle_ptr = new HANDLE;
	*((HANDLE*)handle_ptr) = CreateMutex(NULL, FALSE, NULL);
	if (handle_ptr==NULL){  //TODO: handle better
		cerr << "could not create mutex!" << endl;
		exit(1);
	}else{

	}

#elif defined WINCE
#define MINISIP_MUTEX_IMPLEMENTED
	handle_ptr = malloc(1, sizeof(HANDLE));
	*((HANDLE*)handle_ptr) = CreateMutex(NULL, FALSE, NULL);
	if (hMutex==NULL){  //TODO: handle better
		cerr << "could not create mutex!"<< endl;
		exit(1);
	}else{

	}
#endif

#ifndef MINISIP_MUTEX_IMPLEMENTED
#error Mutex not fully implemented
#endif

}

Mutex::~Mutex(){

#ifdef HAVE_PTHREAD_H
	pthread_mutex_destroy((pthread_mutex_t*)handle_ptr);
	delete (pthread_mutex_t*)handle_ptr;

#elif defined _MSC_VER
	if (!CloseHandle(*((HANDLE*)handle_ptr))){
		cerr << "Could not free mutex"<<endl;
		massert(1==0); //TODO: Handle better - exception
	}
	delete handle_ptr;

#elif defined WINCE
#error Mutex delete not implemented
#endif


	
}


void Mutex::lock(){
#ifdef HAVE_PTHREAD_H
	    
	    int ret=pthread_mutex_lock((pthread_mutex_t*)handle_ptr);
	    if (ret!=0){
	    	perror("pthread_mutex_lock");
		exit(1);
	    }
	    
#elif defined _MSC_VER
	    WaitForSingleObject(*((HANDLE*)handle_ptr),INFINITE);
#elif defined WINCE
	    WaitForSingleObject(*((HANDLE*)handle_ptr),INFINITE);
#endif

}

void Mutex::unlock(){
#ifdef HAVE_PTHREAD_H
	
	int ret=pthread_mutex_unlock((pthread_mutex_t*)handle_ptr);
	if (ret!=0){
		perror("pthread_mutex_unlock");
		exit(1);
	}

#elif defined _MSC_VER
	if (!ReleaseMutex( *( (HANDLE*)handle_ptr) )){
		massert(1==0); // Could not release mutex TODO: Handle better - exception
	}

#elif defined WINCE
	    ReleaseMutex(*((HANDLE*)handle_ptr));
#endif
}

