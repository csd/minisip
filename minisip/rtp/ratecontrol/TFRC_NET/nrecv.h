
#ifndef NRECV_H
#define NRECV_H

#include "cdn.h"
#include "../TFRC_RECEIVER/secontainer.h"
#include "../TFRC_SENDER/fbcontainer.h"


class nrecv {
 public:
  int sockfd, dlen;
  char *mesg;
  in_port_t loport;
  secont *sic;
  fbcont *con;
  struct sockaddr_in losock, desock;
  //decod dec(mesg);
  nrecv(in_port_t loportp, int dlenp);
  ~nrecv();
  void initns ();
  int recvdata(double art);
  void decdat(double artt);
  int get_sockfd();
  struct sockaddr_in get_desock();
  void set_scont(secont *sicp); //container for the data comming from the sender
  void set_rcont(fbcont * conp); //container for the data comming from the rceiver (feedbck data)
};

#endif

