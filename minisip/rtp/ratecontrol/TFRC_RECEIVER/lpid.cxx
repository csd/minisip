// TFRC LOST PACKET IDENTIFIER

#include <iostream>
#include <cstdlib>
#include "lpid.h"
using namespace std;

losspid::losspid () {
	lco=0;
	fpl=0;
}

losspid::~losspid () {
}

int losspid::isexp(void) {
	int lconow;
	if (snarr==snexp) {
		incsnexp();
		return (1);
		
	}
	else {
		if (snarr>snexp) {
			if (fpl==0){
				fpl=1;
			}
			else {
				fpl=2;
			}
			lconow=(int) (snarr-snexp);
			lco=lco+lconow;
			losslst.set_values(lconow);
			for (short i=snexp; i<snarr; i++) {
				losslst.set_lsn(i,(int)(i-snexp));
			}
			set_snexp(snarr+1);
			return (2);
		}
		else {
			return (0);
		}
	}
}

int losspid::set_snexp (short snexpp) {
	snexp=snexpp;
	return (0);
}

int losspid::set_snarr (short snarrp) {
	snarr=snarrp;
	return (0);
}

int losspid::incsnexp (void){
	snexp++;
	return (0);
}

int losspid::inclco (void) {
	lco++;
	return (0);
}

int losspid::get_lco (void) {
	return (lco);
}

int losspid::clr_lco (void) {
	lco=0;
	return (0);
}


lplist* losspid::get_llptr (void) {
	//losslstptr = &losslst;
	return (&losslst);
}

int losspid::isfirstl(void) {
	if (fpl==1) {
		return (1);
	}
	else
		return (0);
}
