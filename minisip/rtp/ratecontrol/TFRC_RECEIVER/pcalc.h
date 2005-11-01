//TFRC loss event rate calculation


#ifndef PCALC_H
#define PCALC_H
#include "lplist.h"
#include "lintlst.h"
#include "packethist.h"
#include "rxrate.h"

class pcalc {
public:
	lintlst lintl;
	lplist* llptr;
	packhist* phptr;
	rxrate * rxra;
	float avgint, p;
	double ltime;
	int lint;
	static const int s=1460;
	pcalc();
	~pcalc();
	//static const float weight[]={1, 1, 1, 1, 0.8, 0.6, 0.4, 0.2};
	//float weight[8];
	int isp (void);
	int calc_avglint (void);
	int calc_lrate (void);
	int check_p (void);
	int set_llptr (lplist* llptrp);
	int set_p (float pp);
	int set_packhist (packhist* phptrp);
	int inli (void);
	int set_rxrate(rxrate* rxrap);
	int set_prevals(short prevpsnp, double prevptip);
//protected:
	float calcx (float tp, float xrecp);
	float prevp;
	short prevpsn;
	double prevpti;
	int interpol (short oldsn, double oldat, short asn, double aat, short snl);
	int calc_lint (short snlp);

};

#endif



