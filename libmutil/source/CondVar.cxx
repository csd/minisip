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

#include<config.h>
#include<libmutil/CondVar.h>

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
	/*INTERNAL_COND_WAIT*/ internalStruct = new pthread_cond_t;
	/*INTERNAL_MUTEX*/ internalMutexStruct = new pthread_mutex_t;

	pthread_cond_init( INTERNAL_COND_WAIT, NULL );
	pthread_mutex_init( INTERNAL_MUTEX, NULL );
	//pthread_mutex_lock( INTERNAL_MUTEX );
	init = true;
#elif defined WIN32
#define MINISIP_CONDVAR_IMPLEMENTED
	INTERNAL_COND_WAIT = CreateEvent( NULL, FALSE, FALSE, NULL );
	INTERNAL_MUTEX = CreateMutex(NULL, FALSE, NULL);
	WaitForSingleObject( INTERNAL_MUTEX, INFINITE );
	
#endif


#ifndef MINISIP_CONDVAR_IMPLEMENTED
#error CondVar not fully implemented
#endif
}

CondVar::~CondVar(){
#ifdef HAVE_PTHREAD_H
	pthread_mutex_destroy( INTERNAL_MUTEX );
	delete INTERNAL_MUTEX;

	pthread_cond_destroy( INTERNAL_COND_WAIT );
	delete INTERNAL_COND_WAIT;
#elif defined WIN32
	CloseHandle( INTERNAL_COND_WAIT );
	CloseHandle( INTERNAL_MUTEX );
#endif
}

void CondVar::wait( uint32_t timeout ){
#ifdef HAVE_PTHREAD_H
	if( init ){
		pthread_mutex_lock( INTERNAL_MUTEX );
		init = false;
	}
	if( timeout == 0 ){
		pthread_cond_wait( INTERNAL_COND_WAIT, INTERNAL_MUTEX );
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
		
		pthread_cond_timedwait( INTERNAL_COND_WAIT, INTERNAL_MUTEX,
				&ts );
	}
				
#elif defined WIN32
	if( timeout == 0 ){
		SignalObjectAndWait( INTERNAL_MUTEX, INTERNAL_COND_WAIT, 
			     INFINITE, FALSE );
	}
	else{
		SignalObjectAndWait( INTERNAL_MUTEX, INTERNAL_COND_WAIT,
		timeout, FALSE );
	}
	WaitForSingleObject( INTERNAL_MUTEX, INFINITE );
#endif
}

void CondVar::broadcast(){
#ifdef HAVE_PTHREAD_H
	pthread_cond_broadcast( INTERNAL_COND_WAIT );
#elif defined WIN32
	PulseEvent( INTERNAL_COND_WAIT );
#endif
}

void CondVar::signal(){
#ifdef HAVE_PTHREAD_H
	pthread_cond_signal( INTERNAL_COND_WAIT );
#elif defined WIN32
	PulseEvent( INTERNAL_COND_WAIT );
#endif
}



