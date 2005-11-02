#include "nsend.h"


nsend::nsend (char *ipadp, in_port_t dportp) {
  dport=dportp;
  ipad=ipadp;
  initns();
}

nsend::nsend (in_port_t dportp) {
  dport=dportp;
  sockfd=socket(AF_INET, SOCK_DGRAM, 0);
}

nsend::~nsend () {
}

void nsend::initns(void) {
  bzero(&desock, sizeof(desock));
  desock.sin_family=AF_INET;
  desock.sin_port=htons(dport);
  inet_pton(AF_INET, ipad, &desock.sin_addr);
  sockfd=socket(AF_INET, SOCK_DGRAM, 0);
}

void nsend::set_dskt(struct sockaddr_in desktp) {
  desock=desktp;
  desock.sin_port=htons(dport);
}


int nsend::senddata(char *datap, int datalen) {
  int ns;
  ns=sendto(sockfd, datap, datalen, 0, (sockaddr *) &desock, (socklen_t) sizeof(desock));
  return(ns);
}
