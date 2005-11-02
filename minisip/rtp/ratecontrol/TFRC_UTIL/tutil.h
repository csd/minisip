//utility library for TFRC SENDER

#ifndef TUTIL_H
#define TUTIL_H

#include <cstdlib>
//#include <sys/time.h>
#include <time.h>
#include <libmutil/mtime.h>

class tutil {
 public:
  //timeval * basetimeptr;
  tutil();
  ~tutil();
  int gentimebase (double& a);
  int genrandn (int& d);
  void Sleep (int ms);
};

#endif


