// TFRC time related clases
//class timep contains the time members common for the classes that
// calculates the rtt and the one that generates the sending timestamp

#ifndef TFRCTIME_H
#define TFRCTIME_H

#include <sys/time.h>
#include "fbcontainer.h"

class timep {
public:
	int alrand;
	double starttm;
	timeval *stimeptr;
	timep (int randp, double starttmp);
	timep();
	~timep();
	double gentimems (void);
};

class rttcalc: public timep {
public:
  unsigned long delayt, echots;
	double arrti;
	unsigned long iseqn;
	rttcalc (int randp, double startmp);
	~rttcalc ();
	float get_rttnext (unsigned long);
	int set_iniseqn (unsigned long iseqn);
	int set_iptrval(fbcont *ap);
//private:
	float rttsamp, rtt_next;
	int fn;
	int detrttsamp (void);
	int calcrtt (void);
	int calcrtt_p1 (void);
};

class stsgen: public timep {
public:
	stsgen(int randp, double startmp);
	~stsgen();
	unsigned long get_sts(void);
private:
	int sendtsgen(void);
	unsigned long sts;
};

#endif
