
#include<libmutil/Library.h>
#include<libmutil/massert.h>

#ifdef _MSC_VER
#include<Windows.h>
#else
#include<dlfcn.h>
#endif
#include<iostream>

using namespace std;

Library::Library(const string &path):path(path){
#ifdef _MSC_VER
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
#ifdef _MSC_VER
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
#ifdef _MSC_VER
	massert(sizeof(FARPROC)==sizeof(void*));
	HMODULE *hptr =(HMODULE*)handle;
	return GetProcAddress(*hptr, name.c_str());
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
