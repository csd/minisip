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


#include<semaphore.h>
#define SEMHANDLE (((sem_t*)(handlePtr)))


SemaphoreException::SemaphoreException(std::string w)
	: Exception(w)
{

}

static int X=0;

Semaphore::Semaphore(){
// Unnamed semaphores are not implemented (although the functions exist)
// on OS X resulting in run-time exception. Use named semaphore instead.
#ifdef DARWIN
	X+=1000000;
	string name="/mutil"+itoa(rand()+X)+itoa(time(NULL));
	printf("EE: creating semaphore <%s>\n", name.c_str() );
	if ( (handlePtr=sem_open(name.c_str(), O_CREAT | O_EXCL, 0777, 0 ))==SEM_FAILED){

		merror("Semaphore::Semaphore: sem_open");
		throw SemaphoreException("Could not create Semaphore (sem_open())");

	}
#else
	handlePtr = (void*) new sem_t;
	if (sem_init( SEMHANDLE, 0, 0)){
		merror("Semaphore::Semaphore: sem_init");
		throw SemaphoreException("Could not create Semaphore (sem_init())");
	}
#endif
}
 
Semaphore::~Semaphore(){
#ifdef DARWIN
	if (sem_close(SEMHANDLE)){
		merror("Semaphore::~Semaphore: sem_close");
	}
#else
	if (sem_destroy(SEMHANDLE)){
		merror("Semaphore::~Semaphore: sem_destroy");
	}
	delete (sem_t*)handlePtr;
#endif
}

void Semaphore::inc(){
	if( sem_post( SEMHANDLE ) ){
		merror("Semaphore::inc: sem_post");
		throw SemaphoreException("sem_post() failed");
	}
}

void Semaphore::dec(){
	while( sem_wait( SEMHANDLE ) ){
		switch( errno ){
			case EINTR:
				break;
			default:
				merror("Semaphore::dec: sem_wait");
				throw SemaphoreException("sem_wait() failed");
		}
	}
}
