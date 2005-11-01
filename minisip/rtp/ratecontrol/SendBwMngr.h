#ifndef SENDBWMGR_H
#define SENDBWMGR_H

#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <libmutil/MemObject.h>
#include <libmutil/Thread.h>
#include "../RtpPacket.h" 
#include "TFRC_SENDER/fbcontainer.h"
#include "TFRC_SENDER/tfrctime.h"
#include "TFRC_SENDER/rcalc.h"
#include "TFRC_NET/nsend.h"
#include "TFRC_NET/nrecv.h"
#include "TFRC_NET/cdn.h"
#include "TFRC_SENDER/setimer.h"
#include "TFRC_UTIL/tutil.h"

class MediaStreamSender;

class SendBwMngr : public Runnable{
 public:
  SendBwMngr();
  ~SendBwMngr();
  void set_rtp_header_v(MRef<RtpPacket*> rtp);
  virtual void run();

  void start(){
	if (!t)
  		t = new Thread(this);
  }
  void stop(){
  	//not implemented
  }

  /**
   * Sets which object will be contacted when the 
   * target bandwidth changes.
   */
  void setBwListener(MediaStreamSender* listener);
  
 private:
  tutil ut;
  int ranum;
  double basetime;
  float x, ex, rttv;
  unsigned long psts;

  MediaStreamSender *listener;

  Thread *t;
};

#include"../../mediahandler/MediaStream.h"

#endif


