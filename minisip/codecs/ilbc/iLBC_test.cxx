    
//A.1 iLBC_test.c 
    
   /****************************************************************** 
    
       iLBC Speech Coder ANSI-C Source Code 
    
       iLBC_test.c  
    
       Copyright (c) 2001, 
       Global IP Sound AB. 
       All rights reserved. 
    
   ******************************************************************/ 
    
   #include <math.h> 
   #include <stdlib.h> 
   #include <stdio.h> 
   #include <string.h> 
   #include "iLBC_define.h" 
   #include "iLBC_encode.h" 
   #include "iLBC_decode.h" 
    
   /* Runtime statistics */ 
   #include <time.h> 
    
   #define TIME_PER_FRAME  30   
   #define ILBCNOOFWORDS   (NO_OF_BYTES/2) 
    
   /*----------------------------------------------------------------* 
    *  Encoder interface function  
    *---------------------------------------------------------------*/ 
    
   short encode( /* (o) Number of bytes encoded */ 
       iLBC_Enc_Inst_t *iLBCenc_inst,  /* (i/o) Encoder instance */  
       short *encoded_data, /* (o) The encoded bytes */ 
       short *data /* (i) The signal block to encode */ 
   ){ 
     
       float block[BLOCKL]; 
       int k; 
    
       /* convert signal to float */ 
    
       for(k=0;k<BLOCKL;k++) block[k] = (float)data[k]; 
    
       /* do the actual encoding */ 
    
       iLBC_encode((unsigned char *)encoded_data, block, iLBCenc_inst); 
    
       return (NO_OF_BYTES); 
   } 
    
   /*----------------------------------------------------------------* 
    *  Decoder interface function  
    *---------------------------------------------------------------*/ 
    
   short decode( /* (o) Number of decoded samples */ 
       iLBC_Dec_Inst_t *iLBCdec_inst, /* (i/o) Decoder instance */  
       short *decoded_data, /* (o) Decoded signal block */ 
       short *encoded_data, /* (i) Encoded bytes */ 
       short mode  /* (i) 0=PL, 1=Normal */ 
   ){ 
       int k; 
       float decblock[BLOCKL], dtmp; 
    
       /* check if mode is valid */ 
    
       if (mode<0 || mode>1) { 
           printf("\nERROR - Wrong mode - 0, 1 allowed\n"); exit(3);} 
    
       /* do actual decoding of block */ 
    
       iLBC_decode(decblock, (unsigned char *)encoded_data,  
           iLBCdec_inst, mode); 
    
       /* convert to short */ 
    
       for(k=0;k<BLOCKL;k++){  
           dtmp=decblock[k]; 
           if (dtmp<MIN_SAMPLE) 
               dtmp=MIN_SAMPLE; 
           else if (dtmp>MAX_SAMPLE) 
               dtmp=MAX_SAMPLE; 
           decoded_data[k] = (short) dtmp; 
       } 
    
       return (BLOCKL); 
   } 
    
   /*------------------------------------------------------------------* 
    *  Main program to test iLBC encoding and decoding  
     
    * 
    *  Usage: 
    *    exefile_name.exe <infile> <bytefile> <outfile> <channel> 
    * 
    *    <infile>   : Input file, speech for encoder (16-bit pcm file) 
    *    <bytefile> : Bit stream output from the encoder 
    *    <outfile>  : Output file, decoded speech (16-bit pcm file) 
    *    <channel>  : Bit error file, optional (16-bit) 
    *                     1 - Packet received correctly 
    *                     0 - Packet Lost 
    * 
    *------------------------------------------------------------------
   */ 
    
   int main(int argc, char* argv[]) 
   { 
    
       /* Runtime statistics */ 
    
       float starttime; 
       float runtime; 
       float outtime; 
    
       FILE *ifileid,*efileid,*ofileid, *cfileid; 
       short data[BLOCKL]; 
       short encoded_data[ILBCNOOFWORDS], decoded_data[BLOCKL]; 
       int len; 
       short pli; 
       int blockcount = 0; 
       int packetlosscount = 0; 
    
       /* Create structs */ 
       iLBC_Enc_Inst_t Enc_Inst; 
       iLBC_Dec_Inst_t Dec_Inst; 
    
       /* get arguments and open files */ 
    
       if ((argc!=4) && (argc!=5)) { 
           fprintf(stderr,  
               "\n*-----------------------------------------------*\n"); 
           fprintf(stderr,  
               "   %s input encoded decoded (channel)\n\n", argv[0]);  
           fprintf(stderr,  
               "   input   : Speech for encoder (16-bit pcm file)\n"); 
           fprintf(stderr,  
               "   encoded : Encoded bit stream\n"); 
           fprintf(stderr,  
               "   decoded : Decoded speech (16-bit pcm file)\n"); 
           fprintf(stderr,  
               "   channel : Packet loss pattern, optional (16-bit)\n"); 
           fprintf(stderr,  
               "                  1 - Packet received correctly\n"); 
           fprintf(stderr,  
     
               "                  0 - Packet Lost\n"); 
           fprintf(stderr,  
               "*-----------------------------------------------*\n\n"); 
           exit(1); 
       } 
       if( (ifileid=fopen(argv[1],"rb")) == NULL){ 
           fprintf(stderr,"Cannot open input file %s\n", argv[1]);  
           exit(2);} 
       if( (efileid=fopen(argv[2],"wb")) == NULL){ 
           fprintf(stderr, "Cannot open encoded file file %s\n",  
               argv[2]); exit(1);} 
       if( (ofileid=fopen(argv[3],"wb")) == NULL){ 
           fprintf(stderr, "Cannot open decoded file %s\n",  
               argv[3]); exit(1);} 
       if (argc==5) { 
           if( (cfileid=fopen(argv[4],"rb")) == NULL) { 
               fprintf(stderr, "Cannot open channel file %s\n",  
                   argv[4]);  
               exit(1); 
           } 
       } else { 
           cfileid=NULL; 
       } 
    
       /* print info */ 
    
       fprintf(stderr, "\n"); 
       fprintf(stderr,  
           "*---------------------------------------------------*\n"); 
       fprintf(stderr,  
           "*                                                   *\n"); 
       fprintf(stderr,  
           "*      iLBC test program                            *\n"); 
       fprintf(stderr,  
           "*                                                   *\n"); 
       fprintf(stderr,  
           "*                                                   *\n"); 
       fprintf(stderr,  
           "*---------------------------------------------------*\n"); 
       fprintf(stderr, "\nInput file     : %s\n", argv[1]); 
       fprintf(stderr,"Encoded file   : %s\n", argv[2]); 
       fprintf(stderr,"Output file    : %s\n", argv[3]); 
       if (argc==5) { 
           fprintf(stderr,"Channel file   : %s\n\n", argv[4]); 
       } 
    
       /* Initialization */ 
    
       initEncode(&Enc_Inst); 
       initDecode(&Dec_Inst, 1); 
    
       /* Runtime statistics */ 
    
     
       starttime=clock()/(float)CLOCKS_PER_SEC;  
    
       /* loop over input blocks */ 
    
       while( fread(data,sizeof(short),BLOCKL,ifileid)==BLOCKL){ 
            
           blockcount++; 
            
           /* encoding */ 
    
           fprintf(stderr, "--- Encoding block %i --- ",blockcount); 
           len=encode(&Enc_Inst, encoded_data, data); 
           fprintf(stderr, "\r"); 
    
           /* write byte file */ 
    
           fwrite(encoded_data,sizeof(short),len,efileid); 
    
           /* get channel data if provided */ 
           if (argc==5) { 
               if (fread(&pli, sizeof(short), 1, cfileid)) { 
                   if ((pli!=0)&&(pli!=1)) { 
                       fprintf(stderr, "Error in channel file\n"); 
                       exit(0); 
                   } 
                   if (pli==0) { 
                       /* Packet loss -> remove info from frame */ 
                       memset(encoded_data, 0,  
                           sizeof(short)*ILBCNOOFWORDS); 
                       packetlosscount++; 
                   } 
               } else { 
                   fprintf(stderr, "Error. Channel file too short\n"); 
                   exit(0); 
               } 
           } else { 
               pli=1; 
           } 
            
           /* decoding */ 
    
           fprintf(stderr, "--- Decoding block %i --- ",blockcount); 
           len=decode(&Dec_Inst, decoded_data, encoded_data, pli); 
           fprintf(stderr, "\r"); 
    
           /* write output file */ 
    
           fwrite(decoded_data,sizeof(short),len,ofileid); 
       } 
    
       /* Runtime statistics */ 
    
       runtime = (float)(clock()/(float)CLOCKS_PER_SEC-starttime); 
     
       outtime = (float)((float)blockcount* 
           (float)TIME_PER_FRAME/1000.0); 
       printf("\nLength of speech file: %.1f s\n", outtime); 
       printf("Packet loss          : %.1f%%\n",  
           100.0*(float)packetlosscount/(float)blockcount); 
       printf("Time to run iLBC_encode+iLBC_decode:"); 
       printf(" %.1f s (%.1f %% of realtime)\n", runtime,  
           (100*runtime/outtime)); 
        
       /* close files */ 
    
       fclose(ifileid);  fclose(efileid); fclose(ofileid); 
       if (argc==5) { 
           fclose(cfileid); 
       } 
       return(0); 
   } 
    
    
