//clases to code and decoude the tfrc information


#ifndef CDN_H
#define CDN_H


#include <sys/types.h> /* basic system data types */
#include <sys/socket.h> /* basic socket definitions */
//#include <sys/time.h> /* timeval{} for select() */
#include <time.h> /* timespec{} for pselect() */
#include <netinet/in.h> /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h> /* inet(3) functions */
#include <errno.h>
#include <fcntl.h> /* for nonblocking */
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> /* for S_xxx file mode constants */
#include <sys/uio.h> /* for iovec{} and readv/writev */
#include <unistd.h>
#include <sys/wait.h>
#include <sys/un.h> /* for Unix domain sockets */


class gather {
public:
  char *gav;
  gather(int gavsize);
  ~gather();
  char * pega (uint16_t *seqnump, unsigned long *stsp, float *rttp);
  char * pega (unsigned long *stspp, unsigned long *delayp, float *rxrap, float *lossrp);
};


class decod {
public:
  char * srxbuf;
  short seqnum;
  unsigned long st_est, delay; 
  float rtt_rr, lossr;
  decod(char *srxbufp);
  ~decod();
  void decsend(); //decode the message from th sender
  void decrx();  //decode the message from the receiver
};


#endif


