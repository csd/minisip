#include "tutil.h"


tutil::tutil () {
  basetimeptr=new timeval;
}

tutil::~tutil () {
  delete basetimeptr;
}

int tutil::gentimebase (double& a){ //function to store the time in milliseconds inside a
	// this function is used to generate the base time for the sending timestamp generation
	gettimeofday(basetimeptr, NULL);
	a=(double)(((double)(basetimeptr->tv_sec)*1000.0)+((double)(basetimeptr->tv_usec)/1000.0));
	return (0);
}

int tutil::genrandn (int& d) { //function to generate a random number up to 32767
	// this random number is the base to start the time counting. 
        int limit=32768;
	srand(time(NULL));
	d=rand()%limit;
	return(0);
}

void tutil::Sleep (int ms) { //funtion to sleep for ms milliseconds
  clock_t endwait;
  endwait = clock () + ms;
  while (clock() < endwait);
}
