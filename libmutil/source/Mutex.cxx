/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/


#include<libmutil/Mutex.h>

#include<config.h>
#include<stdio.h>
#include<stdlib.h>

// BSD 5.x: malloc.h has been replaced by stdlib.h
// #include<malloc.h>

#ifdef WIN32
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


Mutex& Mutex::operator=(const Mutex &m){
	//Do not copy the Mutex reference - keep our own.
	return *this;
}

Mutex::Mutex(const Mutex &m){
	createMutex();
}

void Mutex::createMutex(){
#ifdef HAVE_PTHREAD_H
#define MINISIP_MUTEX_IMPLEMENTED
	//handle_ptr = malloc(sizeof(pthread_mutex_t));
	handle_ptr = new pthread_mutex_t;
	pthread_mutex_init( (pthread_mutex_t*)handle_ptr, NULL);

#elif defined WIN32
#define MINISIP_MUTEX_IMPLEMENTED
	handle_ptr = malloc(sizeof(HANDLE));
	//    hMutex = CreateMutex(NULL, FALSE, NULL);
	*((HANDLE*)handle_ptr) = CreateMutex(NULL, FALSE, NULL);
	if (handle_ptr==NULL){  //TODO: handle better
		fprintf( stderr, "could not create mutex!" );
		exit(1);
	}else{

	}

#elif defined WINCE
#define MINISIP_MUTEX_IMPLEMENTED
	handle_ptr = malloc(1, sizeof(HANDLE));
	//    hMutex = CreateMutex(NULL, FALSE, NULL);
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
//	free(handle_ptr);

#elif defined WIN32
#warning Mutex delete not implemented


#elif defined WINCE
#warning Mutex delete not implemented
#endif


	
}


void Mutex::lock(){
#ifdef HAVE_PTHREAD_H
	    //pthread_mutex_lock(&mutexlock);
	    
	    int ret=pthread_mutex_lock((pthread_mutex_t*)handle_ptr);
	    if (ret!=0){
	    	perror("pthread_mutex_lock");
		exit(1);
	    }
	    
//	    locked=true;
#elif defined WIN32
//	    WaitForSingleObject(hMutex,INFINITE);
	    WaitForSingleObject(*((HANDLE*)handle_ptr),INFINITE);
//	    locked=true;
    
#elif defined WINCE
	    WaitForSingleObject(*((HANDLE*)handle_ptr),INFINITE);
//	    locked=true;
#endif

}

void Mutex::unlock(){
#ifdef HAVE_PTHREAD_H
	
//	if (locked){

//	    pthread_mutex_unlock(&mutexlock);
	    int ret=pthread_mutex_unlock((pthread_mutex_t*)handle_ptr);
//	    locked=false;
	    if (ret!=0){
	    	perror("pthread_mutex_unlock");
		exit(1);
	    }

//	}

#elif defined WIN32
	if (locked){
//	    ReleaseMutex(hMutex);
	    ReleaseMutex( *( (HANDLE*)handle_ptr) );
//	    locked=false;
	}

#elif defined _WINCE_
	    ReleaseMutex(*((HANDLE*)handle_ptr));
//	    locked=false;
#endif
}

