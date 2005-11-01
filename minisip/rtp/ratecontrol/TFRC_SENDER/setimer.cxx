#include "setimer.h"
#include <cmath>
using namespace std;

stimer::stimer (int ss, fbcont * fbcp, ratecalc * ratp, rttcalc * rttcp) {
  psi=ss;
  fbc=fbcp;
  rat=ratp;
  rttc=rttcp;
  timeout=rttc->gentimems();
}

stimer::~stimer () {
}

void stimer::resetto () {
  timeout=(rttc->gentimems())+(max((float) (1000*4*(rttc->rtt_next)), (float) (2*psi/(rat->x_next))));
}

int stimer::istime () {
  if (timeout > rttc->gentimems())
    return(0);
  else
    return(1);
}
