#ifndef SENDBWMGR_H
#define SENDBWMGR_H

#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include "TFRC_SENDER/fbcontainer.h"
#include "TFRC_SENDER/tfrctime.h"
#include "TFRC_SENDER/rcalc.h"
#include "TFRC_NET/nsend.h"
#include "TFRC_NET/nrecv.h"
#include "TFRC_NET/cdn.h"
#include "TFRC_SENDER/setimer.h"
#include "TFRC_UTIL/tutil.h"


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


