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



#include<libmutil/Thread.h>


#include<config.h>

#ifdef WIN32 
//#include"stdafx.h"
#include<windows.h>
#endif

#ifndef _MSC_VER
#include <unistd.h>
#endif


#include<assert.h>
//#include<stdio.h>
#include<libmutil/dbg.h>

using namespace std;

ThreadException::ThreadException(string d):desc(d){

}

string ThreadException::what(){
	return desc;
}

#ifdef WIN32
#define MINISIP_THREAD_IMPLEMENTED
static DWORD WINAPI ThreadStarter( LPVOID lpParam ) 
{ 
        MRef<Runnable *> self = *(static_cast<MRef <Runnable *> *>(lpParam));
        delete (static_cast<MRef <Runnable *> *>(lpParam));

//	printf("ThreadStarter: thread created\n");
	self->run();
//	printf("ThreadStarter: thread terminated\n");
    return 0; 
} 

typedef struct tmpstruct{
    void *fun;
    void *arg;
} tmpstruct;

static DWORD WINAPI StaticThreadStarter(LPVOID lpParam)
{
//	printf("StaticThreadStarter: thread created\n");
	void (*f)();
	//f=(void())&lpParam;
	f=(void (*)())lpParam;
	(*f)();
//	((void (void)) lpParam)();
//	printf("StaticThreadStarter: thread terminated\n");
    return 0;
}


static DWORD WINAPI StaticThreadStarterArg(LPVOID lpParam)
{
	
	//printf("StaticThreadStarter: ALIVE - thread created\n");
        tmpstruct *tmp = (tmpstruct*)lpParam;
	void* (*f)(void*);
	//f=(void())&lpParam;
	//printf("StaticThreadStarter: running function");
	f=(void* (*)(void*)) tmp->fun;
	(*f)(tmp->arg);
//	((void (void)) lpParam)();
//	printf("StaticThreadStarter: thread terminated\n");
    return 0;
}




#endif //WIN32

#ifdef HAVE_PTHREAD_H
#define MINISIP_THREAD_IMPLEMENTED
static void *LinuxThreadStarter(void *arg){
	/* Keep a reference to yourself as long as you run */
	MRef<Runnable *> self = *(static_cast<MRef <Runnable *> *>(arg));
	delete (static_cast<MRef <Runnable *> *>(arg));
	#ifdef DEBUG_OUTPUT
		mdbg << "LinuxThreadStarter: thread created"<< end;
	#endif
	
	self->run();
	
	#ifdef DEBUG_OUTPUT
		mdbg <<"LinuxThreadStarter: thread terminated"<< end;
	#endif
	return NULL;
	//pthread_exit( (void *) 0 ); //cesc
}

static void *LinuxStaticThreadStarter(void *arg){
	#ifdef DEBUG_OUTPUT
		mdbg << "LinuxStaticThreadStarter: thread created"<< end;
	#endif
	void (*f)();
	f=(void (*)())arg;
	(*f)();
	
	#ifdef DEBUG_OUTPUT
		mdbg <<"LinuxStaticThreadStarter: thread terminated"<< end;
	#endif
	return NULL;
}

#endif


#ifndef MINISIP_THREAD_IMPLEMENTED
#error Thread not fully implemented
#endif


Thread::Thread(MRef<Runnable *> runnable){
	assert(runnable);
        MRef<Runnable *> *self = new MRef<Runnable *>(runnable);
#ifdef WIN32
	DWORD threadId;

	handle_ptr = new HANDLE;

	*((HANDLE*)handle_ptr) = CreateThread(
			NULL,                        // default security attributes
			0,                           // use default stack size
			ThreadStarter,                  // thread function
			(LPVOID) self,                // argument to thread function
			0,                           // use default creation flags
			&threadId);
	
	if (*((HANDLE*)handle_ptr)==NULL){
                delete self;
		throw new ThreadException("Could not create thread.");
        }
//	printf("In Thread, windows part - thread created\n");

#endif //WIN32

#ifdef HAVE_PTHREAD_H
	//handle_ptr = malloc(sizeof(pthread_t));
	handle_ptr = new pthread_t;
	int ret;
	
	//set attribs for the threads ...
/*	
	pthread_attr_t attr;
	pthread_attr_init( &attr);
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );
	//pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL );
	//pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL );
*/
	
	ret = pthread_create(
			(pthread_t*)handle_ptr, 
			NULL, // either NULL or &attr,
			LinuxThreadStarter, 
			self);
	
// 	pthread_attr_destroy( &attr );
	
	if ( ret != 0 ){
                delete self;
		#ifdef DEBUG_OUTPUT
			merr << "In Thread, linux part - thread NOT created" << end;
		#endif
		throw new ThreadException("Could not create thread.");
	}
	#ifdef DEBUG_OUTPUT
		mdbg << "In Thread, linux part - thread created" << end;
	#endif
	
#endif

}

Thread::~Thread(){
	if( handle_ptr ){
#ifdef WIN32
		delete (HANDLE *)handle_ptr;
#endif
#ifdef HAVE_PTHREAD_H
		delete (pthread_t *)handle_ptr;
#endif
	}
	handle_ptr = NULL;
}

