// TFRC FEEDBACK INFO CONTAINER


#ifndef FBINFO_H
#define FBINFO_H
#include "packethist.h"
#include "rxclock.h"
#include "pcalc.h"
#include "rxrate.h"


class fbinfo : public rxclock {
public:
	unsigned long echots, delay;
	float lossrate, rxr;
	packhist * ph;
	pcalc * pc;
	rxrate * rxra;
	fbinfo(packhist * php, pcalc * pcp, rxrate * rxrap);
	~fbinfo();
	int calc_del(void);
	int updatev(void);
private:
	double arrti;
};

#endif



