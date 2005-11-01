#ifndef SENDBWMGR_H
#define SENDBWMGR_H

#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include "fbcontainer.h"
#include "tfrctime.h"
#include "rcalc.h"
#include "../TFRC_NET/nsend.h"
#include "../TFRC_NET/nrecv.h"
#include "../TFRC_NET/cdn.h"
#include "setimer.h"
#include "../TFRC_UTIL/tutil.h"


class SendBwMngr {
 public:
  SendBwMngr();
  ~SendBwMngr();
  void set_rtp_header_v();
  void runcli();
 private:
  tutil ut;
  int ranum;
  double basetime;
  float x, ex, rttv;
  unsigned long psts;
};

#endif


