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
#include<libmutil/CondVar.h>
#include<libmutil/Mutex.h>
#include<libmutil/merror.h>
#include<libmutil/mtime.h>
#if defined HAVE_PTHREAD_H
#include<pthread.h>
#include<sys/time.h>
#include<time.h>

#define INTERNAL_COND_WAIT ((pthread_cond_t *)internalStruct)
#define INTERNAL_MUTEX ((pthread_mutex_t *)internalMutexStruct)

#endif


CondVar::CondVar(){
	condvarMutex = new Mutex;
	internalStruct = new pthread_cond_t;

	pthread_cond_init( INTERNAL_COND_WAIT, NULL );
}

CondVar::~CondVar(){
	delete condvarMutex;
	condvarMutex=NULL;
	pthread_cond_destroy( INTERNAL_COND_WAIT );
	delete INTERNAL_COND_WAIT;
}

std::string CondVar::getMemObjectType() const {
	return "CondVar";
}

void CondVar::wait( uint32_t timeout ){
	condvarMutex->lock();
	wait( *condvarMutex, timeout );
	condvarMutex->unlock();
}

void CondVar::wait( Mutex &mutex, uint32_t timeout ){
	if( timeout == 0 ){
		pthread_cond_wait( INTERNAL_COND_WAIT, (pthread_mutex_t*)(mutex.handle_ptr) );
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
		
		pthread_cond_timedwait( INTERNAL_COND_WAIT, (pthread_mutex_t*)mutex.handle_ptr,
				&ts );
	}
}

void CondVar::broadcast(){
	condvarMutex->lock();
	pthread_cond_broadcast( INTERNAL_COND_WAIT );
	condvarMutex->unlock();
}

