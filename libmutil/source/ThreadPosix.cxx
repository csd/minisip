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


#include <unistd.h>

#ifdef HAVE_PTHREAD_H
#include<pthread.h>
#include<signal.h>
#endif	// HAVE_PTHREAD_H

#ifdef HAVE_EXECINFO_H
#include<execinfo.h>
#endif	// HAVE_EXECINFO_H

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

#ifdef HAVE_EXECINFO_H
#include <ucontext.h>
#include <dlfcn.h>

#if defined(REG_RIP)
#define REGFORMAT "%016lx"
#elif defined(REG_EIP)
#define REGFORMAT "%08x"
#else
#define REGFORMAT "%x"
#endif	// defined(REG_RIP)

//Thanks to Jaco Kroon for this function
//http://tlug.up.ac.za/wiki/index.php/Obtaining_a_stack_trace_in_C_upon_SIGSEGV
static void signalHandler(int signum, siginfo_t* info, void*ptr) {
    static const char *si_codes[3] = {"", "SEGV_MAPERR", "SEGV_ACCERR"};

    int f = 0;
    ucontext_t *ucontext = (ucontext_t*)ptr;
    Dl_info dlinfo;
    void **bp = 0;
    void *ip = 0;

    fprintf(stderr, "EXCEPTION CAUGHT:\n");
    fprintf(stderr, "info.si_signo = %d\n", signum);
    fprintf(stderr, "info.si_errno = %d\n", info->si_errno);
    fprintf(stderr, "info.si_code  = %d (%s)\n", info->si_code, si_codes[info->si_code]);
    fprintf(stderr, "info.si_addr  = %p\n", info->si_addr);
    
/* For register content, enable the following two lines: 
    for(i = 0; i < NGREG; i++)
        fprintf(stderr, "reg[%02d]       = 0x" REGFORMAT "\n", i, ucontext->uc_mcontext.gregs[i]);
*/
#if defined(REG_RIP)
    ip = (void*)ucontext->uc_mcontext.gregs[REG_RIP];
    bp = (void**)ucontext->uc_mcontext.gregs[REG_RBP];
#elif defined(REG_EIP)
    ip = (void*)ucontext->uc_mcontext.gregs[REG_EIP];
    bp = (void**)ucontext->uc_mcontext.gregs[REG_EBP];
#else
    fprintf(stderr, "Unable to retrieve Instruction Pointer (not printing stack trace).\n");
#define SIGSEGV_NOSTACK
#endif	// defined(REG_RIP)

#ifndef SIGSEGV_NOSTACK
    fprintf(stderr, "Stack trace:\n");
    while(bp && ip) {
        if(!dladdr(ip, &dlinfo))
            break;

#if __WORDSIZE == 64
        fprintf(stderr, "% 2d: %p <%s+%lu> (%s)\n",
                ++f,
                ip,
                dlinfo.dli_sname,
                (unsigned long)((unsigned long)ip - (unsigned long)dlinfo.dli_saddr),
                dlinfo.dli_fname);
#else
        fprintf(stderr, "% 2d: %p <%s+%u> (%s)\n",
                ++f,
                ip,
                dlinfo.dli_sname,
                (unsigned)((unsigned)ip - (unsigned)dlinfo.dli_saddr),
                dlinfo.dli_fname);

#endif
        if(dlinfo.dli_sname && !strcmp(dlinfo.dli_sname, "main"))
            break;

        ip = bp[1];
        bp = (void**)bp[0];
    }
    fprintf(stderr, "End of stack trace\n");
#endif	// SIGSEGV_NOSTACK
}


static bool handleSignal(int sig){
	struct sigaction sa;
	sa.sa_handler = NULL;
	sa.sa_sigaction =  signalHandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_ONESHOT | SA_SIGINFO;
	return sigaction(sig, &sa, NULL) == 0;
}

#endif	// HAVE_EXECINFO_H



#ifdef HAVE_EXECINFO_H
/**
 *  
 *
 */
