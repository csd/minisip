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

#include<libmutil/Thread.h>

//#include"stdafx.h"
#include<windows.h>

#ifndef _MSC_VER
#include <unistd.h>
#endif	// _MSC_VER

#include<libmutil/dbg.h>
#include<libmutil/merror.h>
#include<libmutil/Exception.h>
#include<libmutil/massert.h>

using namespace std;

ThreadException::ThreadException(char *desc):Exception(desc){

}

void startFunction( void* (*f)() ){
	try{
		(*f)();
	}catch(Exception &e){
		cerr << "Thread: caught exception "<< flush << e.what()<<endl;
		cerr << "Thread: Stack trace:\n"+e.stackTrace()<<flush;
	}catch(Exception *e){
		cerr << "Thread: caught exception "<< flush << e->what()<<endl;
		cerr << "Thread: Stack trace:\n"+e->stackTrace()<<flush;
	}catch(exception &e){
		cerr << "Thread: caught exception:"<< flush << e.what()<<endl;
	}catch(exception *e){
		cerr << "Thread: caught exception:"<< flush << e->what()<<endl;
	}

}

void *startFunctionArg( void* (*f)(void*), void* arg){
	try{
		(*f)(arg);
	}catch(Exception &e){
		cerr << "Thread: caught exception "<< flush << e.what()<<endl;
		cerr << "Thread: Stack trace:\n"+e.stackTrace()<<flush;
	}catch(Exception *e){
		cerr << "Thread: caught exception "<< flush << e->what()<<endl;
		cerr << "Thread: Stack trace:\n"+e->stackTrace()<<flush;
	}catch(exception &e){
		cerr << "Thread: caught exception:"<< flush << e.what()<<endl;
	}catch(exception *e){
		cerr << "Thread: caught exception:"<< flush << e->what()<<endl;
	}

	return NULL;
}


void startRunnable(MRef<Runnable*> r){
	try{
		r->run();
	}catch(Exception &e){
		cerr << "Thread: caught exception "<< flush << e.what()<<endl;
		cerr << "Thread: Stack trace:\n"+e.stackTrace()<<flush;
	}catch(Exception *e){
		cerr << "Thread: caught exception "<< flush << e->what()<<endl;
		cerr << "Thread: Stack trace:\n"+e->stackTrace()<<flush;
	}catch(exception &e){
		cerr << "Thread: caught exception:"<< flush << e.what()<<endl;
	}catch(exception *e){
		cerr << "Thread: caught exception:"<< flush << e->what()<<endl;
	}
}

typedef struct tmpstruct{
    void *fun;
    void *arg;
} tmpstruct;

static DWORD WINAPI ThreadStarter( LPVOID lpParam ) { 
	MRef<Runnable *> self = *(static_cast<MRef <Runnable *> *>(lpParam));
	delete (static_cast<MRef <Runnable *> *>(lpParam));
	startRunnable(self);
	//self->run();
	return 0; 
} 

static DWORD WINAPI StaticThreadStarter(LPVOID lpParam) {
	void* (*f)();
	f=(void* (*)())lpParam;
	startFunction(f);
	//(*f)();
	return 0;
}

static DWORD WINAPI StaticThreadStarterArg(LPVOID lpParam)
{
	//printf("StaticThreadStarter: ALIVE - thread created\n");
        tmpstruct *tmp = (tmpstruct*)lpParam;
	void* (*f)(void*);
	f=(void* (*)(void*)) tmp->fun;
	void *arg = tmp->arg;
	delete tmp;
	//(*f)(tmp->arg);
	startFunctionArg(f, arg);
	
	return 0;
}


void setupDefaultSignalHandling(){
#ifdef DEBUG_OUTPUT
	cerr << "libmutil: setupDefaultSignalHandling: No stack trace signal handler available"<<endl;
#endif
}


Thread::Thread(MRef<Runnable *> runnable){
	massert(runnable);
        MRef<Runnable *> *self = new MRef<Runnable *>(runnable);
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
		merror("Thread::Thread: CreateThread");
                delete self;
		throw ThreadException("Could not create thread.");
        }
