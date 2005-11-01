#ifndef RECVBWMGR_H
#define RECVBWMGR_H

#include <iostream>
#include <cstdlib>
#include "secontainer.h"
#include "packethist.h"
#include "rxclock.h"
#include "lpid.h"
#include "pcalc.h"
#include "rxrate.h"
#include "fbinfo.h"
#include "rxsend.h"
#include "../TFRC_NET/nsend.h"
#include "../TFRC_NET/nrecv.h"


class RecvBwMngr{
 public:
  RecvBwMngr(char *remoteipp);
  ~RecbBwMngr();
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