#define SIGNAL_HANDLER_DECLARED
void setupDefaultSignalHandling(){	

	if (!handleSignal(SIGSEGV)){
		cerr << "Thread: Could not install stack trace output for the SIGSEGV signal"<<endl;
	}
	if (!handleSignal(SIGBUS)){
		cerr << "Thread: Could not install stack trace output for the SIGSEGV signal"<<endl;
	}
	if (!handleSignal(SIGFPE)){
		cerr << "Thread: Could not install stack trace output for the SIGSEGV signal"<<endl;
	}
	if (!handleSignal(SIGILL)){
		cerr << "Thread: Could not install stack trace output for the SIGSEGV signal"<<endl;
	}
	if (!handleSignal(SIGSYS)){
		cerr << "Thread: Could not install stack trace output for the SIGSEGV signal"<<endl;
	}
}
#endif	// HAVE_EXECINFO_H

static void *LinuxThreadStarter(void *arg){
#ifdef HAVE_EXECINFO_H
	setupDefaultSignalHandling();
#endif	// HAVE_EXECINFO_H
	/* Keep a reference to yourself as long as you run */
	MRef<Runnable *> self = *(static_cast<MRef <Runnable *> *>(arg));
	delete (static_cast<MRef <Runnable *> *>(arg));
#ifdef DEBUG_OUTPUT
	mdbg << "LinuxThreadStarter: thread created"<< end;
#endif	// DEBUG_OUTPUT

	startRunnable(self);
	//self->run();

#ifdef DEBUG_OUTPUT
	mdbg <<"LinuxThreadStarter: thread terminated"<< end;
#endif	// DEBUG_OUTPUT
	return NULL;
	//pthread_exit( (void *) 0 ); //cesc
}

static void *LinuxStaticThreadStarter(void *arg){
#ifdef HAVE_EXECINFO_H
	setupDefaultSignalHandling();
#endif	// HAVE_EXECINFO_H
	#ifdef DEBUG_OUTPUT
		mdbg << "LinuxStaticThreadStarter: thread created"<< end;
	#endif
	void* (*f)();
	f=(void* (*)())arg;
	//(*f)();
	startFunction(f);
	#ifdef DEBUG_OUTPUT
		mdbg <<"LinuxStaticThreadStarter: thread terminated"<< end;
	#endif	// DEBUG_OUTPUT
	return NULL;
}

static void *LinuxStaticThreadStarterArg(void *arg){
#ifdef HAVE_EXECINFO_H
	setupDefaultSignalHandling();
#endif	// HAVE_EXECINFO_H
	#ifdef DEBUG_OUTPUT
		mdbg << "LinuxStaticThreadStarter: thread created"<< end;
	#endif	// DEBUG_OUTPUT
        tmpstruct *tmp = (tmpstruct*)arg;
        void* (*f)(void*);
        f=(void* (*)(void*)) tmp->fun;
        void *argptr = tmp->arg;
        delete tmp;
        startFunctionArg(f, argptr);

	#ifdef DEBUG_OUTPUT
		mdbg <<"LinuxStaticThreadStarter: thread terminated"<< end;
	#endif	// DEBUG_OUTPUT
	return NULL;
}



#ifndef SIGNAL_HANDLER_DECLARED
void setupDefaultSignalHandling(){
#ifdef DEBUG_OUTPUT
	cerr << "libmutil: setupDefaultSignalHandling: No stack trace signal handler available"<<endl;
#endif	// DEBUG_OUTPUT
}
#endif	// SIGNAL_HANDLER_DECLARED



Thread::Thread(MRef<Runnable *> runnable){
	massert(runnable);
        MRef<Runnable *> *self = new MRef<Runnable *>(runnable);

	//handle_ptr = malloc(sizeof(pthread_t));
	//handle_ptr = new pthread_t;
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
			//(pthread_t*)handle_ptr, 
			(pthread_t*)handle.hptr, 
			NULL, // either NULL or &attr,
			LinuxThreadStarter, 
			self);
	
