/* aecfix.cpp
 * this is unfinished work in progress.
 *
 * Copyright (C) DFS Deutsche Flugsicherung (2004, 2005). 
 * All Rights Reserved.
 *
 * fixed point Acoustic Echo Cancellation NLMS-pw algorithm
 * usage: copy aecfix.cpp to aec.cpp and aecfix.h to aec.h
 *
 * Version 0.4 16bit/32bit fixed point implementation
 * Version 0.3.1 Allow change of stability parameter delta
 * Version 0.3 filter created with www.dsptutor.freeuk.com
 */

#include<config.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <libminisip/aecfix.h>

#if 1
/* Vector Dot Product */
long dotp(short a[], short b[])
{
  long sum0 = 0, sum1 = 0;
  int j;

  for (j = 0; j < NLMS_LEN; j += 2) {
    // optimize: partial loop unrolling
    sum0 += (long)a[j] * (long)b[j];
    sum1 += (long)a[j + 1] * (long)b[j + 1];
  }
  return (sum0 + sum1) / 32768;
}
#else

/* this is work in progress. */
#include <mmintrin.h>

#define _m_from_WORDs(w) (*(__m64 *)(w))
 
long dotp(short a[], short b[])
{
  int i;
  __m64 mm0, mm1, mm2, mm3, mm4;
  short suml[4];   // don't init sum from C - this confuses the GCC!
  short sumh[4];
  
  /* mmx - Intel Pentium-MMX and above */

  mm2 = _m_psubw(mm2, mm2);   // set mm2 to 0
  mm4 = _m_psubw(mm4, mm4);
  for (i = 0; i < NLMS_LEN; i += 4, a += 4, b += 4) {
    mm0 = _m_from_WORDs(a);
    mm3 = mm0;
    mm1 = _m_from_WORDs(b);
    
    /* Intel notation: first operand is destination */
    /* GNU as notation: first operand is source */
    // mm0 = _mm_mullo_pi16 (mm0, mm1);
    mm3 = _mm_mulhi_pi16 (mm3, mm1);
    // mm2 = _mm_add_pi16(mm2, mm0);
    mm4 = _mm_add_pi16(mm4, mm3);
  }
  _m_from_WORDs(suml) = mm2;
  _m_from_WORDs(sumh) = mm4;
  _mm_empty();
  return suml[0] + suml[1] + suml[2] + suml[3] 
   + 65536 * (sumh[0] + sumh[1] + sumh[2] + sumh[3]);
}
#endif

AEC::AEC()
{
  max_max_x = 0;
  hangover = 0;
  memset(max_x, 0, sizeof(max_x));
  dtdCnt = dtdNdx = 0;

  memset(x, 0, sizeof(x));
  memset(xf, 0, sizeof(xf));
  memset(w, 0, sizeof(w));
  j = NLMS_EXT;
  delta = 0;
  setambient(NoiseFloor);
  micAvg = NoiseFloor;
  gain = 65536;
}


long AEC::nlms_pw(long d, long x_, int update)
{
  x[j] = (short)x_;
  xf[j] = (short)Fx.highpass(x_);     // pre-whitening of x

  // calculate error value 
  // (mic signal - estimated mic signal from spk signal)
  long e = d - dotp(w, x + j);
  long ef = Fe.highpass(e);     // pre-whitening of e
  // optimize: iterative dotp(xf, xf)
  dotp_xf_xf +=
    (xf[j] * xf[j] - xf[j + NLMS_LEN - 1] * xf[j + NLMS_LEN - 1]);
  if (update) {
    // calculate variable step size
    long mikro_ef = (Stepsize * ef) / dotp_xf_xf;

    // update tap weights (filter learning)
    int i;
    for (i = 0; i < NLMS_LEN; i += 2) {
      // optimize: partial loop unrolling
      w[i] += mikro_ef * xf[i + j];
      w[i + 1] += mikro_ef * xf[i + j + 1];
    }
  }

  if (--j < 0) {
    // optimize: decrease number of memory copies
    j = NLMS_EXT;
    memmove(x + j + 1, x, (NLMS_LEN - 1) * sizeof(short));
    memmove(xf + j + 1, xf, (NLMS_LEN - 1) * sizeof(short));
  }
  
  // Saturation
  if (e > 32767) {
    return 32767;
  } else if (e < -32767) {
    return -32767;
  } else {
    return e;
  }
}


int AEC::dtd(long d, long x)
{
  // optimized implementation of max(|x[0]|, |x[1]|, .., |x[L-1]|):
  // calculate max of block (DTD_LEN values)
  x = labs(x);
  if (x > max_x[dtdNdx]) {
    max_x[dtdNdx] = x;
    if (x > max_max_x) {
      max_max_x = x;
    }
  }
  if (++dtdCnt >= DTD_LEN) {
    dtdCnt = 0;
    // calculate max of max
    max_max_x = 0;
    for (int i = 0; i < NLMS_LEN / DTD_LEN; ++i) {
      if (max_x[i] > max_max_x) {
        max_max_x = max_x[i];
      }
    }
    // rotate Ndx
    if (++dtdNdx >= NLMS_LEN / DTD_LEN)
      dtdNdx = 0;
    max_x[dtdNdx] = 0;
  }
  // The Geigel DTD algorithm with Hangover timer Thold
  // GeigelThreshold -6dB = 0.5 * max_max_x or max_max_x / 2
  if (labs(d) >= max_max_x / 2) {
    hangover = Thold;
  }

  if (hangover)
    --hangover;

  return (hangover > 0);
}


int AEC::doAEC(int d, int x)
{
  // Mic Highpass Filter - to remove DC
  d = acMic.highpass(d);

  // Mic Highpass Filter - cut-off below 300Hz
  d = cutoff.highpass(d);
  
  // Amplify, for e.g. Soundcards with -6dB max. volume
  d = (gain * d) / 65536;

  // ambient mic level estimation
  micAvg += (labs(65536 * d) - micAvg) / 10000;

  // Spk Highpass Filter - to remove DC
  x = acSpk.highpass(x);

  // Double Talk Detector
  int update = !dtd(d, x);

  // Acoustic Echo Cancellation
  d = nlms_pw(d, x, update);

  // Acoustic Echo Suppression
  if (update) {
    // Non Linear Processor (NLP): attenuate low volumes
    // NLPAttenuation -6dB = d * 0.5 or d / 2
    d /= 2;
  }
  
  return d;
}
