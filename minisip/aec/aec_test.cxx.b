/* aec_test.cxx
 * Test stub for Acoustic Echo Cancellation NLMS-pw algorithm
 * Author: Andre Adrian, DFS Deutsche Flugsicherung
 * <Andre.Adrian@dfs.de>
 *
 * Version 1.1
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "aec.h"
 
#define TAPS  	(80*8)

typedef signed short MONO;

typedef struct {
  signed short	l;
  signed short	r;
} STEREO;

/* Read a raw audio file (8KHz sample frequency, 16bit PCM, stereo)
 * from stdin, echo cancel it and write it to stdout
 */
int main(int argc, char *argv[]) {
  STEREO inbuf[TAPS], outbuf[TAPS];
  
  fprintf(stderr, "usage: aec_test <in.raw >out.raw\n");

  AEC aec;
  
  int taps;
  while (taps = fread(inbuf, sizeof(STEREO), TAPS, stdin)) {
    int i;
    for (i = 0; i < taps; ++i) {
      int s0 = inbuf[i].l;    /* left channel microphone */
      int s1 = inbuf[i].r;    /* right channel speaker */
      
      /* and do NLMS*/
      s0 = aec.doAEC(s0, s1);

      /* copy back */
      outbuf[i].l = 0;        /* left channel silence */
      outbuf[i].r = s0;       /* right channel echo cancelled mic */
    }
    
    fwrite(outbuf, sizeof(STEREO), taps, stdout);
  }
  fflush(NULL);  
  return 0;
}
