// TFRC LOSS INTERVAL LIST 

#include <iostream>
#include <cstdlib>
#include "lintlst.h"
using namespace std;

lintlst::lintlst () {
	initialize();
}

lintlst::~lintlst () {
};

int lintlst::shift (void) {
		for (int i=8; i>=1; i--) {
		lilst[i]=lilst[i-1];
		//return (0);
	}
}

int lintlst::set_intv (int intvp) {
	shift();
	lilst[0]=intvp;
}

int lintlst::get_intv (int indexp) {
	return (lilst[indexp]);
}

int lintlst::initialize (void) {
	for (int o=0; o<=8; o++) {
		lilst[o]=0;
	}
}
