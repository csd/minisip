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


#include<libmutil/Thread.h>


#include<config.h>

#ifdef WIN32
//#include"stdafx.h"
#include<windows.h>
#endif

#include<stdio.h>

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
//	printf("ThreadStarter: thread created\n");
	((Runnable *) lpParam)->run();
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
	
	printf("StaticThreadStarter: ALIVE - thread created\n");
        tmpstruct *tmp = (tmpstruct*)lpParam;
	void* (*f)(void*);
	//f=(void())&lpParam;
	printf("StaticThreadStarter: running function");
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
//	cerr << "LinuxThreadStarter: thread created"<< endl;
	((Runnable *)arg)->run();
//	cerr <<"LinuxThreadStarter: thread terminated"<< endl;
	return NULL;
}

static void *LinuxStaticThreadStarter(void *arg){
//	printf("LinuxStaticThreadStarter: thread created");
	void (*f)();
	//f=(void())&lpParam;
	f=(void (*)())arg;
	(*f)();
//	printf("LinuxStaticThreadStarter: thread terminated");
	return NULL;
}

/*
static void *LinuxStaticThreadStarter(void *obj, void *arg){
//	printf("LinuxStaticThreadStarter: thread created");
	void* (*f)(void*);
	//f=(void())&lpParam;
	f=(void* (*)(void*))obj;
	(*f)(arg);
//	printf("LinuxStaticThreadStarter: thread terminated");
	return NULL;
}
*/
#endif


#ifndef MINISIP_THREAD_IMPLEMENTED
#error Thread not fully implemented
#endif


Thread::Thread(Runnable *runnable){
#ifdef WIN32
	DWORD threadId;

	handle_ptr = malloc(sizeof(HANDLE));
//	threadHandle = CreateThread(
	*((HANDLE*)handle_ptr) = CreateThread(
			NULL,                        // default security attributes
			0,                           // use default stack size
			ThreadStarter,                  // thread function
			(LPVOID) runnable,                // argument to thread function
			0,                           // use default creation flags
			&threadId);
//	if (threadHandle==NULL)
	if (*((HANDLE*)handle_ptr)==NULL)
		throw new ThreadException("Could not create thread.");
//	printf("In Thread, windows part - thread created\n");

#endif //WIN32

#ifdef HAVE_PTHREAD_H
	handle_ptr = malloc(sizeof(pthread_t));
//	if (pthread_create(&threadHandle, NULL, LinuxThreadStarter, runnable)){
	if (pthread_create((pthread_t*)handle_ptr, NULL, LinuxThreadStarter, runnable)){
		throw new ThreadException("Could not create thread.");
	}
//	printf("In Thread, linux part - thread created\n");
	
#endif

}

int Thread::createThread(void f()){
#ifdef WIN32
	HANDLE threadHandle;
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
//	cerr << "Running createThread"<< endl;
	pthread_create(&threadHandle, NULL, LinuxStaticThreadStarter, (void*)f);
	return (int)threadHandle;
#endif

}

int Thread::createThread(void *f(void*), void *arg){
#ifdef WIN32
//        assert(1==0 /*UNIMPLEMENTED - ARGUMENT TO THREAD*/);

        tmpstruct *argptr = (tmpstruct*)malloc(sizeof (tmpstruct));
        argptr->fun = (void*)f;
        argptr->arg = arg;
        
	HANDLE threadHandle;
	DWORD id;
	
	printf("createThread: Creating thread\n");
	threadHandle = CreateThread( 
			NULL,                        // default security attributes 
			0,                           // use default stack size  
			StaticThreadStarterArg,                  // thread function 
			argptr,                // argument to thread function 
			0,                           // use default creation flags 
			&id);
	printf("createThread: done Creating thread\n");

	if (threadHandle==NULL)
		throw new ThreadException("Could not create thread.");
	return (int)threadHandle;
#endif
	
#ifdef HAVE_PTHREAD_H
	pthread_t threadHandle;
//	cerr << "Running createThread"<< endl;
	pthread_create(&threadHandle, NULL, f, arg);
	return (int)threadHandle;
#endif

}


