// TFRC LOST PACKET IDENTIFIED


#ifndef LPID_H
#define LPID_H
#include "lplist.h"

class losspid {
public:
	short snexp, snarr;
	int lco, fpl;
	lplist losslst;
	losspid();
	~losspid();
	int isexp(void);
	int set_snexp (short snexpp);
	int set_snarr (short snarrp);
	int incsnexp (void);
	int inclco (void);
	int get_lco (void);
	int clr_lco (void);
	lplist* get_llptr (void);
	int isfirstl(void);
	//int set_values(short snarrp);
};


#endif


