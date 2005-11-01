// TFRC RXSEND

#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include "rxsend.h"

using namespace std;

rxsend::rxsend (fbinfo * fbip) {
	fbii=fbip;
}

rxsend::~rxsend () {
}

void rxsend::sendfbi (void) {
        char *fbdatap;
	unsigned long echotst;
	gather gsend(16);
	fbii->updatev();
	fbdatap=gsend.pega(&(fbii->echots), &(fbii->delay), &(fbii->rxr), &(fbii->lossrate));
       	memcpy(fbdata,fbdatap,16);
	
}
