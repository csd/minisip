/* aec.cxx
 * Acoustic Echo Cancellation NLMS-pw algorithm
 * Author: Andre Adrian, DFS Deutsche Flugsicherung
 * <Andre.Adrian@dfs.de>
 *
 * Version 1.1
 */

#include<config.h>

#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include<libminisip/media/aec/aec.h>

#ifdef _WIN32_WCE
# define M_PI           3.14159265358979323846  /* pi */
#endif

/* ================================================================ */
/* Exponential Smoothing or IIR Infinite Impulse Response Filter */

IIR6::IIR6()
{
  memset(this, 0, sizeof(IIR6));
}

float IIR6::lowpass(float in)
{
  const float AlphaLp = 0.15f;	/* controls Transfer Frequence */

  /* Lowpass = Exponential Smoothing */
  lowpassf[0] = in;
  int i;
  for (i = 0; i < 2*POL; ++i) {
    lowpassf[i+1] += AlphaLp*(lowpassf[i] - lowpassf[i+1]);
  }
  return lowpassf[2*POL];   
}

/* ================================================================ */

/*
 * Algorithm:  Recursive single pole FIR high-pass filter
 *
 * Reference: The Scientist and Engineer's Guide to Digital Processing
 */

FIR1::FIR1()
{
  float x = (float) exp(-2.0 * M_PI * PreWhiteTransferFreq/8000.0f);
  
  a0 = (1.0f + x) / 2.0f;
  a1 = -(1.0f + x) / 2.0f;
  b1 = x;
  last_in = 0.0f;
  last_out = 0.0f;
}

/* ================================================================ */

/* Vector Dot Product */
float dotp(float a[], float b[]) {
  float sum0 = 0.0, sum1 = 0.0;
  int j;
  
  for (j = 0; j < NLMS_LEN; j+= 2) {
    // optimize: partial loop unrolling
    sum0 += a[j] * b[j];
    sum1 += a[j+1] * b[j+1];
  }
  return sum0+sum1;
}

AEC::AEC()
{
  max_max_x = 0.0f;
  hangover = 0;
  memset(max_x, 0, sizeof(max_x));
  dtdCnt = dtdNdx = 0;
  
  memset(x, 0, sizeof(x));
  memset(xf, 0, sizeof(xf));
  memset(w, 0, sizeof(w));
  j = NLMS_EXT;
  lastupdate = 0;
  dotp_xf_xf = 0.0f;
}

/* Normalized Least Mean Square Algorithm pre-whitening (NLMS-pw)
 * The LMS algorithm was developed by Bernard Widrow
 * book: Widrow/Stearns, Adaptive Signal Processing, Prentice-Hall, 1985
 *
 * in mic: microphone sample (PCM as floating point value)
 * in spk: loudspeaker sample (PCM as floating point value)
 * in update: 0 for convolve only, 1 for convolve and update 
 * return: echo cancelled microphone sample
 */
float AEC::nlms_pw(float mic, float spk, int update)
{
  float d = mic;      	      	  // desired signal
  x[j] = spk;
  xf[j] = Fx.highpass(spk);       // pre-whitening of x

  // calculate error value (mic signal - estimated mic signal from spk signal)
  float e = d - dotp(w, x + j);
  
  if (update) {    
    float ef = Fe.highpass(e);    // pre-whitening of e
    
    if (lastupdate) {
      // optimize: iterative dotp(xf, xf)
      dotp_xf_xf += (xf[j]*xf[j] - xf[j+NLMS_LEN-1]*xf[j+NLMS_LEN-1]);
    } else {
      dotp_xf_xf = dotp(xf+j, xf+j);
    } 
      
    // calculate variable step size
    float mikro_ef = 0.5f/dotp_xf_xf * ef;
    
    // update tap weights (filter learning)
    int i;
    for (i = 0; i < NLMS_LEN; i += 2) {
      // optimize: partial loop unrolling
      w[i] += mikro_ef*xf[i+j];
      w[i+1] += mikro_ef*xf[i+j+1];
    }
  }
  lastupdate = update;
  
  if (--j < 0) {
    // optimize: decrease number of memory copies
    j = NLMS_EXT;
    memmove(x+j+1, x, (NLMS_LEN-1)*sizeof(float));    
    memmove(xf+j+1, xf, (NLMS_LEN-1)*sizeof(float));    
  }
  
  return e;
}


/* Geigel Double-Talk Detector
 *
 * in d: microphone sample (PCM as floating point value)
 * in x: loudspeaker sample (PCM as floating point value)
 * return: 0 for no talking, 1 for talking
 */
int AEC::dtd(float d, float x)
{
  // optimized implementation of max(|x[0]|, |x[1]|, .., |x[L-1]|):
  // calculate max of block (DTD_LEN values)
  x = fabsf(x);
  if (x > max_x[dtdNdx]) {
    max_x[dtdNdx] = x;
    if (x > max_max_x) {
      max_max_x = x;
    }
  }
  if (++dtdCnt >= DTD_LEN) {
    dtdCnt = 0;
    // calculate max of max
    max_max_x = 0.0f;
    for (int i = 0; i < NLMS_LEN/DTD_LEN; ++i) {
      if (max_x[i] > max_max_x) {
        max_max_x = max_x[i];
      }
    }
    // rotate Ndx
    if (++dtdNdx >= NLMS_LEN/DTD_LEN) dtdNdx = 0;
    max_x[dtdNdx] = 0.0f;
  }

  // The Geigel DTD algorithm with Hangover timer Thold
  if (fabsf(d) >= GeigelThreshold * max_max_x) {
    hangover = Thold;
  }
    
  if (hangover) --hangover;
  
  if (max_max_x < UpdateThreshold) {
    // avoid update with silence
    return 1;
  } else {
    return (hangover > 0);
  }
}


int AEC::doAEC(int d, int x) 
{
  float s0 = (float)d;
  float s1 = (float)x;
  
  // Mic and Spk signal remove DC (IIR highpass filter)
  s0 = dc0.highpass(s0);
  s1 = dc1.highpass(s1);

  // Mic Highpass Filter - telephone users are used to 300Hz cut-off
  s0 = hp0.highpass(s0);

  // Double Talk Detector
  int update = !dtd(s0, s1);

  // Acoustic Echo Cancellation
  s0 = nlms_pw(s0, s1, update);

  // Acoustic Echo Suppression
  if (update) {
    // Non Linear Processor (NLP): attenuate low volumes
    s0 *= NLPAttenuation;
  }
  
  // Saturation
  if (s0 > MAXPCM) {
    return (int)MAXPCM;
  } else if (s0 < -MAXPCM) {
    return (int)-MAXPCM;
  } else {
    return (int)floorf( ( float)(s0+0.5) );
  }
}


