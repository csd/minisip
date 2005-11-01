//////////     gather.cpp

#include "cdn.h"

gather::gather (int gavsize){
  gav=new char[gavsize];
}
  

gather::~gather () {
  delete [] gav;
}

char * gather::pega(uint16_t *seqnum, unsigned long *stsg, float *rtt) {
        memcpy(gav, seqnum, sizeof(*seqnum));
	memcpy(&gav[sizeof(*seqnum)], stsg, sizeof(*stsg));
	memcpy(&gav[sizeof(*seqnum)+sizeof(*stsg)], rtt, sizeof(*rtt));
	return(gav);
	
}

char * gather::pega(unsigned long *stsg1, unsigned long *delay, float *rxra, float *lossr) {
        memcpy(gav, stsg1, sizeof(*stsg1));
	memcpy(&gav[sizeof(*stsg1)], delay, sizeof(*delay));
	memcpy(&gav[sizeof(*stsg1)+sizeof(*delay)], rxra, sizeof(*rxra));
	memcpy(&gav[sizeof(*stsg1)+sizeof(*delay)+sizeof(*rxra)], lossr, sizeof(*lossr));
	return(gav);
	
}


///////////// decod.cpp


decod::decod (char* srxbufp) {
  srxbuf=srxbufp;
}

decod::~decod () {
}

void decod::decsend() {
  memcpy(&seqnum, srxbuf, sizeof(seqnum));
  memcpy(&st_est, &srxbuf[sizeof(seqnum)], sizeof(st_est));
  memcpy(&rtt_rr, &srxbuf[sizeof(seqnum)+sizeof(st_est)], sizeof(rtt_rr));
}

void decod::decrx() {
  memcpy(&st_est, srxbuf, sizeof(st_est));
  memcpy(&delay, &srxbuf[sizeof(st_est)], sizeof(delay));
  memcpy(&rtt_rr, &srxbuf[sizeof(st_est)+sizeof(delay)], sizeof(rtt_rr));
  memcpy(&lossr, &srxbuf[sizeof(st_est)+sizeof(delay)+sizeof(rtt_rr)], sizeof(lossr));
}
