//TFRC CONTAINER FOR FEEDBACK INFORMATION
#include <iostream>
#include <cstdlib>
#include "fbcontainer.h"
using namespace std;

fbcont::fbcont () {
}

fbcont::~fbcont () {
}

int fbcont::set_values (float pp, float xrecp, unsigned long rxdelayp, unsigned long echotsp, double arrtimep) {
	p=pp;
	xrec=xrecp;
	rxdelay=rxdelayp;
	echots=echotsp;
	arrtime=arrtimep;
	return (0);
}

