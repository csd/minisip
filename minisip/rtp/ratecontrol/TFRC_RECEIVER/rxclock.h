//TFRC RECEIVER CLOCK


#ifndef RXCLOCK_H
#define RXCLOCK_H
#include <sys/time.h>
#include "packethist.h"

class rxclock {
public:
	timeval * stimeptr;
	rxclock ();
	~rxclock ();
	double get_timems (void);
};

class fbtimer : public rxclock {
public:
	double timeout;
	packhist * phisto;
	fbtimer(packhist * phistop);
	~fbtimer();
	int resetto (void);
	int tonow (void);
	int isittime (void);
	int set_to (double top);
};





#endif



