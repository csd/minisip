
#include<libmutil/Library.h>
#include<libmutil/massert.h>

#if defined _MSC_VER || __MINGW32__
#include<windows.h>
# define USE_WIN32_LIBS
#else
#include<dlfcn.h>
# define USE_POSIX_LIBS
#endif
#include<iostream>

using namespace std;

Library::Library(const string &path):path(path){
#ifdef USE_WIN32_LIBS
	massert(sizeof(FARPROC)==sizeof(void*));
	handle = new HMODULE;
	
	HMODULE *ptr =(HMODULE*)handle;
	*ptr = LoadLibrary( path.c_str() );
#else
	handle = dlopen(path.c_str(), RTLD_LAZY);
#ifdef DEBUG_OUTPUT
	if( !handle ){
		cerr << dlerror() << endl;
	}
#endif
#endif
}

Library::~Library(){
#ifdef USE_WIN32_LIBS
	if (!FreeLibrary(*((HMODULE*)handle)) ){
		cerr << "Library: ERROR: Could not close library."<< endl;
		
	}
	delete handle;
	handle=NULL;
#else
	if(handle){
		dlclose(handle);
		handle=NULL;
	}
#endif
}

void *Library::getFunctionPtr(string name){
#ifdef USE_WIN32_LIBS
	massert(sizeof(FARPROC)==sizeof(void*));
	HMODULE *hptr =(HMODULE*)handle;
	return (void*)GetProcAddress(*hptr, name.c_str());
#else
	void * ptr = dlsym(handle, name.c_str());
	return ptr;
#endif
}

MRef<Library *> Library::open(const string &path){
	MRef<Library *> ret = new Library(path);
	if(ret->handle){
		return ret;
	}
	ret = NULL;
	return ret;
}

const string &Library::getPath(){
	return path;
}
