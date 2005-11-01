		// TFRC Sending Rate Calculator

#ifndef RCALC_H
#define RCALC_H

#include "fbcontainer.h"
#include "tfrctime.h"

class ratecalc {
 public:
  const static int s=1460;
  double tldo;
  timep clot;
  const static int tmbi=64;
  const static float misr=2466; // minimum sending rate 16kbps
  float p, r, xcalc, xrec;
  ratecalc();
  ~ratecalc();
  int set_values(fbcont * ap, float rp); //set values
  float get_xnext (void);
  float get_coxnext (void);
  float x_next, effx;
  int Cxcalc (void); 
  int Cxnext (void); 
  int Cxslows (void);
  int Ceffx(void);
  int Cxhalf (void);
  int reset_xrec(void);
		
};

#endif

