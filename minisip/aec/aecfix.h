/* aecfix.h
 * this is unfinished work in progress.
 *
 * Copyright (C) DFS Deutsche Flugsicherung (2004, 2005). 
 * All Rights Reserved.
 * Author: Andre Adrian
 *
 * fixed point Acoustic Echo Cancellation NLMS-pw algorithm
 * usage: copy aecfix.cpp to aec.cpp and aecfix.h to aec.h
 *
 * Version 0.4 16bit/32bit fixed point implementation
 * Version 0.3.1 Allow change of stability parameter delta
 * Version 0.3 filter created with www.dsptutor.freeuk.com
 */

#ifndef _AEC_H                  /* include only once */

// use double if your CPU does software-emulation of float
typedef float REAL;

/* dB Values */
const REAL M0dB = 1.0f;
const REAL M3dB = 0.71f;
const REAL M6dB = 0.50f;
const REAL M9dB = 0.35f;
const REAL M12dB = 0.25f;
const REAL M18dB = 0.125f;
const REAL M24dB = 0.063f;

/* dB values for 16bit PCM */
/* MxdB_PCM = 32767 * 10 ^(x / 20) */
const int M10dB_PCM = 10362;
const int M20dB_PCM = 3277;
const int M25dB_PCM = 1843;
const int M30dB_PCM = 1026;
const int M35dB_PCM = 583;
const int M40dB_PCM = 328;
const int M45dB_PCM = 184;
const int M50dB_PCM = 104;
const int M55dB_PCM = 58;
const int M60dB_PCM = 33;
const int M65dB_PCM = 18;
const int M70dB_PCM = 10;
const int M75dB_PCM = 6;
const int M80dB_PCM = 3;
const int M85dB_PCM = 2;
const int M90dB_PCM = 1;

const int MAXPCM = 32767;

/* Design constants (Change to fine tune the algorithms */

/* The following values are for hardware AEC and studio quality 
 * microphone */

/* maximum NLMS filter length in taps. A longer filter length gives 
 * better Echo Cancellation, but slower convergence speed and
 * needs more CPU power (Order of NLMS is linear) */
#define NLMS_LEN  (80*WIDEB*8)

/* convergence speed. Range: >0 to <1 (0.2 to 0.7). Larger values give
 * more AEC in lower frequencies, but less AEC in higher frequencies. */
/* Scaled by 32768. Scale * Stepsize = 32768 * 0.7 = 22938 */
const long Stepsize = 22938;

/* minimum energy in xf. Range: M70dB_PCM to M50dB_PCM. Should be equal
 * to microphone ambient Noise level */
const int NoiseFloor = M75dB_PCM;

/* Double Talk Detector Speaker/Microphone Threshold. Range <=1
 * Large value (M0dB) is good for Single-Talk Echo cancellation, 
 * small value (M12dB) is good for Doulbe-Talk AEC */
/* Value is not used. See function dtd() */
const REAL GeigelThreshold = M6dB;

/* Double Talk Detector hangover in taps. Not relevant for Single-Talk 
 * AEC */
const int Thold = 15 * WIDEB * 8;

/* for Non Linear Processor. Range >0 to 1. Large value (M0dB) is good
 * for Double-Talk, small value (M12dB) is good for Single-Talk */
/* Value is not used. See function doAEC() */
const REAL NLPAttenuation = M6dB;

/* Below this line there are no more design constants */


/* Exponential Smoothing or IIR Infinite Impulse Response Filter */
class IIR_HP {
  long x;

public:
   IIR_HP() {
    x = 0;
  };
  long highpass(long in) {
    const long a0 = 100;      /* controls Transfer Frequency */
    /* Highpass = Signal - Lowpass. Lowpass = Exponential Smoothing */
    x += (in - x) / a0;
    return in - x;
  };
};


/* 13 taps FIR Finite Impulse Response filter
 * Coefficients calculated with
 * www.dsptutor.freeuk.com/KaiserFilterDesign/KaiserFilterDesign.html
 */
class FIR_HP13 {
  long z[14];

public:
   FIR_HP13() {
    memset(this, 0, sizeof(FIR_HP13));
  };
  long highpass(long in) {
    const long a[14] = {
      // Kaiser Window FIR Filter, Filter type: High pass
      // Passband: 300.0 - 4000.0 Hz, Order: 12
      // Transition band: 100.0 Hz, Stopband attenuation: 10.0 dB
      // values scaled by 65536
      -2830, -3056, -3249, -3404,
      -3517, -3586, 54132, -3586,
      -3517, -3404, -3249, -3056,
      -2830, 0
    };
    memmove(z + 1, z, 13 * sizeof(long));
    z[0] = in;
    long sum0 = 0, sum1 = 0;
    int j;

    for (j = 0; j < 14; j += 2) {
      // optimize: partial loop unrolling
      sum0 += (a[j] * z[j]) / 65536;
      sum1 += (a[j + 1] * z[j + 1]) / 65536;
    }
    return sum0 + sum1;
  }
};


