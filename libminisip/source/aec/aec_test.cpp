/* aec_test.cpp
 *
 * Copyright (C) DFS Deutsche Flugsicherung (2004). All Rights Reserved.
 *
 * Test stub for Acoustic Echo Cancellation NLMS-pw algorithm
 * Author: Andre Adrian, DFS Deutsche Flugsicherung
 * <Andre.Adrian@dfs.de>
 *
 * compile
c++ -DWIDEB=1 -O2 -march=pentium3 -o aec_test aec_test.cpp aec.cpp -lm
 *
 * Version 1.3 set/get ambient in dB
 */

#include<config.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <libminisip/aec.h>

#define TAPS  	(80*WIDEB*8)

typedef signed short MONO;

typedef struct {
  signed short l;
  signed short r;
} STEREO;

float dB2q(float dB)
{
  /* Dezibel to Ratio */
  return powf(10.0f, dB / 20.0f);
}

float q2dB(float q)
{
  /* Ratio to Dezibel */
  return 20.0f * log10f(q);
}

/* Read a raw audio file (8KHz sample frequency, 16bit PCM, stereo)
 * from stdin, echo cancel it and write it to stdout
 */
int main(int argc, char *argv[])
{
  STEREO inbuf[TAPS], outbuf[TAPS];

  fprintf(stderr, "usage: aec_test [ambient in dB] <in.raw >out.raw\n");

  AEC aec;

  if (argc >= 2) {
    aec.setambient(MAXPCM*dB2q(atof(argv[1])));    
  }

  int taps;
  float ambient;
  while (taps = fread(inbuf, sizeof(STEREO), TAPS, stdin)) {
    int i;
    for (i = 0; i < taps; ++i) {
      int s0 = inbuf[i].l;      /* left channel microphone */
      int s1 = inbuf[i].r;      /* right channel speaker */

      /* and do NLMS */
      s0 = aec.doAEC(s0, s1);

      /* copy back */
      outbuf[i].l = 0;          /* left channel silence */
      outbuf[i].r = s0;         /* right channel echo cancelled mic */
    }

    fwrite(outbuf, sizeof(STEREO), taps, stdout);
  }
  ambient = aec.getambient();
  float ambientdB = q2dB(ambient / 32767.0f);
  fprintf(stderr, "Ambient = %2.0f dB\n", ambientdB);
  fflush(NULL);

  return 0;
}