int Thread::createThread(void f()){
#ifdef WIN32
	HANDLE threadHandle;
	assert(sizeof(threadHandle)==4);
	DWORD id;
	LPVOID fptr;
	fptr = (void*)f;
	threadHandle = CreateThread( 
			NULL,                        // default security attributes 
			0,                           // use default stack size 
			StaticThreadStarter,                  // thread function 
			fptr,                // argument to thread function
			0,                           // use default creation flags
			&id);

	if (threadHandle==NULL)
		throw new ThreadException("Could not create thread.");
	return (int)threadHandle;
#endif
	
#ifdef HAVE_PTHREAD_H
	pthread_t threadHandle;
	#ifdef DEBUG_OUTPUT
 		mdbg << "Running createThread"<< end;
	#endif
	pthread_create(&threadHandle, NULL, LinuxStaticThreadStarter, (void*)f);
	return (int)threadHandle;
#endif

}

int Thread::createThread(void *f(void*), void *arg){
#ifdef WIN32
//        assert(1==0 /*UNIMPLEMENTED - ARGUMENT TO THREAD*/);

	tmpstruct *argptr = new struct tmpstruct;
        argptr->fun = (void*)f;
        argptr->arg = arg;
        
	HANDLE threadHandle;
	DWORD id;
	
	#ifdef DEBUG_OUTPUT
		mdbg << "createThread: Creating thread" << end;
	#endif
	threadHandle = CreateThread( 
			NULL,                        // default security attributes 
			0,                           // use default stack size  
			StaticThreadStarterArg,                  // thread function 
			argptr,                // argument to thread function 
			0,                           // use default creation flags 
			&id);
	#ifdef DEBUG_OUTPUT
		mdbg << "createThread: done Creating thread" << end;
	#endif

	if (threadHandle==NULL)
		throw new ThreadException("Could not create thread.");
	return (int)threadHandle;
#endif
	
#ifdef HAVE_PTHREAD_H
	pthread_t threadHandle;
	#ifdef DEBUG_OUTPUT
		mdbg << "Running createThread" << end;
	#endif
	pthread_create(&threadHandle, NULL, f, arg);
	return (int)threadHandle;
#endif

}

void * Thread::join(){
#ifdef HAVE_PTHREAD_H
	void * returnValue;
	int ret;
	
	#ifdef DEBUG_OUTPUT
		mdbg << "Thread::join(): before join" << end;
	#endif
	ret = pthread_join( 
			*( (pthread_t *)handle_ptr ), 
			&returnValue );
	
	if( ret != 0 ){
		#ifdef DEBUG_OUTPUT
			merr << "Thread::join(): ERROR" << end;
		#endif
		return NULL;
	} 
	
	return returnValue;
        
#elif defined WIN32
	HANDLE handle = *((HANDLE*)handle_ptr);
        WaitForSingleObject( handle, INFINITE );
        return NULL;
#endif
}

void Thread::join(int handle){
#ifdef _MSC_VER
	HANDLE h = (HANDLE)handle;
	WaitForSingleObject( h, INFINITE );
#else
	if( pthread_join( handle, NULL) ){
		#ifdef DEBUG_OUTPUT
			mdbg << "Thread::join(): ERROR" << end;
		#endif
	}
#endif
}

void Thread::msleep(int ms){
#ifdef _MSC_VER
	Sleep(ms);
#else
	usleep(ms * 1000);
#endif
}


bool Thread::kill( ) {
#ifdef HAVE_PTHREAD_H
	int ret;
	
	#ifdef DEBUG_OUTPUT
		mdbg << "Thread::kill(): before cancel" << end;
	#endif
	ret = pthread_cancel( *( (pthread_t *)handle_ptr ) );
	
	if( ret != 0 ){
		#ifdef DEBUG_OUTPUT
			merr << "Thread::kill(): ERROR" << end;
		#endif
		return false;
	} 
	
	return true;
        
#elif defined WIN32
	HANDLE handle = *((HANDLE*)handle_ptr);
	BOOL ret;
	
	ret = TerminateTHread( handle, NULL );
        if( ret == 0 ) {
		#ifdef DEBUG_OUTPUT
			merr << "Thread::kill(): ERROR" << end;
		#endif
		return false;
	}
	return true;
#endif
}

bool Thread::kill( int handle) {
#ifdef HAVE_PTHREAD_H
	int ret;
	
	#ifdef DEBUG_OUTPUT
		mdbg << "Thread::kill(): before cancel" << end;
	#endif
	ret = pthread_cancel( handle );
	
	if( ret != 0 ){
		#ifdef DEBUG_OUTPUT
			merr << "Thread::kill(): ERROR" << end;
		#endif
		return false;
	} 
	
	return true;
        
#elif defined WIN32
	HANDLE h = (HANDLE)handle;
	BOOL ret;
	
	ret = TerminateTHread( h, NULL );
        if( ret == 0 ) {
		#ifdef DEBUG_OUTPUT
			merr << "Thread::kill(): ERROR" << end;
		#endif
		return false;
	}
	return true;
#endif
}