/* Recursive single pole IIR Infinite Impulse response filter
 * Coefficients calculated with
 * http://www.dsptutor.freeuk.com/IIRFilterDesign/IIRFiltDes102.html
 */
class IIR1 {
  long x, y;

public:
   IIR1() {
    memset(this, 0, sizeof(IIR1));
  };
  long highpass(long in) {
    // Chebyshev IIR filter, Filter type: HP
    // Passband: 3700 - 4000.0 Hz
    // Passband ripple: 1.5 dB, Order: 1
    // values scaled by 65536
    const long a0 = 6936;
    const long a1 = -6936;
    const long b1 = 51664;
    
    long out = (a0 * in + a1 * x + b1 * y) / 65536;
    x = in;
    y = out;
    return out;
  }
};


/* Recursive two pole IIR Infinite Impulse Response filter
 * Coefficients calculated with
 * http://www.dsptutor.freeuk.com/IIRFilterDesign/IIRFiltDes102.html
 */
class IIR2 {
  REAL x[2], y[2];

public:
   IIR2() {
    memset(this, 0, sizeof(IIR2));
  };
  REAL highpass(REAL in) {
    // Butterworth IIR filter, Filter type: HP
    // Passband: 2000 - 4000.0 Hz, Order: 2
    const REAL a[] = { 0.29289323f, -0.58578646f, 0.29289323f };
    const REAL b[] = { 1.3007072E-16f, 0.17157288f };
    REAL out =
      a[0] * in + a[1] * x[0] + a[2] * x[1] - b[0] * y[0] - b[1] * y[1];

    x[1] = x[0];
    x[0] = in;
    y[1] = y[0];
    y[0] = out;
    return out;
  }
};


// Extention in taps to reduce mem copies
#define NLMS_EXT  (10*8)

// block size in taps to optimize DTD calculation 
#define DTD_LEN   16


class AEC {
  // Time domain Filters
  IIR_HP acMic, acSpk;          // DC-level remove Highpass)
  FIR_HP13 cutoff;              // 300Hz cut-off Highpass
  long gain;                    // Mic signal amplify, scaled by 65536
  IIR1 Fx, Fe;                  // pre-whitening Highpass for x, e

  // Geigel DTD (Double Talk Detector)
  short max_max_x;               // max(|x[0]|, .. |x[L-1]|)
  int hangover;
  // optimize: less calculations for max()
  short max_x[NLMS_LEN / DTD_LEN];
  int dtdCnt;
  int dtdNdx;

  // NLMS-pw
  short x[NLMS_LEN + NLMS_EXT];   // tap delayed loudspeaker signal
  short xf[NLMS_LEN + NLMS_EXT];  // pre-whitening tap delayed signal
  short w[NLMS_LEN];              // tap weights, scaled by 32768
  int j;                          // optimize: less memory copies
  long dotp_xf_xf;                // iterative dotp(xf,xf)
  long delta;                     // noise floor to stabilize NLMS
  long micAvg;                    // average Mic level, scaled by 65536

public:
   AEC();

/* Geigel Double-Talk Detector
 *
 * in d: microphone sample (16bit PCM value)
 * in x: loudspeaker sample (16bit PCM value)
 * return: 0 for no talking, 1 for talking
 */
  int dtd(long d, long x);

/* Normalized Least Mean Square Algorithm pre-whitening (NLMS-pw)
 * The LMS algorithm was developed by Bernard Widrow
 * book: Haykin, Adaptive Filter Theory, 4. edition, Prentice Hall, 2002
 *
 * in d: microphone sample (16bit PCM value)
 * in x_: loudspeaker sample (16bit PCM value)
 * in update: 0 for convolve only, 1 for convolve and update 
 * return: echo cancelled microphone sample
 */
  long nlms_pw(long d, long x_, int update);

/* Acoustic Echo Cancellation and Suppression of one sample
 * in   d:  microphone signal with echo
 * in   x:  loudspeaker signal
 * return:  echo cancelled microphone signal
 */
  int AEC::doAEC(int d, int x);

  float AEC::getambient() {
    return (float)micAvg / 65536.0f;
  };
  void AEC::setambient(float Min_xf) {
    dotp_xf_xf -= delta;  // subtract old delta
    delta = NLMS_LEN * (long)(Min_xf * Min_xf);
    dotp_xf_xf += delta;  // add new delta
  };
  void AEC::setgain(float gain_) {
    gain = (long)(65536.0f * gain_);
  };
};

#define _AEC_H
#endif
