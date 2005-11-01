// TFRC RECEIVER RATE CALCULATION

#ifndef RXRATE_H
#define RXRATE_H
#include "lplist.h"
#include "packethist.h"
#include "lpid.h"

class rxrate {
public:
	float recvrate;
	short prevsn, seqnum;
	losspid * lpptr;
	packhist * ph;
	int rxpacks;
	double prevtime;
	//static const int s=1460;
	static const int s=172;
	rxrate (losspid* lpptrpi, packhist *php);
	~rxrate ();
	int calc_rxpa(void);
	int set_ival(short prevsnp, double prevtimep);
	int calc_recvrate (void);
};

#endif
