/* semaphoretest: distributed with @PACKAGE@-@PACKAGE_VERSION@ */
/**
 * Purpose: Test the most basic functionality of the Semaphore implementation.
 * 
 * The application starts two threads that acts as producer and consumers. The
 * producer starts by producing 11 units, and then sleeps for [0..1] seconds.
 * For each produced item a line is output. The consumer thread prints a line
 * for each unit.
 * When the semaphore implementation works, there should first be 11 
 * producer/consumer lines output (typically with all 11 producer messages first)
 * and then another nine producer/consumer lines output during ~5 seconds. 
 * The final consumer line should read "consumer dec n=30". The application
 * will have to be canceled manually.
 */

#include<libmutil/Thread.h>
#include<libmutil/Mutex.h>
#include<libmutil/Semaphore.h>
#include<iostream>

using namespace std;

Semaphore sem;
Semaphore done;

Mutex m;
volatile bool _done = false;
volatile int p = 0, c = 0;

void produce_n() { m.lock(); p++; m.unlock(); sem.inc(); }
void set_done(bool v) { m.lock(); _done = v; m.unlock(); sem.inc(); }

void producer(){
	cerr << "producer: Starting thread..." << endl;
	for (int j=0; j<10; j++){
		sem.inc();
		Thread::msleep(rand()%200);
	}
	for (int i=0; i< 20; i++){
		cerr << "producer: inc" << endl;
		produce_n();
		
		Thread::msleep(rand()%200);
	}
	cerr << "producer: ... setting done flag..." << endl;
	set_done(true);
	cerr << "producer: ... and exiting." << endl;
}

bool _can_consume() {
	if (c < p) return 1;
	m.unlock(); 
	sem.dec(); 
	m.lock(); 
	return c < p;
}
int consume_n() { 
	int n = -1; 
	m.lock();
	while (!_can_consume() && !_done) ;
	if (!_done) n = ++c; 
	m.unlock(); 
	return n; 
}

void consumer(){
	cerr << "consumer: Starting thread..."<< endl;
	while (true){
		int n = consume_n();
		if (-1 == n) break;
		cerr << "consumer: step n=" << n << endl;
		Thread::msleep(rand()%200);
	}
	cerr << "consumer: got done flag..." << endl;
}


int main(int argc, char **argv){
	srand(time(NULL));
	cerr << "semaphoretest: Starting producer thread..." << endl;
	ThreadHandle t1 = Thread::createThread(producer);
	cerr << "semaphoretest: ... starting consumer thread..." << endl;
	ThreadHandle t2 = Thread::createThread(consumer);
	cerr << "semaphoretest: ... waiting for thread 1" << endl;
	Thread::join(t1);
	cerr << "semaphoretest: ... waiting for thread 2" << endl;
	Thread::join(t2);
	cerr << "semaphoretest: ... done!" << endl;
	return 0;
}
