/* mutextest: distributed with @PACKAGE@-@PACKAGE_VERSION@ */
#include<libmutil/Thread.h>
#include<libmutil/Mutex.h>
#include<iostream>
#include<assert.h>

using namespace std;

Mutex m;

static int n=0;

void f(){
	m.lock();
	assert(n==0);
	for (int i=0; i<10000; i++)
		n++;
	for (int i=0; i<10000; i++)
		n--;
	assert(n==0);
	m.unlock();
}

void loop(){
	for (int i=0; i<10000; i++){
		f();
	}
}


int main(int argc, char **argv){
	cerr << "Hello world"<< endl;
	{Mutex dummy;}
	
	ThreadHandle t1 = Thread::createThread(loop);
	ThreadHandle t2 = Thread::createThread(loop);

	cerr << "Waiting for thread 1"<< endl;
	Thread::join(t1);
	cerr << "Waiting for thread 2"<< endl;
	Thread::join(t2);

	return 0;
}

