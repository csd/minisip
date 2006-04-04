/*
 * FILE:    codec_g711.h
 * PROGRAM: RAT
 * AUTHOR:  Orion Hodson
 *
 * Copyright (c) 1995-2001 University College London
 * All rights reserved.
 *
 * $Id: codec_g711.h,v 1.1.1.1 2003/10/13 15:46:02 root Exp $
 */

#ifndef CODEC_G711_H
#define CODEC_G711_H

#include<libminisip/libminisip_config.h>

unsigned char linear2alaw(short pcm_val);	/* 2's complement (16-bit range) */


/*
 * alaw2linear() - Convert an A-law value to 16-bit linear PCM
 *
 */
short alaw2linear( unsigned char	a_val);


#define	BIAS		(0x84)		/* Bias for linear code. */
#define CLIP            8159

/*
* linear2ulaw() - Convert a linear PCM value to u-law
*
* In order to simplify the encoding process, the original linear magnitude
* is biased by adding 33 which shifts the encoding range from (0 - 8158) to
* (33 - 8191). The result can be seen in the following encoding table:
*
*	Biased Linear Input Code	Compressed Code
*	------------------------	---------------
*	00000001wxyza			000wxyz
*	0000001wxyzab			001wxyz
*	000001wxyzabc			010wxyz
*	00001wxyzabcd			011wxyz
*	0001wxyzabcde			100wxyz
*	001wxyzabcdef			101wxyz
*	01wxyzabcdefg			110wxyz
*	1wxyzabcdefgh			111wxyz
*
* Each biased linear code has a leading 1 which identifies the segment
* number. The value of the segment number is equal to 7 minus the number
* of leading 0's. The quantization interval is directly available as the
* four bits wxyz.  * The trailing bits (a - h) are ignored.
*
* Ordinarily the complement of the resulting code word is used for
* transmission, and so the code word is complemented before it is returned.
*
* For further information see John C. Bellamy's Digital Telephony, 1982,
* John Wiley & Sons, pps 98-111 and 472-476.
*/
unsigned char linear2ulaw( short pcm_val);	/* 2's complement (16-bit range) */



/*
 * ulaw2linear() - Convert a u-law value to 16-bit linear PCM
 *
 * First, a biased linear code is derived from the code word. An unbiased
 * output can then be obtained by subtracting 33 from the biased code.
 *
 * Note that this function expects to be passed the complement of the
 * original code word. This is in keeping with ISDN conventions.
 */
short ulaw2linear( unsigned char	u_val);



extern short    mulawtolin[256];
extern unsigned char lintomulaw[65536];

extern short    alawtolin[256];
extern unsigned char lintoalaw[8192]; 

#define s2u(x)	lintomulaw[((unsigned short)(x))]
#define u2s(x)	mulawtolin[((unsigned char)(x))]
#define s2a(x)  lintoalaw[((unsigned short)(x))>>3]
#define a2s(x)  alawtolin[((unsigned char)(x))]

struct s_coded_unit;

void g711_init(void);

unsigned short                      g711_get_formats_count (void);
const struct s_codec_format* g711_get_format (unsigned short idx);
//int                          g711_encode     (unsigned short idx, 
//                                              unsigned char *state, 
//                                              short  *in, 
//                                              struct s_coded_unit *out);
//int                          g711_decode     (unsigned short idx, 
//                                              unsigned char *state, 
//                                              struct s_coded_unit *in, 
//                                              short     *out);

#endif
