// TFRC FEEDBACK INFO CONTAINER

#include <iostream>
#include <cstdlib>
#include "fbinfo.h"
using namespace std;


fbinfo::fbinfo (packhist * php, pcalc * pcp, rxrate * rxrap) {
	ph=php;
	pc=pcp;
	rxra=rxrap;
}

fbinfo::~fbinfo () {
}

int fbinfo::calc_del (void) {
	delay=(unsigned long) (get_timems()-arrti);
	return(0);
}

int fbinfo::updatev(void) {
	arrti=(ph->get_pack(0)).arrtime;
	echots=(ph->get_pack(0)).sendts;
	rxr=(rxra->recvrate);
	lossrate=(pc->p);
	calc_del();
}


