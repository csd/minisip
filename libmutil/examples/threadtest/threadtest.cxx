
#include<libmutil/Thread.h>

#include<iostream>
#include<Windows.h>

using namespace std;

class Test : public Runnable{
public:
	void run(){
		int n=8;
		while (n--){
			cerr << "Thread in object\n"<< endl;
			Thread::sleep(1000);
		}
		cerr << "Thread in object done\n"<< endl;
	}
	
};

void fun(){
	int n=5;
	while (n--){
		cerr << "Thread in function\n"<< endl;
		Thread::sleep(1000);
	}
	cerr << "Thread in function done\n"<< endl;

}


int main(int argc, char **argv){
	cerr << "Hello world"<< endl;
	Test t;
	Thread objthread(&t);
	int funthread = Thread::createThread(fun);
	int n=3;
	while (n--){
		cerr << "Main thread\n"<< endl;
		Thread::sleep(1000);
	}
	
	cerr << "Main thread done, waiting for thread in object\n"<< endl;
	objthread.join();
	cerr << "Done waiting for thread"<< endl;
	return 0;
}

