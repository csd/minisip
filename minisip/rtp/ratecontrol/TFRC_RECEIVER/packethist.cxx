//TFRC PACKET HISTORY

#include <iostream>
#include <cstdlib>
#include "packethist.h"

using namespace std;

/*
packhist::packhist (secont ppackp) {
	shift();
	set_pack(ppackp);
	//ppack=ppackp;
}
*/

packhist::packhist() {
	initial();
}

packhist::~packhist () {
}

int packhist::shift () {
	for (int i=6; i>=1; i--) {
	phist[i]=phist[i-1];
	}
	return (0);
}

int packhist::set_pack(secont incpp) {
	shift();
	phist[0]=incpp;
	return (0);
}

secont packhist::get_pack(int indexp) {
	return (phist[indexp]);
}

int packhist::initial (void) {
	secont empty;
	empty.set_values(0,0,0,0);
	for (int i=0; i<=6; i++) {
	phist[i]=empty;
	}
	return(0);
}

/*
secont get_prevp(short seqnump) {
}


secont get_postp(short seqnump) {
}
*/
