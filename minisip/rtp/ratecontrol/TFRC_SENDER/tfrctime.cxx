// TFRC time related clases
//class timep contains the time members common for the classes that
// calculates the rtt and the one that generates the sending timestamp

#include <iostream>
#include <cstdlib>
#include <sys/time.h>
#include "tfrctime.h"
using namespace std;

timep::timep (int randp, double starttmp) {
	alrand=randp;
	starttm=starttmp;
	stimeptr=new timeval;
}


timep::timep() {
  stimeptr=new timeval;
}

timep::~timep () {
	delete stimeptr;
}

double timep::gentimems () {
 	gettimeofday(stimeptr, NULL);
	return ((double) ((((double)(stimeptr->tv_sec))*1000.0)+(((double)(stimeptr->tv_usec))/1000.0)));
}

rttcalc::rttcalc (int randp, double startmp) : timep (randp, startmp) {
	rtt_next=0; //check
	
}

rttcalc::~rttcalc () {
}

	
int rttcalc::set_iptrval (fbcont *ap) {
	delayt=ap->rxdelay;
	echots=ap->echots;
	arrti=ap->arrtime;
	return (0);
}


float rttcalc::get_rttnext (unsigned long seqn) {
  if ((seqn-iseqn)>1) {
    detrttsamp();
    calcrtt();
    return (rtt_next);
  }
  else {
    if ((seqn-iseqn)==1) {
      detrttsamp();
      calcrtt_p1();
      return (rtt_next);
    }
    return(0);
  }
}

int rttcalc::set_iniseqn (unsigned long iseqnp) {
	iseqn=iseqnp;
	return (0);
}


int rttcalc::detrttsamp (void) {
  rttsamp=((float)(arrti-starttm))-((float)(echots-alrand))-(float)delayt;
  //printf("rttsamp %f\n", rttsamp); 
  return (0);
}
	
int rttcalc::calcrtt (void) {
	float q=0.9;
	rtt_next=q*(rtt_next)+(1-q)*(rttsamp/1000.0);
	return (0);
}

int rttcalc::calcrtt_p1 (void) {
  rtt_next=rttsamp/1000.0;
  return (0);
}




stsgen::stsgen (int randp, double startmp) : timep (randp, startmp) {
}

stsgen::~stsgen () {
}

int stsgen::sendtsgen(void){
	sts=(unsigned long) ((alrand)+(gentimems()-(starttm)));
	return (0);
}

unsigned long stsgen::get_sts(void){
	sendtsgen();
	return (sts);
}
