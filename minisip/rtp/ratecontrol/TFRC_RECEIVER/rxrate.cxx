// TFRC RECEIVER RATE CALCULATION

#include <iostream>
#include <cstdlib>
#include "rxrate.h"
using namespace std;

rxrate::rxrate (losspid* lpptrp, packhist* php) {
	lpptr=lpptrp;
	ph=php;
}

rxrate::~rxrate () {
}

int rxrate::calc_rxpa(void) {
	seqnum=(ph->get_pack(0)).seqnum;
	rxpacks=(seqnum-prevsn)-(lpptr->get_lco());
	//cout << "the number of packets since the previous calculation is: " << rxpacks << "\n";
	prevsn=seqnum;
	lpptr->clr_lco();
	//return (0);
}

int rxrate::calc_recvrate (void) {
        float ti, minx=2466;
	ti=(((ph->get_pack(0)).arrtime)-prevtime)/1000.0;
	//cout << "the period of time for the calculation of rxrate is: " << ti << "\n";
	calc_rxpa();
	if (ti==0)
	  recvrate=minx;
	else 
	  recvrate=(rxpacks*s)/ti;
	//cout << "the received rate is: " << recvrate << "\n";
	prevtime=(ph->get_pack(0)).arrtime;
	//return (0);
}

int rxrate::set_ival(short prevsnp, double prevtimep) {
	prevsn=prevsnp;
	prevtime=prevtimep;
}

