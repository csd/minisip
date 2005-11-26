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

#include<libmutil/Semaphore.h>
#include<libmutil/merror.h>
#include<stdlib.h>
#include<stdio.h>
#include<errno.h>

#include<iostream>
using namespace std;

#include<libmutil/dbg.h>


#if defined WIN32 || defined _MSC_VER
#include<windows.h>
#define SEMHANDLE (*((HANDLE*)(handlePtr)))
#else
#include<semaphore.h>
#define SEMHANDLE (((sem_t*)(handlePtr)))
#endif


Semaphore::Semaphore(){
	
#ifdef WIN32
	handlePtr = (void*) new HANDLE;
	SEMHANDLE = CreateSemaphore(NULL, 0, 1000, NULL);
	if (SEMHANDLE == NULL){
		merror("Semaphore::Semaphore: CreateSemaphore");
		throw SemaphoreException();
	}
#else
	handlePtr = (void*) new sem_t;
	if (sem_init( SEMHANDLE, 0, 0)){
		merror("Semaphore::Semaphore: CreateSemaphore");
		throw SemaphoreException();
	}
	
#endif
}
 
Semaphore::~Semaphore(){
#ifdef WIN32
	if (!CloseHandle(SEMHANDLE)){
		merror("Semaphore::~Semaphore: CloseHandle");
	}
	delete (HANDLE*)handlePtr;
#else
	if (sem_destroy(SEMHANDLE)){
		perror("Semaphore::~Semaphore: sem_destroy");
	}
	delete (sem_t*)handlePtr;
#endif
}

void Semaphore::inc(){
#ifdef WIN32

	if (!ReleaseSemaphore(SEMHANDLE, 1, NULL)){
		merror("Semaphore::inc: ReleaseSemaphore");
		throw SemaphoreException();
	}
	
#else
	if( sem_post( SEMHANDLE ) ){
		merror("Semaphore::inc: sem_post");
		throw SemaphoreException();
	}
#endif
}

void Semaphore::dec(){
#ifdef WIN32
	int dwWaitResult = WaitForSingleObject( 
			SEMHANDLE,   // handle to semaphore
			INFINITE);          // zero-second time-out interval

	switch (dwWaitResult) 
	{ 
	case WAIT_OBJECT_0: 
		// OK .
		break; 

	case WAIT_TIMEOUT: 
		merror("Semaphore::dec: WaitForSingleObject");
		throw SemaphoreException();
		break; 
	}

#else
	while( sem_wait( SEMHANDLE ) ){
		switch( errno ){
			case EINTR:
				break;
			default:
				merror("Semaphore::dec: sem_wait");
				throw SemaphoreException();
		}
	}
	
#endif
}
