// TFRC CONTAINER FOR SENDER CONTROL INFORMATION


#ifndef SECONTAINER_H
#define SECONTAINER_H

class secont {
public:
	short seqnum;
	unsigned long sendts;
	double arrtime;
	float rtt;
	secont ();
	int set_values(short seqnump, unsigned long stsp, float rttp, double arrtimep);
	~secont();
	//int set_arrtime (unsigned long arrtimep);
};

#endif

