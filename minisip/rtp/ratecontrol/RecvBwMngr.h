#ifndef RECVBWMGR_H
#define RECVBWMGR_H

#include <iostream>
#include <cstdlib>
#include "TFRC_RECEIVER/secontainer.h"
#include "TFRC_RECEIVER/packethist.h"
#include "TFRC_RECEIVER/rxclock.h"
#include "TFRC_RECEIVER/lpid.h"
#include "TFRC_RECEIVER/pcalc.h"
#include "TFRC_RECEIVER/rxrate.h"
#include "TFRC_RECEIVER/fbinfo.h"
#include "TFRC_RECEIVER/rxsend.h"
#include "TFRC_NET/nsend.h"
#include "TFRC_NET/nrecv.h"


class RecvBwMngr{
 public:
  RecvBwMngr(char *remoteipp);
  ~RecvBwMngr();
  int set_tfrc_vals(short seqnumb, unsigned long senderts, float rttv);
  int firstpack;
  char *remoteip;
  secont sic;
  packhist ph;
  fbtimer fbt;
  losspid lpi;
  pcalc pc;
  rxrate recrat;
  fbinfo fbi;
  rxsend rxs;
};



#endif
