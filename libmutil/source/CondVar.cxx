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


#include<libmutil/CondVar.h>

#include<config.h>

#include<libmutil/Mutex.h>
#include<libmutil/merror.h>
#ifdef HAVE_PTHREAD_H
#include<pthread.h>
#include<sys/time.h>
#include<time.h>

#define INTERNAL_COND_WAIT ((pthread_cond_t *)internalStruct)
#define INTERNAL_MUTEX ((pthread_mutex_t *)internalMutexStruct)

#elif defined WIN32

#include<windows.h>

#define INTERNAL_COND_WAIT ((HANDLE *)internalStruct)
#define INTERNAL_MUTEX ((HANDLE *)internalMutexStruct)

#endif


CondVar::CondVar(){
#ifdef HAVE_PTHREAD_H
#define MINISIP_CONDVAR_IMPLEMENTED
	condvarMutex = new Mutex;
	internalStruct = new pthread_cond_t;

	pthread_cond_init( INTERNAL_COND_WAIT, NULL );
#elif defined _MSC_VER
#define MINISIP_CONDVAR_IMPLEMENTED
	internalStruct = new HANDLE;
	if ( (*INTERNAL_COND_WAIT = CreateEvent( NULL, TRUE, FALSE, NULL ))==NULL){
		merror("CondVar::CondVar: CreateEvent");
	}
#endif


#ifndef MINISIP_CONDVAR_IMPLEMENTED
#error CondVar not fully implemented
#endif
}

CondVar::~CondVar(){
#ifdef HAVE_PTHREAD_H
	delete condvarMutex;
	condvarMutex=NULL;
	pthread_cond_destroy( INTERNAL_COND_WAIT );
	delete INTERNAL_COND_WAIT;
#elif defined _MSC_VER
	if (!CloseHandle( *INTERNAL_COND_WAIT )){
		merror("CondVar::~CondVar: CloseHandle");
	}
	delete internalStruct;
	internalStruct=NULL;
#endif
}

void CondVar::wait( uint32_t timeout ){
#ifdef HAVE_PTHREAD_H
	condvarMutex->lock();
	if( timeout == 0 ){
		pthread_cond_wait( INTERNAL_COND_WAIT, (pthread_mutex_t*)(condvarMutex->handle_ptr) );
	}
	else{
		struct timeval now;
		struct timespec ts;
		gettimeofday(&now, NULL);
		ts.tv_sec = now.tv_sec;
		ts.tv_nsec = now.tv_usec*1000;
		
		ts.tv_sec += timeout / 1000;
		
		ts.tv_nsec += timeout % 1000 * 1000000;

		if (ts.tv_nsec > 999999999){
			ts.tv_sec++;
			ts.tv_nsec = ts.tv_nsec % 1000000000;
		}
		
		pthread_cond_timedwait( INTERNAL_COND_WAIT, (pthread_mutex_t*)condvarMutex->handle_ptr,
				&ts );
	}
	condvarMutex->unlock();
				
#elif defined WIN32
	if( timeout == 0 ){
		if (WaitForSingleObject(*INTERNAL_COND_WAIT, INFINITE)==WAIT_FAILED){
			merror("CondVar::wait: WaitForSingleObject");
		}
	}
	else{
		if (WaitForSingleObject(*INTERNAL_COND_WAIT, timeout)==WAIT_FAILED){
			merror("CondVar::wait: WaitForSingleObject");
		}
	}
#endif
}

void CondVar::broadcast(){
#ifdef HAVE_PTHREAD_H
	condvarMutex->lock();
	pthread_cond_broadcast( INTERNAL_COND_WAIT );
	condvarMutex->unlock();
#elif defined WIN32
	if (!SetEvent(*INTERNAL_COND_WAIT)){
		merror("CondVar::broadcast: SetEvent");
	}
	if (!ResetEvent(*INTERNAL_COND_WAIT)){
		merror("CondVar::broadcast: ResetEvent");
	}
#endif
}

