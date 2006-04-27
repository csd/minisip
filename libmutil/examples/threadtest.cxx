/* threadtest: distributed with @PACKAGE@-@PACKAGE_VERSION@ */
#include<libmutil/Thread.h>
#include<iostream>

using namespace std;

class Test : public Runnable{
public:
	void run(){
		int n=8;
		while (n--){
			cerr << "Thread in object"<< endl;
			Thread::msleep(rand() % 1000);
		}
		cerr << "Thread in object done"<< endl;
	}
	
};

void fun(){
	int n=5;
	while (n--){
		cerr << "Thread in function"<< endl;
		Thread::msleep(rand() % 1000);
	}
	cerr << "Thread in function done"<< endl;

}


int main(int argc, char **argv){
	cerr << "Hello world"<< endl;
	Test *t = new Test();
	Thread objthread(t);
	ThreadHandle funthread = Thread::createThread(fun);
	int n=3;
	while (n--){
		cerr << "Main thread"<< endl;
		Thread::msleep(rand() % 1000);
	}
	
	cerr << "Main thread done, waiting for thread in object"<< endl;
	objthread.join();
	cerr << "Done waiting for thread"<< endl;
	return 0;
}