// 	pthread_attr_destroy( &attr );
	
	if ( ret != 0 ){
		merror("Thread::Thread: pthread_create");
                delete self;
		#ifdef DEBUG_OUTPUT
			merr << "In Thread, linux part - thread NOT created" << end;
		#endif	// DEBUG_OUTPUT
		throw ThreadException("Could not create thread.");
	}
	#ifdef DEBUG_OUTPUT
		mdbg << "In Thread, linux part - thread created" << end;
	#endif	// DEBUG_OUTPUT

}

Thread::~Thread(){
//	if( handle_ptr ){
//		delete (pthread_t *)handle_ptr;
//	}
//	handle_ptr = NULL;
}


ThreadHandle Thread::createThread(void f()){
	//pthread_t threadHandle;
	ThreadHandle handle;
	#ifdef DEBUG_OUTPUT
 		mdbg << "Running createThread"<< end;
	#endif	// DEBUG_OUTPUT
	pthread_create((pthread_t*)handle.hptr, NULL, LinuxStaticThreadStarter, (void*)f);
	return handle;
}

ThreadHandle Thread::createThread(void *f(void*), void *arg){
	tmpstruct *argptr = new struct tmpstruct;
        argptr->fun = (void*)f;
        argptr->arg = arg;
 
	//pthread_t threadHandle;
	ThreadHandle handle;
	#ifdef DEBUG_OUTPUT
		mdbg << "Running createThread" << end;
	#endif	// DEBUG_OUTPUT
	pthread_create((pthread_t*)handle.hptr, NULL, LinuxStaticThreadStarterArg, argptr);
	return handle;
}

void * Thread::join(){
	void * returnValue;
	int ret;
	
	#ifdef DEBUG_OUTPUT
		mdbg << "Thread::join(): before join" << end;
	#endif	// DEBUG_OUTPUT
	ret = pthread_join( 
			// *( (pthread_t *)handle_ptr ), 
			*((pthread_t *)handle.hptr), 
			&returnValue );
	
	if( ret != 0 ){
		
		#ifdef DEBUG_OUTPUT
			merror("Thread::join: pthread_join");
		#endif	// DEBUG_OUTPUT
		return NULL;
	} 
	
	return returnValue;
}

void Thread::join(const ThreadHandle &handle){
	if( pthread_join( *((pthread_t*)handle.hptr), NULL) ){
		#ifdef DEBUG_OUTPUT
			merror("Thread::join: pthread_join");
		#endif
	}
}

int Thread::msleep(int32_t ms){
	struct timespec request;
	request.tv_sec = ms/1000;
	request.tv_nsec = (long) (ms%1000) * 1000 * 1000;
	return nanosleep( &request, NULL );
}


bool Thread::kill( ) {
	int ret;
	
	#ifdef DEBUG_OUTPUT
		mdbg << "Thread::kill(): before cancel" << end;
	#endif	// DEBUG_OUTPUT
	//ret = pthread_cancel( *( (pthread_t *)handle_ptr ) );
	ret = pthread_cancel( *( (pthread_t *)handle.hptr) );
	
	if( ret != 0 ){
		#ifdef DEBUG_OUTPUT
			merr << "Thread::kill(): ERROR" << end;
		#endif	// DEBUG_OUTPUT
		return false;
	} 
	
	return true;
}

bool Thread::kill( const ThreadHandle &handle) {
	int ret;
	
	#ifdef DEBUG_OUTPUT
		mdbg << "Thread::kill(): before cancel" << end;
	#endif	// DEBUG_OUTPUT
	ret = pthread_cancel( *((pthread_t*)handle.hptr) );
	
	if( ret != 0 ){
		#ifdef DEBUG_OUTPUT
			merr << "Thread::kill(): ERROR" << end;
		#endif	// DEBUG_OUTPUT
		return false;
	} 
	
	return true;
}

ThreadHandle::ThreadHandle(){
	hptr = (void*)new pthread_t;
}

ThreadHandle::~ThreadHandle(){
	delete (pthread_t*)hptr;
	hptr=NULL;
}

ThreadHandle::ThreadHandle(const ThreadHandle &h){
	hptr = (void*)new pthread_t;
	*((pthread_t*)hptr)= *((pthread_t*)h.hptr);
}



