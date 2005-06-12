
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

#include<libmutil/Semaphore.h>
#include<libmutil/Thread.h>
#include<iostream>

using namespace std;

Semaphore sem;


void producer(){
	for (int j=0; j<10; j++)
		sem.inc();
	for (int i=0; i< 20; i++){
		cerr << "producer inc\n"<< endl;
		sem.inc();
		Thread::sleep(rand()%1000);
	}
}

void consumer(){
	int n=0;
	while (true){
		sem.dec();
		cerr << "consumer dec n="<<++n<<"\n"<< endl;
	}
}


int main(int argc, char **argv){
	
	{Semaphore dummy;}

	int t1 = Thread::createThread(producer);
	int t2 = Thread::createThread(consumer);

	cerr << "Waiting for thread 1"<< endl;
	Thread::join(t1);
	cerr << "Waiting for thread 2 (cancel application manually!)"<< endl;
	Thread::join(t2);

	return 0;
}