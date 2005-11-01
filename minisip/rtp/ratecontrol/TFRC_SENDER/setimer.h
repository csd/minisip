#ifndef SETIMER_H
#define SETIMER_H
#include "fbcontainer.h"
#include "tfrctime.h"
#include "rcalc.h"
#include <iostream>
#include <cstdlib>
#include <cmath>

class stimer {
 public:
  double timeout;
  fbcont * fbc;
  ratecalc * rat;
  rttcalc * rttc;
  int psi;
  stimer(int ss, fbcont * fbcp, ratecalc * ratp, rttcalc * rttcp);
  ~stimer();
  void resetto();
  int istime();
};



#endif




