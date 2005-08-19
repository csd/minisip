
#include<libmutil/Library.h>

#ifdef _MSC_VER
#include<Windows.h>
#else
#include<dlfcn.h>
#endif
#include<iostream>

using namespace std;
extern Mutex global;

Library::Library(const string &path):path(path){
#ifdef _MSC_VER
	assert(sizeof(FARPROC)==sizeof(void*));
	handle = new HMODULE;
	
	HMODULE *ptr =(HMODULE*)handle;
	*ptr = LoadLibrary( path.c_str() );
#else
	handle = dlopen(path.c_str(), RTLD_LAZY);
	if( !handle ){
		cerr << dlerror() << endl;
	}
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
		fprintf( stderr, "global: %x\n", &global );
		global.lock();
		global.unlock();
		dlclose(handle);
		fprintf( stderr, "global: %x\n", &global );
		global.lock();
		global.unlock();
		handle=NULL;
	}
#endif
}

void *Library::getFunctionPtr(string name){
#ifdef _MSC_VER
	assert(sizeof(FARPROC)==sizeof(void*));
	HMODULE *hptr =(HMODULE*)handle;
	return GetProcAddress(*hptr, name.c_str());
#else
	cerr << "Looking for symbol " << name << endl;
	void * ptr = dlsym(handle, name.c_str());
	cerr << "ptr: " << ptr << endl;
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
