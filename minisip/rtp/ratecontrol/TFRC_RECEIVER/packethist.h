// TFRC PACKET HISTORY

#ifndef PACKETHIST_H
#define PACKETHIST_H
#include "secontainer.h"

class packhist {
public:
	//sinfo ppack;
	secont phist[7];
	//packhist(secont ppackp);
	packhist();
	~packhist();
	int set_pack(secont incpp);
	secont get_pack(int indexp);
	int shift(void);
private:
	int initial (void);
	//secont get_prevp(short seqnump);
	//secont get_postp(short seqnump);
};

#endif
