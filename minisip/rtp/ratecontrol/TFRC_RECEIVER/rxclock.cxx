//TFRC RX CLOCK

#include <iostream>
#include <cstdlib>
//#include <sys/time.h>
#include "rxclock.h"
using namespace std;

rxclock::rxclock() {
	//stimeptr=new timeval;
}

rxclock::~rxclock () {
	//delete stimeptr;
}

double rxclock::get_timems (void) {
	//gettimeofday(stimeptr, NULL);
	return ((double) mtime()); 
}

int fbtimer::set_to (double top) {
	timeout=top;
}

fbtimer::fbtimer (packhist * phistop) {
	phisto= phistop;
}

fbtimer::~fbtimer () {
}

int fbtimer::resetto (void) {
  //cout << "en resetto ahora es " << get_timems() << "\n";
  //cout << "en resetto se añada un rrt de " << phisto->get_pack(0).rtt << "\n";
  timeout=(get_timems())+((phisto->get_pack(0)).rtt);
  //printf("ahora es: %20e\n", get_timems());
  //printf("tiout es: %20e\n", timeout);
  return (0);
}

int fbtimer::tonow (void) {
	timeout=(get_timems());
	return (0);
}

int fbtimer::isittime (void) {
  //printf("timeout es: %20e\n", timeout);
  //printf("ahora   es: %20e\n", get_timems());
	if (timeout <= get_timems()) {
		return (1);
	}
	else {
		return (0);
	}
}


	


