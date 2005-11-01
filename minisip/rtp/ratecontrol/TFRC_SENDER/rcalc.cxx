// TFRC Sending Rate Calculator

#include <iostream>
#include <cstdlib>
#include <cmath>
#include "rcalc.h"

using namespace std;


int ratecalc::set_values (fbcont *ap, float rp) {
	p=ap->p;
	xrec=ap->xrec;
	r=rp;
	return (0);
}


ratecalc::ratecalc () {
	x_next=misr;
	tldo=clot.gentimems();
}

ratecalc::~ratecalc () {
}

int ratecalc::Cxcalc (void) {
	xcalc=s/(r*(sqrt(2*p/3)+(12*p*sqrt(3*p/8)*(1+32*pow(p,2)))));
	//cout << "Xcalc is: "  <<  xcalc  << "\n"; //erase me after testing
	return (0);
}

int ratecalc::Cxnext (void) {
        //float a=s/tmbi; //minimum threshold of 1 packet each 64 secs (as in specs)
  float a=misr; //minimum threshold of misr (defined in rcalc.h)
        Cxcalc();
	x_next=max(min(xcalc,2*xrec),a);
	//cout << "in congestion control by equation \n"; //erase me after testing
	return (0);
}

int ratecalc::Cxslows (void) {
  //x_next=max(min(2*x_next, 2*xrec),(s/r)); // original solution
  if (((clot.gentimems())-tldo) >= (r*1000) ) {
  x_next=max(min(2*x_next, 2*xrec), (float)(misr)); 
  //cout << "in slow start  \n"; 
  tldo=clot.gentimems();
  }
  return(0);
}



float ratecalc::get_xnext (void) {
	if (p>0) {
		Cxnext();
	}
	else {
		Cxslows();
	}
	return (x_next);
}

int ratecalc::Ceffx(void) {
	float reals=172.0, h=40.0;
	effx=(x_next*(reals/(reals+h)))*(8/1000.0); // in kbps for the application
}


int ratecalc::reset_xrec (void) {
  //float a=s/tmbi;
  float a=misr;
	Cxcalc();
	if (xcalc > (2*xrec)) {
		xrec=max((xrec/2), a/2);
	}
	else
		xrec=xcalc/4;
}

int ratecalc::Cxhalf (void) {
  //float a=s/tmbi;
  float a=misr;
	x_next=max((x_next/2),a);
	return (0);
}

float ratecalc::get_coxnext (void) {
	if (p>0) {
		reset_xrec();
		Cxnext();
	}
	else {
		Cxhalf();
	}
	return (x_next);
}