//	printf("In Thread, windows part - thread created\n");

}

Thread::~Thread(){
	if( handle_ptr ){
		delete (HANDLE *)handle_ptr;
	}
	handle_ptr = NULL;
}

ThreadHandle Thread::createThread(void f()){
	//HANDLE threadHandle;
	ThreadHandle handle;
	
	//massert(sizeof(threadHandle)==4);
	
	DWORD id;
	LPVOID fptr;
	fptr = (void*)f;
	//threadHandle = CreateThread( 
	*handle.hptr = CreateThread( 
			NULL,                        // default security attributes 
			0,                           // use default stack size 
			StaticThreadStarter,                  // thread function 
			fptr,                // argument to thread function
			0,                           // use default creation flags
			&id);

	if (*handle.hptr==NULL){
		merror("Thread::Thread: CreateThread");
		throw ThreadException("Could not create thread.");
	}
	return handle;
}

ThreadHandle Thread::createThread(void *f(void*), void *arg){
//        massert(1==0 /*UNIMPLEMENTED - ARGUMENT TO THREAD*/);

	tmpstruct *argptr = new struct tmpstruct;
        argptr->fun = (void*)f;
        argptr->arg = arg;
        
	//HANDLE threadHandle;
	ThreadHandle handle;
	DWORD id;
	
	#ifdef DEBUG_OUTPUT
		mdbg << "createThread: Creating thread" << end;
	#endif	// DEBUG_OUTPUT
	*handle.hptr = CreateThread( 
			NULL,                        // default security attributes 
			0,                           // use default stack size  
			StaticThreadStarterArg,                  // thread function 
			argptr,                // argument to thread function 
			0,                           // use default creation flags 
			&id);
	#ifdef DEBUG_OUTPUT
		mdbg << "createThread: done Creating thread" << end;
	#endif	// DEBUG_OUTPUT

	if (*handle.hptr==NULL){
		merror("Thread::createThread: CreateThread");
		throw ThreadException("Could not create thread.");
	}
	return handle;
}

void * Thread::join(){
	HANDLE handle = *((HANDLE*)handle.hptr);
	join(handle);
//	if (WaitForSingleObject( handle, INFINITE )==WAIT_FAILED){
//		merror("Thread::join: WaitForSingleObject");
//	}
        return NULL;
}

void Thread::join(ThreadHandle handle){
	HANDLE h = *((HANDLE*)handle.hptr);
	if (WaitForSingleObject( h, INFINITE )==WAIT_FAILED){
		merror("Thread::join:WaitForSingleObject");
	}
}

int Thread::msleep(int32_t ms){
	Sleep(ms); //function returns void
	return 0;
}


bool Thread::kill( ) {
	return kill( handle );
	
#if 0
	HANDLE h = *((HANDLE*)handle.hptr);
	BOOL ret;
	DWORD lpExitCode;

	GetExitCodeThread( h, &lpExitCode );
	ret = TerminateThread( h, lpExitCode );
        if( ret == 0 ) {
		
		#ifdef DEBUG_OUTPUT
			merror("Thread::kill: TerminateThread");
		#endif
		return false;
	}
	return true;
#endif	
}

bool Thread::kill( const ThreadHandle &handle) {
	HANDLE h = *((HANDLE*)handle.hptr);
	BOOL ret;
	DWORD lpExitCode;
	
	GetExitCodeThread( h, &lpExitCode );
	ret = TerminateThread( h, lpExitCode );
        if( ret == 0 ) {
		#ifdef DEBUG_OUTPUT
			merror("Thread::kill: TerminateThread");
		#endif
		return false;
	}
	return true;
}

ThreadHandle::ThreadHandle(){
	hptr = (void*) new HANDLE;
}

ThreadHandle::~ThreadHandle(){
	delete (HANDLE*)hptr;
	hptr=NULL;
}

ThreadHandle::ThreadHandle(const ThreadHandle &h){
	hptr = (void*)new HANDLE;
	*((HANDLE*)hptr)= *((HANDLE*)h.hptr);
}



