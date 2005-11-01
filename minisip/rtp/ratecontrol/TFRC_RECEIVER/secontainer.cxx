#include <iostream>
#include <cstdlib>
#include "secontainer.h"
using namespace std;

secont::secont () {
}

int secont::set_values (short seqnump, unsigned long sendtsp, float rttp, double arrtimep) {
	seqnum=seqnump;
	sendts=sendtsp;
	rtt=rttp*1000.0; //converting the round trip time to milliseconds
	arrtime=arrtimep;
	return(0);
		
}

secont::~secont () {
}

/*
int secont::set_arrtime (unsigned long arrtimep) {
	arrtime=arrtimep;
	return(0);
}
*/
