
#include<config.h>

#include<libmutil/MemObject.h>

#include<libmutil/itoa.h>
#include<assert.h>
#include<libmutil/Mutex.h>

MObject::MObject() : refCount(0){
}

MObject::~MObject(){
}


int MObject::decRefCount(){
	int ref;
	refLock.lock();
	ref=refCount--;
	refLock.unlock();
	return ref;
}

void MObject::incRefCount(){
	refLock.lock();
	refCount++;
	refLock.unlock();
}


