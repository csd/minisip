
#ifndef NSEND_H
#define NSEND_H

#include "cdn.h"

class nsend {
 public:
  int sockfd;
  in_port_t dport;
  char *ipad;
  struct sockaddr_in desock;
  nsend(char *ipadp, in_port_t dportp);
  nsend(in_port_t dportp);
  ~nsend();
  void initns ();
  int senddata(char* datap, int datalen);
  void set_dskt(struct sockaddr_in desktp);
};


#endif



