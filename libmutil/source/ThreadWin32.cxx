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
#include<libmutil/mtime.h>

using namespace std;

ThreadException::ThreadException(const char *desc):Exception(desc){

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
	HANDLE h;
	DWORD threadId;

	h = CreateThread(
			NULL,                        // default security attributes
			0,                           // use default stack size
			ThreadStarter,                  // thread function
			(LPVOID) self,                // argument to thread function
			0,                           // use default creation flags
			&threadId);
	
	if (h==NULL){
		merror("Thread::Thread: CreateThread");
                delete self;
		throw ThreadException("Could not create thread.");
        }
	//*((HANDLE*)handle.hptr) = h;
	handle.handle = (uint64_t)h;

}

Thread::~Thread(){
}

ThreadHandle Thread::createThread(void f()){
	ThreadHandle handle;
	HANDLE h;
	DWORD id;
	LPVOID fptr;
	fptr = (void*)f;
	h = CreateThread( 
			NULL,                        // default security attributes 
			0,                           // use default stack size 
			StaticThreadStarter,                  // thread function 
			fptr,                // argument to thread function
			0,                           // use default creation flags
			&id);

	if (h==NULL){
		merror("Thread::Thread: CreateThread");
		throw ThreadException("Could not create thread.");
	}
	//*((HANDLE*)handle.hptr) = h;
	handle.handle = (uint64_t)h;

	return handle;
}

ThreadHandle Thread::createThread(void *f(void*), void *arg){
//        massert(1==0 /*UNIMPLEMENTED - ARGUMENT TO THREAD*/);

	tmpstruct *argptr = new struct tmpstruct;
        argptr->fun = (void*)f;
        argptr->arg = arg;
        
	ThreadHandle handle;
	HANDLE h;
	DWORD id;
	
	#ifdef DEBUG_OUTPUT
		mdbg << "createThread: Creating thread" << endl;
	#endif	// DEBUG_OUTPUT
		h = CreateThread( 
			NULL,                        // default security attributes 
			0,                           // use default stack size  
			StaticThreadStarterArg,                  // thread function 
			argptr,                // argument to thread function 
			0,                           // use default creation flags 
			&id);
	#ifdef DEBUG_OUTPUT
		mdbg << "createThread: done Creating thread" << endl;
	#endif	// DEBUG_OUTPUT

	if (h==NULL){
		merror("Thread::createThread: CreateThread");
		throw ThreadException("Could not create thread.");
	}
	//*((HANDLE*)handle.hptr) = h;
	handle.handle=(uint64_t)h;
	return handle;
}

void * Thread::join(){
	join(handle);
        return NULL;
}

void Thread::join(const ThreadHandle &handle){
	//HANDLE h = *((HANDLE*)handle.hptr);
	HANDLE h = (HANDLE)handle.handle;
	if (WaitForSingleObject( h, INFINITE )==WAIT_FAILED){
		merror("Thread::join:WaitForSingleObject");
	}
}

int Thread::msleep(int32_t ms){
	return ::msleep( ms );
}


bool Thread::kill( ) {
	return kill( handle );
}

bool Thread::kill( const ThreadHandle &handle) {
	//HANDLE h = *((HANDLE*)handle.hptr);
	HANDLE h = (HANDLE)handle.handle;
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

ThreadHandle Thread::getCurrent() {
	ThreadHandle th;
// 	*((HANDLE*)th.hptr) = GetCurrentProcess();
	th.handle = (uint64_t)GetCurrentProcess();
        return th;
}

ThreadHandle::ThreadHandle(){
	//hptr = (void*) new HANDLE;
	handle=0;
}

ThreadHandle::~ThreadHandle(){
//	delete (HANDLE*)hptr;
//	hptr=NULL;
}

ThreadHandle::ThreadHandle(const ThreadHandle &h){
	//hptr = (void*)new HANDLE;
	//*((HANDLE*)hptr)= *((HANDLE*)h.hptr);
	handle=h.handle;
}



LIBMUTIL_API void setThreadName(string,uint64_t){ }
LIBMUTIL_API void printThreads(){ }

